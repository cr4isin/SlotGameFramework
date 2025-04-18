#include "CountScatterCombo.h"

CountScatterCombo::CountScatterCombo(int numReels, int numRows, const SymbolSet& symbolSet)
    : numReels(numReels), numRows(numRows), symbolSet(symbolSet)
{
    // Default to max count = numReels (1 per reel)
	countScatterCombos.resize(numReels * numRows + 1);
}

void CountScatterCombo::SetCombo(int count, double pay, int bonusCode, int progressive)
{
    Combo& combo = countScatterCombos[count];
    combo.basePay = pay;
    combo.pay = pay;
    combo.bonusCode = bonusCode;
    combo.progressive = progressive;
}

Combo CountScatterCombo::GetCombo(int count) const
{
    return countScatterCombos[count];
}

void CountScatterCombo::SetScatterSymbol(int symbol)
{
    scatterSymbol = symbol;
}

int CountScatterCombo::GetScatterSymbol() const
{
    return scatterSymbol;
}

int CountScatterCombo::GetNumReels() const
{
    return numReels;
}

int CountScatterCombo::GetNumRows() const
{
    return numRows;
}

void CountScatterCombo::AddResult(const int& count)
{
	combo = countScatterCombos[count];
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

void CountScatterCombo::ClearResults()
{
	pay = 0;
	bonusCode = 0;
	bonusHit = false;
	progressive = 0;
	progHit = false;
}

void CountScatterCombo::PrintResults()
{
	if (pay > 0 || bonusHit || progHit)
	{
		if (combo.pay > 0)
		{
			cout << "Count Scatter pays " << combo.pay << "\n";
		}
		if (combo.bonusCode > 0)
		{
			cout << "Count Scatter hit bonus code " << combo.bonusCode << "\n";
		}
		if (combo.progressive > 0)
		{
			cout << "Count Scatter awards progressive " << combo.progressive << "\n";
		}
		cout << "\n";
	}
}