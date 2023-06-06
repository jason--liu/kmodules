#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/uuid.h>
#include <linux/mm.h>

/* https : //gitee.com/solo-king/linux-kernel-base-usage/blob/master/flagstaff/rbtreeTest.c */

struct my_data {
	u8 key;
	struct rb_node node;
};

struct my_map {
	char *name;
	struct rb_root my_root;
};

struct my_map *g_map;
struct my_data rep_data;

static ssize_t inset_node_to_tree(struct rb_root *root, struct my_data *my_data)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;

	while (*new) {
		/* struct my_data*this = container_of(*new, struct my_data, node) */
		struct my_data *this = rb_entry(*new, struct my_data, node);

		parent = *new;

		if (my_data->key < this->key)
			new = &((*new)->rb_left);
		else if (my_data->key > this->key)
			new = &((*new)->rb_right);
		else
			return -1;
	}

	rb_link_node(&my_data->node, parent, new);
	rb_insert_color(&my_data->node, root);

	return 0;
}

static struct my_data *get_key(struct rb_root *root, int key)
{
	struct rb_node *node = root->rb_node;

	while (node) {
		struct my_data *data = rb_entry(node, struct my_data, node);
		if (!data) {
			printk("get data is null\n");
			continue;
		}
		if (key > data->key)
			node = node->rb_right;
		else if (key < data->key)
			node = node->rb_left;
		else
			return data;
	}
	return NULL;
}

static int __init data_init(void)
{
	int count;
	struct my_data *data;

	rep_data.key = 250;

	g_map = kzalloc(sizeof(struct my_map), GFP_KERNEL);
	g_map->name = "test map";
	g_map->my_root = RB_ROOT;

	for (count = 0; count < 100; count++) {
		struct my_data *my_data =
			kvzalloc(sizeof(struct my_data), GFP_KERNEL);
		my_data->key = count;
		inset_node_to_tree(&g_map->my_root, my_data);
	}

	data = get_key(&g_map->my_root, 99);
	if (data) {
        printk("not find the target key, do insert.\n");
        rb_replace_node(&data->node, &rep_data.node, &g_map->my_root);
	}

	/* insert */

	return 0;
}

static void __exit data_exit(void)
{
	struct rb_root *root = &g_map->my_root;
	struct rb_node *node = rb_first(root);
	struct my_data *data;

	while (node) {
		data = rb_entry(node, struct my_data, node);
		printk("data key = %d\n", data->key);
		rb_erase(node, root);
		node = rb_next(node);
		/* kvfree(data); */
	}

	kfree(g_map);
}

MODULE_LICENSE("GPL");
module_init(data_init);
module_exit(data_exit);
