#pragma once

#include "General.h"

class SlotReels 
{
public:
	SlotReels(vector<vector<int>> reels, int maxRows = 25);
	SlotReels(vector<vector<int>> reels, vector<vector<int>> reelWeights, int maxRows = 25);

	size_t m_numReels;
	vector<size_t> m_reelSizes;
	vector<vector<int>> m_reels;
	bool m_weightedReels = false;
	vector<size_t> m_totalWeights;
	vector<vector<int>> m_reelWeights;

	int GetIndexFromStop(int reelIndex, int stop);
	void ReplaceSymbolOnReel(int reelIndex, int oldSymbol, int newSymbol);
};