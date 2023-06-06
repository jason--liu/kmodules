#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

#define OURMODNAME "dma_maxsize"
/* static int stepsz = 200000; */
static int stepsz = 4096;

static struct device *misc_dev;

static int misc_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t misc_read(struct file *filep, char __user *ubuf, size_t count,
			 loff_t *off)
{
	return 0;
}

static int misc_release(struct inode *inode, struct file *filep)
{
    return 0;
}

static ssize_t misc_write(struct file *filep, const char __user *ubuf,
			  size_t count, loff_t *off)
{
	return 0;
}

static const struct file_operations misc_ops = {
	.open = misc_open,
	.read = misc_read,
    .write = misc_write,
	.release = misc_release,
};

static struct miscdevice misc_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "misc_dev",
	.mode = 0666,
	.fops = &misc_ops,
};

static int test_maxmallocsz(void)
{
	/* size_t size2alloc = 0; */
	size_t size2alloc = stepsz;
	dma_addr_t handle;
	while (1) {
		//void *p = kmalloc(size2alloc, GFP_KERNEL);
		void *p = dma_alloc_coherent(misc_dev, size2alloc, &handle,
					     GFP_KERNEL | GFP_DMA32);
		if (!p) {
			pr_alert("dma alloc fail,size2alloc = %zu\n", size2alloc);
			return -ENOMEM;
		}
		pr_info("kmalloc(%7zu) = 0x%px, phy_addr = 0x%px\n", size2alloc, p, handle);
		/* kfree(p); */
		dma_free_coherent(misc_dev, size2alloc, p, handle);
		size2alloc += stepsz;
	}
	return 0;
}

static int __init dma_maxsize_init(void)
{
	int ret;

	pr_info("%s: inserted\n", OURMODNAME);
	ret = misc_register(&misc_miscdev);
	if (ret < 0) {
		pr_notice("misc device registration failed, aborting\n");
		return ret;
	}

	pr_info("misc device register sussess.\n");
	misc_dev = misc_miscdev.this_device;
	misc_dev->dma_mask = kmalloc(sizeof(misc_dev->dma_mask), GFP_KERNEL);

	ret = dma_set_mask_and_coherent(misc_dev, DMA_BIT_MASK(32));
	if (ret)
		pr_err("dma_set_mask_and_coherent failed\n");

	return test_maxmallocsz();
}

static void __exit dma_maxsize_exit(void)
{
	pr_info("%s: removed\n", OURMODNAME);
}

module_init(dma_maxsize_init);
module_exit(dma_maxsize_exit);
MODULE_LICENSE("GPL");
