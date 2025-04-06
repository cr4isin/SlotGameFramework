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
    size_t GetSymbolKey(const vector<int>& combo) const;
    size_t GetSymbolKey(const int& reel, const int& symbol) const;
    Combo GetCombo(const vector<int>& combo) const;
    void SetCombo(const vector<int>& combo, double basePay, int bonusCode, bool overWrite = false);
    int GetNumSymbols() const;
    int GetNumReels() const;

private:
    int numReels;
    int numSymbols;
    const SymbolSet& symbolSet;
    vector<Combo> combos;
    vector<vector<size_t>> symbolKeys;
};
