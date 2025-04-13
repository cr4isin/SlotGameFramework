#pragma once

#include "SymbolSet.h"

class CountScatterCombo
{
public:
    CountScatterCombo(int numReels, int numRows, const SymbolSet& symbolSet);

    void SetCombo(int count, double pay, int bonusCode = 0, int progressive = 0);
    Combo GetCombo(int count) const;

    void SetScatterSymbol(int symbol); // optional setter for later use
    int GetScatterSymbol() const;
    int GetNumReels() const;
    int GetNumRows() const;

private:
    int numReels;
    int numRows;
    const SymbolSet& symbolSet;

    int scatterSymbol = -1;               // symbol to count
    std::vector<Combo> combos;            // indexed by count
};
