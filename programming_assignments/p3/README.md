This project is implementing a virtual memory system. The 4 major steps of the system are:
split the Address, translate the Address, handle Page Faults, implement Page Replacement Algorithm.
We had to seperate the virtual address to get page and offset values, compute physical address.
Figure out if a page has to be evicted, along with avoiding any thrashing. 
The final step was implementing a clock-sweep algorithm for when pages needed to be evicted, with
the first page that had the "used" boolean value false being evicted. 
