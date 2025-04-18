#include "ScatterCombo.h"

ScatterCombo::ScatterCombo(int numReels, const SymbolSet& symbolSet)
    : numReels(numReels), symbolSet(symbolSet)
{
    // Initialize scatterSymbols to -1 (ANY) for all reels
    scatterSymbols.resize(numReels, -1);

    // Initialize combos vector to hold 2^numReels combinations
    int totalCombos = static_cast<int>(std::pow(2, numReels));
    scatterCombos.resize(totalCombos);
}

void ScatterCombo::SetCombo(const std::vector<bool>& reelsHit, double pay, int bonusCode, int progressive)
{
    int key = GetComboKey(reelsHit);
    Combo& combo = scatterCombos[key];
    combo.basePay = pay;
    combo.pay = pay;
    combo.bonusCode = bonusCode;
    combo.progressive = progressive;
}

void ScatterCombo::SetScatterSymbols(const std::vector<int>& symbols)
{
    scatterSymbols = symbols;
}

Combo ScatterCombo::GetCombo(int comboKey) const
{
    return scatterCombos[comboKey];
}

int ScatterCombo::GetComboKey(const std::vector<bool>& reelsHit) const
{
    int key = 0;
    for (int i = 0; i < numReels; ++i)
    {
        if (reelsHit[i])
        {
            key |= (1 << i);
        }
    }
    return key;
}

vector<int> ScatterCombo::GetScatterSymbols() const
{
    return scatterSymbols;
}

int ScatterCombo::GetNumReels() const
{
    return numReels;
}

void ScatterCombo::AddResult(const int& comboKey)
{
	combo = scatterCombos[comboKey];
	pay += combo.pay;

	if (combo.bonusCode > 0)
	{
		bonusHit = true;
		bonusCode = combo.bonusCode;
	}
	if (combo.progressive > 0)
	{
		progHit = true;
		progressive = combo.progressive;
	}
}

void ScatterCombo::ClearResults()
{
	pay = 0;
	bonusCode = 0;
	bonusHit = false;
	progressive = 0;
	progHit = false;
}

void ScatterCombo::PrintResults()
{
	if (pay > 0 || bonusHit || progHit)
	{
		if (combo.pay > 0)
		{
			cout << "Scatter pays " << combo.pay << "\n";
		}
		if (combo.bonusCode > 0)
		{
			cout << "Scatter hit bonus code " << combo.bonusCode << "\n";
		}
		if (combo.progressive > 0)
		{
			cout << "Scatter awards progressive " << combo.progressive << "\n";
		}
		cout << "\n";
	}
}
