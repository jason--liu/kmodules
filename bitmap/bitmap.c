#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/bitmap.h>
#include <linux/delay.h>
#include <linux/uuid.h>

#define BITMAP_SIZE 32

unsigned long my_bitmap[2];

/* DECLARE_BITMAP(my_bitmap, BITMAP_SIZE); */

static int __init bitmap_init(void)
{
	char uuid[16];
	generate_random_uuid(uuid);
    printk("%pU\n",uuid);
	return 0;
	int start_bit = 0;
	int next_unset_bit;
	int ret;

	/* bitmap_set(my_bitmap, 0, BITMAP_SIZE); */
	/* bitmap_fill(my_bitmap, BITMAP_SIZE); */
	/* bitmap_clear(my_bitmap, 0, BITMAP_SIZE); */
	/* bitmap_zero(my_bitmap, BITMAP_SIZE); */
	/* bitmap_set(&my_bitmap[1], 0, BITMAP_SIZE); */

#if 0
	clear_bit(1, my_bitmap);
	clear_bit(3, my_bitmap);
	clear_bit(4, my_bitmap);
	clear_bit(7, my_bitmap);
	clear_bit(8, my_bitmap);
	clear_bit(20, my_bitmap);
	clear_bit(22, my_bitmap);
	/* clear_bit(64, my_bitmap); */

	clear_bit(11, my_bitmap);
	clear_bit(23, my_bitmap);
	clear_bit(34, my_bitmap);
	clear_bit(17, my_bitmap);
	clear_bit(18, my_bitmap);
	clear_bit(20, my_bitmap);
	/* clear_bit(32, my_bitmap); */
    set_bit(44, &my_bitmap[1]);
#endif
	set_bit(0, &my_bitmap[1]);
	set_bit(1, &my_bitmap[1]);
	set_bit(2, &my_bitmap[1]);
	set_bit(3, &my_bitmap[1]);

	ret = test_bit(1, &my_bitmap[1]);
	printk("ret = %d\n", ret);
	ret = test_bit(2, &my_bitmap[1]);
	printk("ret = %d\n", ret);
	ret = test_bit(3, &my_bitmap[1]);
	printk("ret = %d\n", ret);

	printk("===========\n");
	bitmap_zero(&my_bitmap[1], 4);
	ret = test_bit(1, &my_bitmap[1]);
	printk("ret = %d\n", ret);
	ret = test_bit(2, &my_bitmap[1]);
	printk("ret = %d\n", ret);
	ret = test_bit(3, &my_bitmap[1]);
	printk("ret = %d\n", ret);
	next_unset_bit = find_first_zero_bit(my_bitmap, BITMAP_SIZE);
	/* ret = test_bit(44, my_bitmap); */
	return 0;
	while (next_unset_bit < 64) {
		msleep(1000);
		printk("Unset bit at index %d\n", next_unset_bit);

		start_bit = next_unset_bit + 1;
		next_unset_bit =
			find_next_zero_bit(my_bitmap, BITMAP_SIZE, start_bit);
	}

	return 0;
}

static void __exit bitmap_exit(void)
{
}

MODULE_LICENSE("GPL");
module_init(bitmap_init);
module_exit(bitmap_exit);
