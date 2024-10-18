
#include "WeightTable.h"

WeightTable::WeightTable()
{
	throw runtime_error("Tried to access a weight table that does not exist");
}

WeightTable::WeightTable(vector<long long> weights, vector<double> values)
{
	this->weights = weights;
	this->values = values;

	// Validate Values
	if (values.size() == 0)
	{
		for (int i = 0; i < weights.size(); i++)
		{
			this->values.push_back(i);
		}
	}
	else if (weights.size() != values.size())
	{
		throw runtime_error("Weight Table size does not match the Value Table");
	}

	// Set up Cumulative Weights and Zero Indexes
	int numZeroes = 0;
	for (int i = 0; i < this->weights.size(); i++)
	{
		if (this->weights.at(i) > 0)
		{
			totalWeight += this->weights.at(i);
			cumulativeWeights.push_back(totalWeight);
			numZeroIndexes.push_back(numZeroes);
		}
		else
		{
			this->weights.at(i) = 0;
			numZeroes++;
		}
	}
	if (totalWeight <= 0)
	{
		throw runtime_error("Weight Table has no positive weights");
	}
	weightsWithoutReplacement = this->weights;
	totalWeightWithoutReplacement = totalWeight;

	// Set up RNG devices
	random_device rd;
	weightRNG.seed(rd());
	weightGenerator = uniform_int_distribution<long long>(0, totalWeight - 1);
	weightGeneratorWithoutReplacement = uniform_int_distribution<long long>(0, totalWeightWithoutReplacement - 1);
}

int WeightTable::GetIndexOfWeight(long long weight)
{
	int low = 0;
	int high = cumulativeWeights.size();

	while (low < high)
	{
		int mid = (low + high) / 2;

		if (weight < cumulativeWeights[mid])
		{
			high = mid;
		}
		else
		{
			low = mid + 1;
		}
	}

	return low + numZeroIndexes[low];
}

void WeightTable::Draw(long long &weight, int &index, double &value)
{
	weight = weightGenerator(weightRNG);
	index = GetIndexOfWeight(weight);
	value = values[index];
}

long long WeightTable::DrawWeight()
{
	long long weight = weightGenerator(weightRNG);
	return weight;
}

int WeightTable::DrawIndex()
{
	long long weight = weightGenerator(weightRNG);
	int index = GetIndexOfWeight(weight);
	return index;
}

double WeightTable::DrawValue()
{
	long long weight = weightGenerator(weightRNG);
	int index = GetIndexOfWeight(weight);
	double value = values[index];
	return value;
}

int WeightTable::GetIndexOfWeightWithoutReplacement(long long weight)
{
	int index = -1;
	while (weight >= 0)
	{
		weight -= weightsWithoutReplacement[++index];
	}
	return index;
}

void WeightTable::DrawWithoutReplacement(long long& weight, int& index, double& value)
{
	if (totalWeightWithoutReplacement <= 0)
	{
		throw runtime_error("Dynamic Weight Table is empty");
	}
	weight = weightGenerator(weightRNG);
	index = GetIndexOfWeightWithoutReplacement(weight);
	value = values[index];
	weightsWithoutReplacement[index] = 0;
	totalWeightWithoutReplacement -= weights[index];
	if (totalWeightWithoutReplacement > 0)
	{
		weightGenerator = uniform_int_distribution<long long>(0, totalWeightWithoutReplacement - 1);
	}
}

long long WeightTable::DrawWeightWithoutReplacement()
{
	long long weight;
	int index;
	double value;
	DrawWithoutReplacement(weight, index, value);
	return weight;
}

int WeightTable::DrawIndexWithoutReplacement()
{
	long long weight;
	int index;
	double value;
	DrawWithoutReplacement(weight, index, value);
	return index;
}

double WeightTable::DrawValueWithoutReplacement()
{
	long long weight;
	int index;
	double value;
	DrawWithoutReplacement(weight, index, value);
	return value;
}

void WeightTable::ResetTable()
{
	totalWeightWithoutReplacement = totalWeight;
	weightsWithoutReplacement = weights;
	weightGenerator = uniform_int_distribution<long long>(0, totalWeightWithoutReplacement - 1);
}

void WeightTable::ResetIndex(int index)
{
	totalWeightWithoutReplacement += weights[index] - weightsWithoutReplacement[index];
	weightsWithoutReplacement[index] = weights[index];
	weightGenerator = uniform_int_distribution<long long>(0, totalWeightWithoutReplacement - 1);
}

void WeightTable::ZeroIndex(int index)
{
	totalWeightWithoutReplacement -= weightsWithoutReplacement[index];
	weightsWithoutReplacement[index] = 0;
	weightGenerator = uniform_int_distribution<long long>(0, totalWeightWithoutReplacement - 1);
}
