
#include "SlotReels.h"

SlotReels::SlotReels(vector<vector<int>> reels, int maxRows) {
	m_reels = reels;
	m_numReels = m_reels.size();
	m_reelWeights.resize(m_numReels);
	m_weightedReels = false;
	for (int iReel = 0; iReel < m_numReels; iReel++) 
	{
		size_t reelSize = m_reels.at(iReel).size();
		m_reelSizes.push_back(reelSize);
		for (size_t i = 0; i < maxRows; i++) 
		{
			m_reels.at(iReel).push_back(m_reels.at(iReel).at(i % reelSize));
		}
		m_totalWeights.push_back(reelSize);
		for (int i = 0; i < reelSize; i++) 
		{
			m_reelWeights.at(iReel).push_back(1);
		}
	}
	random_device rd;
	stopRNG.seed(rd());
	for (int iReel = 0; iReel < m_numReels; iReel++)
	{
		uniform_int_distribution<> dist(0, m_totalWeights.at(iReel) - 1);
		stopGenerator.push_back(dist);
	}
}

SlotReels::SlotReels(vector<vector<int>> reels, vector<vector<int>> reelWeights, int maxRows)
{
	m_reels = reels;
	m_numReels = m_reels.size();
	m_reelWeights = reelWeights;
	m_weightedReels = true;
	for (int iReel = 0; iReel < m_numReels; iReel++) 
	{
		size_t reelSize = m_reels.at(iReel).size();
		m_reelSizes.push_back(reelSize);
		for (size_t i = 0; i < maxRows; i++) 
		{
			m_reels.at(iReel).push_back(m_reels.at(iReel).at(i % reelSize));
		}
		size_t totalWeight = 0;
		for (int i = 0; i < reelSize; i++) 
		{
			totalWeight += m_reelWeights.at(iReel).at(i);
		}
		m_totalWeights.push_back(totalWeight);
	}
	random_device rd;
	stopRNG.seed(rd());
	for (int iReel = 0; iReel < m_numReels; iReel++)
	{
		uniform_int_distribution<> dist(0, m_totalWeights.at(iReel) - 1);
		stopGenerator.push_back(dist);
	}
}

int SlotReels::GetIndexFromStop(int reelIndex, int stop)
{
	if (m_weightedReels)
	{
		int index = 0;
		while (stop >= 0) {
			stop -= m_reelWeights.at(reelIndex).at(index++);
		}
		return index-1;
	}
	else
	{
		return stop;
	}
}

void SlotReels::ReplaceSymbolOnReel(int reelIndex, int oldSymbol, int newSymbol)
{
	for (int i = 0; i < m_reels[reelIndex].size(); i++)
	{
		if (m_reels[reelIndex][i] == oldSymbol)
		{
			m_reels[reelIndex][i] = newSymbol;
		}
	}
}

vector<int> SlotReels::GenerateRandomStops()
{
	vector<int> stops(m_numReels);
	for (int iReel = 0; iReel < m_numReels; iReel++)
	{
		stops[iReel] = stopGenerator[iReel](stopRNG);
	}
	return stops;
}
