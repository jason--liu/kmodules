#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/delay.h>

static struct task_struct *kthread_task;

static int kthread_task_fn(void *dummy)
{
	while (1) {
		/* printk("%s %d\n", __func__, __LINE__); */
		udelay(5);
        cond_resched();
	}
	return 0;
}

static int __init kthread_test_init(void)
{
	kthread_task = kthread_run(kthread_task_fn, NULL, "kthread_test");
	/* if (!IS_ERR(kthread_task)) */
	/* 	return -ENOMEM; */
	return 0;
}
static void __exit kthread_test_exit(void)
{
	return;
}
module_init(kthread_test_init);
module_exit(kthread_test_exit);
MODULE_LICENSE("GPL");
