#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <asm/fixmap.h>
#include <asm/pgtable.h>

#define OURMODNAME "show_kernel_seg"
#define ELLPS  "|                           [ . . . ]                         |\n"
#define BITS_PER_LONG 64

/* SHOW_DELTA_*(low, hi) :
 * Show the low val, high val and the delta (hi-low) in either bytes/KB/MB/GB,
 * as required.
 * Inspired from raspberry pi kernel src: arch/arm/mm/init.c:MLM()
 */
#define SHOW_DELTA_b(low, hi) (low), (hi), ((hi) - (low))
#define SHOW_DELTA_K(low, hi) (low), (hi), (((hi) - (low)) >> 10)
#define SHOW_DELTA_M(low, hi) (low), (hi), (((hi) - (low)) >> 20)
#define SHOW_DELTA_G(low, hi) (low), (hi), (((hi) - (low)) >> 30)
#define SHOW_DELTA_MG(low, hi)                                                 \
	(low), (hi), (((hi) - (low)) >> 20), (((hi) - (low)) >> 30)

#define PRINT_CTX() do {                                                      \
	int PRINTCTX_SHOWHDR = 0;                                                 \
	char intr = '.';                                                          \
	if (!in_task()) {                                                         \
		if (in_irq() && in_softirq())                                         \
			intr = 'H'; /* hardirq occurred inside a softirq */               \
		else if (in_irq())                                                    \
			intr = 'h'; /* hardirq is running */                              \
		else if (in_softirq())                                                \
			intr = 's';                                                       \
	}                                                                         \
	else                                                                      \
		intr = '.';                                                           \
										                                      \
	if (PRINTCTX_SHOWHDR == 1)                                                \
		pr_debug("CPU)  task_name:PID  | irqs,need-resched,hard/softirq,preempt-depth  /* func_name() */\n"); \
	pr_debug(                                                                    \
	"%03d) %c%s%c:%d   |  "                                                      \
	"%c%c%c%u   "                                                                \
	"/* %s() */\n"                                                               \
	, raw_smp_processor_id(),                                                    \
	(!current->mm?'[':' '), current->comm, (!current->mm?']':' '), current->pid, \
	(irqs_disabled()?'d':'.'),                                                   \
	(need_resched()?'N':'.'),                                                    \
	intr,                                                                        \
	(preempt_count() && 0xff),                                                   \
	__func__                                                                     \
	);                                                                           \
} while (0)

static void show_userspace_info(void)
{
	pr_info("+--------Above is kernel-seg;below, user VAS-----------+\n"
            ELLPS
		"|Process environment "
		" %px - %px     | [%4zd bytes]\n"
		"|          arguments "
		" %px - %px     | [%4zd bytes]\n"
		"|        stack start  %px\n"
		"|       heap segment "
		" %px - %px     | [%4zd KB]\n"
		"|static data segment "
		" %px - %px     | [%4zd bytes]\n"
		"|       text segment "
		" %px - %px     | [ %4zd KB]\n"
            ELLPS
		"+------------------------------------------------------+\n",
		SHOW_DELTA_b(current->mm->env_start, current->mm->env_end),
		SHOW_DELTA_b(current->mm->arg_start, current->mm->arg_end),
		current->mm->start_stack,
		SHOW_DELTA_K(current->mm->start_brk, current->mm->brk),
		SHOW_DELTA_b(current->mm->start_data, current->mm->end_data),
		SHOW_DELTA_K(current->mm->start_code, current->mm->end_code)
	);

	pr_info(
#if (BITS_PER_LONG == 64)
		"Above: TASK_SIZE         = %llu size of userland  [  %ld GB]\n"
#else // 32-bit
		"Above: TASK_SIZE         = %lu size of userland  [  %ld MB]\n"
#endif
		" # userspace memory regions (VMAs) = %d\n"
		" Above statistics are wrt 'current' thread (see below):\n",
#if (BITS_PER_LONG == 64)
		TASK_SIZE, (TASK_SIZE >> 30),
#else // 32-bit
		TASK_SIZE, (TASK_SIZE >> 20),
#endif
		current->mm->map_count);

	PRINT_CTX(); /* show which process is the one in context */
}

static void show_kernelseg_info(void)
{
	pr_info("\nSome Kernel Details [by decreasing address]\n"
		"+-------------------------------------------------------------+\n");
	pr_info(ELLPS "|fixmap region:        "
		      " %px -%px        | [%4ld MB]\n",
		SHOW_DELTA_M(FIXADDR_START, (FIXADDR_START + FIXADDR_SIZE)));
	pr_info("|module region:       "
		" %px - %px       | [%ld MB]\n",
		SHOW_DELTA_M(MODULES_VADDR, MODULES_END));
	pr_info("|module region:       "
		" %px - %px       | [%ld MB]\n",
		SHOW_DELTA_M(MODULES_VADDR, MODULES_END));

	pr_info("|vmalloc region:       "
		" %px - %px       | [%4ld MB = %2ld GB]\n",
		SHOW_DELTA_MG(VMALLOC_START, VMALLOC_END));

	pr_info("|lowmem region:       "
		" %px -%px    | [%4ld MB = %2ld GB]\n"
		"|             (above:PAGE_OFFSET    -    highmem)       |",
		SHOW_DELTA_MG((unsigned long)PAGE_OFFSET,
			      (unsigned long)high_memory));

#ifdef CONFIG_HIGHMEM
	pr_info("|HIGHMEM region:    "
		" %px - %px | [%4ld MB]\n",
		SHOW_DELTA_M(PKMAP_BASE,
			     (PKMAP_BASE) + (LAST_PKMAP * PAGE_SIZE)));
#endif

	pr_info(ELLPS);
}

static int __init kernel_seg_init(void)
{
	pr_info("%s: inserted\n", OURMODNAME);
	show_userspace_info();
	show_kernelseg_info();
	return 0;
}

static void __exit kernel_seg_exit(void)
{
	pr_info("%s: removed\n", OURMODNAME);
}

module_init(kernel_seg_init);
module_exit(kernel_seg_exit);
MODULE_LICENSE("GPL");
