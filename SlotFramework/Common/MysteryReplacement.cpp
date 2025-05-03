#include "MysteryReplacement.h"

MysteryReplacement::MysteryReplacement(WeightTable table, vector<vector<ReplacementInstruction>> instructions)
    : weightTable(std::move(table)), instructions(std::move(instructions))
{
}

const vector<ReplacementInstruction>& MysteryReplacement::GetRandomInstructions()
{
    int instructionIndex = weightTable.DrawIndex();
    return instructions[instructionIndex];
}

const vector<vector<ReplacementInstruction>>& MysteryReplacement::GetAllInstructions()
{
    return instructions;
}

const vector<ReplacementInstruction>& MysteryReplacement::GetInstructions(int index)
{
    return instructions[index];
}

WeightTable& MysteryReplacement::GetWeightTable()
{
    return weightTable;
}
