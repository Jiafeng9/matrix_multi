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
int number_of_runs = 4;
double average_seq_time = 0;
double average_par_time = 0;

int main(int argc, char *argv[])
{
	// printf("%d \n", argc);
	pthread_t *threads; //
	thread_args *args;	//
	mode = argv[1][0];

	// TODO - deal with command line arguments, save the "mode"
	if (argc == 2)
	{
		printf("Usage: ./mmmSol S <size>\n");
		printf("Usage: ./mmmSol P <num threads> <size>\n");
		return 1;
	}
	else if (mode == 'S')
	{
		mode = argv[1][0];
		size = atoi(argv[2]);
	}

	else if (mode == 'P')
	{
		if (argc == 3)
		{
			printf("Error: parallel mode requires [num threads]\n");
			return 1;
		}
		else
		{
			mode = argv[1][0];
			num_threads = atoi(argv[2]);
			size = atoi(argv[3]);
		}
	}
	else
	{
		printf("Error: mode must be either S (sequential) or P (parallel) \n");
		return 1;
	}

	// "size" and "num threads" into globals so threads can see them
	// initialize my matrices
	mmm_init();
	while (number_of_runs > 0)
	{

		double clockstart, clockend;
		clockstart = rtclock(); // start the clock

		// << stuff I want to clock here >>
		if (mode == 'S' || mode == 'P')
		{
			// printf("s \n");
			mmm_seq();
			clockend = rtclock(); // stop the clock
			if(number_of_runs!=4){
				average_seq_time = average_seq_time + (clockend - clockstart); //ignore the first time
			}
			//printf("%d \n", number_of_runs);
			number_of_runs--;
		}
	}

	number_of_runs = 4;

	while (number_of_runs > 0)
	{
		if (mode == 'P')
		{
			double clockstart, clockend;
			clockstart = rtclock(); // start the clock

			// prepare thread arguments
			args = (thread_args *)malloc(num_threads * sizeof(thread_args));
			int remainder = size % num_threads;
			int base_size = size / num_threads;

			if (remainder == 0)
			{
				for (int i = 0; i < num_threads; i++)
				{
					args[i].tid = i;
					args[i].begin = i * size / num_threads;
					// printf("%d \n",size / num_threads);
					// printf("%ld \n",args[i].begin);
					args[i].end = (i + 1) * size / num_threads;
				}
			}
			else
			{
				for (int i = 0; i < num_threads; i++)
				{
					args[i].tid = i;
					if (i == 0)
					{
						args[i].begin = 0;
						args[i].end = base_size + remainder;
					}
					else
					{
						args[i].begin = args[i - 1].end;
						args[i].end = args[i].begin + base_size;
						// printf("%ld \n",args[i].begin);
						// printf("%ld \n",args[i].end);
					}
				}
			}

			// allocate space to hold threads
			threads = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
			for (int i = 0; i < num_threads; i++)
			{
				pthread_create(&threads[i], NULL, mmm_par, &args[i]);
			}
			/** JOIN PHASE **/
			for (int i = 0; i < num_threads; i++)
			{
				pthread_join(threads[i], NULL);
			}

			free(threads);
			threads = NULL;
			free(args);
			args = NULL;
			clockend = rtclock(); // stop the clock
			if(number_of_runs!=4){
				average_par_time = average_par_time + (clockend - clockstart); //ignore the first time

			}
			number_of_runs--;
		}else{
			number_of_runs--;
		}
	}

	if (mode == 'S')
	{
		printf("========\n");
		printf("mode: sequential\n");
		printf("thread count: 1\n");
		printf("size: %d\n", size);
		printf("========\n");
		printf("Sequential Time (avg of 3 runs): %.6f sec\n", average_seq_time / 3);
		mmm_freeup();
	}
	else
	{
		printf("========\n");
		printf("mode: parallel\n");
		printf("thread count: %d\n", num_threads);
		printf("size: %d\n", size);
		printf("========\n");
		printf("Sequential Time (avg of 3 runs): %.6f sec\n", average_seq_time / 3);
		printf("Parrallel Time (avg of 3 runs): %.6f sec\n", average_par_time / 3);
		printf("Speedup:%.6f \n",average_seq_time/average_par_time);
		double difference_value;
		difference_value=mmm_verify();
		printf("Verifying... largest error between parallel and sequential matrix: %.6f\n",difference_value);
		mmm_freeup();
	}

	// free some stuff up
	return 0;
}
