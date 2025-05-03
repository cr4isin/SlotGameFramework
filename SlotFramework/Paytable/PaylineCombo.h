#pragma once

#include "SymbolSet.h"

class PaylineCombo 
{
public:
    PaylineCombo() = default;
    PaylineCombo(int numReels, int numLines, const SymbolSet& symbolSet);
    size_t GetSymbolKey(const vector<int>& combo) const;
    size_t GetSymbolKey(const int& reel, const int& symbol) const;
    Combo GetCombo(const vector<string>& combo) const;
    Combo GetCombo(const vector<int>& combo) const;
    Combo GetCombo(const size_t& symbolKey) const;
    void SetCombo(const vector<int>& combo, double pay, int bonusCode = 0, int progressive = 0, bool overWrite = false);
    int GetNumSymbols() const;
    int GetNumReels() const;

    // For Results
    void AddResult(const size_t& symbolKey, const int& index);
    void ClearResults();
    void PrintResults();

    vector<Combo> combos;
    double pay = 0;
    vector<int> bonusCodes;
    bool bonusHit = false;
    vector<int> progressives;
    bool progHit = false;

private:
    int numReels = 0;
    int numSymbols = 0;
    int numLines = 0;
    SymbolSet symbolSet;
    vector<Combo> paylineCombos;
    vector<vector<size_t>> symbolKeys;
};
