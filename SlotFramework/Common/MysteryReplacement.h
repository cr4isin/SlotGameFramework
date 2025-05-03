#pragma once

#include "WeightTable.h"

struct ReplacementInstruction {
    int originalSymbol;
    int newSymbol;
    int reel = -1;  // -1 means apply on all reels
};

class MysteryReplacement {
public:
    MysteryReplacement() = default;
    MysteryReplacement(WeightTable table, vector<vector<ReplacementInstruction>> instructions);
    const vector<ReplacementInstruction>& GetRandomInstructions();
    const vector<vector<ReplacementInstruction>>& GetAllInstructions();
    const vector<ReplacementInstruction>& GetInstructions(int index);
    WeightTable& GetWeightTable();

private:
    WeightTable weightTable;
    vector<vector<ReplacementInstruction>> instructions;
};
