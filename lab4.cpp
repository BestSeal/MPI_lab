#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>
#include <iostream>
#include <vector>
#include <algorithm>


std::vector<int> multiply(std::vector<int> A, std::vector<int> B)
{
	std::vector<int> res_poly(A.size() + B.size() - 1, 0);

	for (int i = 0; i < A.size(); i++)
	{
		for (int j = 0; j < B.size(); j++)
		{
			res_poly[i + j] += A[i] * B[j];
		}
			
	}
	res_poly.resize(A.size());
	return res_poly;
}

void printPoly(std::vector<int> poly)
{
	for (int i = 0; i < poly.size(); i++)
	{
		if (poly[i] == 0)
		{
			continue;
		}
		if (i != 0)
		{
			std::cout << " + " << poly[i] << "x^" << i;
		}
		else
		{
			std::cout << poly[i];
		}
	}
	std::cout << std::endl;
}

int main(int* argc, char* argv[])
{
	std::vector<std::vector<int>> polies =
	{
		{ 2, 1, 2, 3, 4, 5},
		{ 1, 6, 5, 4},
		{ 0, 1, 2, 3, 4, 5},
		{ 1, 2, 1, 4, 3},
	};

	int poly_size = polies.size() * polies[0].size() - 1;
	std::vector<int> res_poly(poly_size, 0);

	for (int i = 0; i < polies.size(); i++)
	{
		polies[i].resize(poly_size);
	}

	int* tmp_poly1 = new int[poly_size];
	int* tmp_poly2 = new int[poly_size];
	int shouldListen = 1;
	std::vector<int> tmpBuf1;
	std::vector<int> tmpBuf2;
	int size, rank;

	MPI_Init(argc, &argv);

	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	MPI_Datatype MPI_Poly;
	MPI_Type_contiguous(poly_size, MPI_INT, &MPI_Poly);
	MPI_Type_commit(&MPI_Poly);

	if (rank == 0)
	{
		int incomingPolies = 0;
		while (polies.size() > 1)
		{
			for (int i = 1; i < size; i++)
			{
				MPI_Send(&shouldListen, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
				std::cout << "Send to process " << i << std::endl;
				MPI_Send(&polies.back().front(), 1, MPI_Poly, i, 1, MPI_COMM_WORLD);
				polies.pop_back();
				printPoly(polies.back());
				MPI_Send(&polies.back().front(), 1, MPI_Poly, i, 1, MPI_COMM_WORLD);
				polies.pop_back();
				++incomingPolies;
				if (polies.size() <= 1)
				{
					break;
				}
			}
			std::cout << "Incoming polies: " << incomingPolies << std::endl;
			for (int i = 1; i <= incomingPolies; i++)
			{
				std::cout << "Recieving from process " << i << std::endl;
				MPI_Recv(tmp_poly1, 1, MPI_Poly, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				polies.push_back(std::vector<int>(tmp_poly1, tmp_poly1 + poly_size));
				std::cout << "Recieved from process " << i << std::endl;
			}
			std::cout << "Polies size " << polies.size() << std::endl;
			incomingPolies = 0;
		}
		shouldListen = 0;
		printPoly(polies[0]);
		for (int i = 1; i < size; i++)
		{
			MPI_Send(&shouldListen, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
		}
	}
	else
	{
		while (true)
		{
			MPI_Recv(&shouldListen, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			if (shouldListen != 1)
			{
				break;
			}
			std::vector<int> res_poly(poly_size, 0);
			MPI_Recv(tmp_poly1, 1, MPI_Poly, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			std::cout << "Recieved poly 1 " << std::endl;
			MPI_Recv(tmp_poly2, 1, MPI_Poly, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			std::cout << "Recieved poly 2 " << std::endl;
			res_poly = multiply(std::vector<int>(tmp_poly1, tmp_poly1 + poly_size), std::vector<int>(tmp_poly2, tmp_poly2 + poly_size));
			MPI_Send(&res_poly.front(), 1, MPI_Poly, 0, 1, MPI_COMM_WORLD);

		}
		std::cout << "Out " << rank << std::endl;
	}
	MPI_Finalize();
	return 0;
}
