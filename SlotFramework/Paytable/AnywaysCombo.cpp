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
			if (symbolSet.GetSubstitutions(symbolOnReel).contains(symbolOnReel))
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
	paytable.resize(numSymbols, vector<Combo>(numReels));
}

int AnywaysCombo::GetSymbolSubstitutionCount(int symbolForCombo, int symbolOnReel)
{
	return symbolSubstitutionCount[symbolForCombo][symbolOnReel];
}

Combo AnywaysCombo::GetCombo(int symbol, int comboLength)
{
	return paytable[symbol][comboLength];
}

void AnywaysCombo::SetCombo(int symbol, int comboLength, double pay, int bonusCode, int progressive)
{
	Combo& combo = paytable[symbol][comboLength];
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
