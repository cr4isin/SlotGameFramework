#pragma once
#include "General.h"

class DynamicWeightTable
{
public:
	DynamicWeightTable(vector<long long> weights = {1});
	DynamicWeightTable(vector<long long> weights, vector<double> values);
	int GetIndexOfWeight(long long weight);
	void Call(long long& weight, int& index, double& value);
	void Reset();
	void ResetIndex(int index);
private:
	vector<long long> m_weights;
	vector<double> m_values;
	vector<long long> currentWeights;
	long long totalWeight = 0;
	long long currentTotalWeight = 0;
	mt19937_64 weightRNG;
	uniform_int_distribution<long long> weightGenerator;
};