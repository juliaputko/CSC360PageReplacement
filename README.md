### JULIA PUTKO
### V00889506
## CSC360 Assignment 4 
* Program starts: 
### Page Hit 
* if position i in the page table already has the page to be inserted, set the victim frame to be i, set it's reference bit to 1 (for use in second chance alg), update it's time of use value to the clock value (which is equal to the number of memory references) 
* check if the memwrite value is true, indicating that is it is W, and set the dirty bit to true if it is true. 
### Page Miss
* page fault value increments
#### Free Spot exists
* there is a free spot in the page table, so we set that location as the victim frame and put the page there. 
* set that position's reference bit to 1, and update it's time of use value to the clock value (equal to number of memory references)
* set dirty bit of that location to true if memwrite is true.
* we then incrememnt swap ins 
#### Replacement Algorithm 
##### FIFO 
* set dirty bit to true if memwrite is true on page being swapped in 
* call FIFO function to find the frame  
* to find frame to replace using FIFO, the alg take the number of page faults and substracts 1 (because the page fault number is incremented before the replacement, and we only want the number of pagefaults before this replacement occures) and does mod the size of memory, which does affect the number if the number is within the size of memory, but loops the number back around if it exceeds the size of memory. We use the number of page faults, becuse it gives us the number of Page misses that occured, and therefore where in the FIFO queue we are. The resulting frame is where we do the page replacement. 
* We then check if the page being swapped out has it's dirty bit set to TRUE, and if it does, we increment swap outs, and reset it's vlaue to 0 (FALSE)
* then we increment swap ins

##### LRU 
* set dirty bit to true if memwrite is true on page being swapped in
* call LRU function to find the frame to replace 
* to find the frame we set least to equal the number of memory references, because the memory references at this point will be the max number of memory references, and we want to guarentee everything will be smaller than it 
* we then look through the locations in out page table, checking if any of the times of use on any of the locations is smaller than our least value
	* if it is smaller, we set that time of use value to least, and continue checking through all the values  
	* we set the frame location to be the location i that has the smallest time of use number 
* at the end of this loop, we will have checked through all the frames and the victim frame location will the the locaton with the smallest time of use value, therefore this value will be out least recently used value. 
* we do the replacement with this victim frame
* we update the victim frame's time of use value
* We then check if the page being swapped out has it's dirty bit set to TRU
E, and if it does, we increment swap outs, and reset it's vlaue to 0 (FALSE)
* then we increment the number of swap ins 
##### SECONDCHANCE  
*  set dirty bit to true if memwrite is true on page being swapped in
*  call SECOND CHANCE function to find the frame to replace using the second chance alg. 
* we create a variable called found which we set to one once we have found our victim frame, and informs our program to exit a loop 
* we set the frame # we start with the same as what we would set it using a FIFO algorithm, so we use the page faults minus one mod the size of memory again.
* then, because this is a second change algorithm, we check the reference bit of the frame we are looking at, and only choose it as our victim frame if the reference bit it set to 0
	* if the reference bit it 0, we do the replacement to this frame and set it's reference bit to 1. We set the found variable to 1, indicating we no longer need to go through our queue of frames because we have found the victim frame we are going to replace
	* if the reference bit it 1, we set it to 0, because we have looked at it, and we go look at the next frame in our queue, which we calculate the same way as we do with FIFO, but add to the page faults-1 the frame +1 and then do mod the size of memory. This will give us the next in line frame in the queue.
* we continue until found is equal to 1
* we then check if the page being swapped out has it's dirty bit set to TRUE, and increment swap outs if it does, and reset it's value to FALSE. 


  
