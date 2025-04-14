#pragma once

#include "General.h"
#include "SlotGrid.h"
#include "MathXML.h"

class SlotGame 
{
public:

	// ==================== Functions ====================
	// Setup
	SlotGame(string mathXML, int baseBet, int betMult, int totalBet = 0);
	void SetBetScheme(int baseBet, int betMult, int totalBet = 0);
	// Game Functions
	virtual double PlayGame();
	virtual double PlayBonus(int bonusCode);
	// Other Functions
	void AddToTracker(string name, double value);
	void ClearTrackers();
	void AddToHistogram(string name, double value, long long numHits = 1);
	void PrintHistograms(string simName);
	void RunSims(int numGames, vector<string>& args);
	void FreePlay(bool clearConsole = false);
	void CyclePositions();
	void CyclePositionsRecursive(map<double, size_t>& hist, vector<int>& positions, double& maxScore, vector<int>& maxPositions, int currentReel = 0);
	// ==================== General Variables ====================
	string configName = "";
	MathXML mathXML;
	int baseBet = 1;
	int betMult = 1;
	int totalBet = 1;
	bool inFreePlay = false;
	struct Tracker
	{
		double value = 0;
		double maxValue = 0;
		int totalHits = 0;
		int totalWins = 0;
	};
	struct TotalTracker
	{
		double value = 0;
		double maxValue = 0;
		int gameHits = 0;
		int gameWins = 0;
		int totalHits = 0;
		int totalWins = 0;
	};
	map<string, Tracker> trackers;
	map<string, TotalTracker> totalTrackers;
	map<string, map<double, long long>> histograms;
};