#include "mpi_labs.h"

/* Реализуйте функцию star,
которая создаёт N+1 процессов
(1 «центральный» и N «крайних»)
и посылает сообщение центральному процессу,
который посылает сообщение всем остальным процессам и дожидается от них ответа,
после чего это повторяется (всего M раз). После того, как все события получены,
все процессы заканчивают работу.*/
int proc_master(int ProcNum)
{
	MPI_Status status;
	int msg_master = 42;
	int response;

	for (int i = 1; i < ProcNum; i++)
	{
		MPI_Send(&msg_master, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
	}
	for (int i = 1; i < ProcNum; i++)
	{
		MPI_Recv(&response, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		printf("Proc0 get response from proc%d:\t%d\n", status.MPI_SOURCE, response);
		fflush(0);
	}
	return (0);
}

int proc_child(int ProcRank)
{
	MPI_Status status;
	int msg_child = 5;
	int request;

	MPI_Recv(&request, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	printf("Proc%d get response from proc%d:\t%d\n", ProcRank, status.MPI_SOURCE, request);
	fflush(0); // сброс буфера

	MPI_Send(&msg_child, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

	printf("\n");
	fflush(0); // сброс буфера

	return (0);
}

int star(int ProcNum, int ProcRank, int m)
{
	for (int i = 0; i < m; i++)
	{
		// printf("%d/%d\n", ProcRank, ProcNum);
		if (ProcRank == 0) {
			proc_master(ProcNum);
		}
		else {
			proc_child(ProcRank);
		}
	}
	return (0);
}


/* Модифицировать программу, написанную на Л.Р. №1,
   так чтобы она работала на основе коллективной передачи сообщений
*/
int star_broadcast(int ProcNum, int ProcRank, int m)
{
	int msg_master = 42;
	int response;
	int msg_child = 5;
	int request_child;

	for (int i = 0; i < m; i++)
	{
		if (ProcRank == 0)
		{
			// Рассылка данных всем процессам
			MPI_Bcast(&msg_master, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// Получение ответа от всех процессов
			MPI_Reduce(&msg_child, &response, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

			printf("Master get %d\n", response);
			fflush(0);
		}
		else
		{
			// Все дети получают сообщение от мастера
			MPI_Bcast(&request_child, 1, MPI_INT, 0, MPI_COMM_WORLD);
			printf("Proc%d get response from master:\t%d\n", ProcRank, request_child);
			fflush(0); // сброс буфера
			// Все child отправляют сообщение master
			MPI_Reduce(&msg_child, NULL, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
		}
	}
	return (0);
}

/* Модифицировать программу, написанную на Л.Р. №1,
   так чтобы она работала на основе обобщенной передачи сообщений.
   Процесс передаёт процессам различающиеся данные.
*/
int star_scatter(int ProcNum, int ProcRank, int m)
{
	int *msg_master = (int*)malloc(sizeof(int) * ProcNum);
	int response;
	int *response_master =(int*)malloc(sizeof(int) * ProcNum);
	int msg_child = 5;
	int request_child;

	for (int i = 0; i < ProcNum; i++)
	{
		msg_master[i] = 42;
	}
	for (int i = 0; i < m; i++)
	{
		if (ProcRank == 0)
		{
			// Рассылка данных всем процессам
			MPI_Scatter(msg_master, 1, MPI_INT, &request_child, 1, MPI_INT, 0, MPI_COMM_WORLD);
			// Получение ответа от всех процессов
			MPI_Gather(&msg_child, 1, MPI_INT, response_master, 1, MPI_INT, 0, MPI_COMM_WORLD);

			for (int i = 0; i < ProcNum - 1; i++)
			{
				printf("Master get response from proc%d: %d\n", i+1, response_master[i]);
				fflush(0);
			}
		}
		else
		{
			// Все дети получают сообщение от мастера
			MPI_Scatter(msg_master, 1, MPI_INT, &request_child, 1, MPI_INT, 0, MPI_COMM_WORLD);

			printf("Proc%d get response from master:\t%d\n", ProcRank, request_child);
			fflush(0); // сброс буфера
			// Все child отправляют сообщение master
			//MPI_Reduce(&msg_child, NULL, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
			MPI_Gather(&msg_child, 1, MPI_INT, response_master, 1, MPI_INT, 0, MPI_COMM_WORLD);
		}
	}
	return (0);
}

int labs123(int argc, char* argv[])
{
	int ProcNum, ProcRank;

//	if (argc != 2)
//		return (0);
//	int m = atoi(argv[1]);
	int m = 5;

	// <программный код без использования MPI функций>
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	// <программный код с использованием MPI функций>

	//lab1 star(ProcNum, ProcRank, m);
	//lab2 star_broadcast(ProcNum, ProcRank, m);
	//lab3
	star_scatter(ProcNum, ProcRank, m);

	MPI_Finalize();
	// <программный код без использования MPI функций>
	return 0;
}