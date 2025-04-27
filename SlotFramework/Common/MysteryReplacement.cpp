#include "MysteryReplacement.h"

MysteryReplacement::MysteryReplacement(WeightTable table, vector<vector<ReplacementInstruction>> instructions)
    : weightTable(std::move(table)), instructions(std::move(instructions))
{
}

const vector<ReplacementInstruction>& MysteryReplacement::GetInstructions()
{
    int instructionIndex = weightTable.DrawIndex();
    return instructions[instructionIndex];
}
