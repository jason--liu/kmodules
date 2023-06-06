#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/mod_devicetable.h>
#include <linux/pm_runtime.h>
static int stepsz = 0x200;

static int test_maxmallocsz(struct platform_device *pdev)
{
	/* size_t size2alloc = 0; */
	size_t size2alloc = stepsz;
	dma_addr_t handle;
	while (1) {
		//void *p = kmalloc(size2alloc, GFP_KERNEL);
		void *p = dma_alloc_coherent(&pdev->dev, size2alloc, &handle,
					     GFP_KERNEL | GFP_DMA32);
		if (!p) {
			pr_alert("dma alloc fail,size2alloc = %zu\n",
				 size2alloc);
			return -ENOMEM;
		}
		pr_info("kmalloc(%7zu) = 0x%px, phy_addr = 0x%llx\n",
			size2alloc, p, handle);
		/* kfree(p); */
		dma_free_coherent(&pdev->dev, size2alloc, p, handle);
		size2alloc += stepsz;
	}
	return 0;
}
static const struct of_device_id dma_maxsize_match[] = {
	{ .compatible = "dma_maxsize" },
	{}
};

MODULE_DEVICE_TABLE(of, dma_maxsize_match);

static int dma_maxsize_probe(struct platform_device *pdev)
{
    /* int ret; */
	/* ret = dma_set_mask_and_coherent(misc_dev, DMA_BIT_MASK(32)); */
	/* if (ret) { */
	/* 	pr_err("dma_set_mask_and_coherent failed\n"); */
	/* 	return ret; */
	/* } */
	/* return test_maxmallocsz(pdev); */
	pm_runtime_use_autosuspend(&pdev->dev);
	pm_runtime_set_autosuspend_delay(&pdev->dev, 5000);
	pm_runtime_set_active(&pdev->dev);
	pm_runtime_enable(&pdev->dev);

	/* pm_runtime_allow(&pdev->dev); */
	pm_runtime_mark_last_busy(&pdev->dev);
	/* pm_runtime_put_autosuspend(&pdev->dev); */
	return 0;
}

static int dma_maxsize_remove(struct platform_device *pdev)
{
    pm_runtime_disable(&pdev->dev);

    return 0;
}

static int __maybe_unused dma_runtime_suspend(struct device *dev)
{
	/* struct spi_master *master = dev_get_drvdata(dev); */
	/* struct mxic_spi *mxic = spi_master_get_devdata(master); */

	/* mxic_spi_clk_disable(mxic); */
	/* clk_disable_unprepare(mxic->ps_clk); */
    printk("%s %d\n", __func__, __LINE__);

	return 0;
}

static int __maybe_unused dma_runtime_resume(struct device *dev)
{
	/* struct spi_master *master = dev_get_drvdata(dev); */
	/* struct mxic_spi *mxic = spi_master_get_devdata(master); */
	/* int ret; */

	/* ret = clk_prepare_enable(mxic->ps_clk); */
	/* if (ret) { */
	/* 	dev_err(dev, "Cannot enable ps_clock.\n"); */
	/* 	return ret; */
	/* } */
	printk("%s %d\n", __func__, __LINE__);
    return 0;

	/* return mxic_spi_clk_enable(mxic); */
}

static const struct dev_pm_ops dma_dev_pm_ops = { SET_RUNTIME_PM_OPS(
	dma_runtime_suspend, dma_runtime_resume, NULL) };

static struct platform_driver dma_maxsize_driver = {
    .driver = {
        .name="dma_maxsize",
        .of_match_table=dma_maxsize_match,
        .pm = &dma_dev_pm_ops,
   },
    .probe = dma_maxsize_probe,
    .remove = dma_maxsize_remove,
};

static int __init dma_maxsize_init(void)
{
	if (platform_driver_register(&dma_maxsize_driver) == 0)
		platform_device_register_simple("dma_maxsize", -1, NULL, 0);

	return 0;
}

static void __exit dma_maxsize_exit(void)
{
	platform_driver_unregister(&dma_maxsize_driver);
}

module_init(dma_maxsize_init);
module_exit(dma_maxsize_exit);
MODULE_LICENSE("GPL");
