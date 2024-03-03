#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "rtclock.h"
#include "mmm.h"

// shared  globals
unsigned char mode;
unsigned int size, num_threads;
double **A, **B, **SEQ_MATRIX, **PAR_MATRIX;

int main(int argc, char *argv[]) {
	printf("%d \n",argc);
	pthread_t *threads; // 
    thread_args *args;  // 

	// TODO - deal with command line arguments, save the "mode"
	if(argc == 3) {
        mode = argv[1][0];
		//printf("%c \n",mode);
        size = atoi(argv[2]);
    } else if (argc == 4) {
        mode = argv[1][0];
        num_threads = atoi(argv[2]);
        size = atoi(argv[3]);
	}else{
		printf("Invalid parameters\n");
        return 1; // Exit with an error code
	}
	
	// "size" and "num threads" into globals so threads can see them
	// initialize my matrices
	mmm_init();

	double clockstart, clockend;
	clockstart = rtclock();	// start the clock

	// << stuff I want to clock here >>
	if(mode == 'S'){
		//printf("s \n");
		mmm_seq();
	}else{
		// prepare thread arguments
  		args = (thread_args*) malloc(num_threads * sizeof(thread_args));
		int remainder = size % num_threads;
		int base_size = size / num_threads;

		if (remainder ==0){
			for (int i = 0; i < num_threads; i++) {
    			args[i].tid = i;
    			args[i].begin = i * size / num_threads;
				//printf("%d \n",size / num_threads);
		        //printf("%ld \n",args[i].begin);
    	        args[i].end = (i + 1) * size/ num_threads;
				}
			}
		else{
			for (int i = 0; i < num_threads; i++) {
				args[i].tid = i;
    			if (i == 0) {
        			args[i].begin = 0;
        			args[i].end = base_size + remainder;
    			} else {
        			args[i].begin = args[i-1].end;
        			args[i].end = args[i].begin + base_size;
					//printf("%ld \n",args[i].begin);
					//printf("%ld \n",args[i].end);
   				}

			}
		}	

		// allocate space to hold threads
  		threads = (pthread_t*) malloc(num_threads * sizeof(pthread_t));
  		for (int i = 0; i < num_threads; i++) {
    		pthread_create(&threads[i], NULL, mmm_par, &args[i]);
  		}

		/** JOIN PHASE **/
  		for (int i = 0; i < num_threads; i++) {
    		pthread_join(threads[i], NULL);
		}

		free(threads);
  		threads = NULL;

  		free(args);
  		args = NULL;
		}

	
	
	clockend = rtclock(); // stop the clock
	printf("Time taken: %.6f sec\n", (clockend - clockstart));

	  mmm_freeup();
	// free some stuff up
	return 0;
}
