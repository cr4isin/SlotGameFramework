#pragma once
#include "General.h"

class WeightTable
{
public:
	WeightTable(vector<long long> weights);
	WeightTable(vector<long long> weights, vector<double> values);
	int GetIndexOfWeight(long long weight);
	void Call(long long& weight, int& index, double& value);
private:
	vector<long long> m_weights;
	vector<double> m_values;
	vector<long long> m_cumulativeWeights;
	vector<int> m_numZeroIndexes;
	mt19937_64 weightRNG;
	uniform_int_distribution<long long> weightGenerator;
};