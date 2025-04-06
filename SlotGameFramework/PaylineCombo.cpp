#include "PaylineCombo.h"

PaylineCombo::PaylineCombo(int numReels, const SymbolSet& symbolSet)
    : numReels(numReels),
	numSymbols(symbolSet.GetNumSymbols()),
    symbolSet(symbolSet)
{
    // Resize combos to hold all possible symbol combos
    int totalCombos = static_cast<int>(pow(numSymbols, numReels));
    combos.resize(totalCombos);

    // Pre-calculate symbol keys
    symbolKeys.resize(numReels);
    for (int iReel = 0; iReel < numReels; iReel++)
    {
        symbolKeys[iReel].resize(numSymbols);
        for (int symbol = 0; symbol < numSymbols; symbol++)
        {
            symbolKeys[iReel][symbol] = symbol * pow(numSymbols, iReel);
        }
    }
}

size_t PaylineCombo::GetSymbolKey(const vector<int>& combo) const {
    size_t key = 0;
    for (int iReel = 0; iReel < numReels; iReel++)
    {
        key += symbolKeys[iReel][combo[iReel]];
    }
    return key;
}

size_t PaylineCombo::GetSymbolKey(const int& reel, const int& symbol) const
{
    return symbolKeys[reel][symbol];
}

PaylineCombo::Combo PaylineCombo::GetCombo(const vector<int>& combo) const {
    size_t key = GetSymbolKey(combo);
    return combos[key];
}

void PaylineCombo::SetCombo(const vector<int>& combo, double basePay, int bonusCode, bool overWrite)
{
	vector<size_t> comboSymbolKeys = { 0 };
	for (int iReel = 0; iReel < numReels; iReel++)
	{
		vector<size_t> newComboSymbolKeys;
		int symbol = combo[iReel];

		if (symbol < 0) // ANY
		{
			for (int symbolKey : comboSymbolKeys)
			{
				for (int subSymbol = 0; subSymbol < numSymbols; subSymbol++)
				{
					newComboSymbolKeys.push_back(symbolKey + symbolKeys[iReel][subSymbol]);
				}
			}
		}
		else
		{
			for (int symbolKey : comboSymbolKeys)
			{
				for (int subSymbol : symbolSet.GetInverseSubstitutions(symbol))
				{
					newComboSymbolKeys.push_back(symbolKey + symbolKeys[iReel][subSymbol]);
				}
			}
		}

		comboSymbolKeys = newComboSymbolKeys;
	}
	for (int symbolKey : comboSymbolKeys)
	{
		vector<int> currentCombo = ChangeBase(symbolKey, numSymbols, numReels);
		int multiplier = 1;
		for (int symbol : currentCombo)
		{
			multiplier *= symbolSet.GetMultiplier(symbol);
		}
		Combo& comboInfo = combos[symbolKey];
		if (!comboInfo.set || overWrite)
		{
			comboInfo.basePay = basePay;
			comboInfo.multiplier = multiplier;
			comboInfo.totalPay = comboInfo.basePay * comboInfo.multiplier;
			comboInfo.bonusCode = bonusCode;
			comboInfo.set = true;
		}
	}
}

int PaylineCombo::GetNumSymbols() const
{
	return numSymbols;
}

int PaylineCombo::GetNumReels() const
{
	return numReels;
}
