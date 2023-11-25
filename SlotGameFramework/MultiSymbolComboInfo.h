#pragma once

#include "General.h"

// This class contains the info about every possible symbol combination
// It is used when efficiently evaluating grid lines
// (standard lines from left to right, evaluated in binary tree)

class MultiSymbolComboInfo
{
public:
	enum multiplierType
	{
		PRODUCT,
		SUM,
		MAX,
	};

	MultiSymbolComboInfo(int numReels, int numSymbols, map<int, vector<double>> paytable, map<int, set<int>> wildMapping, map<int, int> symbolMultipliers, multiplierType multType = PRODUCT);
	~MultiSymbolComboInfo();

	void GetComboInfo(size_t symbolkey, int reel, double& pay, bool& breaks);
	size_t GetSymbolLocation(const int reel, const int symbol);

private:
	set<int> SetIntersect(set<int> seta, set<int> setb);
	void EvaluateSymbolCombos(int reel = 0, double pay = 0, int multiplier = 1, size_t symbol_key = 0, set<int> possible_symbols = {});

	struct symbolCombo
	{
		double pay = 0;
		bool breaks = false;
	};

	int m_numReels;
	int m_numSymbols;
	multiplierType m_multType;
	map<int, set<int>> m_wildMapping;
	map<int, int> m_symbolMultipliers;
	map<int, vector<double>> m_paytable;
	vector<vector<symbolCombo>> m_combos;
	vector<vector<size_t>> m_combo_location;
};
