
#include <lib/debug.h>
#include <lib/types.h>
#include "import.h"

#define PAGESIZE     4096
#define VM_USERLO    0x40000000
#define VM_USERHI    0xF0000000
#define VM_USERLO_PI (VM_USERLO / PAGESIZE)
#define VM_USERHI_PI (VM_USERHI / PAGESIZE)

//VM_USERLO = starting memory address VM_USERLO_PI = starting page index
//VM_USERHI = ending memory address VM_USERHI_PI = ending page indexl

/**
 * The initialization function for the allocation table AT.
 * It contains two major parts:
 * 1. Calculate the actual physical memory of the machine, and sets the number
 *    of physical pages (NUM_PAGES).
 * 2. Initializes the physical allocation table (AT) implemented in the MATIntro layer
 *    based on the information available in the physical memory map table.
 *    Review import.h in the current directory for the list of available
 *    getter and setter functions.
 */
void pmem_init(unsigned int mbi_addr)
{
    unsigned int nps;
    
    unsigned int r;
    unsigned int max_end = 0;

    devinit(mbi_addr);
    mem_spinlock_init();


    //calculating total number of physical pages
    
    // Find the highest ending address in the memory map.
    for (r = 0; r < get_size(); r++)
    {
        unsigned int end = get_mms(r) + get_mml(r);

        if (end > max_end)
        {
            max_end = end;
        }
    }

    // Convert the highest address into a number of pages.
    nps = max_end / PAGESIZE;

    set_nps(nps);

    //AT (physical allocation table) initialization

    unsigned int i;

    for (i = 0; i < nps; i++)
    {
        // Pages reserved for the kernel.
        if (i < VM_USERLO_PI || i >= VM_USERHI_PI)
        {
            at_set_perm(i, 1);
        }
        else //pages putside user region are reserved for the kernel
        {
            unsigned int found = 0;

            unsigned int page_start = i * PAGESIZE;
            unsigned int page_end = (i + 1) * PAGESIZE;

            // checks whether page fits inside a usable bios range
            // if it does, mark it as normal in the allocation table AT.
            for (r = 0; r < get_size(); r++)
            {
                unsigned int range_start = get_mms(r);
                unsigned int range_end = range_start + get_mml(r);

                // The entire page must be inside a usable range.
                if (is_usable(r) == 1 && range_start <= page_start && page_end <= range_end)
                {
                    found = 1;
                    break;
                }
            }

            if (found == 1)
            {
                at_set_perm(i, 2); //if entire page is uasable mark its permission as 2. 
            }
            else
            {
                at_set_perm(i, 0);
            }
        }
    }
}

