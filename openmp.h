//
// Created by snorcros on 10/1/20.
//

#ifndef OPENMP_OPENMP_H
#define OPENMP_OPENMP_H

#include <cstdio>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <unistd.h>

// Lab1
void	print_matrix(int **matrix, int M, int N);
void	fill_matrix(int **matrix, int M, int N);
void	free_table(int **arr, int position);
int		**new_table(int m, int n);
double	count_seven(int num);
double	seven_in_str(int *arr, int len);
int		lab1(int argc, char **argv);

// Lab2
void	check_example(int NMAX, int LIMIT);
int		lab2(int argc, char **argv);

// Lab3
int		parall_reduction(int *arr_A, int *arr_B, int *arr_C, int N, int flag);
void	fill_arr(int *arr, int N);
void	print_arr(int *arr, int N);
int		lab3(int argc, char **argv);

// Lab4
int parall_sync(int *arr_A, int *arr_B, int *arr_C, int N, int flag);
int lab4(int argc, char **argv);

// Lab5
int distributed(int *arr_A, int *arr_B, int *arr_C, int N, int flag);
int lab5(int argc, char **argv);

// Lab6
int semaphore(int *arr_A, int *arr_B, int *arr_C, int N, int flag);
int barrier(int *arr_A, int *arr_B, int *arr_C, int N, int flag);
int lab6(int argc, char **argv);

#endif //OPENMP_OPENMP_H
