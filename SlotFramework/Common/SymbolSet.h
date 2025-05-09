#pragma once

#include "General.h"

class SymbolSet
{
public:
	SymbolSet(vector<string> symbols = {}, map<string, set<string>> symbolSubstitutions = {}, map<string, int> symbolMultipliers = {}, map<string, ColorStyle> symbolColors = {});
	int GetSymbolIndex(const string& symbol) const;
	string GetSymbolString(int symbol) const;
	set<int> GetSubstitutions(int symbol) const;
	set<int> GetInverseSubstitutions(int symbol) const;
	int GetMultiplier(int symbol) const;
	ColorStyle GetColor(int symbol) const;
	int GetNumSymbols() const;
	int GetMaxSymbolLength() const;
	const vector<string>& GetSymbolList() const;
	int operator[](const std::string& symbol) const;

private:
	int numSymbols;
	int maxSymbolLength = 3;
	vector<string> symbols;
	map<string, int> symbolIndex;
	vector<set<int>> symbolSubstitutions;
	vector<set<int>> inverseSymbolSubstitutions;
	vector<int> symbolMultipliers;
	vector<ColorStyle> symbolColors;
};