
#include "SlotGame.h"

// ============================== Setup ==============================
SlotGame::SlotGame(string gameName, string mathXMLFileName, int baseBet, int betMult, int totalBet, bool useCurrentDirectory)
	:gameName(gameName)
{
	SetBetScheme(baseBet, betMult, totalBet);
	if (!useCurrentDirectory)
	{
		inputDir = "../Games/" + gameName + "/xml/";
		outputDir = "../Games/" + gameName + "/results/";
	}
	mathXML.LoadFile(inputDir + mathXMLFileName);
	configName = mathXML.GetConfigName();
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
		string filename = outputDir + "Histogram_" + simName + "_" + name + ".txt";
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
	string filename = outputDir + "SimData_" + simName + ".txt";
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

void SlotGame::FreePlay(int bonusCode, bool clearConsole)
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
		double score = 0;
		if (bonusCode == 0) score += PlayGame();
		else score += PlayBonus(bonusCode);
		coinIn += totalBet;
		coinOut += score;
		cout << "Score:\t" << score << "\nBet:\t" << totalBet << "\n\nCoin In:\t" << coinIn << "\nCoin Out:\t" << coinOut << "\n\nPress Enter to Play again... ";
		cin.get();
	}
}
