#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/genhd.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/err.h>

static struct work_struct *work1;

static int count;
static void work1_func(struct work_struct *work)
{
	++count;
	printk("%s %d count=%d\n", __FUNCTION__, __LINE__, count);
	msleep(10 * 1000);

	printk("work1=%p, work=%p\n", work1, work);

	if (test_bit(WORK_STRUCT_PENDING_BIT, work_data_bits(work))) {
		printk("pending bit=1\n");
	} else
		printk("pending bit=0\n");
	schedule_work(work1);
}

static int __init schedule_work_init(void)
{
    bool ret;
    if(IS_ERR(NULL))
    	printk("is err pass0\n");


    printk("is err pass1 %ld\n", PTR_ERR(NULL));
    work1=kmalloc(sizeof(struct work_struct),GFP_KERNEL);
    INIT_WORK(work1,work1_func);
    schedule_work(work1);
    return 0;
}
static void __exit schedule_work_exit(void)
{
	pr_info(" removed\n");
}
module_init(schedule_work_init);
module_exit(schedule_work_exit);
MODULE_LICENSE("GPL");
