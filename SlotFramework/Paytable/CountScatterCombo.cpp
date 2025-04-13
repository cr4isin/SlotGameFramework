#include "CountScatterCombo.h"

CountScatterCombo::CountScatterCombo(int numReels, const SymbolSet& symbolSet)
    : numReels(numReels), symbolSet(symbolSet)
{
    // Default to max count = numReels (1 per reel)
    combos.resize(numReels + 1);
}

void CountScatterCombo::SetCombo(int count, double pay, int bonusCode, int progressive, bool overWrite)
{
    if (count < 0 || count >= combos.size())
        return;

    Combo& combo = combos[count];
    if (!combo.set || overWrite)
    {
        combo.basePay = pay;
        combo.pay = pay;
        combo.bonusCode = bonusCode;
        combo.progressive = progressive;
        combo.set = true;
    }
}

Combo CountScatterCombo::GetCombo(int count) const
{
    if (count < 0 || count >= combos.size())
        return Combo{};
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
