#include <linux/acpi.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/irqdesc.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/platform_device.h>

static struct resource my_pdev_resources[] = { {
	/* irq */
	.start = 2,
	.end = 2,
	.flags = IORESOURCE_IRQ,
} };

static struct platform_device my_pdev = { .name = "my_pdev",
					  .id = PLATFORM_DEVID_AUTO,
					  .resource = my_pdev_resources,
					  .num_resources = ARRAY_SIZE(
						  my_pdev_resources) };

static irqreturn_t my_pdev_handle_irq(int irq, void *dev_id)
{
	struct platform_device *pdev = dev_id;

	dev_info(&pdev->dev, "%s called\n", __func__);

	return IRQ_HANDLED;
}

static int my_pdev_probe(struct platform_device *pdev)
{
	int ret, irq;

	dev_info(&pdev->dev, "%s called\n", __func__);
	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	ret = devm_request_irq(&pdev->dev, irq, my_pdev_handle_irq, 0,
			       pdev->name, pdev);
	if (ret != 0) {
		ret = -ENXIO;
		dev_err(&pdev->dev, "request_irq failed\n");
		return ret;
	}

	local_irq_disable();
	generic_handle_irq(irq);
	local_irq_enable();

	return 0;
}

static int my_pdev_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver my_pdev_driver = {
    .driver = {
        .name="my_pdev",
        /* .of_match_table=dma_maxsize_match, */
        /* .pm = &dma_dev_pm_ops, */
   },
    .probe = my_pdev_probe,
    .remove = my_pdev_remove,
};

static int __init my_pdev_init(void)
{
	if (platform_driver_register(&my_pdev_driver) == 0)
		platform_device_register(&my_pdev);

	return 0;
}

static void __exit my_pdev_exit(void)
{
	platform_device_unregister(&my_pdev);
	platform_driver_unregister(&my_pdev_driver);
}

module_init(my_pdev_init);
module_exit(my_pdev_exit);
MODULE_LICENSE("GPL");
