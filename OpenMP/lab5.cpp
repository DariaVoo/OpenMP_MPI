//
// Created by snorcros on 10/1/20.
//
#include "openmp.h"

/** Модифицировать программы, составленные в Л.Р. №№3,4,
 * распределив вычислительную нагрузку по секциям (как минимум двумя способами).
 * */

int distributed(int *arr_A, int *arr_B, int *arr_C, int N, int flag)
{
	float product = 1;
	float num = 0.0;
	unsigned int start_time, end_time, search_time;

	start_time =  clock();
	#pragma omp parallel  sections  shared(product) private(num) if (flag)
	{
		#pragma omp section
		for (int i = 0; i < int(N / 2); i++)
		{
			if (arr_A[i] % 2 == 0 && arr_C[i] != 0)
				num = arr_B[i] / arr_C[i];
			else
				num = arr_B[i] + arr_A[i];

			if (num != 0)
			{
				#pragma omp atomic
				product *= num;
			}
		}

		#pragma omp section
		for (int i = int(N / 2); i < N; i++)
		{
			if (arr_A[i] % 2 == 0 && arr_C[i] != 0)
				num = arr_B[i] / arr_C[i];
			else
				num = arr_B[i] + arr_A[i];

			if (num != 0)
			{
				#pragma omp atomic
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

int lab5(int argc, char **argv)
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
		distributed(A, B, C, N, 1);
		distributed(A, B, C, N, -1);
		printf("\n");
	}
	return (0);
}
