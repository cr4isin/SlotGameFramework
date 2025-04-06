#include "PaylineCombo.h"

PaylineCombo::PaylineCombo(int numReels, const SymbolSet& symbolSet)
    : numReels(numReels),
    numSymbols(symbolSet.GetNumSymbols()),
    symbolSet(symbolSet)
{
    int totalCombos = static_cast<int>(std::pow(numSymbols, numReels));
    combos.resize(totalCombos); // all default-initialized Combo structs
}

PaylineCombo::Combo PaylineCombo::GetCombo(const std::vector<int>& symbolIndices) const {
    int key = ComputeSymbolKey(symbolIndices);
    return combos[key];
}

int PaylineCombo::ComputeSymbolKey(const std::vector<int>& symbolIndices) const {
    int key = 0;
    for (int i = 0; i < symbolIndices.size(); ++i) {
        key = key * numSymbols + symbolIndices[i];
    }
    return key;
}

void PaylineCombo::SetCombo(const std::vector<int>& symbolPattern, double pay, int bonusCode) {
    std::vector<int> pattern = symbolPattern;
    SetComboRecursive(pattern, 0, pay, bonusCode);
}

void PaylineCombo::SetComboRecursive(std::vector<int>& pattern,
    int index,
    double pay,
    int bonusCode) {
    if (index == numReels) {
        int key = ComputeSymbolKey(pattern);
        combos[key].pay = pay;
        combos[key].bonusCode = bonusCode;
        return;
    }

    if (pattern[index] == -1) { // -1 = ANY
        for (int sym = 0; sym < numSymbols; ++sym) {
            pattern[index] = sym;
            SetComboRecursive(pattern, index + 1, pay, bonusCode);
        }
        pattern[index] = -1; // reset for recursion backtracking
    }
    else {
        SetComboRecursive(pattern, index + 1, pay, bonusCode);
    }
}
