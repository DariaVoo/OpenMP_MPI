//
// Created by snorcros on 11/18/20.
//
#include "mpi_labs.h"

/**
 * Реализуйте на основе технологии MPI
 * многопоточную программу умножения
 * длинных целых чисел методом Шёнхаге — Штрассена
 * */

// Инициализация массива, представляющего собой число
void init_num(int *num, int len, int simple)
{
	int i;

	i = 0;
	while (i < len)
	{
		num[i] = simple;
		i++;
	}
}

// Печать массива, представляющего собой число
void print_num(int *num, int len)
{
	int i;

	i = len;
	while (i > 0)
	{
		i--;
		printf("%d", num[i]);
	}
	printf("\n");
}

int *multiply(int *num1, int len_num1, int *num2, int len_num2, int ProcRank, int ProcNum)
{
	int *result;
	int len_res = len_num1 + len_num2;

	result = (int *)malloc(sizeof(int) * len_res);
	for (int i = 0; i < len_num1 + len_num2; i++)
		result[i] = 0;
	if (len_num2 == ProcNum)
	{
		int i = ProcRank;
		int rest = 0;
		for (int j = len_num1 - 1; j >= 0; j--)
		{
			int intTemp = num2[i] * num1[j] + rest;
			if (intTemp < 10)
			{
				if (result[(i + j + 1)] + intTemp < 10)
					result[(i + j + 1)] += intTemp;
				else
				{
					div_t yetAnotherRest = div(result[(i + j + 1)] + intTemp, 10);
					result[(i + j + 1)] = yetAnotherRest.rem;
					result[(i + j)] += yetAnotherRest.quot;
				}
			}
			else
			{
				if (result[(i + j + 1)] + (intTemp % 10) < 10)
				{
					result[(i + j + 1)] += intTemp % 10;
					result[(i + j)] += div(intTemp, 10).quot;
				}
				else
				{
					div_t yetAnotherRest = div(result[(i + j + 1)] + (intTemp % 10), 10);
					result[(i + j + 1)] = yetAnotherRest.rem;
					result[(i + j)] += div(intTemp, 10).quot + yetAnotherRest.quot;
				}
			}
		}

		int *res_all_proc;
		if (ProcRank == 0)
			res_all_proc = (int *)malloc(sizeof(int) * len_res * ProcNum);

		// отправляем результаты умножения главному процессу
		MPI_Gather(result, (len_num1 + len_num2), MPI_INT, res_all_proc, len_res, MPI_INT,
				   0, MPI_COMM_WORLD);

		printf("Proc%d send %d (first num in res)\n", ProcRank, result[len_res]);
		fflush(0);

		if (ProcRank == 0)
		{
			int tail;

			for (int i = 0; i < (len_num1 + len_num2); i++)
				result[i] = 0;

			// объединяем результаты полученные от детей (делаем переносы через разряды)
			for (int i = 0; i < (len_num1 + len_num2); i++)
			{
				tail = 0;
				for (int j = 0; j < (len_num1 + len_num2) * ProcNum; j = j + (len_num1 + len_num2))
					tail += res_all_proc[i + j];

				if (tail < 10)
					result[i] += tail;
				else
				{
					div_t rest = div(tail, 10);
					result[i] = rest.rem;
					int k = i;
					while (result[k - 1] + rest.quot >= 10)
					{
						k--;
						rest = div(result[k] + rest.quot, 10);
						result[k] = rest.rem;
					}
					result[k - 1] += rest.quot;
				}
			}
		}

		free(res_all_proc);
		return result;
	}
	else
		return NULL;
}

int lab6(int *num1, int *num2, int N, int ProcNum, int ProcRank)
{
	int *res_mul;

	res_mul = multiply(num1, N, num2, N, ProcRank, ProcNum);
	if (ProcRank == 0)
	{
		if (res_mul != NULL)
		{
			printf("\nResult:\n");
			fflush(0);
			print_num(res_mul, N * 2);
		}
		else
			printf("The count of processes and digits in the second number must be the same!");
	}
	free(res_mul);
	return (0);
}

int main(int argc, char* argv[])
{
	if (argc != 2)
		return (0);

	int ProcNum, ProcRank;
//	int N = atoi(argv[1]); // Длина целых чисел
	int N = 5; // Длина целых чисел
	int *num1 = (int *)malloc(sizeof(int) * N);
	int *num2 = (int *)malloc(sizeof(int) * N);

	init_num(num1, N, 4);
	init_num(num2, N, 2);

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	// <программный код с использованием MPI функций>
	if (ProcRank == 0)
	{
		printf("Будем перемножать числа:\n");
		fflush(0); // сброс буфера
		print_num(num1, N);
		print_num(num2, N);
	}

	lab6(num1, num2, N, ProcNum, ProcRank);

	MPI_Finalize();
	free(num1);
	free(num2);
	return (0);
}


