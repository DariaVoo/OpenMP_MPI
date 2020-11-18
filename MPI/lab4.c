//
// Created by snorcros on 10/2/20.
//
#include "mpi_labs.h"

/**
 * Реализуйте тип «длинное целое».
 * Напишите программу, которая осуществляет умножение A целых чисел заданной длины.
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

// Умножение на один из элементов b
int *dot_single(int *a_num, int b_num, int *ans, int len)
{
	int i, j, cur, ost;

	i = 1;
	j = 0;
	ost = 0;
	while (i < len + 1 && j < len)
	{
		cur = a_num[j] * b_num + ost;
		ans[i] = cur % 10;
		ost = (int)(cur / 10);
		i++;
		j++;
	}
	return (ans);
}

// Сумма 2х длинных чисел со смещением index в первом числе, результат записываем в a_num
int *sum_two_long(int *a_num, int *b_num, int index, int len)
{
	int i, j, cur, ost;

	i = index;
	j = 0;
	ost = 0;

	printf("Proc MAIN in sum index %d\n", index);
	fflush(0); // сброс буфера

	while (j < len)
	{
		cur = a_num[i] + b_num[j] + ost;
		a_num[i] = cur % 10;
		ost = (int)(cur / 10);
		i++;
		j++;
	}

	printf("Proc MAIN sum Ok\n");
	fflush(0); // сброс буфера
	return (a_num);
}

// Генерация индексов чисел на которые будут умножать дети
int *gen_indexes(int *arr_index, int start, int len)
{
	int i;

	i = 1;
	while (i < len)
	{
		arr_index[i] = start;
		start++;
		i++;
	}
	return (arr_index);
}

void build_type(t_arr_num *ans, MPI_Datatype *new_type, int len)
{
	MPI_Datatype types[2] = {MPI_INT, MPI_INT}; // типы данных в структуре t_arr_num
	int block_len[2] = {1, len}; // длина каждого поля структуры t_arr_num/массив смещений в структуре
	MPI_Aint displacements[2]; // массив смещений каждого элемента относительно начала сообщения
	MPI_Aint addresses[3];

	// Вычисление смещений элементов относительно ans
	MPI_Get_address(ans, &addresses[0]);
	MPI_Get_address(&(ans->index), &addresses[1]);
	MPI_Get_address(&(ans->arr), &addresses[2]);
	displacements[0] = addresses[1] - addresses[0];
	displacements[1] = addresses[2] - addresses[0];
	MPI_Type_create_struct(2, block_len, displacements, types, new_type);
	MPI_Type_commit(new_type);
}

void lab4_two_int(int ProcRank, int ProcNum)
{
	MPI_Datatype mpi_two_int; // создаём свой тип
	int *child_msg;
	int res_child[ProcNum * 2];

	if (ProcRank == 0)
	{
		MPI_Gather(child_msg, 1, mpi_two_int, res_child, 1, mpi_two_int, 0, MPI_COMM_WORLD);
		for (int i = 0; i < ProcNum * 2 - 1; i += 2)
		{
			printf("Master get response from proc%d: %d\t", i+1, res_child[i]);
			fflush(0);

			printf("Master get response from proc%d: %d\n", i+1, res_child[i+1]);
			fflush(0);
		}

	} else {
		MPI_Gather(child_msg, 1, mpi_two_int, res_child, 1, mpi_two_int, 0, MPI_COMM_WORLD);
	}
}

void build_type_arr_struct(MPI_Datatype *new_type, int len)
{
	MPI_Type_contiguous(len, MPI_INT, new_type);
	MPI_Type_commit(new_type);
}

int *lab4(int *a, int *b, int len, int ProcRank, int ProcNum, int count)
{
	int *ans_child;
	int *res_child;
	int len_struct = len + 1;
	int len_b = len;
	int len_ans = len * 2;

	int *indexes = (int *) malloc(sizeof(int) * ProcNum);
	int *answer_master = (int *) malloc(sizeof(int) * (len_ans));


	MPI_Datatype mpi_type_arr_num; // создаём свой тип данных
	build_type_arr_struct(&mpi_type_arr_num, len_struct);

	MPI_Datatype mpi_type_ans; // создаём свой тип данных для промежуточного результата
	build_type_arr_struct(&mpi_type_ans, len * 2);

	init_num(answer_master, len_ans, 0);
	for (int m = 1; m < count; m++)
	{
		len_struct = len + 1;
		for (int i = 0; i < len_b; i += ProcNum) // идём по числу b
		{
			if (ProcRank == 0)
			{
				indexes = gen_indexes(indexes, i, ProcNum);
				// Рассылка индексов чисел в b, каждый процесс будет умножать число a на b[index]
				MPI_Scatter(indexes, 1, MPI_INT, indexes, 1, MPI_INT, 0, MPI_COMM_WORLD);

				res_child = (int *)malloc(sizeof(int) * ProcNum * len_struct);
				ans_child = (int *)malloc(sizeof(int) * (len_struct));

				// Получение ответа от всех процессов
				MPI_Gather(ans_child, 1, mpi_type_arr_num, res_child, 1, mpi_type_arr_num, 0, MPI_COMM_WORLD);

				printf("Proc MAIN Gather OK\n");
				fflush(0); // сброс буфера
				printf("Proc MAIN index0 %d\n", res_child[1]);
				fflush(0); // сброс буфера

				// Складываем все массивы
				int k = 0;
				while (k < len_struct * ProcNum) {
					sum_two_long(answer_master, &res_child[k + 1], res_child[k], len_b);
					k += len_struct;
				}

				// Отправляем результат всем процессам
				for (int l = 1; l < ProcNum; l++)
				{
					MPI_Send(answer_master, 1, mpi_type_ans, l, 0, MPI_COMM_WORLD);
				}

				printf("Proc MAIN OK\n");
				fflush(0); // сброс буфера
				free(ans_child);
				free(res_child);
			} else {
				int b_num_index;

				// Получаем от мастера индекс в массиве b
				MPI_Scatter(&b_num_index, 1, MPI_INT, &b_num_index, 1, MPI_INT, 0, MPI_COMM_WORLD);
				printf("Proc child get index: %d\n", b_num_index);
				fflush(0); // сброс буфера

				ans_child = (int *) malloc(sizeof(int) * (len_struct));
				ans_child[0] = b_num_index;
				dot_single(a, b[b_num_index], ans_child, len);

				// отправляем мастеру массив, полученный в результате умножения
				MPI_Gather(ans_child, 1, mpi_type_arr_num, res_child, 1, mpi_type_arr_num, 0, MPI_COMM_WORLD);

				printf("CHILD index %d\n", ans_child[0]);
				fflush(0); // сброс буфера

				printf("Proc child OK\n");
				fflush(0); // сброс буфера

				// Получаем результат
				MPI_Status status;
				MPI_Recv(answer_master, 1, mpi_type_ans, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

				free(ans_child);
			}
		}
		a = answer_master;
		len = len_b * 2;
	}

	free(indexes);
	MPI_Type_free(&mpi_type_arr_num);
	printf("Done! Res: ");
	return (answer_master);
}


int mainnn(int argc, char* argv[])
{
	if (argc != 3)
		return (0);

	int ProcNum, ProcRank;
	int N = atoi(argv[1]); // Длина целых чисел
	int count = atoi(argv[2]); // Количество умножений
	int *num = (int *)malloc(sizeof(int) * N);
	int *res;

	init_num(num, N, 2);
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	// <программный код с использованием MPI функций>
	if (ProcRank == 0)
	{
		printf("Будем перемножать %d раза число:\n", count);
		fflush(0); // сброс буфера
		print_num(num, N);
	}

	res = lab4(num, num, N, ProcRank, ProcNum, count);

	MPI_Finalize();

	print_num(res, N * 2);
	free(num);
	return (0);
}