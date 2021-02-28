#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/module.h>
#include <linux/of_address.h>

static void __iomem *pbs_regs;

static struct of_device_id of_pbs_table[] = {
	{.compatible = "annapurna-labs,al-pbs"},
	{ /* end of list */ },
};

void __iomem *alpine_pbs_regs_base_get(void)
{
	return pbs_regs;
}

int __init alpine_pbs_init(void)
{
	struct device_node *pbs_node;

	pr_info("Initializing Peripheral Bus System - PBS\n");

	pbs_node = of_find_matching_node(NULL, of_pbs_table);
	if (!pbs_node) {
		pr_err("pbs entry was not found in device-tree\n");
		return -ENODEV;
	}

	pbs_regs = of_iomap(pbs_node, 0);
	if (!pbs_regs) {
		pr_err("fail to map PBS memory\n");
		return -ENOMEM;
	}

	return 0;
}

arch_initcall(alpine_pbs_init);
