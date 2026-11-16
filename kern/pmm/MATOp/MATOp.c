#include <lib/debug.h>
#include <lib/types.h>
#include "import.h"

#define PAGESIZE     4096
#define VM_USERLO    0x40000000
#define VM_USERHI    0xF0000000
#define VM_USERLO_PI (VM_USERLO / PAGESIZE)
#define VM_USERHI_PI (VM_USERHI / PAGESIZE)

//VM_USERLO = starting memory address VM_USERLO_PI = starting page index
//VM_USERHI = ending memory address VM_USERHI_PI = ending page index

//memoization starts from the starting memory index(VM_USERLO_PI)
unsigned int palloc()
{
    unsigned int palloc_free_index = 0; //for storing allocated page index

    // Remember where the previous search stopped.
    static unsigned int last_index = VM_USERLO_PI;

    unsigned int start_index;
    unsigned int i;

    mem_lock();

    start_index = last_index;
    i = start_index;

    do
    {
        // Check whether this page is normal and unallocated.
        if (at_is_norm(i) == 1 &&
            at_is_allocated(i) == 0)
        {
            // Mark the page as allocated if it is.
            at_set_allocated(i, 1);

            palloc_free_index = i;

            // Start the next search from the following next page.
            last_index = i + 1;

            if (last_index >= VM_USERHI_PI){ //if we didnt reach the end 
                last_index = VM_USERLO_PI;
            }

            break;
        }

        // Move to the next page.
        i++;

        // Wrap around to the beginning of the user region.
        if (i >= VM_USERHI_PI)
        {
            i = VM_USERLO_PI;
        }

    } while (i != start_index);

    mem_unlock();

    return palloc_free_index;
}

//free a page and mark it as unallocated in the allocation table AT 
void pfree(unsigned int pfree_index)
{
    mem_lock();

    at_set_allocated(pfree_index, 0);

    mem_unlock();
}
