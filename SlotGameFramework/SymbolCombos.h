#pragma once

#include "General.h"

// This class calculates the pay for every possible combination of symbols on a line
// Standard lines from left-to-right, evaluated one by one
// Set bothWays to true to evaluate from left-to-right and right-to-left. (Max win is used per line.)

class SymbolCombos
{
public:
	SymbolCombos();
	SymbolCombos(int numReels, int numSymbols, map<int, vector<double>> paytable, map<int, set<int>> wildMapping, map<int, int> symbolMultipliers);
	SymbolCombos(int numReels, int numSymbols, map<int, set<int>> wildMapping, map<int, int> symbolMultipliers);
	void GetComboInfo(size_t symbolkey, double& pay, int& bonusCode);
	size_t GetSymbolLocation(const int reel, const int symbol);
	void SetCombo(vector<int> combo, double pay, int bonusCode, bool overWrite = false);

private:
	void EvaluateSymbolCombos(int reel = 0, double pay = 0, int multiplier = 1, size_t symbol_key = 0, set<int> possible_symbols = {});
	struct Combo
	{
		double basePay = 0;
		double totalPay = 0;
		int multiplier = 1;
		int bonusCode = 0;
		bool set = false;
	};
	int m_numReels;
	int m_numSymbols;
	map<int, set<int>> m_wildMapping;
	map<int, set<int>> m_inverseWildMapping;
	map<int, int> m_symbolMultipliers;
	map<int, vector<double>> m_paytable;
	vector<Combo> m_combos;
	vector<vector<size_t>> m_combo_location;
};
