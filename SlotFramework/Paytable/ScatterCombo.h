#pragma once

#include "SymbolSet.h"

class ScatterCombo
{
public:
    ScatterCombo(int numReels, const SymbolSet& symbolSet);

    void SetCombo(const std::vector<bool>& reelsHit, double pay, int bonusCode = 0, int progressive = 0);
    void SetScatterSymbols(const std::vector<int>& symbols);
    Combo GetCombo(int comboKey) const;
    int GetComboKey(const std::vector<bool>& reelsHit) const;
    vector<int> GetScatterSymbols() const;
    int GetNumReels() const;

private:
    int numReels;
    const SymbolSet& symbolSet;

    std::vector<int> scatterSymbols; // size = numReels, -1 by default
    std::vector<Combo> combos;       // size = 2^numReels
};
