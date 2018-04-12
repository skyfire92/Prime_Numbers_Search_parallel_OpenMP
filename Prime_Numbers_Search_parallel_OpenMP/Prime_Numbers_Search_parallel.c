#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

/* Function returns number of prime numbers in given interval performing max sqrt(tested_number) divisions in each iteration*/
int PrimeNumbersSearchAlgorithm(int begin, int end);							//arguments: start and end of interval
/* Function returns number of prime numbers in given interval performing max sqrt(tested_number) divisions in each iteration*/
void PrepareAndSearchInterval(int number_of_threads, int interval_begin, int interval_end);	//liczba watkow, poczatek i koniec przedzialu jako argumenty

int main(int argc, char *argv[])
{
	int interval_begin, interval_end, number_of_threads; 	//variables storing input from command line
	
	while (1)
	{
		if (argc == 4)
		{
			if ((sscanf(argv[1], "%i", &interval_begin) != 1) || (sscanf(argv[2], "%i", &interval_end) != 1) || (sscanf(argv[3], "%i", &number_of_threads) != 1))
			{
				fprintf(stderr, "Error - please enter integer number");
				return;
			}
			break;
		}
		else
		{
			printf("Enter only 3 arguments, first and last number of searched interval and number of threads to use (at least 1)\n");
			return;
		}
	}

	if (interval_begin <= 1 || interval_end <= 1)
	{
		printf("Searched interval cannot contain negative numbers and numbers less than 2\n");
		return; //end if begin/end of interval is negative or less than 2
	}

	if (interval_begin > interval_end)
	{
		printf("End of searched interval cannot be less than beginning number\n");
		return; //end if end of searched interval is less than beginning number
	}

	if (number_of_threads<1)
	{
		printf("Number of threads cannot be less than 1\n");
		return; //end if number of threads is wrong
	}

	clock_t total_time = clock(); //start measuring time of execution

	PrepareAndSearchInterval(number_of_threads, interval_begin, interval_end);

	printf("\nTotal search time: %8.6fs\n", (double)(clock() - total_time) / CLOCKS_PER_SEC);
	printf("End\n");
	return 0;
}

int PrimeNumbersSearchAlgorithm(int begin, int end)
{
	int prime_numbers_count = 0;
	int tested_number = begin;

	for (; tested_number <= end; tested_number++)
	{
		int square_root = sqrt(tested_number);
		int i, flag = 0;

		for (i = 2; i <= square_root; i++)					//for i=1 returns 0, for i=2 returns 1 because 2 can only be divided by itself
		{
			if (tested_number%i == 0)						//complex number condition
			{
				flag = 1;
				break;
			}
		}

		if (!flag)
		{
			prime_numbers_count++;
		}
	}
	return prime_numbers_count;
}

void PrepareAndSearchInterval(int number_of_threads, int interval_begin, int interval_end)
{
	printf("ALGORITHM DIVIDING MAIN INTERVAL INTO T EQUAL INTERVALS (T - NUMBER OF THREADS)\n\n");

	printf("Interval from %d to %d\n\n", interval_begin, interval_end);

	int * interval_starts = malloc(number_of_threads * sizeof(int));				//dynamic table containing starts of divided intervals
	int * interval_ends = malloc(number_of_threads * sizeof(int));					//dynamic table cotaining ends of divided intervals
	int * results = malloc(number_of_threads * sizeof(int));						//dynamic table containing results of every thread's calculations

	omp_set_num_threads(number_of_threads);											//setting desired number of threads
	int interval_length = (interval_end - interval_begin) / number_of_threads;		//setting interval length based on desired number of threads

	//BEGINNING OF SETTING INTERVALS BOUNDARIES
	interval_starts[0] = interval_begin;
	interval_ends[0] = interval_begin + interval_length;

	for (int i = 1; i < number_of_threads; i++)
	{
		interval_starts[i] = interval_ends[i - 1] + 1;
		interval_ends[i] = interval_starts[i] + interval_length;
	}
	interval_ends[number_of_threads - 1] = interval_end;
	//END OF SETTING INTERVALS BOUNDARIES

	printf("Number of used threads and created intervals : % 2d, Length of every interval: %8d\n\n", number_of_threads, interval_length);

	for (int i = 0; i < number_of_threads; i++)
	{
		printf("Interval no. %2d start: %8d, Interval no. %2d end: %8d\n", i + 1, interval_starts[i], i + 1, interval_ends[i]);
	}
	printf("\n");

	double start, end; //variables for measuring time

	//PARALLEL SECTION
	int i = 0;
#pragma omp parallel for
	for (i = 0; i < number_of_threads; i++)
	{
		start = omp_get_wtime(); //START OF TIME MEASUREMENT
		results[i] = PrimeNumbersSearchAlgorithm(interval_starts[i], interval_ends[i]);
		end = omp_get_wtime();	//END OF TIME MEASUREMENT
		printf_s("Parallel computations lasted for %8.6f seconds, Thread number: %2d found %d prime numbers in interval (%8d,%8d)\n", end - start, omp_get_thread_num(), results[i], interval_starts[i], interval_ends[i]);
	}
	//END OF PARALLEL SECTION

	int sum = 0; //sum of prime numbers found by every thread
	for (int i = 0; i < number_of_threads; i++)
	{
		sum = results[i] + sum;
	}

	free(interval_starts); //free memory allocated by malloc
	free(interval_ends);
	free(results);

	printf("Longest thread time: %8.6f seconds\n", end - start);
	printf("Total prime number count: %8d\n", sum);
}
