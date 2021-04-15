/*
 * JULIA PUTKO 
 * V00889506
 *
 * Skeleton code for CSC 360, Spring 2021,  Assignment #4
 *
 * Prepared by: Michael Zastre (University of Victoria) 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * Some compile-time constants.
 */

#define REPLACE_NONE 0
#define REPLACE_FIFO 1
#define REPLACE_LRU  2
#define REPLACE_SECONDCHANCE 3
#define REPLACE_OPTIMAL 4


#define TRUE 1
#define FALSE 0
#define PROGRESS_BAR_WIDTH 60
#define MAX_LINE_LEN 100


/*
 * Some function prototypes to keep the compiler happy.
 */
int setup(void);
int teardown(void);
int output_report(void);
long resolve_address(long, int);
void error_resolve_address(long, int);
long fifo(long, long);
long lru(long, int, long);
long secondchance(long, long);

/*
 * Variables used to keep track of the number of memory-system events
 * that are simulated.
 */
int page_faults = 0;
int mem_refs    = 0;
int swap_outs   = 0;
int swap_ins    = 0;


/*
 * Page-table information. You may want to modify this in order to
 * implement schemes such as SECONDCHANCE. However, you are not required
 * to do so.
 */
struct page_table_entry *page_table = NULL;
struct page_table_entry {
    long page_num;
    int dirty;
    int free;
    long reference_bit;			// set referenced bit
    int time_of_use;			// time of use recorded 
};


/*
 * These global variables will be set in the main() function. The default
 * values here are non-sensical, but it is safer to zero out a variable
 * rather than trust to random data that might be stored in it -- this
 * helps with debugging (i.e., eliminates a possible source of randomness
 * in misbehaving programs).
 */

int size_of_frame = 0;  /* power of 2 */
int size_of_memory = 0; /* number of frames */
int page_replacement_scheme = REPLACE_NONE;


/*
 * Function to convert a logical address into its corresponding 
 * physical address. The value returned by this function is the
 * physical address (or -1 if no physical address can exist for
 * the logical address given the current page-allocation state.
 */

/*
* FIFO implementation
* number of page faults -1 (because pagefaults has been inceremented before call to FIFO)
* mod%size_of_memory will give the frames in order, increasing as the number of page faults increases
*/
long fifo(long frame, long page)
{
    frame = (page_faults-1)%size_of_memory;
    page_table[frame].page_num = page;
    return frame;
}

/*
* LRU implementation 
* look through all of the frames for the smallest time_of_use number 
* continously setting and checking until we have looked through all the frames 
* the location with the smallest time of use will become the victim frame 
* time of use counter is updated every time a new page is read
*/
long lru(long frame, int i, long page)
{
    int least = mem_refs;          
        for ( i = 0; i < size_of_memory; i++){           //check all frames 
            if ((page_table[i].time_of_use) < least){    //set minumum to that if it is smaller
                least = page_table[i].time_of_use;       //the frame we will change will be that position;
                frame = i;
            } 
        }
    page_table[frame].page_num = page; 
    page_table[frame].time_of_use = mem_refs;
    return frame; 
}

/*
* SECONDCHANCE implementation
* acts like a FIFO queue if reference bits are set to 0
* if reference bits are set to 1, they are given a second chance, so we set its bit to 0 and move down
*     in the queue
* keep looping through the queue, and setting 1 bits to 0 until a frame ref bit 0 is found
* this will become the victim frame
*/
long secondchance(long frame, long page)
{
	int found = 0;                                  //indicated that we found a frame to replace
   // if (set ==1){
     //   frame = ((page_faults -1)+frame)%size_of_memory;
    //}
    //else{ //set is not equal to 1


	frame = ((page_faults-1))%size_of_memory;      // ibegin like a FIFO queue
	if (frame > 0){
		frame++;    //eg we move to the next one if mod size of memory doesnt bring the frame to the beginning 
	}

	do{
        	if (page_table[frame].reference_bit == 0){ // if bit is 0
			page_table[frame].page_num = page;  // load it into the page
			page_table[frame].reference_bit = 1;   //set it's bit to 1
			found = 1;				 // set found to 1, so that we dont continue looking once we have found our victim frame
        	}
        	else {                          // bit it 1, set it to 0, go to the next one
			page_table[frame].reference_bit = 0;
		
			
			frame = ((page_faults-1)+( frame+1))%size_of_memory;
		}
  // added to frame will give the next frame in the queue, starting from where we left off
	
		//page fautls -1 because we incremented pagefaults before we did the relacement
        //+ frame+1, will make it so the next frame we go to is next in line in the queue
        //%size_of_memory, so that we will ontinue to loop, no need to check if we are at the end, because by modding it by the size, it will never reach the end

    	} while (found != 1);

    //set = 0;

	return frame;
}




long resolve_address(long logical, int memwrite)
{
    int i;
    long page, frame;
    long offset;
    long mask = 0;
    long effective;

    /* Get the page and offset */
    page = (logical >> size_of_frame);

    for (i=0; i<size_of_frame; i++) {
        mask = mask << 1;
        mask |= 1;
    }
    offset = logical & mask;

    /* Find page in the inverted page table. */
    frame = -1;
    for ( i = 0; i < size_of_memory; i++ ) {
        if (!page_table[i].free && page_table[i].page_num == page) {
            frame = i;
            break;
        }
    }

    /* If frame is not -1, then we can successfully resolve the
     * address and return the result. */
    if (frame != -1) {   // page HIT 

	page_table[frame].time_of_use = mem_refs;           // it is in memory //
        page_table[frame].reference_bit = 1; 

        
        if (memwrite == TRUE){                              //set dirty bit to 1 is memwrite is set (is W)
            page_table[frame].dirty = TRUE;
        }

        effective = (frame << size_of_frame) | offset;
        return effective;
    }


    /* If we reach this point, there was a page fault. Find
     * a free frame. */
    page_faults++;
 

    for ( i = 0; i < size_of_memory; i++) {
        if (page_table[i].free) {
            frame = i;
            break;
        }
    }

    /* If we found a free frame, then patch up the
     * page table entry and compute the effective
     * address. Otherwise return -1.
     */
    if (frame != -1) {		//free spot 
        page_table[frame].page_num = page;
	page_table[frame].time_of_use = mem_refs; 	//update time of use to clock value (aka memrefs)
	page_table[frame].reference_bit = 1; 		//set reference bit to 1, because we inserted a new page 
        page_table[i].free = FALSE;

	if (memwrite == TRUE){				//set dirty to true if memwrite is set to W
		page_table[frame].dirty = TRUE;
	}

        swap_ins++;
        effective = (frame << size_of_frame) | offset;
        return effective;
    } else {

	if (page_replacement_scheme == REPLACE_FIFO){
		
		if (memwrite == TRUE){
			page_table[frame].dirty = TRUE;
		}

		frame = fifo(frame, page);

		if (page_table[frame].dirty == TRUE){
			swap_outs++;
			page_table[frame].dirty = FALSE;
		}		

		swap_ins++;
		effective = (frame << size_of_frame) | offset;
            	return effective; 
			
	}
	else if (page_replacement_scheme == REPLACE_LRU){
            
            if (memwrite == TRUE){              // is the page being swapped in dirty? 
                page_table[frame].dirty = TRUE;
            }
            frame = lru(frame, i, page);  
            
            if (page_table[frame].dirty == TRUE){  //is the page being swapped out dirty? 
                swap_outs++;
                page_table[frame].dirty = FALSE; 
            }
            
            swap_ins++;  //increment the amount of swap ins -- should be the same number as page faults 
            effective = (frame << size_of_frame) | offset;
            return effective;  //return result
        
        }

	else if (page_replacement_scheme == REPLACE_SECONDCHANCE){
           
            if (memwrite == TRUE){             
               page_table[frame].dirty = TRUE;
            }
            frame = secondchance(frame, page);

            if (page_table[frame].dirty == TRUE){
                swap_outs++;
                page_table[frame].dirty = FALSE;  
            }
           
            swap_ins++;  //increment the amount of swap ins -- should be the same number as page faults 
            effective = (frame << size_of_frame) | offset;
            return effective;  //return result

        }
	else{
		return -1;
	
	}

    }
}



/*
 * Super-simple progress bar.
 */
void display_progress(int percent)
{
    int to_date = PROGRESS_BAR_WIDTH * percent / 100;
    static int last_to_date = 0;
    int i;

    if (last_to_date < to_date) {
        last_to_date = to_date;
    } else {
        return;
    }

    printf("Progress [");
    for (i=0; i<to_date; i++) {
        printf(".");
    }
    for (; i<PROGRESS_BAR_WIDTH; i++) {
        printf(" ");
    }
    printf("] %3d%%", percent);
    printf("\r");
    fflush(stdout);
}


int setup()
{
    int i;

    page_table = (struct page_table_entry *)malloc(
        sizeof(struct page_table_entry) * size_of_memory
    );

    if (page_table == NULL) {
        fprintf(stderr,
            "Simulator error: cannot allocate memory for page table.\n");
        exit(1);
    }

    for (i=0; i<size_of_memory; i++) {
        page_table[i].free = TRUE;
    }

    return -1;
}


int teardown()
{

    return -1;
}


void error_resolve_address(long a, int l)
{
    fprintf(stderr, "\n");
    fprintf(stderr, 
        "Simulator error: cannot resolve address 0x%lx at line %d\n",
        a, l
    );
    exit(1);
}


int output_report()
{
    printf("\n");
    printf("Memory references: %d\n", mem_refs);
    printf("Page faults: %d\n", page_faults);
    printf("Swap ins: %d\n", swap_ins);
    printf("Swap outs: %d\n", swap_outs);

    return -1;
}


int main(int argc, char **argv)
{
    /* For working with command-line arguments. */
    int i;
    char *s;

    /* For working with input file. */
    FILE *infile = NULL;
    char *infile_name = NULL;
    struct stat infile_stat;
    int  line_num = 0;
    int infile_size = 0;

    /* For processing each individual line in the input file. */
    char buffer[MAX_LINE_LEN];
    long addr;
    char addr_type;
    int  is_write;

    /* For making visible the work being done by the simulator. */
    int show_progress = FALSE;

    /* Process the command-line parameters. Note that the
     * REPLACE_OPTIMAL scheme is not required for A#3.
     */
    for (i=1; i < argc; i++) {
        if (strncmp(argv[i], "--replace=", 9) == 0) {
            s = strstr(argv[i], "=") + 1;
            if (strcmp(s, "fifo") == 0) {
                page_replacement_scheme = REPLACE_FIFO;
            } else if (strcmp(s, "lru") == 0) {
                page_replacement_scheme = REPLACE_LRU;
            } else if (strcmp(s, "secondchance") == 0) {
                page_replacement_scheme = REPLACE_SECONDCHANCE;
            } else if (strcmp(s, "optimal") == 0) {
                page_replacement_scheme = REPLACE_OPTIMAL;
            } else {
                page_replacement_scheme = REPLACE_NONE;
            }
        } else if (strncmp(argv[i], "--file=", 7) == 0) {
            infile_name = strstr(argv[i], "=") + 1;
        } else if (strncmp(argv[i], "--framesize=", 12) == 0) {
            s = strstr(argv[i], "=") + 1;
            size_of_frame = atoi(s);
        } else if (strncmp(argv[i], "--numframes=", 12) == 0) {
            s = strstr(argv[i], "=") + 1;
            size_of_memory = atoi(s);
        } else if (strcmp(argv[i], "--progress") == 0) {
            show_progress = TRUE;
        }
    }

    if (infile_name == NULL) {
        infile = stdin;
    } else if (stat(infile_name, &infile_stat) == 0) {
        infile_size = (int)(infile_stat.st_size);
        /* If this fails, infile will be null */
        infile = fopen(infile_name, "r");  
    }


    if (page_replacement_scheme == REPLACE_NONE ||
        size_of_frame <= 0 ||
        size_of_memory <= 0 ||
        infile == NULL)
    {
        fprintf(stderr, 
            "usage: %s --framesize=<m> --numframes=<n>", argv[0]);
        fprintf(stderr, 
            " --replace={fifo|lru|optimal} [--file=<filename>]\n");
        exit(1);
    }


    setup();

    while (fgets(buffer, MAX_LINE_LEN-1, infile)) {
        line_num++;
        if (strstr(buffer, ":")) {
            sscanf(buffer, "%c: %lx", &addr_type, &addr);
            if (addr_type == 'W') {
                is_write = TRUE;
            } else {
                is_write = FALSE;
            }

            if (resolve_address(addr, is_write) == -1) {
                error_resolve_address(addr, line_num);
            }
            mem_refs++;
        } 

        if (show_progress) {
            display_progress(ftell(infile) * 100 / infile_size);
        }
    }
    

    teardown();
    output_report();

    fclose(infile);

    exit(0);
}
