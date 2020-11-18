//
// Created by snorcros on 10/4/20.
//
#include <time.h>
#include "mpi_labs.h"

/**
 * Реализуйте тип «комплексная матрица».
 * Напишите программу, которая осуществляет
 * умножение A матриц размером NxN методом Штрассена.*/

// Умножение двух матриц A и B, результат записываем в D
void  mult(t_complexMatrix *A, t_complexMatrix *B, t_complexMatrix *D, int N)
{
	t_complexMatrix sum;
	sum.Re = 0;
	sum.Im = 0;

	for (int i = 0; i < N; i++)
		for (int l = 0; l < N; l++)
		{
			sum.Re = 0;
			sum.Im = 0;
			for (int j = 0; j < N; j++)
			{
				sum.Re += A[i * N + j].Re * B[j * N + l].Re - A[i * N + j].Im * B[j * N + l].Im;
				sum.Im += A[i * N + j].Re * B[j * N + l].Im + B[j * N + l].Re * A[i * N + j].Im;
			}
			D[i * N + l].Re = sum.Re;
			D[i * N + l].Im = sum.Im;
		}
}

void rand_complex(t_complexMatrix *complex)
{
	srand((unsigned) time(NULL));
	complex->Re = 1.0 + rand() % 9;
	complex->Im = 2.0 + rand() % 9;
}

void build_type_complex(MPI_Datatype *new_type)
{
	MPI_Type_contiguous(2, MPI_INT, new_type);
	MPI_Type_commit(new_type);
}

void lab5(t_complexMatrix Matrix, int count_proc, int t, int N, int ProcNum, int ProcRank)
{
	MPI_Datatype mpi_complex;
	t_complexMatrix **matrix_child; // cодержит матрицы для умножения конкретным процессом
	t_complexMatrix *resmul_child; // общий результат умножения
	t_complexMatrix **res_child; // результат умножения конкретного процесса-ребёнка

	t_complexMatrix *ans_child;
	t_complexMatrix *origin_matrix;
	t_complexMatrix *resmul_origin;

	matrix_child = (t_complexMatrix *)malloc(sizeof(t_complexMatrix) * (count_proc + 1));
	res_child = (t_complexMatrix *)malloc(sizeof(t_complexMatrix) * (count_proc + 1));
	for (int i = 0; i <= count_proc; i++)
	{
		matrix_child[i] = (t_complexMatrix *)malloc(sizeof(t_complexMatrix) * (N * N));
		res_child[i] = (t_complexMatrix *)malloc(sizeof(t_complexMatrix) * (N * N));
	}
	ans_child = (t_complexMatrix *)malloc(sizeof(t_complexMatrix) * (N * N));
	origin_matrix = (t_complexMatrix *)malloc(sizeof(t_complexMatrix) * (N * N));
	resmul_origin = (t_complexMatrix *)malloc(sizeof(t_complexMatrix) * (N * N));

	// Cоздаём новый тип
	build_type_complex(&mpi_complex);
	if (ProcNum - 1 >= count_proc)
	{
		count_proc++;
		// заполняем матрицы
		if (ProcRank >= 0 && ProcRank < count_proc)
		{
			resmul_child = (t_complexMatrix *)malloc(sizeof(t_complexMatrix) * ((count_proc + 1) * N * N));
			for (int i = 0; i < N * N; i++)
			{
				rand_complex(&matrix_child[ProcRank][i]);
				rand_complex(&origin_matrix[i]);
			}
			printf("first element: %d + %d\n", matrix_child[ProcRank][0].Re, matrix_child[ProcRank][0].Im);
			fflush(0);
		}

		MPI_Group MPI_GROUP_WORLD;
		MPI_Group new_group;
		MPI_Comm comm;
		int *process_ranks;	// ранги процессов, которые войдут в данную группу

		// Получаем группу, относящуюся к MPI_COMM_WORLD
		MPI_Comm_group(MPI_COMM_WORLD, &MPI_GROUP_WORLD);
		process_ranks= (int *)malloc(sizeof(int) * count_proc);
		for (int i = 0; i < count_proc; i++)
		{
			process_ranks[i] = i;
		}

		// Создаём новую группу
		MPI_Group_incl(MPI_GROUP_WORLD, count_proc, process_ranks, &new_group);
		//Создаём новый коммуникатор
		MPI_Comm_create(MPI_COMM_WORLD, new_group, &comm);

		// умножаем матрицы
		if (ProcRank >= 1 && ProcRank < count_proc)
		{
			mult(matrix_child[ProcRank], matrix_child[ProcRank], res_child[ProcRank], N);
			printf("Proc%d first element res: %d + %d\n", ProcRank, res_child[ProcRank]->Re, res_child[ProcRank]->Im);
			fflush(0);
		}

		if (ProcRank >= 0 && ProcRank < count_proc)
		{
			// отправляем результат умножения главному процессу
			MPI_Gather(res_child[ProcRank], N * N, mpi_complex, resmul_child, N * N, mpi_complex, 0, comm);

			printf("Proc%d send: %d + %d\n", ProcRank, res_child[ProcRank]->Re, res_child[ProcRank]->Im);
			fflush(0);
		}

		if (ProcRank == 0)
		{
			if (!t) // если чётное количесво умножений
				for (int i = 0; i < N * N; i++)
				{
					origin_matrix[i].Re = resmul_child[2 * N * N + i].Re;
					origin_matrix[i].Im = resmul_child[2 * N * N + i].Im;
				}

			printf("origin get res from CHILD(proc1): %d + %d\n", resmul_child[2 * N * N].Re, resmul_child[2 * N * N].Im);
			fflush(0);

			// умножение полученных матриц
			for (int i = !t + 1; i < count_proc; i++)
			{
				for (int j = 0; j < N * N; j++)
				{
					ans_child[j].Re = resmul_child[i * N * N + j].Re;
					ans_child[j].Im = resmul_child[i * N * N + j].Im;
				}
				mult(origin_matrix, ans_child, resmul_origin, N);
				for (int j = 0; j < N * N; j++)
				{
					origin_matrix[j].Re = resmul_origin[j].Re;
					origin_matrix[j].Im = resmul_origin[j].Im;
				}
			}
			// Печатаем результат
			printf("\nProc%d Result:\n", ProcRank);
			fflush(0);
			for (int k = 0; k < N; k++)
			{
				for (int j = 0; j < N; j++)
				{
					printf("%d + %d im\t\t", origin_matrix[k * N + j].Re, origin_matrix[k * N + j].Im);
					fflush(0);
				}
				printf("\n");
				fflush(0);
			}
		}
	}

	if (ProcRank >= 0 && ProcRank < count_proc)
		free(resmul_child);
	free(ans_child);
	free(origin_matrix);
	free(resmul_origin);
	MPI_Type_free(&mpi_complex);
}

int mainn(int agrc, char* argv[])
{
	int ProcNum, ProcRank;
	t_complexMatrix Matrix;
	int N; // размерность матрицы
	int A; // сколько матриц надо перемножить
	int countProc; // количество процессов в группе
	int t; // 0 - чётное 1 - не чётное количество умножений

	N = 5;
	A = 4;
	t = 0;
	if (A % 2 == 0)
		countProc = A / 2;
	else
	{
		countProc = A / 2;
		t = 1;
	}

	MPI_Init(&agrc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	lab5(Matrix, countProc, t, N, ProcNum, ProcRank);

	MPI_Finalize();
	return 0;
}