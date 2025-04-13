#pragma once

#include "SymbolSet.h"
#include "ComboTypes.h"

class CountScatterCombo
{
public:
    CountScatterCombo(int numReels, const SymbolSet& symbolSet);

    void SetCombo(int count, double pay, int bonusCode = 0, int progressive = 0, bool overWrite = false);
    Combo GetCombo(int count) const;

    void SetScatterSymbol(int symbol); // optional setter for later use
    int GetScatterSymbol() const;

private:
    int numReels;
    const SymbolSet& symbolSet;

    int scatterSymbol = -1;               // symbol to count
    std::vector<Combo> combos;            // indexed by count
};
