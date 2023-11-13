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

	void GetComboInfo(size_t symbolkey, int reel, double& pay, bool& breaks)
	{
		symbolCombo thisSymbolCombo = m_combos[reel][symbolkey];
		pay = thisSymbolCombo.pay;
		breaks = thisSymbolCombo.breaks;
		return;
	}
	size_t GetSymbolLocation(const int reel, const int symbol)
	{
		return m_combo_location[reel][symbol];
	}

private:
	set<int> SetIntersect(set<int> seta, set<int> setb);
	void EvaluateSymbolCombos(int reel = 0, double pay = 0, int multiplier = 1, size_t symbol_key = 0, set<int> possible_symbols = {});

	struct symbolCombo
	{
		symbolCombo()
		{
			pay = 0;
			breaks = false;
		};
		double pay;
		bool breaks;
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
