#include "DynamicWeightTable.h"

DynamicWeightTable::DynamicWeightTable()
{
	throw runtime_error("Tried to access a weight table that does not exist");
}

DynamicWeightTable::DynamicWeightTable(vector<long long> weights)
{
	m_weights = weights;
	for (int i = 0; i < m_weights.size(); i++)
	{
		if (m_weights.at(i) >= 0)
		{
			totalWeight += m_weights.at(i);
		}
		else
		{
			m_weights.at(i) = 0;
		}
		m_values.push_back(i);
	}
	if (totalWeight <= 0)
	{
		throw runtime_error("Weight Table has no positive weights");
	}
	currentWeights = m_weights;
	currentTotalWeight = totalWeight;
	random_device rd;
	weightRNG.seed(rd());
	weightGenerator = uniform_int_distribution<long long>(0, currentTotalWeight - 1);
}

DynamicWeightTable::DynamicWeightTable(vector<long long> weights, vector<double> values)
{
	m_weights = weights;
	m_values = values;
	for (int i = 0; i < m_weights.size(); i++)
	{
		if (m_weights.at(i) >= 0)
		{
			totalWeight += m_weights.at(i);
		}
		else
		{
			m_weights.at(i) = 0;
		}
	}
	if (totalWeight <= 0)
	{
		throw runtime_error("Weight Table has no positive weights");
	}
	if (m_weights.size() != m_values.size())
	{
		throw runtime_error("Weight Table size does not match the Value Table");
	}
	currentWeights = m_weights;
	currentTotalWeight = totalWeight;
	random_device rd;
	weightRNG.seed(rd());
	weightGenerator = uniform_int_distribution<long long>(0, currentTotalWeight - 1);
}

int DynamicWeightTable::GetIndexOfWeight(long long weight)
{
	int index = -1;
	while (weight >= 0)
	{
		weight -= currentWeights[++index];
	}
	return index;
}

void DynamicWeightTable::Call(long long& weight, int& index, double& value)
{
	if (currentTotalWeight <= 0)
	{
		throw runtime_error("Dynamic Weight Table is empty");
	}
	weight = weightGenerator(weightRNG);
	index = GetIndexOfWeight(weight);
	value = m_values[index];
	currentWeights[index] = 0;
	currentTotalWeight -= m_weights[index];
	if (currentTotalWeight > 0)
	{
		weightGenerator = uniform_int_distribution<long long>(0, currentTotalWeight - 1);
	}
}

void DynamicWeightTable::Reset()
{
	currentWeights = m_weights;
	currentTotalWeight = totalWeight;
	weightGenerator = uniform_int_distribution<long long>(0, currentTotalWeight - 1);
}

void DynamicWeightTable::ResetIndex(int index)
{
	currentWeights[index] = m_weights[index];
	currentTotalWeight += m_weights[index];
	weightGenerator = uniform_int_distribution<long long>(0, currentTotalWeight - 1);
}

void DynamicWeightTable::ZeroIndex(int index)
{
	currentTotalWeight -= currentWeights[index];
	currentWeights[index] = 0;
	weightGenerator = uniform_int_distribution<long long>(0, currentTotalWeight - 1);
}
