#pragma once

#include "SymbolSet.h"

class AnywaysCombo
{
public:
    AnywaysCombo() = default;
    AnywaysCombo(int numReels, const SymbolSet& symbolSet);
    int GetSymbolSubstitutionCount(const int& symbolForCombo, const int& symbolOnReel);
    Combo GetCombo(int symbol, int comboLength);
    void SetCombo(int symbol, int comboLength, double pay, int bonusCode = 0, int progressive = 0);
    int GetNumSymbols() const;
    int GetNumReels() const;

    // For Results
    void AddResult(const int& symbol, const int& comboLength, const int& numCombos);
    void ClearResults();
    void PrintResults();

    vector<Combo> combos;
    double pay = 0;
    vector<int> bonusCodes;
    bool bonusHit = false;
    vector<int> progressives;
    bool progHit = false;

private:
    int numReels;
    int numSymbols;
    SymbolSet symbolSet;
    vector<vector<Combo>> anywaysCombos;
    vector<vector<int>> symbolSubstitutionCount;
};
