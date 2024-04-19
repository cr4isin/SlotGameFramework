
#include "SlotReels.h"

SlotReels::SlotReels()
{
	// Empty Default Constructor
}

SlotReels::SlotReels(vector<vector<int>> reels, int maxRows) {
	m_reels = reels;
	m_numReels = m_reels.size();
	m_reelWeights.resize(m_numReels);
	m_weightedReels = false;
	for (int iReel = 0; iReel < m_numReels; iReel++) 
	{
		int reelSize = m_reels.at(iReel).size();
		m_reelSizes.push_back(reelSize);
		for (int i = 0; i < maxRows; i++) 
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
	m_cumulativeWeights.resize(m_numReels);
	m_numZeroIndexes.resize(m_numReels);
	for (int iReel = 0; iReel < m_numReels; iReel++) 
	{
		int reelSize = m_reels.at(iReel).size();
		m_reelSizes.push_back(reelSize);
		for (int i = 0; i < maxRows; i++) 
		{
			m_reels.at(iReel).push_back(m_reels.at(iReel).at(i % reelSize));
		}
		int numZeroes = 0;
		int totalWeight = 0;
		for (int i = 0; i < reelSize; i++) 
		{
			if (m_reelWeights.at(iReel).at(i) > 0)
			{
				totalWeight += m_reelWeights.at(iReel).at(i);
				m_cumulativeWeights.at(iReel).push_back(totalWeight);
				m_numZeroIndexes.at(iReel).push_back(numZeroes);
			}
			else
			{
				numZeroes++;
			}
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

int SlotReels::GetNumReels()
{
	return m_numReels;
}

bool SlotReels::IsWeighted()
{
	return m_weightedReels;
}

int SlotReels::GetReelSize(int reelIndex)
{
	return m_reelSizes[reelIndex];
}

int SlotReels::GetSymbol(int reelIndex, int position)
{
	return m_reels[reelIndex][position];
}

int SlotReels::GetReelWeight(int reelIndex)
{
	return m_totalWeights[reelIndex];
}

int SlotReels::GetWeight(int reelIndex, int position)
{
	return m_reelWeights[reelIndex][position];
}

void SlotReels::GenerateRandomStops(vector<int>& stops)
{
	for (int iReel = 0; iReel < m_numReels; iReel++)
	{
		stops[iReel] = stopGenerator[iReel](stopRNG);
	}
}

void SlotReels::ConvertStopsToPositions(vector<int>& stops)
{
	if (!m_weightedReels)
	{
		return;
	}
	for (int iReel = 0; iReel < m_numReels; iReel++)
	{
		int low = 0;
		int high = m_cumulativeWeights[iReel].size();

		while (low < high)
		{
			int mid = (low + high) / 2;

			if (stops[iReel] < m_cumulativeWeights[iReel][mid])
			{
				high = mid;
			}
			else
			{
				low = mid + 1;
			}
		}
		stops[iReel] =  low + m_numZeroIndexes[iReel][low];
	}
}

void SlotReels::GenerateRandomPositions(vector<int>& positions)
{
	GenerateRandomStops(positions);
	ConvertStopsToPositions(positions);
}

void SlotReels::FillReel(vector<int>& reel, int reelIndex, int position, int numRows)
{
	copy(m_reels[reelIndex].begin() + position, m_reels[reelIndex].begin() + position + numRows, reel.begin());
}
