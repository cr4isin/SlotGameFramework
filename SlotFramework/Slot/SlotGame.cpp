
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

void SlotGame::ClearHistograms()
{
	histograms.clear();
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
		PrintTools::SpawnProcesses(args[0], numProcesses - 1);
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
			cout << F("{}/100\t{:0.8f}\tE {}\tR {}\tT {}\n", step, coinOut/coinIn, PrintTools::Duration(elapsedTime), PrintTools::Duration(remainingTime), PrintTools::Duration(totalTime));
		}
	}
	// Write results to a file
	string filename = outputDir + "SimData_" + simName + ".txt";
	string outputString = F("{:0.14f}\t{}\t{}\t{}\t{}\t{}\t{}", coinOut / coinIn, totalBet, numGames, maxWin, hits, wins, SlotTools::Median(spinsHist));
	for (auto const& [name, totalTracker] : totalTrackers)
	{
		outputString += F("\t{}\t{:0.14f}\t{}\t{}\t{}\t{}\t{}", name, totalTracker.value / numGames, totalTracker.maxValue, totalTracker.gameHits, totalTracker.gameWins, totalTracker.totalHits, totalTracker.totalWins);
	}
	outputString += "\n";
	PrintTools::WriteStringToFile(filename, outputString);
	// Write histograms to a file
	filename = outputDir + "Histogram_" + simName + ".txt";
	if (histograms.size() > 0) PrintTools::WriteAllHistogramsToFile(filename, histograms);
}

void SlotGame::FreePlay(bool clearConsole)
{
	inFreePlay = true;
	string input;
	int bonusCode = 0;
	double coinIn = 0;
	double coinOut = 0;

	cout << "Base Bet: " << baseBet << "   Bet Mult: " << betMult << "   Total Bet: " << totalBet << "\nPress [Enter] to Play or enter a bonus code: ";

	getline(cin, input);
	if (!input.empty()) {
		istringstream iss(input);
		int newCode = 0;
		if (iss >> newCode) bonusCode = newCode;
	}

	while (true)
	{
		// Clear screen
		if (clearConsole) cout << ANSI::FullCLear() << "\n";
		else cout << "\n";

		// Play Game/Bonus
		double score = 0;
		if (bonusCode == 0) score += PlayGame();
		else score += PlayBonus(bonusCode);

		// Track/Print Results
		coinIn += totalBet;
		coinOut += score;
		cout << "Score:\t" << score 
			<< "\nBet:\t" << totalBet 
			<< "\n\nCoin In:\t" << coinIn 
			<< "\nCoin Out:\t" << coinOut 
			<< "\n\nPress [Enter] to Play again or enter a bonus code: ";

		// Getting input for next game
		getline(cin, input);
		if (input.empty()) continue;
		istringstream iss(input);
		int newCode = 0;
		if (iss >> newCode) bonusCode = newCode;
	}
}
