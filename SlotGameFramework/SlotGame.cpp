
#include "SlotGame.h"

// ============================== Constructor and Destructor ==============================
SlotGame::SlotGame()
{
	// Create pointers here
	symbolCombos = new SymbolCombos(numReels, numSymbols, paytable, symbolSubstitutions, symbolMultipliers);
}

SlotGame::~SlotGame()
{
	// Delete pointers here
	delete baseReelSet;
	delete freeReelSet;
	delete baseGrid;
	delete freeGrid;
	delete symbolCombos;
}

// ============================== Setup ==============================
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

void SlotGame::SetupGame()
{
	SetupGrids();
	SetupReels();
	SetupWeightTables();
}

void SlotGame::SetupGrids()
{
	baseGrid = new SlotGrid(numReels, numRows);
	baseGrid->SetLines(lines, baseBet);
	baseGrid->SetSymbolPrintInfo(symbolStrings, symbolColors);

	freeGrid = new SlotGrid(numReels, numRows);
	freeGrid->SetLines(lines, baseBet);
	freeGrid->SetSymbolPrintInfo(symbolStrings, symbolColors);

	//baseGrid->SetWays(numSymbols, paytable, symbolSubstitutions, symbolMultipliers); // Ways Example
}

void SlotGame::SetupReels()
{
	baseReelSet = new SlotReels(baseReels);
	freeReelSet = new SlotReels(freeReels);
}

void SlotGame::SetupWeightTables()
{
	weightTable.emplace("freeSpinWild", WeightTable(freeSpinWildWeights, freeSpinWildValues));
}

// ============================== Game Functions ==============================
double SlotGame::PlayGame()
{
	if (inFreePlay) cout << "=== Base Game ===\n";
	double score = 0;
	int index = 0;
	long long weight = 0;
	double value = 0;

	// Generate positions and fill grid
	vector<int> positions = baseReelSet->GenerateRandomPositions();
	baseGrid->FillGrid(positions, baseReelSet);

	// Evaluate Lines
	score += betMult * baseGrid->EvaluateGridLines(symbolCombos);

	// Evaluate Scatter pays
	numBonus = baseGrid->CountSymbolOnGrid(BONUS);
	score += totalBet * paytable[BONUS][numBonus];
	AddToTracker("BaseGame", score);

	// Trigger free games
	if (numBonus >= 3)
	{
		score += PlayBonus();
	}

	return score;
}
double SlotGame::PlayBonus()
{
	double score = 0;
	int index = 0;
	long long weight = 0;
	double value = 0;
	vector<int> positions;
	int spinsRemaining = numFreeGames[numBonus];
	int spinNumber = 1;

	while (spinsRemaining > 0)
	{
		if (inFreePlay) cout << "=== Free Spin " << spinNumber << " ===\n";

		double spinScore = 0;

		// Generate positions and fill grid
		positions = freeReelSet->GenerateRandomPositions();
		freeGrid->FillGrid(positions, freeReelSet);

		// Determine which 2 reels to fill with WILDs
		weightTable["freeSpinWild"].Call(weight, index, value);
		vector<int> wildReels = ChangeBase(value, 2, numReels);
		for (int iReel = 0; iReel < numReels; iReel++)
		{
			if (wildReels[iReel] == 1)
			{
				freeGrid->FillReelWithSymbol(WILD, iReel);
			}
		}

		// Evaluate Lines/Ways
		spinScore += betMult * freeGrid->EvaluateGridLines(symbolCombos);

		// Evaluate Scatter pays and bonus triggers
		numBonus = freeGrid->CountSymbolOnGrid(BONUS);
		spinScore += totalBet * paytable[BONUS][numBonus];
		AddToTracker("FreeSpin", spinScore);
		spinsRemaining += numFreeGames[numBonus];

		score += spinScore;
		spinNumber++;
		spinsRemaining--;
	}

	AddToTracker("FreeSpinBonus", score);
	return score;
}

// ============================== Other Functions ==============================
void SlotGame::DoSomething()
{
	// Blank function for testing
}

void SlotGame::AddToTracker(string name, double value)
{
	gameTotalValues[name] += value;
	gameTotalHits[name]++;
	if (value > 0) gameTotalWins[name]++;
}

void SlotGame::ClearTrackers()
{
	gameTotalValues.clear();
	gameTotalHits.clear();
	gameTotalWins.clear();
}

void SlotGame::AddToHistogram(string name, double value, long long numHits)
{
	histograms[name][value] += numHits;
}

void SlotGame::PrintHistograms(string simName)
{
	for (auto const& [name, hist] : histograms)
	{
		string filename = "Histogram_" + simName + "_" + name + ".txt";
		ofstream outputFile(filename);
		for (auto const& [value, count] : hist)
		{
			outputFile << FormatDouble(value, 15) << "\t" << count << "\n";
		}
		outputFile.close();
	}
}

void SlotGame::RunSims(int trialSize, vector<string>& args, bool outputHistograms)
{
	// Opening more processes if necessary
	int numProcesses = 0;
	if (args.size() > 1)
	{
		numProcesses = stoi(args[1]);
	}
	else 
	{
		std::cout << "How many processes would you like to run? (1-" << thread::hardware_concurrency() << "): ";
		std::cin >> numProcesses;
		args.push_back("1");
	}
	if (numProcesses > 1)
	{
		SpawnProcesses(args[0], numProcesses - 1, 2); // Final argument is the delay in seconds between opening processes
	}

	// Initializing variables for the sim
	string simName = to_string(baseBet) + "cr_" + to_string(betMult) + "x";
	int percentile = trialSize / 100;
	double coinIn = 0;
	double coinOut = 0;
	int maxWin = 0;
	int hits = 0;
	int wins = 0;
	double buyIn = 50 * totalBet;
	double credits = buyIn;
	int numSpins = 0;
	map<int, int> spinsHist;
	map<string, double> trialValue;
	map<string, int> trialGameHits;
	map<string, int> trialGameWins;
	map<string, int> trialTotalHits;
	map<string, int> trialTotalWins;
	auto startTime = chrono::high_resolution_clock::now();
	cout << "Running Sim: " << simName << "\n";
	for (int iGame = 1; iGame <= trialSize; iGame++)
	{
		// Play Game
		double score = PlayGame();
		coinOut += score;
		coinIn += totalBet;
		// Update Values
		if (score > maxWin) maxWin = score;
		if (score > 0) hits++;
		if (score > totalBet) wins++;
		// Median Spins
		numSpins++;
		credits += score - totalBet;
		if (credits < totalBet || numSpins > 1000)
		{
			spinsHist[numSpins]++;
			credits = buyIn;
			numSpins = 0;
		}
		// Save and Reset Trackers
		for (auto const& [name, value] : gameTotalValues)
		{
			trialValue[name] += value;
			trialGameHits[name]++;
			if (value > 0) trialGameWins[name]++;
			trialTotalHits[name] += gameTotalHits[name];
			trialTotalWins[name] += gameTotalWins[name];
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
	if (outputHistograms) PrintHistograms(simName);
	string filename = "SimsOutput_" + simName + ".txt";
	ofstream outputFile(filename, ios::app);
	outputFile << FormatDouble(coinOut / coinIn, 15) << "\t" << totalBet << "\t" << trialSize << "\t" << maxWin << "\t" << hits << "\t" << wins << "\t" << GetMedian(spinsHist);
	for (auto const& [name, value] : trialValue)
	{
		outputFile << "\t" << name;
		outputFile << "\t" << FormatDouble(trialValue[name] / trialSize, 15);
		outputFile << "\t" << trialGameHits[name];
		outputFile << "\t" << trialGameWins[name];
		outputFile << "\t" << trialTotalHits[name];
		outputFile << "\t" << trialTotalWins[name];
	}
	outputFile << "\n";
	outputFile.close();
	
}

void SlotGame::FreePlay(bool clearConsole)
{
	inFreePlay = true;
	baseGrid->SetInFreePlay(true);
	freeGrid->SetInFreePlay(true);
	string input;
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
		cout << "Score: " << score << "\nCoin In:\t" << coinIn << "\nCoin Out:\t" << coinOut << "\n\nPress Enter to Play again... ";
		cin.get();
	}
}

void SlotGame::CyclePositions()
{
	// Will cycle through all positions of a reel set and create a histogram of every possible score
	map<double, size_t> hist;
	vector<int> positions(numReels, 0);
	double maxScore = 0;
	vector<int> maxPositions(numReels, 0);
	CyclePositionsRecursive(hist, positions, maxScore, maxPositions);

	string filename = "CyclePositionsOutput_" + to_string(baseBet) + "cr_" + to_string(betMult) + "x.txt";
	ofstream outputFile(filename);
	outputFile << "Max Pay Positions:\t";
	for (int iReel = 0; iReel < numReels; iReel++)
	{
		outputFile << maxPositions[iReel] << "\t";
	}
	outputFile << "\n\n";
	for (auto const& [score, combo] : hist)
	{
		outputFile << score << "\t" << combo << "\n";
	}
	outputFile.close();
}

void SlotGame::CyclePositionsRecursive(map<double, size_t>& hist, vector<int>& positions, double& maxScore, vector<int>& maxPositions, int currentReel)
{
	// Function used to run CyclePositions()
	// Might need to edit your grid, reels, and evaluation type being used
	if (currentReel < positions.size())
	{
		for (int i = 0; i < baseReelSet->GetReelSize(currentReel); i++)
		{
			if (currentReel == 0)
			{
				cout << i << " / " << baseReelSet->GetReelSize(currentReel) << "\n";
			}
			positions.at(currentReel) = i;
			baseGrid->FillGridReel(currentReel, positions[currentReel], baseReelSet);
			CyclePositionsRecursive(hist, positions, maxScore, maxPositions, currentReel + 1);
		}
	}
	else
	{
		double score = betMult * baseGrid->EvaluateGridLines(symbolCombos);
		int combos = 1;
		for (int iReel = 0; iReel < positions.size(); iReel++)
		{
			combos *= baseReelSet->GetWeight(iReel, positions[iReel]);
		}
		hist[score] += combos;
		if (score > maxScore)
		{
			maxPositions = positions;
			maxScore = score;
		}
	}
}