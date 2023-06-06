#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mm.h>

#define OURMODNAME "lowlevel_mem"

static const void *gptr1, *gptr2, *gptr3, *gptr4, *gptr5;
static int bsa_alloc_order = 3;

static void show_phy_pages(const void *kaddr, size_t len, bool contiguity_check)
{
	void *vaddr = kaddr;
	const char *hdr =
		"-pg#-  -------va-------     --------pa--------   --PFN--\n";
	phys_addr_t pa;
	int loops = len / PAGE_SIZE, i;
	long pfn, prev_pfn = 1;

#ifdef CONFIG_X86
	if (!virt_addr_valid(vaddr)) {
		pr_info("%s(): invalid virtual address (0x%px)\n", __func__,
			vaddr);
		return;
	}
	/* Worry not, the ARM implementation of virt_to_phys() performs an
     * internal validity check
     */
#endif
	pr_info("%s(): start kaddr %px, len %zu, contiguity_check is %s\n",
		__func__, vaddr, len, contiguity_check ? "on" : "off");
	pr_info("%s", hdr);
	if (len % PAGE_SIZE)
		loops++;

	for (i = 0; i < loops; i++) {
		pa = virt_to_phys(vaddr + (i * PAGE_SIZE));
		pfn = PHYS_PFN(pa);

		if (!!contiguity_check) {
			if (i && pfn != prev_pfn + 1) {
				pr_notice(
					" *** physical NON-contiguity detected (i=%d) ***\n",
					i);
				break;
			}
		}
		pr_info("%05d    0x%px    %pa    %ld\n", i,
			vaddr + (i * PAGE_SIZE), &pa, pfn);
		if (!!contiguity_check)
			prev_pfn = pfn;
	}
}

static int bas_alloc(void)
{
	int stat = -ENOMEM;
	u64 numpg2alloc = 0;
	const struct page *pg_ptr1;

	pr_info("%s: 0. Show identity mapping: RAM page frames : kernel virtual pages :: 1:1\n",
		OURMODNAME);

	show_phy_pages((void *)PAGE_OFFSET, 5 * PAGE_SIZE, 1);
	/* 1. Allocate one page */
	gptr1 = (void *)__get_free_page(GFP_KERNEL);
	if (!gptr1)
		goto out1;
	pr_info("%s: 1. __get_free_page() alloc'ed 1 page from the BSA @ %pK (%px)\n",
		OURMODNAME, gptr1, gptr1);

	/* 2. Allocate 2^order pages with the __get_free_pages() */
	gptr2 = (void *)__get_free_pages(GFP_KERNEL | __GFP_ZERO, 8);
	if (!gptr2)
		goto out2;
	pr_info("%s: 2. __get_free_pages() alloc'ed 8 pages from the BSA @ %pK (%px)\n",
		OURMODNAME, gptr2, gptr2);
	show_phy_pages(gptr2, 8 * PAGE_SIZE, 1);

	/* 3. Allocate and init one page with the get_zeroed_page() */
	gptr3 = (void *)get_zeroed_page(GFP_KERNEL);
	if (!gptr3)
		goto out3;
	pr_info("%s: 3. get_zeroed_page() alloc'ed 1 page from the BSA @ %pK (%px)\n",
		OURMODNAME, gptr3, gptr3);

	/* 4. Allocate one page with alloc_page() */
	pg_ptr1 = alloc_page(GFP_KERNEL);
	if (!pg_ptr1)
		goto out4;
	gptr4 = page_address(pg_ptr1);
	pr_info("%s: 4. alloc_page() alloc'ed 1 page from the BSA @ %pK (%px)\n"
		" (struct page addr=%pK (%px))\n",
		OURMODNAME, (void *)gptr4, (void *)gptr4, pg_ptr1, pg_ptr1);

	/* 5. Allocate and init 2^5 = 32 pages with the alloc_pages() */
	gptr5 = page_address(alloc_pages(GFP_KERNEL, 5));
	if (!gptr5)
		goto out5;
	pr_info("%s: 5. alloc_pages() alloc'ed %d pages from the BSA @ %pK (%px)\n",
		OURMODNAME, 32, gptr5, gptr5);
	return 0;
out5:
	free_page((unsigned long)gptr4);
out4:
	free_page((unsigned long)gptr3);
out3:
	free_pages((unsigned long)gptr2, 8);
out2:
	free_page((unsigned long)gptr1);
out1:
	return stat;
}

static int lowlevel_mem_init(void)
{
	return bas_alloc();
}

static void  lowlevel_mem_exit(void)
{
	pr_info("%s: free-ing up the BSA memory chunks...\n", OURMODNAME);
	free_pages((unsigned long)gptr5, 5);
	free_page((unsigned long)gptr4);
	free_page((unsigned long)gptr3);
	free_pages((unsigned long)gptr2, 8);
	free_page((unsigned long)gptr1);
	pr_info("%s: removed\n", OURMODNAME);
}

module_init(lowlevel_mem_init);
module_exit(lowlevel_mem_exit);
MODULE_LICENSE("GPL");
