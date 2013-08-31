#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

struct timeval start, end;
long elapsedTimeTotal = 0;
long averageTime = 0;
char buffer[1024];	

int iterations = 0;
int delay = 0;
int i = 0;
int j = 0;	
FILE* resultsFile;

void RunCloseMeasurement()
{
	iterations = 10000;
	printf("\nClose(100) measurements\n%d iterations\n\n", iterations);
	fprintf(resultsFile, "\nClose(100) measurements\n%d iterations\n\n", iterations);

	for(j = 0; j < 	21; j++)
	{
		if(j != 0)
		{
			delay += 50;	
			sprintf(buffer, "echo \"%d\" > /proc/delay_mod\n", delay);			
			system(buffer);
		}

		gettimeofday(&start, NULL);
		
		while(i < iterations)
		{
			close(100);
			i++;
		}

		gettimeofday(&end, NULL);

		elapsedTimeTotal = (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);
		
		averageTime = elapsedTimeTotal / iterations;
		printf("Delay: %d Elapsed average: %ld\n", delay, averageTime);		
		fprintf(resultsFile, "Delay: %d Elapsed average: %ld\n", delay, averageTime);

		elapsedTimeTotal = 0;
		i = 0;
	}
}

void RunMakeKernelMeasurements()
{
	iterations = 3;

	printf("\nmake kernel measurements\n%d iterations\n\n", iterations);
	fprintf(resultsFile, "\nmake kernel measurements\n%d iterations\n\n", iterations);

	chdir("/home/451user/project1/kernel/linux-2.6.38.2");
	system("make clean");

	for(j = 0; j < 	6; j++)
	{
		if(j != 0)
		{
			delay += 50;	
			sprintf(buffer, "echo \"%d\" > /proc/delay_mod\n", delay);			
			system(buffer);
		}

		while(i < iterations)
		{
			// flush the buffer cache
			system("echo 3 > sudo /proc/sys/vm/drop_caches");

			gettimeofday(&start, NULL);

			system("make kernel");

			gettimeofday(&end, NULL);
		
			elapsedTimeTotal += (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);

			system("make clean");

			i++;
		}
		
		averageTime = elapsedTimeTotal / iterations;
		printf("Delay: %d Elapsed average: %ld\n", delay, elapsedTimeTotal, averageTime);		
		fprintf(resultsFile, "Delay: %d Elapsed average: %ld\n", delay, elapsedTimeTotal, averageTime);

		i = 0;
		elapsedTimeTotal = 0;
	}
}

void RunGrepMeasurements()
{
	iterations = 5;

	printf("\ngrep measurements\n%d iterations\n\n", iterations);
	fprintf(resultsFile, "\ngrep measurements\n%d iterations\n\n", iterations);

	chdir("/home/451user/project1/kernel/linux-2.6.38.2/arch");
	
	for(j = 0; j < 	11; j++)
	{
		if(j != 0)
		{
			delay += 50;	
			sprintf(buffer, "echo \"%d\" > /proc/delay_mod\n", delay);			
			system(buffer);
		}		

		system("echo 3 > sudo /proc/sys/vm/drop_caches");

		while(i < iterations)
		{
			// flush the buffer cache
			system("echo 3 > sudo /proc/sys/vm/drop_caches");

			gettimeofday(&start, NULL);

			system("grep -r \"*sys_call_table(,%eax,4)\" *");

			gettimeofday(&end, NULL);
		
			elapsedTimeTotal += (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);

			i++;
		}

		averageTime = elapsedTimeTotal / iterations;
		printf("Delay: %d Elapsed average: %ld\n", delay, elapsedTimeTotal, averageTime);		
		fprintf(resultsFile, "Delay: %d Elapsed average: %ld\n", delay, elapsedTimeTotal, averageTime);

		elapsedTimeTotal = 0;
		i = 0;
	}
}

int main(int argc, char* argv[])
{
	if(argv[1] == NULL)
	{	
		printf("\nPlease supply the test type (close_test, make_kernel_test, or grep_test).\n\n");
		return -1;
	}	

	if(0 == strcmp(argv[1], "close_test"))
	{
		resultsFile = fopen("results_close_test.txt", "w+");
		RunCloseMeasurement();
	}
	else if(0 == strcmp(argv[1], "make_kernel_test"))
	{
		resultsFile = fopen("results_make_kernel_test.txt", "w+");
		RunMakeKernelMeasurements();
	}
	else if(0 == strcmp(argv[1], "grep_test"))
	{
		resultsFile = fopen("results_grep_test.txt", "w+");
		RunGrepMeasurements();
	}
	else
	{
		printf("\nPlease supply the test type (close_test, make_kernel_test, or grep_test).\n\n");
		return -1;
	}

	sprintf(buffer, "echo \"0\" > /proc/delay_mod\n");			
	system(buffer);
	
	if(resultsFile)
	{	
		fclose(resultsFile);
	}
	
	return 0;
}
