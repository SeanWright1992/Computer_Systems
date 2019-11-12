#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "swapfile.h"
#include "process.h"
#include "global.h"
pfn_t get_free_frame(void);
#include<iostream>
using namespace std;
/*******************************************************************************
 * Page fault handler. When the CPU encounters an invalid address mapping in a
 * process' page table, it invokes the CPU via this handler. The OS then
 * allocates a physical frame for the requested page (either by using a free
 * frame or evicting one), changes the process' page table to reflect the
 * mapping and then restarts the interrupted process.
 *
 * @param vpn The virtual page number requested.
 * @param write If the CPU is writing to the page, this is 1. Otherwise, it's 0.
 * @return The physical frame the OS has mapped to the virtual page.
 */

pfn_t pagefault_handler(vpn_t request_vpn, int write) {
  pfn_t victim_pfn;
  vpn_t victim_vpn;
  pcb_t *victim_pcb;

  /* Sanity Check */
  assert(ptbr != NULL); /* do NOT modify */

  /* Add 1 to the Page Fault count */
  count_pagefaults++; /* do NOT modify */
  
  /* Find a free or victim frame */
  victim_pfn = get_free_frame();
  assert(victim_pfn < CPU_NUM_FRAMES); /* make sure the victim_pfn is valid */

  /* Use the Frame Table (reverse lookup table) to find the victim. */
  victim_vpn = frametable[victim_pfn].vpn;
  victim_pcb = frametable[victim_pfn].pcb; /* NULL if the frame was free */

  printf("****PAGE FAULT has occurred at VPN %u. Evicting (PFN %u VPN %u) as victim.\n",
         request_vpn, victim_pfn, victim_vpn); /* do NOT modify this output statement */
   
  if(victim_pcb !=NULL){
	ptbr[victim_vpn].valid=0;  
	if(ptbr[victim_vpn].dirty == 1)
		save_page_to_disk(victim_pfn,victim_vpn, victim_pcb->pid);
	
	ptbr[request_vpn].pfn = victim_pfn;
	ptbr[request_vpn].valid = 1;
	ptbr[request_vpn].used = 0;
	frametable[victim_pfn].pcb = current;
	frametable[victim_pfn].vpn = victim_vpn;
	
  }
  
  /*
   * Retreive the page from disk. Note that is really a lie: we save pages in
   * memory (since doing file I/O for this simulation would be annoying and
   * wouldn't add that much to the learning). Also, if the page technically
   * does't exist yet (i.e., the page has never been accessed yet, we return a
   * blank page. Real systems would check for invalid pages and possibly read
   * stuff like code pages from disk. For purposes of this simulation, we won't
   * worry about that. =)
   */
   
   
   
  
  load_page_from_disk(victim_pfn, request_vpn, current->pid);

  return victim_pfn;
}


/*******************************************************************************
 * Finds a free physical frame. If none are available, uses a clock sweep
 * algorithm to find a used frame for eviction.
 *
 * @return The physical frame number of a free (or evictable) frame.
 */

pfn_t get_free_frame(void) {
   unsigned int i;

   /* See if there are any free frames */
   /* do NOT modify this for loop */
   for (i = 0; i < CPU_NUM_FRAMES; i++)
      if (frametable[i].pcb == NULL)
         return i;
	  
	
	unsigned int j =0; 
	while(true){
			cout<<frametable[j].pcb->pagetable->used<<"value of j:"<<j<<endl;
		if(frametable[j].pcb->pagetable->used ==0){
			frametable[j].pcb->pagetable->used =1;
			return j;
		}
		else
			frametable[j].pcb->pagetable->used =0;
		j= (j+1)%CPU_NUM_FRAMES;
	}

   /* If all else fails, return a random frame */
   return rand() % CPU_NUM_FRAMES;
}
