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

typedef struct {
	int* A;
	int* B;
	int l;
	int r;
} Params;

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
void* nrmsort(void* data)
//A - сортируемый массив, B - буфер для слияния, l - левая граница сортируемого участка, r - правая граница
{
	// for scheduller
	struct sched_param shdprm;
	shdprm.sched_priority=50;
	if(sched_setscheduler(getpid(), SCHED_RR, &shdprm)==-1)
        	printf("Warning. SCHED_RR is not set.\n");

 	Params* params = (Params*) data;
	int* A = params->A;
	int* B = params->B;
	int l = params->l;
	int r = params->r;

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
	if (tA != A)
	{
		//Переписать элементы в исходный массив А
		for (int k = l; k<= r; k++)
		{
			A[k] = B[k];
		}
	}
}

//Сортировка слиянием, распараллеленная
int mp_sort(int* A, int* B, int N, int P)
//A - сортируемый массив, B - буфер для слияния, N - размер массива, P - число параллельно выполняемых потоков
{
	int *tA = A;
	int *tB = B;
	int *t = NULL;
	// Разбиение на блоки и сортировка их по отдельности
	int i;
	int part_size = (int)ceil((float)N/(float)P);

	int index[P];
	int endsOfparts[P];

	struct timeval  tv;
	gettimeofday(&tv, NULL);
	double time_begin = ((double)tv.tv_sec) * 1000 + ((double)tv.tv_usec) / 1000;

	pthread_t * threads = new pthread_t[P];
	Params params[P];
	for (int i = 0; i < P; ++i) {
		params[i] = {tA,tB,i*part_size,min((i+1)*(part_size)-1,N-1)};
		index[i] = i*part_size;
		endsOfparts[i] = min((i+1)*(part_size)-1,N-1);
		if (pthread_create(&threads[i], NULL, nrmsort, (void *)&params[i])) {
			printf("Can not create thread!");
			return -3;
		}
	}

	int retVal = 0;
	for (auto i = 0; i < P ; ++i) {
		if (pthread_join(threads[i], (void **)&retVal)) {
			printf("Can not join thread!\n");
			return -3;
		}
		//printf("By thread %d was returned value %d\n", i, retVal);
	}
	delete[] threads;

	gettimeofday(&tv, NULL);
	double time_end = ((double)tv.tv_sec) * 1000 + ((double)tv.tv_usec) / 1000 ;
	double total_time_ms = time_end - time_begin;
	cout << "Time spent for parallel part (ms): " << total_time_ms << endl;

	int r2,m;
	tA = A;
	tB = B;

	// merge sorted parts
	bool end = false;
	int min_index, min, k = 0;
	if (P > 1)
	{
		while (!end)
		{
			bool min_is_set = false;
			for (int i = 0; i < P; ++i)
			{
				if ((i == 0 || min > tA[index[i]] || !min_is_set) && index[i] <= endsOfparts[i]) {
					min_index = i;
					min = tA[index[i]];
					min_is_set = true;
				}	
			}
			tB[k]=min;
			k++;
			index[min_index]++;
			end = true;
			for (int i = 0; i < P; ++i)
			{
				if (index[i] <= endsOfparts[i]) {
					end = false;
				}	
			}
		}
	}
}

int main(int argc, char** argv)
{
	if(argc < 3) {
		printf("You must specify input file and number of threads!\n");
		return -1;
	}
	string file = string(argv[1]);
	int numThreads = atoi(argv[2]);
	if(numThreads < 1) {
		printf("Number of threads must be bigger than 0!\n");
		return -1;
	}

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

		mp_sort(ptrArr, ptrArrSorted, count, numThreads);

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
