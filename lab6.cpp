#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string> 
#include <complex>

constexpr double PI = 3.14;
using complex = std::complex<double>;

void PrintLargeNumber(std::vector<int> number)
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

void FFT(std::vector<complex>& a, bool invert = false) {

	int n = (int)a.size();
	if (n == 1)  return;

	std::vector<std::complex<double>> a0(n / 2), a1(n / 2);

	for (int i = 0, j = 0; i < n; i += 2, ++j) 
	{
		a0[j] = a[i];
		a1[j] = a[i + 1];
	}

	FFT(a0, invert);
	FFT(a1, invert);

	double ang = 2 * PI / n * (invert ? -1 : 1);
	complex w(1), wn(cos(ang), sin(ang));
	for (int i = 0; i < n / 2; ++i) 
	{
		a[i] = a0[i] + w * a1[i];
		a[i + n / 2] = a0[i] - w * a1[i];
		if (invert)
			a[i] /= 2, a[i + n / 2] /= 2;
		w *= wn;
	}
}

void multiply(const std::vector<int>& num1, const std::vector<int>& num2, std::vector<int>& result) {
	std::vector<complex> fNum1(num1.begin(), num1.end()), fNum2(num2.begin(), num2.end());
	size_t n = 1;

	while (n < std::max(num1.size(), num2.size()))
	{
		n <<= 1;
	}

	n <<= 1;

	fNum1.resize(n), fNum2.resize(n);

	FFT(fNum1), FFT(fNum2);
	for (size_t i = 0; i < n; ++i)
	{
		fNum1[i] *= fNum2[i];
	}
	FFT(fNum1, true);

	result.resize(n);

	for (size_t i = 0; i < n; ++i)
	{
		result[i] = int(fNum1[i].real() + 0.5);
	}
}

std::vector<int> PolyToNumber(std::vector<int> poly)
{
	while (true)
	{
		if (poly.back() <= 0)
		{
			poly.pop_back();
		}
		else
		{
			break;
		}
	}

	int reminder = 0;
	for (size_t i = poly.size() - 1; i > 0; --i)
	{
		poly[i] += reminder;
		reminder = poly[i] / 10;
		poly[i] %= 10;
	}

	poly[0] += reminder;
	reminder = poly[0] / 10;
	poly[0] %= 10;

	while (reminder > 9)
	{
		poly.insert(poly.begin(), reminder & 10);
		reminder /= 10;
	}
	if (reminder > 0)
	{
		poly.insert(poly.begin(), reminder);
	}
	return poly;
}

int main(int* argc, char* argv[])
{
	std::vector<std::vector<int>> nums = {
		{1,2,3,4,5},
		{2,3,4,5,6},
		{5,4,3,2,1},
		{2,9,4,3,2},
	};
	int resSize = 0;
	for (int i = 0; i < nums.size(); i++)
	{
		resSize += nums[i].size();
	}
	std::vector<int> result;

	int mul1Size;
	int mul2Size;
	std::vector<int> mul1;
	std::vector<int> mul2;

	int* resultTmp1 = new int[resSize];
	int* resultTmp2 = new int[resSize];
	int size, rank;

	MPI_Init(argc, &argv);

	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	MPI_Datatype MPI_Large_Number;
	MPI_Type_contiguous(resSize, MPI_INT, &MPI_Large_Number);
	MPI_Type_commit(&MPI_Large_Number);

	if (rank == 0)
	{
		mul1Size = nums[0].size();
		mul2Size = nums[1].size();
		MPI_Send(&mul1Size, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
		MPI_Send(&mul2Size, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
		MPI_Send(&nums[0].front(), 1, MPI_Large_Number, 1, 1, MPI_COMM_WORLD);
		MPI_Send(&nums[1].front(), 1, MPI_Large_Number, 1, 1, MPI_COMM_WORLD);
		mul1Size = nums[2].size();
		mul2Size = nums[3].size();
		MPI_Send(&mul1Size, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
		MPI_Send(&mul2Size, 1, MPI_INT, 2, 1, MPI_COMM_WORLD);
		MPI_Send(&nums[2].front(), 1, MPI_Large_Number, 2, 1, MPI_COMM_WORLD);
		MPI_Send(&nums[3].front(), 1, MPI_Large_Number, 2, 1, MPI_COMM_WORLD);

	}
	else if( rank == 1 || rank == 2)
	{
		std::cout << "go" << std::endl;
		MPI_Recv(&mul1Size, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(&mul2Size, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(resultTmp1, 1, MPI_Large_Number, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		MPI_Recv(resultTmp2, 1, MPI_Large_Number, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		mul1 = std::vector<int>(resultTmp1, resultTmp1 + mul1Size);
		mul2 = std::vector<int>(resultTmp2, resultTmp2 + mul2Size);
		PrintLargeNumber(PolyToNumber(mul1));
		PrintLargeNumber(PolyToNumber(mul2));
		multiply(mul1, mul2, result);
		PrintLargeNumber(PolyToNumber(result));
	}

	MPI_Type_free(&MPI_Large_Number);
	MPI_Finalize();
	return 0;
}