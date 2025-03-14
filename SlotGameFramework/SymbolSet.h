#pragma once

#include "General.h"

class SymbolSet
{
public:
	SymbolSet(vector<string> symbols = {}, map<string, set<string>> symbolSubstitutions = {}, map<string, int> symbolMultipliers = {}, map<string, Colors> symbolColors = {});
	
private:
	int numSymbols = 0;
	vector<string> symbols;
	map<string, int> symbolIndex;
	map<string, set<string>> symbolSubstitutions;
	map<int, set<int>> symbolIndexSubstitutions;
	map<string, int> symbolMultipliers;
	map<int, int> symbolIndexMultipliers;
	map<string, Colors> symbolColors;
	map<int, Colors> symbolIndexColors;
};