//
// Created by snorcros on 10/2/20.
//

#ifndef MPI_MPI_LABS_H
#define MPI_MPI_LABS_H

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct	s_arr_num
{
	int index; // индекс элемента в массиве b, на который мы умножаем
	int *arr;
}				t_arr_num;

typedef struct	s_complexMatrix
{
	int	Re;
	int	Im;
}				t_complexMatrix;

void init_num(int *num, int len, int simple);
void print_num(int *num, int len);

#endif //MPI_MPI_LABS_H
