#include "SymbolSet.h"

SymbolSet::SymbolSet(vector<string> symbols, map<string, set<string>> symbolSubstitutions, map<string, int> symbolMultipliers, map<string, Colors> symbolColors)
{
	// Setting the symbol list and their indexes
	this->symbols = symbols;
	numSymbols = symbols.size();
	for (int iSymbol = 0; iSymbol < numSymbols; iSymbol++)
	{
		symbolIndex[symbols[iSymbol]] = iSymbol;
	}

	// Setting the symbol substitution maps
	for (int iSymbol = 0; iSymbol < numSymbols; iSymbol++)
	{
		set<string> substitutions = symbolSubstitutions[symbols[iSymbol]];
		substitutions.insert(symbols[iSymbol]);
		this->symbolSubstitutions[symbols[iSymbol]] = substitutions;

		set<int> indexSubstitutions;
		for (string sub : substitutions)
		{
			indexSubstitutions.insert(symbolIndex[sub]);
		}
		symbolIndexSubstitutions[iSymbol] = indexSubstitutions;
	}

	// Setting the symbol multiplier maps
	for (int iSymbol = 0; iSymbol < numSymbols; iSymbol++)
	{
		if (symbolMultipliers.contains(symbols[iSymbol]))
		{
			this->symbolMultipliers[symbols[iSymbol]] = symbolMultipliers[symbols[iSymbol]];
			symbolIndexMultipliers[iSymbol] = symbolMultipliers[symbols[iSymbol]];
		}
		else
		{
			this->symbolMultipliers[symbols[iSymbol]] = 1;
			symbolIndexMultipliers[iSymbol] = 1;
		}
	}

	// Setting the symbol color maps
	for (int iSymbol = 0; iSymbol < numSymbols; iSymbol++)
	{
		if (symbolColors.contains(symbols[iSymbol]))
		{
			this->symbolColors[symbols[iSymbol]] = symbolColors[symbols[iSymbol]];
			symbolIndexColors[iSymbol] = symbolColors[symbols[iSymbol]];
		}
		else
		{
			this->symbolColors[symbols[iSymbol]] = tWHITE;
			symbolIndexColors[iSymbol] = tWHITE;
		}
	}
}
