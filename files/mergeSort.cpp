#include <algorithm>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <math.h>
#include <ctime>
#include <sched.h>      // scheduller
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

using namespace std;

//Слияние двух расположенных рядом друг с другом частей массива
void merge(int *A, int *B, int l, int m, int r)
//A - сортируемый массив, B - буфер для слияния, l - первый элемент первой части, r - последний элемент второй части, m - последний элемент первой части
{
	int i=l;
	int j = m+1;
	int k=l;
	//Вставлять минимальные элементы в B пока не кончится одна из последовательностей
	while ((i<=m) && (j<= r))
	{
		if (A[i]<A[j])
		{
			B[k] = A[i];
			i++;
		} else {
			B[k] = A[j];
			j++;
		}
		k++;
	}
	//Скопировать остаток, если таковой имеется
	while (i <= m)
	{
		B[k]=A[i];
		k++;
		i++;
	}
	while (j <= r)
	{
		B[k]=A[j];
		k++;
		j++;
	}
	//Отсортированная часть остаётся в B
	/*printf("Some part: ");
	for(int i = l; i <= r; i++) {
		printf("%d ", B[i]);
	}
	printf("\n");*/
}

//Сортировка слиянием, без рекурсии
void* nrmsort(int* A, int* B, int l, int r)
//A - сортируемый массив, B - буфер для слияния, l - левая граница сортируемого участка, r - правая граница
{
	int p=2;
	int m;
	int i;
	int r2;
	int N = r-l+1;
	int *tA = A;
	int *tB = B;
	int *t = NULL;
	//printf("nrmsort(): l - %d, r - %d, N - %d\n", l, r, N);

	while(p<(2*N))
	{
		for(i=l;i<=r;i+=p)
		{
			r2 = min(i+p-1,r);
			m = min(r,((i+i+p-1) >> 1));
			//printf("i - %d, r2 - %d, m - %d, p - %d\n", i, r2, m, p);
			merge(tA,tB,i,m,r2);
		}
		p *= 2; //Вдвое увеличим размер сливаемых частей
		t = tA; //Поменяем сортируемый массив и буфер местами
		tA = tB;
		tB = t;
		t = NULL;
	}
}

int main(int argc, char** argv)
{
	if(argc < 2) {
		printf("You must specify input file and number of threads!\n");
		return -1;
	}
	string file = string(argv[1]);

	// for scheduller
	struct sched_param shdprm;
	shdprm.sched_priority=50;
	if(sched_setscheduler(getpid(), SCHED_RR, &shdprm)==-1)
        	printf("Warning. SCHED_RR is not set.\n");

	ifstream in(file);
	if(in.is_open())
    	{
		int count = 0; // Count of elements
		in >> count; // Read count of elements
		int *ptrArr, *ptrArrSorted;
		ptrArr = (int*) malloc(count * sizeof(int)); // Array of integers
		ptrArrSorted = (int*) malloc(count * sizeof(int));
		for(int i = 0; i < count; i++) {
		    in >> ptrArr[i];
		}

		struct timeval  tv;
		gettimeofday(&tv, NULL);
		double time_begin = ((double)tv.tv_sec) * 1000 + ((double)tv.tv_usec) / 1000;

		nrmsort(ptrArr, ptrArrSorted, 0, count - 1);

		gettimeofday(&tv, NULL);
		double time_end = ((double)tv.tv_sec) * 1000 + ((double)tv.tv_usec) / 1000 ;
		double total_time_ms = time_end - time_begin;
		cout << "Time spent (ms): " << total_time_ms << endl;
		/*printf("Sorted array: ");
		for(int i = 0; i < count; i++) {
		    printf("%d ", ptrArrSorted[i]);
		}
		printf("\n");*/
		free(ptrArr);
		free(ptrArrSorted);
    	}
	else
	{
		printf("File can not be opened!\n");
		return -2;
	}
    	return 0;
}
