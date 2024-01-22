
#include "WeightTable.h"

WeightTable::WeightTable(vector<long long> weights)
{
	m_weights = weights;
	int numZeroes = 0;
	long long totalWeight = 0;
	for (int i = 0; i < m_weights.size(); i++)
	{
		if (m_weights.at(i) > 0)
		{
			totalWeight += m_weights.at(i);
			m_cumulativeWeights.push_back(totalWeight);
			m_numZeroIndexes.push_back(numZeroes);
		}
		else
		{
			numZeroes++;
		}
		m_values.push_back(i);
	}
	if (totalWeight <= 0)
	{
		throw;
	}
	random_device rd;
	weightRNG.seed(rd());
	weightGenerator = uniform_int_distribution<long long>(0, totalWeight - 1);
}

WeightTable::WeightTable(vector<long long> weights, vector<double> values)
{
	m_weights = weights;
	m_values = values;
	int numZeroes = 0;
	long long totalWeight = 0;
	for (int i = 0; i < m_weights.size(); i++)
	{
		if (m_weights.at(i) > 0)
		{
			totalWeight += m_weights.at(i);
			m_cumulativeWeights.push_back(totalWeight);
			m_numZeroIndexes.push_back(numZeroes);
		}
		else
		{
			numZeroes++;
		}
	}
	if (totalWeight <= 0)
	{
		throw;
	}
	if (m_weights.size() != m_values.size())
	{
		throw;
	}
	random_device rd;
	weightRNG.seed(rd());
	weightGenerator = uniform_int_distribution<long long>(0, totalWeight - 1);
}

int WeightTable::GetIndexOfWeight(long long weight)
{
	int low = 0;
	int high = m_cumulativeWeights.size();

	while (low < high)
	{
		int mid = (low + high) / 2;

		if (weight < m_cumulativeWeights[mid])
		{
			high = mid;
		}
		else
		{
			low = mid + 1;
		}
	}

	return low + m_numZeroIndexes[low];
}

void WeightTable::Call(long long &weight, int &index, double &value)
{
	weight = weightGenerator(weightRNG);
	index = GetIndexOfWeight(weight);
	value = m_values[index];
}
