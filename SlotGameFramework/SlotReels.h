#pragma once

#include "General.h"

class SlotReels 
{
public:
	SlotReels(vector<vector<int>> reels, int maxRows = 25);
	SlotReels(vector<vector<int>> reels, vector<vector<int>> reelWeights, int maxRows = 25);

	int GetNumReels();
	bool IsWeighted();
	int GetReelSize(int reelIndex);
	int GetSymbol(int reelIndex, int position);
	int GetReelWeight(int reelIndex);
	int GetWeight(int reelIndex, int position);

	vector<int> GenerateRandomStops();
	vector<int> ConvertStopsToPositions(vector<int> stops);
	vector<int> GenerateRandomPositions();
	vector<int> GetSubReel(int reelIndex, int position, int numRows);

private:
	int m_numReels; // Number of reels in the reel set
	bool m_weightedReels = false; // True when the reels are weighted
	vector<int> m_reelSizes; // Number of symbols on each reel
	vector<vector<int>> m_reels; // 2D vector (Reel, Symbol) that contains the reel strips. Extra symbols have been appended to the end to allow cycling.
	vector<int> m_totalWeights; // Total weight for each reel
	vector<vector<int>> m_reelWeights; // 2D vector (Reel, Weight) that contains the weights of the reel strips.
	vector<vector<long long>> m_cumulativeWeights; // 2D vector (Reel, Cumulative Weight) that contains the cumululative sum of the weights for each reel.
	vector<vector<int>> m_numZeroIndexes; // 2D vector (Reel, numZeroIndexes) that contains the number of preceding 0 weight positions for each reel.
	mt19937 stopRNG; // Type of RNG to use
	vector<uniform_int_distribution<>> stopGenerator; // RNG distribution for each reel
};