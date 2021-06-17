#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>
#include <iostream>

int main(int *argc, char* argv[])
{
	int size, rank;
	MPI_Init(argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	std::cout << "Hi!";

	MPI_Finalize();

	return 0;
}