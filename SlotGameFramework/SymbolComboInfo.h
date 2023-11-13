#pragma once

#include "General.h"

// This class contains the info about every symbol combo using all reels
// (standard lines from left to right, evaluated one by one)

class SymbolComboInfo
{
public:
	enum multiplierType
	{
		PRODUCT,
		SUM,
		MAX,
	};

	SymbolComboInfo(int numReels, int numSymbols, map<int, vector<double>> paytable, map<int, set<int>> wildMapping, map<int, int> symbolMultipliers, multiplierType multType = PRODUCT);
	~SymbolComboInfo();

	double GetComboInfo(size_t symbolkey)
	{
		return m_combos[symbolkey];
	}

private:
	set<int> SetIntersect(set<int> seta, set<int> setb);
	void EvaluateSymbolCombos(int reel = 0, double pay = 0, int multiplier = 1, size_t symbol_key = 0, set<int> possible_symbols = {});

	int m_numReels;
	int m_numSymbols;
	multiplierType m_multType;
	map<int, set<int>> m_wildMapping;
	map<int, int> m_symbolMultipliers;
	map<int, vector<double>> m_paytable;
	vector<double> m_combos;
};
