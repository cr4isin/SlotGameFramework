
#include "SlotGame.h"

// ============================== Setup ==============================
SlotGame::SlotGame(string configName, int baseBet, int betMult, int totalBet)
{
	SetConfig(configName);
	SetBetScheme(baseBet, betMult, totalBet);
	SetupGrids();
	SetupReels();
	SetupWeightTables();
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

void SlotGame::SetConfig(string configName)
{
	this->configName = configName;
	map<string, int> configMapping =
	{
		{"EXAMPLE", 0}
	};
	configIndex = configMapping.at(configName);
}

void SlotGame::SetupGrids()
{
	symbolCombos = SymbolCombos(numReels, numSymbols, paytable, symbolSubstitutions, symbolMultipliers);

	baseGrid = SlotGrid(numReels, numRows);
	baseGrid.SetLines(lines, baseBet);
	baseGrid.SetSymbolPrintInfo(symbolStrings, symbolColors);

	freeGrid = SlotGrid(numReels, numRows);
	freeGrid.SetLines(lines, baseBet);
	freeGrid.SetSymbolPrintInfo(symbolStrings, symbolColors);

	//baseGrid.SetWays(numSymbols, paytable, symbolSubstitutions, symbolMultipliers); // Ways Example
}

void SlotGame::SetupReels()
{
	baseReelSet = SlotReels(baseReels);
	freeReelSet = SlotReels(freeReels);
}

void SlotGame::SetupWeightTables()
{
	weightTables.emplace("freeSpinWild", WeightTable(freeSpinWildWeights, freeSpinWildValues));
}

// ============================== Game Functions ==============================
double SlotGame::PlayGame()
{
	if (inFreePlay) cout << "=== Base Game ===\n";
	double score = 0;
	vector<int> positions(numReels);

	// Generate positions and fill grid
	baseReelSet.GenerateRandomPositions(positions);
	baseGrid.FillGrid(positions, baseReelSet);

	// Evaluate Lines
	score += baseGrid.EvaluateLines(symbolCombos, betMult);

	// Evaluate Scatter pays
	numBonus = baseGrid.CountSymbolOnGrid(BONUS);
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
	vector<int> positions(numReels);
	int spinsRemaining = numFreeGames[numBonus];
	int spinNumber = 1;

	while (spinsRemaining > 0)
	{
		if (inFreePlay) cout << "=== Free Spin " << spinNumber << " ===\n";

		double spinScore = 0;

		// Generate positions and fill grid
		freeReelSet.GenerateRandomPositions(positions);
		freeGrid.FillGrid(positions, freeReelSet);

		// Determine which 2 reels to fill with WILDs
		int wildPattern = weightTables["freeSpinWild"].DrawValue();
		vector<int> wildReels = ChangeBase(wildPattern, 2, numReels);
		for (int iReel = 0; iReel < numReels; iReel++)
		{
			if (wildReels[iReel] == 1)
			{
				freeGrid.FillReelWithSymbol(WILD, iReel);
			}
		}

		// Evaluate Lines/Ways
		spinScore += freeGrid.EvaluateLines(symbolCombos, betMult);

		// Evaluate Scatter pays and bonus triggers
		numBonus = freeGrid.CountSymbolOnGrid(BONUS);
		spinScore += totalBet * paytable[BONUS][numBonus];
		AddToTracker("FreeGame", spinScore);
		spinsRemaining += numFreeGames[numBonus];

		score += spinScore;
		spinNumber++;
		spinsRemaining--;
	}

	return score;
}

// ============================== Other Functions ==============================
void SlotGame::DoSomething()
{
	// Blank function for testing
}

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

void SlotGame::RunSims(int numGames, vector<string>& args, bool outputHistograms)
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
		SpawnProcesses(args[0], numProcesses - 1, 2); // Final argument is the delay in seconds between opening processes
	}

	// Initializing variables for the sim
	string simName = configName + "_" + to_string(baseBet) + "cr_" + to_string(betMult) + "x";
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
	if (outputHistograms) PrintHistograms(simName);
	string filename = "SimData_" + simName + ".txt";
	ofstream outputFile(filename, ios::app);
	outputFile << FormatDouble(coinOut / coinIn, 15) << "\t" << totalBet << "\t" << numGames << "\t" << maxWin << "\t" << hits << "\t" << wins << "\t" << GetMedian(spinsHist);
	for (auto const& [name, totalTracker] : totalTrackers)
	{
		outputFile << "\t" << name;
		outputFile << "\t" << FormatDouble(totalTracker.value / numGames, 15);
		outputFile << "\t" << totalTracker.maxValue;
		outputFile << "\t" << totalTracker.gameHits;
		outputFile << "\t" << totalTracker.gameWins;
		outputFile << "\t" << totalTracker.totalHits;
		outputFile << "\t" << totalTracker.totalWins;
	}
	outputFile << "\n";
	outputFile.close();
}

void SlotGame::FreePlay(bool clearConsole)
{
	inFreePlay = true;
	baseGrid.SetInFreePlay(true);
	freeGrid.SetInFreePlay(true);
	string input;
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

void SlotGame::CyclePositions()
{
	// Will cycle through all positions of a reel set and create a histogram of every possible score
	map<double, size_t> hist;
	vector<int> positions(numReels, 0);
	double maxScore = 0;
	vector<int> maxPositions(numReels, 0);
	CyclePositionsRecursive(hist, positions, maxScore, maxPositions);

	string filename = "CyclePositionsOutput_" + configName + "_" + to_string(baseBet) + "cr_" + to_string(betMult) + "x.txt";
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
		for (int i = 0; i < baseReelSet.GetReelSize(currentReel); i++)
		{
			if (currentReel == 0)
			{
				cout << i << " / " << baseReelSet.GetReelSize(currentReel) << "\n";
			}
			positions.at(currentReel) = i;
			baseGrid.FillGridReel(currentReel, positions[currentReel], baseReelSet);
			CyclePositionsRecursive(hist, positions, maxScore, maxPositions, currentReel + 1);
		}
	}
	else
	{
		double score = baseGrid.EvaluateLines(symbolCombos, betMult);
		int combos = 1;
		for (int iReel = 0; iReel < positions.size(); iReel++)
		{
			combos *= baseReelSet.GetWeight(iReel, positions[iReel]);
		}
		hist[score] += combos;
		if (score > maxScore)
		{
			maxPositions = positions;
			maxScore = score;
		}
	}
}

void SlotGame::ReadXMLTables(string xmlName)
{
	// Open the XML document
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(xmlName.c_str()) != tinyxml2::XML_SUCCESS)
	{
		cout << "Error reading xml file: " << doc.ErrorStr() << endl;
		return;
	}

	// Follow the element path to the first weight table
	vector<string> elementPath = { "GameMath", "BonusInfo", "WeightedTableList", "WeightedTable" };
	tinyxml2::XMLElement* currentElement = doc.FirstChildElement(elementPath[0].c_str());
	for (int i = 1; i < elementPath.size(); i++)
	{
		if (currentElement) {
			currentElement = currentElement->FirstChildElement(elementPath[i].c_str());
		}
		else {
			cout << "Error reading xml file: Element '" << elementPath[i - 1] << "' not found!" << endl;
			return;
		}
	}

	// Loop through each weight table
	while (currentElement != nullptr)
	{
		string name;
		vector<long long> weights;
		vector<double> values;

		// Get the weight table name
		tinyxml2::XMLElement* Identifier = currentElement->FirstChildElement("Identifier");
		if (Identifier)
		{
			name = Identifier->GetText();
		}
		else
		{
			cout << "Error reading xml file: Identifier not found for a weight table!" << endl;
			return;
		}

		// Loop through the weights and values
		tinyxml2::XMLElement* WeightedElementList = currentElement->FirstChildElement("WeightedElementList");
		if (WeightedElementList)
		{
			tinyxml2::XMLElement* WeightedElement = WeightedElementList->FirstChildElement("WeightedElement");
			while (WeightedElement != nullptr)
			{
				tinyxml2::XMLElement* WeightElement = WeightedElement->FirstChildElement("Weight");
				if (WeightElement)
				{
					weights.push_back(stoll(WeightElement->GetText()));
				}
				else
				{
					cout << "Error reading xml file: Weight not found for a weight table!" << endl;
				}
				tinyxml2::XMLElement* ValueElement = WeightedElement->FirstChildElement("Value");
				if (ValueElement)
				{
					values.push_back(stod(ValueElement->GetText()));
				}
				else
				{
					cout << "Error reading xml file: Value not found for a weight table!" << endl;
				}
				WeightedElement = WeightedElement->NextSiblingElement("WeightedElement");
			}
		}
		weightTables.emplace(name, WeightTable(weights, values));
		currentElement = currentElement->NextSiblingElement("WeightedTable");
	}

	// Follow the element path to the first value table
	elementPath = { "GameMath", "BonusInfo", "ValueTableList", "ValueTable" };
	currentElement = doc.FirstChildElement(elementPath[0].c_str());
	for (int i = 1; i < elementPath.size(); i++)
	{
		if (currentElement) {
			currentElement = currentElement->FirstChildElement(elementPath[i].c_str());
		}
		else {
			cout << "Error reading xml file: Element '" << elementPath[i - 1] << "' not found!" << endl;
			return;
		}
	}

	// Loop through each value table
	while (currentElement != nullptr)
	{
		string name;
		vector<double> values;

		// Get the weight table name
		tinyxml2::XMLElement* Identifier = currentElement->FirstChildElement("Identifier");
		if (Identifier)
		{
			name = Identifier->GetText();
		}
		else
		{
			cout << "Error reading xml file: Identifier not found for a value table!" << endl;
			return;
		}

		// Loop through the values
		tinyxml2::XMLElement* ValueList = currentElement->FirstChildElement("ValueList");
		if (ValueList)
		{
			tinyxml2::XMLElement* ValueElement = ValueList->FirstChildElement("Value");
			while (ValueElement != nullptr)
			{
				values.push_back(stod(ValueElement->GetText()));
				ValueElement = ValueElement->NextSiblingElement("Value");
			}
		}
		valueTables[name] = values;
		currentElement = currentElement->NextSiblingElement("ValueTable");
	}
}
