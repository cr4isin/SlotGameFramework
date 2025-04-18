#pragma once

#include "SymbolSet.h"

class CountScatterCombo
{
public:
    CountScatterCombo() = default;
    CountScatterCombo(int numReels, int numRows, const SymbolSet& symbolSet);

    void SetCombo(int count, double pay, int bonusCode = 0, int progressive = 0);
    Combo GetCombo(int count) const;

    void SetScatterSymbol(int symbol); // optional setter for later use
    int GetScatterSymbol() const;
    int GetNumReels() const;
    int GetNumRows() const;

    // For Results
    void AddResult(const int& count);
    void ClearResults();
    void PrintResults();

    Combo combo;
    double pay = 0;
    int bonusCode;
    bool bonusHit = false;
    int progressive;
    bool progHit = false;

private:
    int numReels;
    int numRows;
    SymbolSet symbolSet;

    int scatterSymbol = -1;               // symbol to count
    std::vector<Combo> countScatterCombos;            // indexed by count
};
