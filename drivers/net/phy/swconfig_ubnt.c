/*
 * swconfig_ubnt.c: swconfig UBNT extension
 * Copyright 2020 Ubiquiti Networks, Inc.
 */

#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/if.h>
#include <linux/switch.h>
#include <linux/proc_fs.h>
#include <../fs/proc/internal.h>

#define UEXT_PROCFS_ROOT "switch_uext"
#define UEXT_WORKQUEUE_NAME "uext_wq"
#define UEXT_PROCFS_ARL_TABLE "arl"

static struct proc_dir_entry *uext_proc_dir = NULL;
static struct workqueue_struct *uext_wq = NULL;

/**
 * @brief looping over list, continue from pos if pos not NULL
 */
#define list_for_each_safe_continue(pos, n, head)                                   \
	for ((pos) = ((pos) == NULL) ? (head)->next : (pos)->next, n = (pos)->next; \
	     (pos) != (head); (pos) = n, n = (pos)->next)

/**
 * @brief Retrieve pointer to the switch_dev structure
 *
 * @param ext - ubnt extension structure
 * @return struct switch_dev * - swconfig device structure
 */
static inline struct switch_dev *uext_to_sw(struct uext *ext)
{
	return container_of(ext, struct switch_dev, ubnt_ext);
}

/**
 * @brief Remove entry from proc fs
 *
 * @param entry - procfs entry structure
 */
static inline void swconfig_uext_proc_remove(struct proc_dir_entry **entry) {
	if (*entry) {
		proc_remove(*entry);
		*entry = NULL;
	}
}

/**
 * @brief Schedule delayed work (uext_wq workqueue)
 *
 * @param work - workqueue structure
 * @param delay - delay in ms
 */
static inline void uext_schedule_delayed_work(struct delayed_work *work, unsigned long delay)
{
	if (likely(uext_wq)) {
		queue_delayed_work(uext_wq, work, delay);
	}
}

/**
 * @brief PROC FS
 */

/**
 * @brief PROC FS - taken from ar8216.c
 */
struct uext_arl_seq_iter {
	/* Current node in list */
	struct list_head *np;
	/* Current table */
	uint8_t tidx;
	/* Current port */
	uint8_t pidx;
	struct uext_arl_cache *cache;
};

/**
 * @brief Return next ARL entry of sequence
 *
 * @param iter - seq iterator
 * @return struct uext_arl_cache_entry* cache entry
 */
static struct uext_arl_cache_entry *uext_arl_proc_cache_entry_next(struct uext_arl_seq_iter *it)
{
	struct uext_arl_cache *cache = it->cache;
	struct list_head *n;
	struct uext_arl_cache_entry *entry;
	struct uext *ubnt_ext = cache->private;
	struct switch_dev *dev = uext_to_sw(ubnt_ext);

	for (; it->pidx < dev->ports; it->pidx++, it->tidx = 0) {
		for (; it->tidx < UEXT_ARL_LIST_HASHSIZE; it->tidx++, it->np = NULL) {
			list_for_each_safe_continue(it->np, n, &cache->table[it->tidx].list)
			{
				entry = list_entry(it->np, struct uext_arl_cache_entry, list);

				if (entry->lut_e.port != it->pidx) {
					continue;
				}

				if (time_is_before_jiffies(entry->last_seen +
							   (cache->age_time * HZ))) {
					list_del(it->np);
					kfree(entry);
				} else {
					return entry;
				}
			}
		}
	}

	return NULL;
}

/**
 * @brief Initialize data for walking through a ARL cache
 *
 * @param s - seq_file structure
 * @param pos - seek position
 * @return void* - pointer to cache entry
 */
static void *uext_arl_proc_seq_start(struct seq_file *s, loff_t *pos)
{
	struct uext_arl_seq_iter *it = s->private;
	struct uext_arl_cache *cache = it->cache;

	if (NULL == cache) {
		return NULL;
	}
	mutex_lock(&cache->lock);

	return uext_arl_proc_cache_entry_next(it);
}

/**
 * @brief Iterator for the ARL cache
 *
 * @param s - seq_file structure
 * @param v - entry
 * @param pos - seek position
 * @return void* - pointer to cache entry
 */
static void *uext_arl_proc_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	return uext_arl_proc_cache_entry_next(s->private);
}

/**
 * @brief seq cleanup function
 *
 * @param s - seq_file structure
 * @param v - entry number
 */
static void uext_arl_proc_seq_stop(struct seq_file *s, void *v)
{
	struct uext_arl_seq_iter *it = s->private;
	struct uext_arl_cache *cache = it->cache;
	mutex_unlock(&cache->lock);
	return;
}

/**
 * @brief Interpret a cache entry
 *
 * @param s - seq_file structure
 * @param v - cache entry
 * @return int - error from errno.h, 0 on success
 */
static int uext_arl_proc_seq_show(struct seq_file *s, void *v)
{
	struct uext_arl_cache_entry *entry = v;
	seq_printf(s, "%d %d " MACSTR " %lu %lu\n", entry->lut_e.port, entry->lut_e.vid,
		   MAC2STR(entry->lut_e.mac), (jiffies + (HZ / 2) - entry->first_add) / HZ,
		   (jiffies - entry->last_seen) / HZ);

	return 0;
}
/**
 * @brief seq ops
 */
static struct seq_operations uext_arl_proc_seq_ops = {
	.start = uext_arl_proc_seq_start,
	.next = uext_arl_proc_seq_next,
	.stop = uext_arl_proc_seq_stop,
	.show = uext_arl_proc_seq_show,
};

/**
 * @brief procfs open handler (read arl table)
 *
 * @param inode - vfs inode data structure
 * @param file - pointer to file structure
 * @return int - error from errno.h, 0 on success
 */
static int uext_arl_proc_open(struct inode *inode, struct file *file)
{
	struct uext_arl_seq_iter *it = NULL;
	int ret = seq_open(file, &uext_arl_proc_seq_ops);
	if (!ret) {
		it = kzalloc(sizeof(*it), GFP_KERNEL);
		if(NULL == it) {
			pr_err("Unable allocate memory for arl iterator\n");
			return -ENOMEM;
		}
		/** pass pointer to uext_arl_cache_entry */
		it->cache = PDE(inode)->data;
		((struct seq_file *)file->private_data)->private = it;
	}
	return ret;
}

/**
 * @brief procfs release file handler
 *
 * @param inode - vfs inode data structure
 * @param file - pointer to file structure
 * @return int - error from errno.h, 0 on success
 */
static int uext_arl_proc_release(struct inode *inode, struct file *file)
{
	struct seq_file *s = file->private_data;
	kfree(s->private);
	seq_release(inode, file);
	return 0;
}

/**
 * @brief arl cache proc fops
 *
 */
static struct file_operations uext_arl_proc_fops = {
	.owner = THIS_MODULE,
	.open = uext_arl_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = uext_arl_proc_release,
};

/**
 * @brief Create procfs dir entry
 *
 * @param name - file name
 * @param parent - procfs parent
 * @return struct proc_dir_entry* - procfs entry, NULL on error
 */
static struct proc_dir_entry *swconfig_uext_proc_create_dir(const char *name,
							    struct proc_dir_entry *parent)
{
	struct proc_dir_entry *entry;

	entry = proc_mkdir(name, parent);
	if (NULL == entry) {
		pr_warn("%s : unable to create procfs dir for %s", __func__, name);
	}
	return entry;
}

/**
 * @brief Creates entry in procfs dir
 *
 * @param name - file name
 * @param parent - parent of entry
 * @param ops - file ops
 * @param data - private data
 * @return struct proc_dir_entry* - returns pointer to an entry
 */
static inline struct proc_dir_entry *swconfig_uext_proc_create_entry(const char *name,
								     struct proc_dir_entry *parent,
								     struct file_operations *ops,
								     void *data)
{
	struct proc_dir_entry *entry;

	entry = proc_create_data(name, S_IFREG | S_IRUGO, parent, ops, data);
	if (NULL == entry) {
		pr_warn("%s : unable to create procfs files for %s", __func__, name);
	}
	return entry;
}

/**
 * @brief Get ARL cache age time
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_arl_cache_age_time_get(struct switch_dev *dev, const struct switch_attr *attr,
					 struct switch_val *val)
{
	struct uext *ext = &dev->ubnt_ext;
	val->value.i = ext->arl_cache->age_time;
	return 0;
}

/**
 * @brief Set ARL cache age time
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_arl_cache_age_time_set(struct switch_dev *dev, const struct switch_attr *attr,
					 struct switch_val *val)
{
	struct uext *ext = &dev->ubnt_ext;
	int age_time = val->value.i;

	if (age_time <= 0 || age_time > 0xffff) {
		return -EINVAL;
	}
	ext->arl_cache->age_time = age_time;
	return 0;
}

/**
 * @brief Get port's cached MIB counter
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_port_mib_get(struct switch_dev *dev, const struct switch_attr *attr,
				   struct switch_val *val)
{
	struct uext *ubnt_ext = &dev->ubnt_ext;
	struct uext_mib_cache *cache;

	if (val->port_vlan >= dev->ports) {
		return -EINVAL;
	}

	if (NULL == ubnt_ext || NULL == ubnt_ext->mib_cache) {
		return -EOPNOTSUPP;
	}
	cache = ubnt_ext->mib_cache;

	mutex_lock(&cache->lock);
	memcpy(val->value.mib, &cache->counters[val->port_vlan], sizeof(*val->value.mib));
	mutex_unlock(&cache->lock);

	return 0;
}

/**
 * @brief Initialize looping over ARL table (lock driver's resources & alloc iterator)
 * @note Every driver is responsible for locking its own resources.
 * Call "ubnt_arl_cache_iter_end" function in the end of a looping!
 * @param dev - switch control structure
 * @param it - uext arl iterator structure
 * @return int - error from errno.h, 0 on success
 */
static inline int ubnt_arl_cache_iter_begin(struct switch_dev *dev, struct uext_arl_iter *it)
{
	it->op = UEXT_ARL_IT_BEGIN;
	it->data = NULL;
	return dev->ops->ubnt_ext_ops.get_arl_entry(dev, NULL, it);
}

/**
 * @brief End looping over ARL table (unlock driver's resources & dealloc iterator)
 * @note Every driver is responsible for locking its own resources.
 * @param dev - switch control structure
 * @param it - uext arl iterator structure
 * @return int - error from errno.h, 0 on success
 */
static inline int ubnt_arl_cache_iter_end(struct switch_dev *dev, struct uext_arl_iter *it)
{
	it->op = UEXT_ARL_IT_END;
	return dev->ops->ubnt_ext_ops.get_arl_entry(dev, NULL, it);
}

/**
 * @brief Get next entry from ARL table
 * @note Call "ubnt_arl_cache_iter_begin" first! Call "ubnt_arl_cache_iter_end" on looping's exit.
 * @param dev - switch control structure
 * @param it - uext arl iterator structure
 * @param entry [out] - entr
 * @return int	- 0 entry_out contains valid entry
 * 				- ENODATA when no more data to read
 * 				- error from errno.h otherwise
 */
static inline int ubnt_arl_cache_iter_next(struct switch_dev *dev, struct uext_arl_iter *it,
					   struct uext_arl_lut_entry *entry_out)
{
	it->op = UEXT_ARL_IT_NEXT;
	return dev->ops->ubnt_ext_ops.get_arl_entry(dev, entry_out, it);
}

/**
 * @brief Sync ARL cache
 *
 * @param work workqueue structure
 */
static void swconfig_uext_arl_cache_worker(struct work_struct *work)
{
	struct uext_arl_cache *cache = container_of(work, struct uext_arl_cache, worker.work);
	struct uext *ubnt_ext = cache->private;
	struct switch_dev *dev = uext_to_sw(ubnt_ext);
	struct uext_arl_iter it = { 0 };
	struct uext_arl_lut_entry entry = { 0 };
	struct uext_arl_cache_entry *entry_it;
	struct list_head *node_p;
	bool hit = false;
	int rc = 0, idx = 0;

	if (NULL == ubnt_ext->arl_cache || NULL == dev->ops ||
	    NULL == dev->ops->ubnt_ext_ops.get_arl_entry) {
		return;
	}

	mutex_lock(&dev->sw_mutex);
	mutex_lock(&cache->lock);

	/* Iteration init */
	rc = ubnt_arl_cache_iter_begin(dev, &it);
	if (rc) {
		if (-EBUSY != rc) {
			pr_err("Unable to initialize an iteration over ARL table rc %d (device %s)\n",
			       rc, dev->name);
		}
		/* Try it again later */
		goto unlock_schedule;
	}

	/* Sync cache */
	for (;;) {
		rc = ubnt_arl_cache_iter_next(dev, &it, &entry);

		if (-ENODATA == rc) {
			/* no more records */
			break;
		} else if (rc) {
			if (-EBUSY != rc) {
				pr_err("Unable to get next ARL entry from device %s (rc %d)\n",
				       dev->name, rc);
			}
			ubnt_arl_cache_iter_end(dev, &it);
			/* Try it again later */
			goto unlock_schedule;
		}

		hit = false;

		idx = UEXT_ARL_LIST_HASH(entry.mac);

		/* Step 1: check to see if MAC address is in cache.
			If so, update port and last_seen. */
		list_for_each (node_p, &(cache->table[idx].list)) {
			entry_it = list_entry(node_p, struct uext_arl_cache_entry, list);

			/**
			 * @note
			 * Switch chip's ARL table is usually small and the place in it is precious.
			 * For many switch chips MAC address appears in ARL lut only once although it
			 * shouldn't (e.g. the same MAC address with a different VID). We on the other
			 * hand are not limited by memory so much.
			 * Update the LUT entry only if the whole set (mac address, vid, port) matches,
			 * create a new entry otherwise.
			 */
			if (0 == memcmp(&entry_it->lut_e, &entry, sizeof(entry_it->lut_e))) {
				entry_it->last_seen = jiffies;
				hit = true;
				break;
			}
		}

		/* Step 2: if it wasn't already found, add it */
		if (false == hit) {
			entry_it = (struct uext_arl_cache_entry *)kzalloc(sizeof(*entry_it),
									  GFP_KERNEL);
			if (NULL == entry_it) {
				pr_err("Unable allocate memory for arl cache entry!\n");
				ubnt_arl_cache_iter_end(dev, &it);
				/* Try it again later */
				goto unlock_schedule;
			}
			memcpy(&entry_it->lut_e, &entry, sizeof(entry_it->lut_e));
			entry_it->last_seen = jiffies;
			entry_it->first_add = jiffies;
			list_add(&entry_it->list, &cache->table[idx].list);
		}
	}

	rc = ubnt_arl_cache_iter_end(dev, &it);
	if (rc) {
		if (-EBUSY != rc) {
			pr_err("Unable to end an iteration over ARL table rc %d (device %s)\n", rc,
			       dev->name);
		}
		goto unlock_schedule;
	}

unlock_schedule:
	mutex_unlock(&cache->lock);
	mutex_unlock(&dev->sw_mutex);
	uext_schedule_delayed_work(&cache->worker, msecs_to_jiffies(UEXT_ARL_READ_WORK_DELAY_MS));
}

/**
 * @brief Initialize ARL cache
 *
 * @param dev - switch control structure
 * @return int - error from errno.h, 0 on success
 */
static int swconfig_uext_arl_cache_init(struct switch_dev *dev)
{
	struct uext *ubnt_ext = &dev->ubnt_ext;
	const struct uext_ops *ubnt_ext_ops = &dev->ops->ubnt_ext_ops;

	struct uext_arl_cache *cache;
	int i = 0;

	if (NULL == ubnt_ext_ops->get_arl_entry) {
		return -EOPNOTSUPP;
	}

	cache = (struct uext_arl_cache *)kzalloc(sizeof(*cache), GFP_KERNEL);
	if (NULL == cache) {
		return -ENOMEM;
	}

	cache->private = ubnt_ext;
	mutex_init(&cache->lock);
	cache->age_time = UEXT_ARL_DEFAULT_AGE_TIME_S;
	for (i = 0; i < UEXT_ARL_LIST_HASHSIZE; ++i) {
		/* Init hash table */
		INIT_LIST_HEAD(&cache->table[i].list);
	}

	INIT_DELAYED_WORK(&cache->worker, swconfig_uext_arl_cache_worker);

	/* Create procfs endpoint */
	cache->procfs = swconfig_uext_proc_create_entry(UEXT_PROCFS_ARL_TABLE, ubnt_ext->procfs,
							&uext_arl_proc_fops, cache);
	if (NULL == cache->procfs) {
		kfree(cache);
		return -ENOMEM;
	}

	ubnt_ext->arl_cache = cache;
	uext_schedule_delayed_work(&cache->worker, msecs_to_jiffies(UEXT_ARL_READ_WORK_DELAY_MS));
	return 0;
}

/**
 * @brief Flush ARL cache
 *
 * @param cache - ARL cache structure
 */
static inline void swconfig_uext_arl_cache_flush(struct uext_arl_cache *cache)
{
	struct uext_arl_cache_entry *entry;
	struct list_head *c, *n;
	int i = 0;

	for (i = 0; i < UEXT_ARL_LIST_HASHSIZE; ++i) {
		list_for_each_safe (c, n, &cache->table[i].list) {
			entry = list_entry(c, struct uext_arl_cache_entry, list);
			list_del(c);
			kfree(entry);
		}
	}
}

/**
 * @brief Clear ARL cache
 *
 * @param dev - switch control structure
 * @return int - error from errno.h, 0 on success
 */
static int swconfig_uext_arl_cache_reset(struct switch_dev *dev)
{
	struct uext *ubnt_ext = &dev->ubnt_ext;
	struct uext_arl_cache *cache;

	if (NULL == ubnt_ext->arl_cache) {
		return -EOPNOTSUPP;
	}

	cache = ubnt_ext->arl_cache;

	mutex_lock(&cache->lock);
	swconfig_uext_arl_cache_flush(cache);
	mutex_unlock(&cache->lock);

	return 0;
}

/**
 * @brief Destroy ARL cache
 *
 * @param dev - switch control structure
 */
static void swconfig_uext_arl_cache_destroy(struct switch_dev *dev)
{
	struct uext *ubnt_ext = &dev->ubnt_ext;
	struct uext_arl_cache *cache;

	if (NULL == ubnt_ext->arl_cache) {
		return;
	}

	cache = ubnt_ext->arl_cache;
	cancel_delayed_work_sync(&cache->worker);
	mutex_lock(&cache->lock);
	swconfig_uext_arl_cache_flush(cache);
	ubnt_ext->arl_cache = NULL;
	swconfig_uext_proc_remove(&cache->procfs);
	mutex_unlock(&cache->lock);
	kfree(cache);
}

/**
 * @brief Get rate x per second
 *
 * @param delta_x - delta x (quantity)
 * @param delta_ms  - delta T (elapsed time)
 * @return uint64_t rate x/s
 */
static inline uint64_t swconfig_uext_mib_get_x_per_sec(uint64_t delta_x, unsigned int delta_ms) {

	if(!delta_ms) {
		WARN(1, "%s : delta_ms (divisor) is zero\n", __func__);
		return 0;
	}

	/* get X per seconds*/
	return div_u64(delta_x * 1000, delta_ms);
}

/**
 * @brief Sync MIB cache
 *
 * @param work workqueue structure
 */
static void swconfig_uext_mib_cache_worker(struct work_struct *work)
{
	struct uext_mib_cache *cache = container_of(work, struct uext_mib_cache, worker.work);
	struct uext *ubnt_ext = cache->private;
	struct switch_dev *dev = uext_to_sw(ubnt_ext);
	int rc = 0, i;
	unsigned int delta_ms;

	/* sampling helpers */
	unsigned long timestamp_old;
	uint64_t rx_total_old;
	uint64_t tx_total_old;
	uint64_t rx_byte_old;
	uint64_t tx_byte_old;

	if (NULL == ubnt_ext->mib_cache || NULL == dev->ops ||
	    NULL == dev->ops->ubnt_ext_ops.get_port_mib) {
		return;
	}
	/* Sync cache */
	mutex_lock(&dev->sw_mutex);
	mutex_lock(&cache->lock);

	for (i=0; i < dev->ports; ++i) {

		/* Save old samples  */
		rx_total_old = cache->counters[i].rx_total;
		tx_total_old = cache->counters[i].tx_total;
		rx_byte_old = cache->counters[i].rx_byte;
		tx_byte_old = cache->counters[i].tx_byte;
		timestamp_old = cache->counters[i].timestamp;

		/* Update counters */
		rc = dev->ops->ubnt_ext_ops.get_port_mib(dev, (cache->counters+i), i);
		if(rc) {
			if (-EBUSY != rc) {
				pr_err("Unable to get %d port's MIB counter from device %s (rc %d)\n", i, dev->name, rc);
			}
			goto unlock_schedule;
		}

		cache->counters[i].timestamp = jiffies;
		delta_ms = jiffies_to_msecs((long)(cache->counters[i].timestamp - timestamp_old));
		/* Calculate Rx/Tx rates in bits/s */
		cache->counters[i].rx_rate = swconfig_uext_mib_get_x_per_sec(
			(int64_t)(cache->counters[i].rx_byte - rx_byte_old), delta_ms) << 3;
		cache->counters[i].tx_rate = swconfig_uext_mib_get_x_per_sec(
			(int64_t)(cache->counters[i].tx_byte - tx_byte_old), delta_ms) << 3;
		/* Calculate Rx/Tx rates in pps/s */
		cache->counters[i].rx_pps = swconfig_uext_mib_get_x_per_sec(
			(int64_t)(cache->counters[i].rx_total - rx_total_old), delta_ms);
		cache->counters[i].tx_pps = swconfig_uext_mib_get_x_per_sec(
			(int64_t)(cache->counters[i].tx_total - tx_total_old), delta_ms);
	}

unlock_schedule:
	mutex_unlock(&cache->lock);
	mutex_unlock(&dev->sw_mutex);
	uext_schedule_delayed_work(&cache->worker, msecs_to_jiffies(UEXT_MIB_READ_WORK_DELAY_MS));
}

/**
 * @brief Initialize MIB cache
 *
 * @param dev - switch control structure
 * @return int - error from errno.h, 0 on success
 */
static int swconfig_uext_mib_cache_init(struct switch_dev *dev)
{
	struct uext *ubnt_ext = &dev->ubnt_ext;
	const struct uext_ops *ubnt_ext_ops = &dev->ops->ubnt_ext_ops;
	struct uext_mib_cache *cache;

	if (NULL == ubnt_ext_ops->get_port_mib) {
		return -EOPNOTSUPP;
	}

	cache = (struct uext_mib_cache *)kzalloc(sizeof(*cache), GFP_KERNEL);
	if (NULL == cache) {
		return -ENOMEM;
	}

	cache->private = ubnt_ext;
	mutex_init(&cache->lock);
	cache->counters = kzalloc(sizeof(*cache->counters) * dev->ports, GFP_KERNEL);
	if (NULL == cache->counters) {
		return -ENOMEM;
	}
	INIT_DELAYED_WORK(&cache->worker, swconfig_uext_mib_cache_worker);
	ubnt_ext->mib_cache = cache;

	uext_schedule_delayed_work(&cache->worker, msecs_to_jiffies(UEXT_MIB_READ_WORK_DELAY_MS));

	return 0;
}

/**
 * @brief Clear MIB cache
 *
 * @param dev - switch control structure
 * @return int - error from errno.h, 0 on success
 */
static int swconfig_uext_mib_cache_reset(struct switch_dev *dev)
{
	struct uext *ubnt_ext = &dev->ubnt_ext;
	struct uext_mib_cache *cache;

	if (NULL == ubnt_ext->mib_cache) {
		return -EOPNOTSUPP;
	}

	cache = ubnt_ext->mib_cache;

	mutex_lock(&cache->lock);
	memset(cache->counters, 0, sizeof(*cache->counters) * dev->ports);
	mutex_unlock(&cache->lock);

	return 0;
}

/**
 * @brief Destroy MIB cache
 *
 * @param dev - switch control structure
 */
static void swconfig_uext_mib_cache_destroy(struct switch_dev *dev)
{
	struct uext *ubnt_ext = &dev->ubnt_ext;
	struct uext_mib_cache *cache;

	if (NULL == ubnt_ext->mib_cache) {
		return;
	}

	cache = ubnt_ext->mib_cache;
	cancel_delayed_work_sync(&cache->worker);
	mutex_lock(&cache->lock);
	kfree(cache->counters);
	ubnt_ext->mib_cache = NULL;
	mutex_unlock(&cache->lock);
	kfree(cache);
}

/**
 * @brief Handle swconfig reset across all caches of ubnt extension
 *
 * @param dev - switch control structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_reset(struct switch_dev *dev)
{
	struct uext *ubnt_ext = NULL;

	if (NULL == dev) {
		return -EINVAL;
	}

	ubnt_ext = &dev->ubnt_ext;

	/* Clear ARL cache */
	swconfig_uext_arl_cache_reset(dev);

	/* Clear MIB cache */
	swconfig_uext_mib_cache_reset(dev);

	return 0;
}

/**
 * @brief Part of register_switch - initialize ubnt extension
 *
 * @param dev - switch control structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_register(struct switch_dev *dev)
{
	const struct uext_ops *ubnt_ext_ops;
	struct uext *ubnt_ext = NULL;

	if (NULL == dev || NULL == dev->ops) {
		return -EINVAL;
	}

	ubnt_ext_ops = &dev->ops->ubnt_ext_ops;
	ubnt_ext = &dev->ubnt_ext;

	ubnt_ext->procfs = swconfig_uext_proc_create_dir(dev->devname, uext_proc_dir);
	if (NULL == ubnt_ext->procfs) {
		return -ENOMEM;
	}

	swconfig_uext_arl_cache_init(dev);
	swconfig_uext_mib_cache_init(dev);

	return 0;
}

/**
 * @brief Part of unregister_switch - destroy ubnt extension
 *
 * @param dev - switch control structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_unregister(struct switch_dev *dev)
{
	struct uext *ubnt_ext = NULL;

	if (NULL == dev || NULL == dev->ops) {
		return -EINVAL;
	}

	ubnt_ext = &dev->ubnt_ext;
	swconfig_uext_arl_cache_destroy(dev);
	swconfig_uext_mib_cache_destroy(dev);

	swconfig_uext_proc_remove(&ubnt_ext->procfs);
	return 0;
}

/**
 * @brief Initialize ubnt swconfig extension (uext)
 *
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_init(void)
{
	/**
	 * @note
	 * Reading information from switch chips could be time consuming, create a workqueue for
	 * uext's workers (don't slow down keventd_wq).
	 */
	if(NULL == uext_wq) {
		uext_wq = alloc_ordered_workqueue(UEXT_WORKQUEUE_NAME, WQ_MEM_RECLAIM | WQ_CPU_INTENSIVE);
		if (NULL == uext_wq) {
			pr_err("Unable to create uext's workqueue\n");
			return -ENOMEM;
		}
	}

	if (NULL == uext_proc_dir) {
		uext_proc_dir = swconfig_uext_proc_create_dir(UEXT_PROCFS_ROOT, NULL);
		if (NULL == uext_proc_dir) {
			return -ENOMEM;
		}
	}
	return 0;
}

/**
 * @brief Destroy ubnt swconfig extension (uext)
 *
 * @param uext - ubnt extension structure
 */
void swconfig_uext_exit(void)
{
	swconfig_uext_proc_remove(&uext_proc_dir);

	if(NULL != uext_wq) {
		destroy_workqueue(uext_wq);
	}

	return;
}

/**
 * @brief Netlink extension for sending SWITCH_TYPE_MIB
 *
 * @param msg - skbuf structure
 * @param info - netlink informative structure
 * @param attr - attribute number
 * @param mib - mib counter structure
 * @return int - error from errno.h, 0 on success
 */

int swconfig_uext_send_mib(struct sk_buff *msg, struct genl_info *info, int attr,
		   const struct switch_port_mib *mib)
{
	struct nlattr *p = NULL;

	p = nla_nest_start(msg, attr);
#define COUNTER(_enum, _name)                                    \
	if (nla_put_u64(msg, SWITCH_MIB_##_enum, mib->_name)) \
		goto nla_put_failure;
	SWITCH_MIB_COUNTERS(COUNTER)
#undef COUNTER
	if (nla_put_msecs(msg, SWITCH_MIB_TIMESTAMP, mib->timestamp))
		goto nla_put_failure;
	nla_nest_end(msg, p);
	return 0;

nla_put_failure:
	nla_nest_cancel(msg, p);
	return -EINVAL;
}

/**
 * @brief Get SVLAN members
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_get_svlan_ports(struct switch_dev *dev, const struct switch_attr *attr,
			struct switch_val *val)
{
	const struct uext_ops *ops = &dev->ops->ubnt_ext_ops;
	int ret;

	if (val->port_vlan >= dev->vlans)
		return -EINVAL;

	if (!ops->get_svlan_ports)
		return -EOPNOTSUPP;

	ret = ops->get_svlan_ports(dev, val);
	return ret;
}

/**
 * @brief Set SVLAN members
 *
 * @param dev - switch control structure
 * @param attr - switch attribute structure
 * @param val - switch value structure
 * @return int - error from errno.h, 0 on success
 */
int swconfig_uext_set_svlan_ports(struct switch_dev *dev, const struct switch_attr *attr,
			struct switch_val *val)
{
	const struct uext_ops *ops = &dev->ops->ubnt_ext_ops;

	if (val->port_vlan >= dev->vlans)
		return -EINVAL;

	/* validate ports */
	if (val->len > dev->ports)
		return -EINVAL;

	if (!ops->set_svlan_ports)
		return -EOPNOTSUPP;

	return ops->set_svlan_ports(dev, val);
}
