#include "openmp.h"

/**
 * Дана матрица из MxN натуральных(ненулевых) элементов (задаются случайно).
 * Написать программу, считающую количество семёрок в десятеричной записи числа
 * всех попарных сумм элементов для каждой строки.
 * */

void	print_matrix(int **matrix, int M, int N)
{
	int i;
	int j;

	for (i = 0; i < M; i++)
	{
		for (j = 0; j < N; j++)
			printf("%d\t", matrix[i][j]);
		printf("\n");
	}
}

void	fill_matrix(int **matrix, int M, int N)
{
	int i;
	int j;
	time_t t;

	/* Intializes random number generator */
	srand((unsigned) time(&t));
	for (i = 0; i < M; i++)
	{
		for (j = 0; j < N; j++)
			matrix[i][j] = rand();
	}
}

void	free_table(int **arr, int position)
{
	int	i;

	i = 0;
	while (i < position)
	{
		free(arr[i]);
		arr[i] = NULL;
		i++;
	}
	free(arr);
	arr = NULL;
}

int		**new_table(int m, int n)
{
	int **ans;
	int i;

	i = -1;
	if (!(ans = (int **)malloc(sizeof(int *) * (n))))
		return (NULL);
	while (++i < n)
		if (!(ans[i] = (int *)malloc(sizeof(int) * (m))))
		{
			free_table(ans, i);
			return (NULL);
		}
		else
			memset(ans[i], 0, m);
	return (ans);
}

double	count_seven(int num)
{
	int	count;
	int buf;

	buf = (int)num;
	count = 0;
	if (num <= 0)
		buf *= -1;
	while (buf > 0)
	{
		if (buf % 10 == 7)
			count++;
		buf /= 10;
	}
	return (count);
}

double	seven_in_str(int *arr, int len)
{
	int i, j;
	double sum;
	double count;

	i = 0;
	count = 0;
	while (i < len)
	{
		j = i + 1;
		while (j < len)
		{
			sum = arr[i] + arr[j];
			count += count_seven(sum);
			j++;
		}
		i++;
	}
	return (count);
}

int lab1(int argc, char **argv)
{
	int M;
	int N;
	int i, count;
	unsigned int start_time, end_time, search_time;

	printf("Enter M\t");
	scanf("%d", &M);
	printf("Enter N\t");
	scanf("%d", &N);

	int **matrix;
	matrix = new_table(M, N);
	fill_matrix(matrix, M, N);
//	print_matrix(matrix, M, N);

	count = 0;
	start_time =  clock();
	#pragma omp parallel shared(count)
	{
		#pragma omp for
		for (i = 0; i < M; i++)
		{
			count += seven_in_str(matrix[i], N);
//			printf("Num %d\n", omp_get_thread_num());
		}
	}
	end_time = clock(); // конечное время
	search_time = end_time - start_time; // искомое время
	printf("Time parallel %lf: \n", search_time / 1000.0);
	printf("Total sevens %d\n", count);

	count = 0;
	start_time =  clock();
	for (i = 0; i < M; i++)
	{
		count += seven_in_str(matrix[i], N);
//			printf("Sevens at the momenet str%d: %d\n", i, count);
	}
	end_time = clock(); // конечное время
	search_time = end_time - start_time; // искомое время
	printf("Time without parallel %lf: \n", search_time / 1000.0);
	printf("Total sevens %d\n", count);

	free_table(matrix, M);
	return (0);
}
