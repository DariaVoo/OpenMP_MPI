//
// Created by snorcros on 10/1/20.
//
#include "openmp.h"

/** Модифицировать программу, составленную по Заданию 1 в Л.Р. №3,
 * используя синхронизацию вместо редукции. */

int parall_sync(int *arr_A, int *arr_B, int *arr_C, int N, int flag)
{
	float product = 1;
	float num = 0.0;
	unsigned int start_time, end_time, search_time;

	start_time =  clock();
	#pragma omp parallel shared(product) if (flag)
	{
	#pragma omp for private(num)
		for (int i = 0; i < N; i++)
		{
			if (arr_A[i] % 2 == 0 && arr_C[i] != 0)
				num = arr_B[i] / arr_C[i];
			else
				num = arr_B[i] + arr_A[i];

			if (num != 0)
			{
				#pragma omp critical
				product *= num;
			}
		}
	}
	end_time = clock(); // конечное время
	search_time = end_time - start_time; // искомое время
	printf("%lf\t", search_time / 1000.0);
//	printf ("Произведение равно %f\n", product);
	return (product);
}

int lab4(int argc, char **argv)
{
	int N;
	int arr_N[] = {10, 200, 500, 1000, 50000};

	printf("- Parallel - sequent -\n");
	printf("N\tTime\t\tTime\n");
	for (int i = 0; i < 5; i++)
	{
		N = arr_N[i];
		int A[N], B[N], C[N];
		// заполняем массивы
		fill_arr(A, N);
		sleep(1); // for random
		fill_arr(B, N);
		sleep(1);
		fill_arr(C, N);

		printf("%d\t", N);
		parall_sync(A, B, C, N, 1);
		parall_sync(A, B, C, N, -1);
		printf("\n");
	}
	return (0);
}
