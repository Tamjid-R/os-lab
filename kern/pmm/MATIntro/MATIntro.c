#include <lib/gcc.h>
#include <lib/spinlock.h>

static spinlock_t mem_lk;

// Number of physical pages that are actually available in the machine.
static unsigned int NUM_PAGES;

/**
 * Structure representing information for one physical page.
 */
struct ATStruct {
    /**
     * The permission of the page.
     * 0: Reserved by the BIOS.
     * 1: Kernel only.
     * >1: Normal (available).
     */
    unsigned int perm;
    /**
     * Whether the page is allocated.
     * 0: unallocated
     * >0: allocated
     */
    unsigned int allocated;
};

/**
 * A 32 bit machine may have up to 4GB of memory.
 * So it may have up to 2^20 physical pages,
 * with the page size being 4KB.
 */
static struct ATStruct AT[1 << 20];

void mem_spinlock_init(void) {
    spinlock_init(&mem_lk);
}

void mem_lock(void) {
    spinlock_acquire(&mem_lk);
}

void mem_unlock(void) {
    spinlock_release(&mem_lk);
}

// The getter function for NUM_PAGES.
unsigned int get_nps(void)
{
    return NUM_PAGES;
}

// The setter function for NUM_PAGES.
void set_nps(unsigned int nps)
{
    NUM_PAGES = nps;
}

//if page with given index is normal return 1 else return 0
unsigned int at_is_norm(unsigned int page_index)
{
    if(AT[page_index].perm>1){
        return 1;
    }
    else{
        return 0;
    }
}

//sets permission of a given index and marks it as unallocated
void at_set_perm(unsigned int page_index, unsigned int perm)
{
    AT[page_index].perm = perm;
    AT[page_index].allocated = 0;
}

// getter for a AT flag that indicates whether a page is allocated or not
unsigned int at_is_allocated(unsigned int page_index)
{
    // TODO [Lab 1]

    if(AT[page_index].allocated == 0){
        return 0;
    }
    else if(AT[page_index].allocated > 0)
    {
        return 1;
    }
}

//sets the flag of a given index in the AT to indicate whether a page is allocated or not
void at_set_allocated(unsigned int page_index, unsigned int allocated)
{
    // TODO [Lab 1]
    AT[page_index].allocated = allocated;
}