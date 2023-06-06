#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/sched.h>

#define OURMODNAME "poison_test"
#define OURCACHENAME "poison_test"

struct myctx {
	u32 iarr[10];
	u64 uarr[6];
	char config[64];
};

static struct kmem_cache *gctx_cachep;
struct myctx *obj;

static void use_the_object(void *s, u8 c, size_t n)
{
	void *vrx;
	memset(s, c, n);
	pr_info(" --------------after memset s, '%c', %zu : -------------------\n",
		c, n);
	print_hex_dump_bytes("obj: ", DUMP_PREFIX_ADDRESS, s,
			     sizeof(struct myctx));

	/* vrx = __vmalloc(PAGE_SIZE, GFP_KERNEL, PAGE_KERNEL_RO); */
	/* *(u64 *)(vrx + 4) = 0x55; */
}

static void use_our_cache(void)
{
	obj = kmem_cache_alloc(gctx_cachep, GFP_KERNEL);
	if (!obj)
		pr_warn("kmem_cache_alloc() failed\n");

	pr_info("Our cache object (@ 0x%pK, actual=0x%px) size is %u bytes; ksize=%zu\n",
		obj, obj, kmem_cache_size(gctx_cachep), ksize(obj));
	use_the_object(obj, 'z', 16);
}

static void our_ctor(void *new)
{
	struct myctx *ctx = new;
	struct task_struct *p = current;

	/* dump_stack(); */

	pr_info("in ctor: just alloced mem object is @ 0x%px\n", ctx);
	memset(ctx, 0, sizeof(struct myctx));

	snprintf(ctx->config, 6 * sizeof(u64) + 5, "%d.%d,%ld,%ld,%ld,%ld",
		 p->tgid, p->pid, p->nvcsw, p->nivcsw, p->min_flt, p->maj_flt);
}

static int create_our_cache(void)
{
	int ret = 0;
	void *ctor_fn = NULL;

	ctor_fn = our_ctor;

	gctx_cachep = kmem_cache_create(
		OURCACHENAME, sizeof(struct myctx), sizeof(long),
		SLAB_POISON | SLAB_RED_ZONE | SLAB_HWCACHE_ALIGN, ctor_fn);
	if (!gctx_cachep) {
		if (IS_ERR(gctx_cachep))
			ret = PTR_ERR(gctx_cachep);
	}

	return ret;
}

static int __init slab_custom_init(void)
{
	pr_info("inserted\n");
	create_our_cache();
	use_our_cache();

	return 0;
}

static void __exit slab_custom_exit(void)
{
	kmem_cache_free(gctx_cachep, obj);
 use_the_object(obj, '!', 10); /* trigger UAF BUG */

	kmem_cache_destroy(gctx_cachep);
	pr_info("custom cache destroyed; removed\n");
}

MODULE_LICENSE("GPL");
module_init(slab_custom_init);
module_exit(slab_custom_exit);
