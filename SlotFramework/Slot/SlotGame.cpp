
#include "SlotGame.h"

// ============================== Setup ==============================
SlotGame::SlotGame(string gameName, string mathXMLFileName, int baseBet, int betMult, int totalBet)
	:gameName(gameName), mathXML(MathXML("../Games/" + gameName + "/xml/" + mathXMLFileName))
{
	configName = mathXML.GetConfigName();
	SetBetScheme(baseBet, betMult, totalBet);
	simName = configName + "_" + to_string(baseBet) + "cr_" + to_string(betMult) + "x";
}

void SlotGame::SetBetScheme(int baseBet, int betMult, int totalBet)
{
	this->baseBet = baseBet;
	this->betMult = betMult;
	if (totalBet <= 0)
	{
		this->totalBet = baseBet * betMult;
	}
	else
	{
		this->totalBet = totalBet;
	}
}

// ============================== Game Functions ==============================
double SlotGame::PlayGame()
{
	double score = 0;

	return score;
}
double SlotGame::PlayBonus(int bonusCode)
{
	double score = 0;

	switch (bonusCode)
	{
		case 0: break;
		default:
		{
			cout << "No such bonus code: " << bonusCode << endl;
			throw runtime_error("Unknown Bonus Code used");
		}
	}

	return score;
}

// ============================== Other Functions ==============================
void SlotGame::AddToTracker(string name, double value)
{
	Tracker& tracker = trackers[name];
	tracker.value += value;
	tracker.totalHits++;
	if (value > 0) tracker.totalWins++;
	if (value > tracker.maxValue) tracker.maxValue = value;
}

void SlotGame::ClearTrackers()
{
	trackers.clear();
}

void SlotGame::AddToHistogram(string name, double value, long long numHits)
{
	histograms[name][value] += numHits;
}

void SlotGame::PrintHistograms()
{
	for (auto const& [name, hist] : histograms)
	{
		string filename = "../Games/" + gameName + "/results/Histogram_" + simName + "_" + name + ".txt";
		WriteHistogramToFile(filename, hist);
	}
}

void SlotGame::RunSims(int numGames, vector<string>& args, int bonusCode)
{
	// Opening more processes if necessary
	int numProcesses = 0;
	if (args.size() > 1)
	{
		numProcesses = stoi(args[1]);
		args[1] = "1";
	}
	else 
	{
		std::cout << "How many processes would you like to run? (1-" << thread::hardware_concurrency() << "): ";
		std::cin >> numProcesses;
		args.push_back("1");
	}
	if (numProcesses > 1)
	{
		SpawnProcesses(args[0], numProcesses - 1, 0); // Final argument is the delay in seconds between opening processes
	}

	// Initializing variables for the sim
	int percentile = numGames / 100;
	double coinIn = 0;
	double coinOut = 0;
	int maxWin = 0;
	int hits = 0;
	int wins = 0;
	double buyIn = 50 * totalBet;
	double credits = buyIn;
	int numSpins = 0;
	map<int, int> spinsHist;
	cout << "Running Sim: " << simName << "\n";
	auto startTime = chrono::high_resolution_clock::now();
	for (int iGame = 1; iGame <= numGames; iGame++)
	{
		// Play Game
		double score = 0;
		if (bonusCode == 0) score += PlayGame();
		else score += PlayBonus(bonusCode);
		coinOut += score;
		coinIn += totalBet;
		// Update Values
		if (score > maxWin) maxWin = score;
		if (score > 0) hits++;
		if (score > totalBet) wins++;
		// Median Spins
		numSpins++;
		credits += score - totalBet;
		if (credits < totalBet || numSpins > 999)
		{
			spinsHist[numSpins]++;
			credits = buyIn;
			numSpins = 0;
		}
		// Save and Reset Trackers
		for (auto const& [name, tracker] : trackers)
		{
			TotalTracker& totalTracker = totalTrackers[name];
			totalTracker.value += tracker.value;
			totalTracker.gameHits++;
			if (tracker.value > 0) totalTracker.gameWins++;
			if (tracker.maxValue > totalTracker.maxValue) totalTracker.maxValue = tracker.maxValue;
			totalTracker.totalHits += tracker.totalHits;
			totalTracker.totalWins += tracker.totalWins;
		}
		ClearTrackers();
		// Print sim status to console
		if (iGame % percentile == 0)
		{
			int step = iGame / percentile;
			long long elapsedTime = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - startTime).count();
			long long remainingTime = elapsedTime * (100 - step) / step;
			long long totalTime = elapsedTime + remainingTime;
			cout << step << "/100\t" << FormatDouble(coinOut / coinIn, 8) << "\tE " << FormatTime(elapsedTime) << "\tR " << FormatTime(remainingTime) << "\tT " << FormatTime(totalTime) << endl;
		}
	}
	// Write results to a file
	PrintHistograms();
	string filename = "../Games/" + gameName + "/results/SimData_" + simName + ".txt";
	ostringstream outputString;
	outputString << FormatDouble(coinOut / coinIn, 14) << "\t" << totalBet << "\t" << numGames << "\t" << maxWin << "\t" << hits << "\t" << wins << "\t" << GetMedian(spinsHist);
	for (auto const& [name, totalTracker] : totalTrackers)
	{
		outputString << "\t" << name;
		outputString << "\t" << FormatDouble(totalTracker.value / numGames, 14);
		outputString << "\t" << totalTracker.maxValue;
		outputString << "\t" << totalTracker.gameHits;
		outputString << "\t" << totalTracker.gameWins;
		outputString << "\t" << totalTracker.totalHits;
		outputString << "\t" << totalTracker.totalWins;
	}
	outputString << "\n";
	WriteStringToFile(filename, outputString.str());
}

void SlotGame::FreePlay(bool clearConsole)
{
	inFreePlay = true;
	cout << "Base Bet: " << baseBet << "   Bet Mult: " << betMult << "   Total Bet: " << totalBet << "\n";
	cout << "Press Enter to Play!";
	cin.get();

	double coinIn = 0;
	double coinOut = 0;
	while (true)
	{
		if (clearConsole) system("cls");
		cout << "\n";
		double score = PlayGame();
		coinIn += totalBet;
		coinOut += score;
		cout << "Score:\t" << score << "\nBet:\t" << totalBet << "\n\nCoin In:\t" << coinIn << "\nCoin Out:\t" << coinOut << "\n\nPress Enter to Play again... ";
		cin.get();
	}
}

//void SlotGame::CyclePositions()
//{
//	 Will cycle through all positions of a reel set and create a histogram of every possible score
//	map<double, size_t> hist;
//	vector<int> positions(numReels, 0);
//	double maxScore = 0;
//	vector<int> maxPositions(numReels, 0);
//	CyclePositionsRecursive(hist, positions, maxScore, maxPositions);
//
//	string filename = "CyclePositionsOutput_" + configName + "_" + to_string(baseBet) + "cr_" + to_string(betMult) + "x.txt";
//	ofstream outputFile(filename);
//	outputFile << "Max Pay Positions:\t";
//	for (int iReel = 0; iReel < numReels; iReel++)
//	{
//		outputFile << maxPositions[iReel] << "\t";
//	}
//	outputFile << "\n\n";
//	for (auto const& [score, combo] : hist)
//	{
//		outputFile << score << "\t" << combo << "\n";
//	}
//	outputFile.close();
//}
//
//void SlotGame::CyclePositionsRecursive(map<double, size_t>& hist, vector<int>& positions, double& maxScore, vector<int>& maxPositions, int currentReel)
//{
//	 Function used to run CyclePositions()
//	 Might need to edit your grid, reels, and evaluation type being used
//	if (currentReel < positions.size())
//	{
//		for (int i = 0; i < Reels_Main.GetReelSize(currentReel); i++)
//		{
//			if (currentReel == 0)
//			{
//				cout << i << " / " << Reels_Main.GetReelSize(currentReel) << "\n";
//			}
//			positions.at(currentReel) = i;
//			grid.FillGridReel(currentReel, positions[currentReel], Reels_Main);
//			CyclePositionsRecursive(hist, positions, maxScore, maxPositions, currentReel + 1);
//		}
//	}
//	else
//	{
//		double score = 0;
//		//double score = grid.EvaluateLines(symbolCombos, betMult);
//		int combos = 1;
//		for (int iReel = 0; iReel < positions.size(); iReel++)
//		{
//			combos *= Reels_Main.GetWeight(iReel, positions[iReel]);
//		}
//		hist[score] += combos;
//		if (score > maxScore)
//		{
//			maxPositions = positions;
//			maxScore = score;
//		}
//	}
//}