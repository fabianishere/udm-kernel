#include <linux/memblock.h>
#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/of_address.h>
#include <linux/stat.h>
#include <linux/reboot.h>
#include <linux/resvmem_reboot.h>
#include <linux/io.h>

static struct kobject *_kobj = NULL;
static struct resvmem_dev *p_resmem = NULL;
static const char *reboot_type_str[] = {
#define TYPE(_name, _enum) #_name,
	RESVMEM_REBOOT_TYPES(TYPE)
#undef TYPE
	NULL
};

void resvmem_set_reboot_reason(u32 reboot_reason)
{
	p_resmem->reason->type = reboot_reason;
}
EXPORT_SYMBOL(resvmem_set_reboot_reason);

static int resvmem_get_reboot_reason(void)
{
	if (p_resmem->reason->type < 0 || p_resmem->reason->type >= RESVMEM_REBOOT_COUNT)
		return RESVMEM_REBOOT_USER_MANUAL;

	return p_resmem->reason->type;
}

static ssize_t reboot_reason_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%s\n", reboot_type_str[resvmem_get_reboot_reason()]);
}

static struct kobj_attribute _resvmem_attr = __ATTR(reboot_reason, S_IRUGO, reboot_reason_show, NULL);

static int __init resvmem_sysfs_init(void)
{
	_kobj = kobject_create_and_add("resvmem", kernel_kobj);
	if (!_kobj) {
		printk("error: failed to create /sys/kernel/resvmem directory\n");
		return -ENOMEM;
	}

	if (sysfs_create_file(_kobj, &_resvmem_attr.attr))
		printk("error: could not create reboot_reason file\n");

	return 0;
}

static int resvmem_probe(struct platform_device *pdev)
{
	int rc = 0;
	struct device *dev = &pdev->dev;
	struct device_node *np = NULL;
	struct resource resv_mem = {0};

	p_resmem = devm_kzalloc(dev, sizeof(struct resvmem_dev), GFP_KERNEL);
	if (!p_resmem)
		return -ENOMEM;

	/* Get reserved memory region from Device-tree */
	np = of_parse_phandle(dev->of_node, "memory-region", 0);
	if (!np) {
		dev_err(dev, "No %s specified\n", "memory-region");
		return -ENODEV;
	}

	rc = of_address_to_resource(np, 0, &resv_mem);
	if (rc) {
		dev_err(dev, "No memory address assigned to the region\n");
		return -ENODEV;
	}

	p_resmem->dev = dev;
	p_resmem->mem_size = resource_size(&resv_mem);
	p_resmem->mem_base = resv_mem.start;
	p_resmem->reason = devm_ioremap(dev, resv_mem.start, p_resmem->mem_size);

	rc = memblock_reserve(p_resmem->mem_base, p_resmem->mem_size);
	if (rc) {
		dev_err(dev, "Failed to reserve memory\n");
		return -ENOMEM;
	}

	dev_info(dev, "Allocated reserved memory, vaddr: 0x%X, paddr: 0x%X\n",
		p_resmem->reason, p_resmem->mem_base);

	/* Init reboot reason as cold boot when first boot */
	if (reboot_mode == REBOOT_COLD)
		resvmem_set_reboot_reason(RESVMEM_REBOOT_COLD_BOOT);

	/* Init sysfsvmem_get_reboot_reason */
	resvmem_sysfs_init();

	return 0;
}

static int resvmem_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id reserve_mem_match[] = {
	{.compatible = "ubnt,resvmem-reboot-reason"},
	{},
};

static struct platform_driver resvmem_driver = {
	.probe = resvmem_probe,
	.remove = resvmem_remove,
	.driver = {
		.name = "resvmem-reboot-reason",
		.of_match_table = reserve_mem_match,
	},
};

MODULE_DEVICE_TABLE(of, reserve_mem_match);
module_platform_driver(resvmem_driver);
MODULE_DESCRIPTION("UBNT resverve memory driver");
MODULE_AUTHOR("WesleyLin <wesley.lin@ui.com");
MODULE_LICENSE("GPL v2");
