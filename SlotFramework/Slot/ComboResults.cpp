#include "ComboResults.h"

ComboResults::ComboResults(int numCombos)
    : numCombos(numCombos)
{
    combos.resize(numCombos);
}

void ComboResults::Add(const Combo& combo, int index) 
{
    combos[index] = combo;
    totalPay += combo.pay;

    if (combo.bonusCode > 0) bonusCodes.push_back(combo.bonusCode);
    if (combo.progressive > 0) progressives.push_back(combo.progressive);
}

void ComboResults::Clear() {
    totalPay = 0.0;
    bonusCodes.clear();
    progressives.clear();
}