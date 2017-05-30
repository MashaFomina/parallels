#include <iostream>
#include <random>
#include <fstream>
#include <string>

using namespace std;

int main()
{
	int sizeArr[] = {1000, 10000, 50000, 100000, 250000, 500000, 25000000};
	default_random_engine generator;
	uniform_int_distribution<int> distribution(0,1000);
	default_random_engine generator1;
	exponential_distribution<double> distribution1(0.005);
	default_random_engine generator2;
	normal_distribution<double> distribution2(500.0,200.0);

	

	for (int k=0; k<(sizeof(sizeArr)/sizeof(*sizeArr)); k++)
	{
		int *ptrArr;
		int size = sizeArr[k];
		ptrArr = (int*) malloc(size * sizeof(int));
		if(ptrArr == NULL)                     
		{
			printf("Error! memory not allocated.");
			exit(0);
		}

		for (int i=0; i<size; ++i) {
			int number = distribution(generator);
	 		ptrArr[i] = number;
		}

		ofstream myfile ("tests/testUniform" + to_string(size) + ".txt");
		if (myfile.is_open())
		{
			myfile << size << " ";
			for(int count = 0; count < size; count ++){
	 			myfile << ptrArr[count] << " ";
			}
			myfile.close();
		}

		for (int i=0; i<size; ++i) {
			double number = distribution1(generator1);
			ptrArr[i] = number;
		}
		ofstream myfile1 ("tests/testExp" + to_string(size) + ".txt");
		if (myfile1.is_open())
		{
			myfile1 << size << " ";
			for(int count = 0; count < size; count ++){
	 			myfile1 << ptrArr[count] << " ";
			}
			myfile1.close();
		}

		for (int i=0; i<size; ++i) {
			int number = distribution2(generator2);
	 		ptrArr[i] = number;
		}

		ofstream myfile2 ("tests/testNorm" + to_string(size) + ".txt");
		if (myfile2.is_open())
		{
			myfile2 << size << " ";
			for(int count = 0; count < size; count ++){
	 			myfile2 << ptrArr[count] << " ";
			}
			myfile2.close();
		}
		free(ptrArr);
	}

	return 0;
}
