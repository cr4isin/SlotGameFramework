#include "CountScatterCombo.h"

CountScatterCombo::CountScatterCombo(int numReels, int numRows, const SymbolSet& symbolSet)
    : numReels(numReels), numRows(numRows), symbolSet(symbolSet)
{
    // Default to max count = numReels (1 per reel)
    combos.resize(numReels * numRows + 1);
}

void CountScatterCombo::SetCombo(int count, double pay, int bonusCode, int progressive)
{
    Combo& combo = combos[count];
    combo.basePay = pay;
    combo.pay = pay;
    combo.bonusCode = bonusCode;
    combo.progressive = progressive;
}

Combo CountScatterCombo::GetCombo(int count) const
{
    return combos[count];
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
