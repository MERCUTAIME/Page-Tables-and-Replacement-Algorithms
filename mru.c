/*
 * This code is provided solely for the personal and private use of students
 * taking the CSC369H course at the University of Toronto. Copying for purposes
 * other than this use is expressly prohibited. All forms of distribution of
 * this code, including but not limited to public repositories on GitHub,
 * GitLab, Bitbucket, or any other online platform, whether as given or with
 * any changes, are expressly prohibited.
 *
 * Authors: Andrew Peterson, Karen Reid, Alexey Khrabrov
 *
 * All of the files in this directory and all subdirectories are:
 * Copyright (c) 2019, 2021 Karen Reid
 */

#include "pagetable.h"
#include "sim.h"

static struct frame *front;
static struct frame *end;
extern struct frame *coremap;
extern size_t memsize;

/* Page to evict is chosen using the accurate MRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int mru_evict(void)
{
	return ((front->pte->frame) >> PAGE_SHIFT);
}

void mru_update_frame(struct frame *f1, struct frame *f2, struct frame *target1, struct frame *target2)
{
	f1->new_frame = target1;
	f2->last_frame = target2;
}

/* This function is called on each access to a page to update any information
 * needed by the MRU algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void mru_ref(pt_entry_t *pte)
{
	int index = (pte->frame >> PAGE_SHIFT);
	struct frame *f_ptr = &(coremap[index]);
	//Check if the frame linked list is empty
	if (!front)
	{
		front = f_ptr;
		end = f_ptr;
		mru_update_frame(front, front, f_ptr, f_ptr);
	}
	else
	{
		//Check which frame pte is in
		//
		if (end == f_ptr)
		{
			end = (*f_ptr).last_frame;
			front = f_ptr;
		}
		else if (front != f_ptr)
		{ // Update all frame only if pte is in
			if ((*f_ptr).new_frame)
			{
				mru_update_frame((*f_ptr).last_frame, (*f_ptr).new_frame, (*f_ptr).new_frame, (*f_ptr).last_frame);
			}

			(*f_ptr).last_frame = end;
			(*f_ptr).new_frame = front;
			mru_update_frame(end, front, f_ptr, f_ptr);
			front = f_ptr;
		}
	}
}

/* Initialize any data structures needed for this replacement algorithm. */
void mru_init(void)
{
	//clear all in coremap
	mru_cleanup();
	front = NULL;
	end = NULL;
}

/* Cleanup any data structures created in mru_init(). */
void mru_cleanup(void)
{
	for (int i = 0; i < (int)memsize; ++i)
	{
		coremap[i].last_frame = NULL;
		coremap[i].new_frame = NULL;
	}
}
