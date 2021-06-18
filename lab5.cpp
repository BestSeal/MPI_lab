#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string> 

void PrintLargeNumber(std::vector<long> number)
{
	std::string largeNumber;
	int separator = 0;
	while (number.size() > 0)
	{
		if (separator == 3)
		{
			largeNumber.append(" ");
			separator = 0;
		}
		largeNumber.append(std::to_string(number.back()));
		number.pop_back();
		separator++;
	}
	std::reverse(largeNumber.begin(), largeNumber.end());
	std::cout << largeNumber << std::endl;
}

std::vector<long> SumVects(std::vector<long> v1, std::vector<long> v2)
{

	std::reverse(v1.begin(), v1.end());
	std::reverse(v2.begin(), v2.end());
	if (v1.size() < v2.size())
	{
		std::swap(v1, v2);
	}
	long buffer = 0;
	for (int i = 0; i < v2.size(); i++)
	{
		buffer = v1[i] + v2[i];
		v1[i] = buffer % 10;
		if (v1.size() - 1 > i)
		{
			v1[i + 1] += buffer / 10;
			buffer = 0;
		}
	}

	if (buffer > 9)
	{
		v1.push_back(buffer / 10);
	}
	std::reverse(v1.begin(), v1.end());
	return v1;
}

long VectorToLong(std::vector<long> vector)
{
	std::reverse(vector.begin(), vector.end());
	long decimal = 1;
	long outNumber = 0;
	for (int i = 0; i < vector.size(); i++)
	{
		outNumber += vector[i] * decimal;
		decimal *= 10;
	}

	return outNumber;
}

std::vector<std::vector<long>> SplitVectorTo3(std::vector<long>* inputVectopr)
{
	std::vector<std::vector<long>> outParts;
	{
		if (inputVectopr->size() % 3 == 0)
		{
			outParts.push_back(std::vector<long>(inputVectopr->begin(), inputVectopr->begin() + inputVectopr->size() / 3));
			outParts.push_back(std::vector<long>(inputVectopr->begin() + inputVectopr->size() / 3, inputVectopr->begin() + inputVectopr->size() / 3 * 2));
			outParts.push_back(std::vector<long>(inputVectopr->begin() + inputVectopr->size() / 3 * 2, inputVectopr->end()));
		}
		else if (inputVectopr->size() % 3 == 1)
		{
			outParts.push_back(std::vector<long>(inputVectopr->begin(), inputVectopr->begin() + inputVectopr->size() / 3 + 1));
			outParts.push_back(std::vector<long>(inputVectopr->begin() + inputVectopr->size() / 3 + 1, inputVectopr->begin() + inputVectopr->size() / 3 * 2 + 1));
			outParts.push_back(std::vector<long>(inputVectopr->begin() + inputVectopr->size() / 3 * 2 + 1, inputVectopr->end()));
		}
		else
		{
			outParts.push_back(std::vector<long>(inputVectopr->begin(), inputVectopr->begin() + inputVectopr->size() / 3 + 1));
			outParts.push_back(std::vector<long>(inputVectopr->begin() + inputVectopr->size() / 3 + 1, inputVectopr->begin() + inputVectopr->size() / 3 * 2 + 2));
			outParts.push_back(std::vector<long>(inputVectopr->begin() + inputVectopr->size() / 3 * 2 + 2, inputVectopr->end()));
		}
	}
	return outParts;
}

std::vector<long> longToVect(long num)
{
	long temp = num;
	std::vector<long> out;
	while (temp > 9)
	{
		out.push_back(temp % 10);
		temp /= 10;
	}
	out.push_back(temp);
	std::reverse(out.begin(), out.end());
	return out;
}

std::vector<long> parts5_tovect(std::vector<long> vectA, std::vector<long> vectB, std::vector<long> vectC, std::vector<long> vectD, std::vector<long> vectE, long shift)
{
	vectA.resize(vectA.size() + 3 * 4 );
	vectB.resize(vectB.size() + 3 * 3);
	vectC.resize(vectC.size() + 3 * 2);
	vectD.resize(vectD.size() + 3);
	vectD = SumVects(vectD, vectE);
	vectC = SumVects(vectC, vectD);
	vectB = SumVects(vectB, vectC);
	vectA = SumVects(vectA, vectB);

	return vectA;
}

std::vector<long> check(std::vector<long> v1, std::vector<long> v2);
std::vector<long> Toom_3(std::vector<long> num1, std::vector<long> num2)
{
	std::vector<std::vector<long>> splittedNum1 = SplitVectorTo3(&num1);
	std::vector<std::vector<long>> splittedNum2 = SplitVectorTo3(&num2);
	std::vector<long> a, b, b_part1, b_part2, c, c_part1, c_part2, c_part3, d, d_part1, d_part2, e;
	//a
	a = check(splittedNum1[0], splittedNum2[0]);
	//b
	b_part1 = check(splittedNum1[1], splittedNum2[0]);
	b_part2 = check(splittedNum1[0], splittedNum2[1]);
	b = SumVects(b_part1, b_part2);
	//c
	c_part1 = check(splittedNum1[2], splittedNum2[0]);
	c_part2 = check(splittedNum1[1], splittedNum2[1]);
	c_part3 = check(splittedNum1[0], splittedNum2[2]);
	c = SumVects(c_part1, c_part2);
	c = SumVects(c, c_part3);
	//d
	d_part1 = check(splittedNum1[2], splittedNum2[1]);
	d_part2 = check(splittedNum1[1], splittedNum2[2]);
	d = SumVects(d_part1, d_part2);
	//e
	e = check(splittedNum1[2], splittedNum2[2]);
	long shift = std::max({ 
			splittedNum1[0].size(), 
			splittedNum1[1].size(),
			splittedNum1[2].size(),
			splittedNum2[0].size(),
			splittedNum2[1].size(),
			splittedNum2[2].size()
			});
	std::vector<long> result = parts5_tovect(a, b, c, d, e, shift);
	return result;
}

std::vector<long> check(std::vector<long> v1, std::vector<long> v2)
{
	if (v1.size() > 6 || v2.size() > 6)
	{
		return Toom_3(v1, v2);
	}
	else
	{
		return longToVect(VectorToLong(v1) * VectorToLong(v2));
	}
}

int main(int* argc, char* argv[])
{
	std::vector<long> num1 = { 1, 1, 1, 1, 1, 1, 4, 1};
	std::vector<long> num2 = { 2, 2, 2, 2, 2, 2, 2};
	std::vector<long> num3 = { 1, 1, 1, 1, 1, 2, 3, 1};
	std::vector<long> num4 = { 2, 2, 2, 2, 1, 3, 4};

	long largeNumberSize = num1.size() + num2.size() - 1;
	int size, rank;

	MPI_Init(argc, &argv);

	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Datatype MPI_Large_Number;
	MPI_Type_contiguous(largeNumberSize, MPI_LONG, &MPI_Large_Number);
	MPI_Type_commit(&MPI_Large_Number);

	long* buffer1 = new long[largeNumberSize];
	long* buffer2 = new long[largeNumberSize];
	std::vector<long> result;
	std::vector<long> v1;
	std::vector<long> v2;

	if (rank == 0)
	{
		MPI_Send(&num1, 1, MPI_Large_Number, 1, 1, MPI_COMM_WORLD);
		MPI_Send(&num2, 1, MPI_Large_Number, 1, 1, MPI_COMM_WORLD);
		MPI_Send(&num3, 1, MPI_Large_Number, 2, 1, MPI_COMM_WORLD);
		MPI_Send(&num4, 1, MPI_Large_Number, 2, 1, MPI_COMM_WORLD);
		MPI_Recv(buffer1, 1, MPI_Large_Number, 1, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(buffer2, 1, MPI_Large_Number, 2, 1, MPI_COMM_WORLD, &status);
		PrintLargeNumber(Toom_3(std::vector<long>(buffer1, buffer1 + largeNumberSize), std::vector<long>(buffer2, buffer2 + largeNumberSize)));
	}
	if (rank == 1)
	{
		MPI_Recv(buffer1, 1, MPI_Large_Number, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(buffer2, 1, MPI_Large_Number, 0, 1, MPI_COMM_WORLD, &status);
		v1 = std::vector<long>(buffer1, buffer1 + largeNumberSize);
		v1.resize(num1.size());
		v2 = std::vector<long>(buffer2, buffer2 + largeNumberSize);
		v2.resize(num2.size());
		result = Toom_3(v1, v2);
		MPI_Send(&result, 1, MPI_Large_Number, 0, 1, MPI_COMM_WORLD);
	}
	if (rank == 2)
	{
		MPI_Recv(buffer1, 1, MPI_Large_Number, 0, 1, MPI_COMM_WORLD, &status);
		MPI_Recv(buffer2, 1, MPI_Large_Number, 0, 1, MPI_COMM_WORLD, &status);
		v1 = std::vector<long>(buffer1, buffer1 + largeNumberSize);
		v1.resize(num3.size());
		v2 = std::vector<long>(buffer2, buffer2 + largeNumberSize);
		v2.resize(num4.size());
		result = Toom_3(v1, v2);
		MPI_Send(&result, 1, MPI_Large_Number, 0, 1, MPI_COMM_WORLD);
	}

	MPI_Type_free(&MPI_Large_Number);
	MPI_Finalize();
	return 0;
}