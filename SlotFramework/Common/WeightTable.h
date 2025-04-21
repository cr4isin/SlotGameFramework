#pragma once
#include "General.h"

class WeightTable
{
public:
	WeightTable() = default;
	WeightTable(vector<long long> weights, vector<double> values = {});
	// Normal Draws
	int GetIndexOfWeight(long long weight);
	void Draw(long long& weight, int& index, double& value);
	long long DrawWeight();
	int DrawIndex();
	double DrawValue();
	// Draws Without Replacement
	int GetIndexOfWeightWithoutReplacement(long long weight);
	void DrawWithoutReplacement(long long& weight, int& index, double& value);
	long long DrawWeightWithoutReplacement();
	int DrawIndexWithoutReplacement();
	double DrawValueWithoutReplacement();
	// Methods for Drawing Without Replacement
	void ResetTable();
	void ResetIndex(int index);
	void ZeroIndex(int index);
private:
	vector<long long> weights;
	vector<double> values;
	vector<long long> weightsWithoutReplacement;
	long long totalWeight = 0;
	long long totalWeightWithoutReplacement = 0;
	vector<long long> cumulativeWeights;
	vector<int> numZeroIndexes;
	mt19937_64 weightRNG;
	uniform_int_distribution<long long> weightGenerator;
	uniform_int_distribution<long long> weightGeneratorWithoutReplacement;
};