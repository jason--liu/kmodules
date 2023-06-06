#include <linux/init.h>
#include <linux/module.h>
#include <linux/mutex.h>

static int __init lock_test_init(void)
{
	struct mutex lock;
	mutex_init(&lock);

	mutex_lock(&lock);
	printk("%s do something\n", __FUNCTION__);
	mutex_unlock(&lock);

	/* BUG_ON(!mutex_is_locked(&lock)); */
	if (mutex_is_locked(&lock))
		mutex_unlock(&lock);

	return 0;
}

static void __exit lock_test_exit(void)
{
    return ;
}

module_init(lock_test_init);
module_exit(lock_test_exit);
MODULE_LICENSE("GPL");
