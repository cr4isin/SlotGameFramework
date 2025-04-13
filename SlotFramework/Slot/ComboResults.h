#pragma once

#include "General.h"

class ComboResults {
public:
    ComboResults(int numCombos = 1);
    void Add(const Combo& combo, int index);
    void Clear();

    int numCombos;
    double totalPay = 0.0;
    std::vector<Combo> combos;
    std::vector<int> bonusCodes;
    std::vector<int> progressives;
};
