#pragma once
#pragma once

#include "SymbolSet.h"

class AnywaysCombo
{
public:

    AnywaysCombo(int numReels, const SymbolSet& symbolSet);
    int GetSymbolSubstitutionCount(int symbolForCombo, int symbolOnReel);
    double GetCombo(int symbol, int comboLength);
    void SetCombo(int symbol, int comboLength, double pay);
    int GetNumSymbols() const;
    int GetNumReels() const;

private:
    int numReels;
    int numSymbols;
    const SymbolSet& symbolSet;
    vector<vector<double>> paytable;
    vector<vector<int>> symbolSubstitutionCount;
};
