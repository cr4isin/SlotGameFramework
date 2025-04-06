#pragma once

#include "SymbolSet.h"

class PaylineCombo 
{
public:
    struct Combo 
    {
        double basePay = 0;
        double totalPay = 0;
        int multiplier = 1;
        int bonusCode = 0;
        int progressive = 0;
        bool set = false;
    };

    PaylineCombo(int numReels, const SymbolSet& symbolSet);

    void SetCombo(const std::vector<int>& symbolPattern, double pay, int bonusCode = 0);
    Combo GetCombo(const std::vector<int>& symbolIndices) const;

private:
    int numSymbols;
    int numReels;
    std::vector<Combo> combos;
    const SymbolSet& symbolSet;

    int ComputeSymbolKey(const std::vector<int>& symbolIndices) const;
    void SetComboRecursive(std::vector<int>& pattern,
        int index,
        double pay,
        int bonusCode);
};
