//
// Created by snorcros on 9/18/20.
//
#include "openmp.h"

const int NMAX=100;
const int LIMIT=10;

void check_example(int NMAX, int LIMIT)
{
	int i, j;
	float sum;
	float a[NMAX][NMAX];
	unsigned int start_time, end_time, search_time;

	for (i = 0; i < NMAX; i++)
		for (j = 0; j < NMAX; j++)
			a[i][j] = i + j;
	if (NMAX > LIMIT)
		printf("parral\t");
	else
		printf("sequent\t");

	start_time =  clock();
	#pragma omp parallel shared(a) if (NMAX > LIMIT)
	{
		#pragma omp for private(i, j, sum)
		for (i = 0; i < NMAX; i++)
		{
			sum = 0;
			for (j = 0; j < NMAX; j++)
				sum += a[i][j];
//			printf("Cумма элементов строки %d равна %f\n", i, sum);
		}
	}
	end_time = clock(); // конечное время
	search_time = end_time - start_time; // искомое время
	printf("%lf\n", search_time / 1000.0);
}


/*Зако́н Амдала — иллюстрирует ограничение роста производительности
 * вычислительной системы с увеличением количества вычислителей.*/
int lab2(int argc, char **argv)
{
	int nmax[] = {10, 50, 100, 200, 250, 1000};
	int limits[] = {9, 49, 100, 3000};

	printf("NMAX\tLIMIT\ttype\tTime\n");
	for (int i = 0; i < 6; i++)
		for (int j = 0; j < 4; j++)
		{
			printf("%d\t\t%d\t\t", nmax[i], limits[j]);
			check_example(nmax[i], limits[j]);
		}
}