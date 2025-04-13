#pragma once

#include "SymbolSet.h"
#include "ComboTypes.h"

class ScatterCombo
{
public:
    ScatterCombo(int numReels, const SymbolSet& symbolSet);

    void SetCombo(const std::vector<bool>& reelsHit, double pay, int bonusCode = 0, int progressive = 0, bool overWrite = false);
    Combo GetCombo(int comboKey) const;

private:
    int numReels;
    const SymbolSet& symbolSet;

    std::vector<int> scatterSymbols; // size = numReels, -1 by default
    std::vector<Combo> combos;       // size = 2^numReels

    int GetComboKey(const std::vector<bool>& reelsHit) const;
};
