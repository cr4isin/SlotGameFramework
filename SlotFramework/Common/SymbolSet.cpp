#include "SymbolSet.h"

SymbolSet::SymbolSet(vector<string> symbols, map<string, set<string>> symbolSubstitutions, map<string, int> symbolMultipliers, map<string, ColorStyle> symbolColors)
	: symbols(symbols),
	numSymbols(symbols.size())
{
	// Creating the symbol index map
	for (int iSymbol = 0; iSymbol < numSymbols; iSymbol++)
	{
		symbolIndex[symbols[iSymbol]] = iSymbol;
		maxSymbolLength = max(maxSymbolLength, symbols[iSymbol].size());
	}

	// Creating the symbol substitution map
	this->symbolSubstitutions.resize(numSymbols);
	for (int iSymbol = 0; iSymbol < numSymbols; iSymbol++)
	{
		set<int> substitutions;
		substitutions.insert(iSymbol);
		for (string symbol : symbolSubstitutions[symbols[iSymbol]])
		{
			substitutions.insert(symbolIndex[symbol]);
		}
		this->symbolSubstitutions[iSymbol] = substitutions;
	}

	// Inverting the symbol substitution map
	inverseSymbolSubstitutions.resize(numSymbols);
	for (int iSymbol = 0; iSymbol < numSymbols; iSymbol++)
	{
		for (int subSymbol : this->symbolSubstitutions[iSymbol])
		{
			inverseSymbolSubstitutions[subSymbol].insert(iSymbol);
		}
	}

	// Creating the symbol multiplier map
	this->symbolMultipliers.resize(numSymbols);
	for (int iSymbol = 0; iSymbol < numSymbols; iSymbol++)
	{
		if (symbolMultipliers.contains(symbols[iSymbol]))
		{
			this->symbolMultipliers[iSymbol] = symbolMultipliers[symbols[iSymbol]];
		}
		else
		{
			this->symbolMultipliers[iSymbol] = 1;
		}
	}

	// Creating the symbol color map
	this->symbolColors.resize(numSymbols);
	for (int iSymbol = 0; iSymbol < numSymbols; iSymbol++)
	{
		if (symbolColors.contains(symbols[iSymbol]))
		{
			this->symbolColors[iSymbol] = symbolColors[symbols[iSymbol]];
		}
		else
		{
			this->symbolColors[iSymbol] = 0;
		}
	}
}

int SymbolSet::GetSymbolIndex(const string& symbol) const 
{
	return symbolIndex.at(symbol);
}

std::string SymbolSet::GetSymbolString(int symbol) const 
{
	return symbols[symbol];
}

std::set<int> SymbolSet::GetSubstitutions(int symbol) const 
{
	return symbolSubstitutions[symbol];
}

set<int> SymbolSet::GetInverseSubstitutions(int symbol) const
{
	return inverseSymbolSubstitutions[symbol];
}

int SymbolSet::GetMultiplier(int symbol) const 
{
	return symbolMultipliers[symbol];
}

ColorStyle SymbolSet::GetColor(int symbol) const
{
	return symbolColors[symbol];
}

int SymbolSet::GetNumSymbols() const 
{
	return numSymbols;
}

int SymbolSet::GetMaxSymbolLength() const
{
	return maxSymbolLength;
}

const std::vector<std::string>& SymbolSet::GetSymbolList() const 
{
	return symbols;
}

int SymbolSet::operator[](const string& symbol) const {
	return symbolIndex.at(symbol);
}
