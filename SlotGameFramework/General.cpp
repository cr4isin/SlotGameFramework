
#include "General.h"

void PrintVec(vector<int> input)
{
	for (int i : input)
	{
		cout << i << " ";
	}
	cout << "\n\n";
}

vector<int> ChangeBase(int input, int base, int length)
{
	int quot = abs(input);
	int rem = 0;
	if (length <= 0)
	{
		if (quot <= 0)
		{
			length = 1;
		}
		else
		{
			length = 1 + log(quot) / log(base);
		}
	}
	vector<int> baseVector(length);
	for (int i = 0; i < length; i++)
	{
		rem = quot % base;
		quot /= base;
		baseVector[i] = rem;
	}
	return baseVector;
}
