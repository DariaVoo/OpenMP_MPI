//
// Created by snorcros on 10/1/20.
//
#include "openmp.h"

/** На основе трёх равно размерных массивов A, B и C (длины N)
 * функция возвращает произведение ненулевых значений,
 * полученных таким образом: если Ai четно: Bi / Ci, иначе Bi + Ai
 * */

int parall_reduction(int *arr_A, int *arr_B, int *arr_C, int N, int flag)
{
	float product = 1;
	float num = 0.0;
	unsigned int start_time, end_time, search_time;

	start_time =  clock();
	#pragma omp parallel shared(product) if (flag)
	{
		#pragma omp for private(num) reduction(*:product)
		for (int i = 0; i < N; i++)
		{
			if (arr_A[i] % 2 == 0 && arr_C[i] != 0)
				num = arr_B[i] / arr_C[i];
			else
				num = arr_B[i] + arr_A[i];

			if (num != 0)
				product *= num;
		}
	}
	end_time = clock(); // конечное время
	search_time = end_time - start_time; // искомое время
	printf("%lf\t", search_time / 1000.0);
//	printf ("Произведение равно %f\n", product);
	return (product);
}

void	fill_arr(int *arr, int N)
{
	int i;

	srand((unsigned int)time(0));
	for (i = 0; i < N; i++)
		arr[i] = rand() % 1000;
}

void print_arr(int *arr, int N)
{
	for (int i = 0; i < N; i++)
		printf("%d ", arr[i]);
	printf("\n");
}

int lab3(int argc, char **argv)
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
		parall_reduction(A, B, C, N, 1);
		parall_reduction(A, B, C, N, -1);
		printf("\n");
	}
	return (0);
}

/*
 * - Parallel - sequent -
N	Time		Time
10	1.344000	0.000000
200	0.046000	0.004000
500	0.029000	0.009000
1000	0.049000	0.019000
5000	0.047000	0.088000

 * */