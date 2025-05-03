#include "AnywaysCombo.h"

AnywaysCombo::AnywaysCombo(int numReels, const SymbolSet& symbolSet)
	: numReels(numReels),
	numSymbols(symbolSet.GetNumSymbols()),
	symbolSet(symbolSet)
{
	// Create the symbol substitution count for each symbol
	symbolSubstitutionCount.resize(numSymbols);
	for (int symbolForCombo = 0; symbolForCombo < numSymbols; symbolForCombo++)
	{
		symbolSubstitutionCount[symbolForCombo].resize(numSymbols);
		for (int symbolOnReel = 0; symbolOnReel < numSymbols; symbolOnReel++)
		{
			if (symbolSet.GetSubstitutions(symbolOnReel).contains(symbolForCombo))
			{
				symbolSubstitutionCount[symbolForCombo][symbolOnReel] = symbolSet.GetMultiplier(symbolOnReel);
			}
			else
			{
				symbolSubstitutionCount[symbolForCombo][symbolOnReel] = 0;
			}
		}
	}

	// Setup paytable size
	anywaysCombos.resize(numSymbols, vector<Combo>(numReels + 1));
	combos.resize(numSymbols);
}

int AnywaysCombo::GetSymbolSubstitutionCount(const int& symbolForCombo,const int& symbolOnReel)
{
	return symbolSubstitutionCount[symbolForCombo][symbolOnReel];
}

Combo AnywaysCombo::GetCombo(int symbol, int comboLength)
{
	return anywaysCombos[symbol][comboLength];
}

void AnywaysCombo::SetCombo(int symbol, int comboLength, double pay, int bonusCode, int progressive)
{
	Combo& combo = anywaysCombos[symbol][comboLength];
	combo.pay = pay;
	combo.basePay = pay;
	combo.bonusCode = bonusCode;
	combo.progressive = progressive;
}

int AnywaysCombo::GetNumSymbols() const
{
	return numSymbols;
}

int AnywaysCombo::GetNumReels() const
{
	return numReels;
}

void AnywaysCombo::AddResult(const int& symbol, const int& comboLength, const int& numCombos)
{
	Combo& combo = anywaysCombos[symbol][comboLength];
	combo.multiplier = numCombos;
	combo.pay = combo.basePay * numCombos;
	combos[symbol] = combo;
	pay += combo.pay;

	if (combo.bonusCode > 0)
	{
		bonusHit = true;
		bonusCodes.push_back(combo.bonusCode);
	}
	if (combo.progressive > 0)
	{
		progHit = true;
		progressives.push_back(combo.progressive);
	}
}

void AnywaysCombo::ClearResults()
{
	pay = 0;
	bonusCodes.clear();
	bonusHit = false;
	progressives.clear();
	progHit = false;
}

void AnywaysCombo::PrintResults()
{
	if (pay > 0 || bonusHit || progHit)
	{
		for (int iSymbol = 0; iSymbol < numSymbols; iSymbol++)
		{
			int comboLength = 0;
			for (int iReel = 1; iReel <= numReels; iReel++) {
				if (anywaysCombos[iSymbol][iReel].basePay == combos[iSymbol].basePay)
				{
					comboLength = iReel;
					break;
				}
			}
			if (combos[iSymbol].pay > 0)
			{
				cout << comboLength << "-" << GetColorCode(symbolSet.GetColor(iSymbol)) << symbolSet.GetSymbolString(iSymbol) << "\033[0m x " << combos[iSymbol].multiplier << " Ways pays " << combos[iSymbol].pay << "\n";
			}
			if (combos[iSymbol].bonusCode > 0)
			{
				cout << comboLength << "-" << GetColorCode(symbolSet.GetColor(iSymbol)) << symbolSet.GetSymbolString(iSymbol) << "\033[0m x " << combos[iSymbol].multiplier << " hit bonus code " << combos[iSymbol].bonusCode << "\n";
			}
			if (combos[iSymbol].progressive > 0)
			{
				cout << comboLength << "-" << GetColorCode(symbolSet.GetColor(iSymbol)) << symbolSet.GetSymbolString(iSymbol) << "\033[0m x " << combos[iSymbol].multiplier << " awards progressive " << combos[iSymbol].progressive << "\n";
			}
		}
		cout << "\n";
	}
}
