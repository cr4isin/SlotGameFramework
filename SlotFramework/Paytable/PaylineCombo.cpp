#include "PaylineCombo.h"

PaylineCombo::PaylineCombo(int numReels, int numLines, const SymbolSet& symbolSet)
    : numReels(numReels),
	numLines(numLines),
	numSymbols(symbolSet.GetNumSymbols()),
    symbolSet(symbolSet)
{
    // Resize combos to hold all possible symbol combos
    int totalCombos = static_cast<int>(pow(numSymbols, numReels));
	paylineCombos.resize(totalCombos);
	combos.resize(numLines);

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

Combo PaylineCombo::GetCombo(const vector<int>& combo) const {
    size_t symbolKey = GetSymbolKey(combo);
    return paylineCombos[symbolKey];
}

Combo PaylineCombo::GetCombo(const size_t& symbolKey) const
{
	return paylineCombos[symbolKey];
}

void PaylineCombo::SetCombo(const vector<int>& combo, double pay, int bonusCode, int progressive, bool overWrite)
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
		Combo& comboInfo = paylineCombos[symbolKey];
		if (!comboInfo.set || overWrite)
		{
			comboInfo.basePay = pay;
			comboInfo.multiplier = multiplier;
			comboInfo.pay = comboInfo.basePay * comboInfo.multiplier;
			comboInfo.bonusCode = bonusCode;
			comboInfo.progressive = progressive;
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

void PaylineCombo::AddResult(const size_t& symbolKey, const int& index)
{
	Combo& combo = paylineCombos[symbolKey];
	combos[index] = combo;
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

void PaylineCombo::ClearResults()
{
	pay = 0;
	bonusCodes.clear();
	bonusHit = false;
	progressives.clear();
	progHit = false;
}

void PaylineCombo::PrintResults()
{
	if (pay > 0 || bonusHit || progHit)
	{
		for (int iLine = 0; iLine < numLines; iLine++)
		{
			if (combos[iLine].pay > 0)
			{
				cout << "Line " << iLine + 1 << " pays " << combos[iLine].pay << "\n";
			}
			if (combos[iLine].bonusCode > 0)
			{
				cout << "Line " << iLine + 1 << " hit bonus code " << combos[iLine].bonusCode << "\n";
			}
			if (combos[iLine].progressive > 0)
			{
				cout << "Line " << iLine + 1 << " awards progressive " << combos[iLine].progressive << "\n";
			}
		}
		cout << "\n";
	}
}
