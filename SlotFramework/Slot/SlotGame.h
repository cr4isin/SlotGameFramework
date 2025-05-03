#pragma once

#include "SlotGrid.h"
#include "MathXML.h"

class SlotGame 
{
public:

	// ==================== Functions ====================
	// Setup
	SlotGame(string gameName, string mathXMLFileName, int baseBet, int betMult = 1, int totalBet = 0, bool useCurrentDirectory = false);
	void SetBetScheme(int baseBet, int betMult = 1, int totalBet = 0);
	// Game Functions
	virtual double PlayGame();
	virtual double PlayBonus(int bonusCode);
	// Other Functions
	void AddToTracker(string name, double value);
	void ClearTrackers();
	void AddToHistogram(string name, double value, long long numHits = 1);
	void PrintHistograms();
	void RunSims(int numGames, vector<string>& args, int bonusCode = 0);
	void FreePlay(int bonusCode = 0, bool clearConsole = false);
	// ==================== General Variables ====================
	const string gameName = "";
	string configName = "";
	string simName = "";
	string inputDir = "";
	string outputDir = "";
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