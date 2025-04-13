#pragma once

#include "SymbolSet.h"

class AnywaysCombo
{
public:
    AnywaysCombo() = default;
    AnywaysCombo(int numReels, const SymbolSet& symbolSet);
    int GetSymbolSubstitutionCount(int symbolForCombo, int symbolOnReel);
    Combo GetCombo(int symbol, int comboLength);
    void SetCombo(int symbol, int comboLength, double pay, int bonusCode = 0, int progressive = 0);
    int GetNumSymbols() const;
    int GetNumReels() const;

private:
    int numReels;
    int numSymbols;
    SymbolSet symbolSet;
    vector<vector<Combo>> paytable;
    vector<vector<int>> symbolSubstitutionCount;
};
