#pragma once

#include "SymbolSet.h"

class ScatterCombo
{
public:
    ScatterCombo() = default;
    ScatterCombo(int numReels, const SymbolSet& symbolSet);

    void SetCombo(const std::vector<bool>& reelsHit, double pay, int bonusCode = 0, int progressive = 0);
    void SetScatterSymbols(const std::vector<int>& symbols);
    Combo GetCombo(int comboKey) const;
    int GetComboKey(const std::vector<bool>& reelsHit) const;
    vector<int> GetScatterSymbols() const;
    int GetNumReels() const;

    // For Results
    void AddResult(const int& comboKey);
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
    SymbolSet symbolSet;

    std::vector<int> scatterSymbols; // size = numReels, -1 by default
    std::vector<Combo> scatterCombos;       // size = 2^numReels
};
