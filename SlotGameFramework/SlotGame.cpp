
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

void SlotGame::SetConfig(string configName)
{
	this->configName = configName;
	map<string, string> configMapping =
	{
		{"GameName", "math.40.937.xml"}
	};
	mathxml = configMapping.at(configName);
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

void SlotGame::SetupGrids()
{
	ReadXMLCombos(mathxml);
	symbolCombos = comboSets.at("MainPaylineCombos");

	baseGrid = SlotGrid(numReels, numRows);
	baseGrid.SetLines(lines, baseBet);
	baseGrid.SetSymbolPrintInfo(symbolStrings, symbolColors);

	freeGrid = SlotGrid(numReels, numRows);
	freeGrid.SetLines(lines, baseBet);
	freeGrid.SetSymbolPrintInfo(symbolStrings, symbolColors);
}

void SlotGame::SetupReels()
{
	ReadXMLReels(mathxml);
	baseReelSet = reelSets.at("Reels_Main");
	freeReelSet = reelSets.at("Reels_FG");
}

void SlotGame::SetupWeightTables()
{
	MathXML mathXML = MathXML(mathxml);

	mathXML.LoadAllWeightTables(weightTables);
	mathXML.LoadAllValueTables(valueTables);
	//ReadXMLTables(mathxml);
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
	AddToHistogram("score", score);
	return score;
}
double SlotGame::PlayBonus()
{
	double score = 0;
	vector<int> positions(numReels);
	int spinsRemaining = valueTables["FSNumSpins"][numBonus-3];
	int spinNumber = 1;

	while (spinsRemaining > 0)
	{
		if (inFreePlay) cout << "=== Free Spin " << spinNumber << " ===\n";

		double spinScore = 0;

		// Generate positions and fill grid
		freeReelSet.GenerateRandomPositions(positions);
		freeGrid.FillGrid(positions, freeReelSet);

		// Determine which 2 reels to fill with WILDs
		int wildPattern = weightTables["FSWildReels"].DrawValue();
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
		if (numBonus >= 3) spinsRemaining += valueTables["FSNumSpins"][numBonus - 3];

		score += spinScore;
		spinNumber++;
		spinsRemaining--;
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

void SlotGame::PrintHistograms(string simName)
{
	for (auto const& [name, hist] : histograms)
	{
		string filename = "Histogram_" + simName + "_" + name + ".txt";
		WriteHistogramToFile(filename, hist);
	}
}

void SlotGame::RunSims(int numGames, vector<string>& args)
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
	PrintHistograms(simName);
	string filename = "SimData_" + simName + ".txt";
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
	baseGrid.SetInFreePlay(true);
	freeGrid.SetInFreePlay(true);
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

	// Get GameMath and BonusInfo Elements
	tinyxml2::XMLElement* GameMath = doc.FirstChildElement("GameMath");
	tinyxml2::XMLElement* BonusInfo = GameMath->FirstChildElement("BonusInfo");

	// Check for BonusInfo (any weight or value tables stored here)
	if (BonusInfo)
	{
		// Check for Weight Tables
		tinyxml2::XMLElement* WeightedTableList = BonusInfo->FirstChildElement("WeightedTableList");
		if (WeightedTableList)
		{
			// Loop through each weight table
			tinyxml2::XMLElement* WeightedTable = WeightedTableList->FirstChildElement("WeightedTable");
			while (WeightedTable != nullptr)
			{
				// Get the weight table name
				string Identifier = WeightedTable->FirstChildElement("Identifier")->GetText();

				// Loop through the weights and values
				vector<long long> weights;
				vector<double> values;
				tinyxml2::XMLElement* WeightedElement = WeightedTable->FirstChildElement("WeightedElementList")->FirstChildElement("WeightedElement");
				while (WeightedElement != nullptr)
				{
					tinyxml2::XMLElement* Weight = WeightedElement->FirstChildElement("Weight");
					weights.push_back(stoll(Weight->GetText()));
					tinyxml2::XMLElement* Value = WeightedElement->FirstChildElement("Value");
					values.push_back(stod(Value->GetText()));
					WeightedElement = WeightedElement->NextSiblingElement("WeightedElement");
				}

				// Add to Map
				weightTables.emplace(Identifier, WeightTable(weights, values));
				WeightedTable = WeightedTable->NextSiblingElement("WeightedTable");
			}
		}

		// Check for Value Tables
		tinyxml2::XMLElement* ValueTableList = BonusInfo->FirstChildElement("ValueTableList");
		if (ValueTableList)
		{
			// Loop through each value table
			tinyxml2::XMLElement* ValueTable = ValueTableList->FirstChildElement("ValueTable");
			while (ValueTable != nullptr)
			{
				// Get the Value Table Name
				string Identifier = ValueTable->FirstChildElement("Identifier")->GetText();

				// Loop through the values
				vector<double> values;
				tinyxml2::XMLElement* Value = ValueTable->FirstChildElement("ValueList")->FirstChildElement("Value");
				while (Value != nullptr)
				{
					values.push_back(stod(Value->GetText()));
					Value = Value->NextSiblingElement("Value");
				}

				// Add to Map
				valueTables[Identifier] = values;
				ValueTable = ValueTable->NextSiblingElement("ValueTable");
			}
		}
	}

	// Check for MysteryReplacementInfo
	tinyxml2::XMLElement* MysteryReplacementInfo = GameMath->FirstChildElement("MysteryReplacementInfo");
	if (MysteryReplacementInfo)
	{
		// Loop through each Repalcement Sequence
		tinyxml2::XMLElement* ReplacementSequence = MysteryReplacementInfo->FirstChildElement("ReplacementSequenceList")->FirstChildElement("ReplacementSequence");
		while (ReplacementSequence != nullptr)
		{
			// Grab the replacement ID as well as the weights and values IDs
			string replacementSequenceID = ReplacementSequence->Attribute("replacementSequenceID");
			string ReplacementInstructionTableID = ReplacementSequence->FirstChildElement("ReplacementAction")->FirstChildElement("ReplacementInstructionTableID")->GetText();
			string ReplacementWeightTableID = ReplacementSequence->FirstChildElement("ReplacementAction")->FirstChildElement("ReplacementWeightTableID")->GetText();

			vector<long long> weights;
			vector<double> values;

			// Cycle through ReplacementInstructions to grab the replacement symbols (values)
			tinyxml2::XMLElement* ReplacementInstructionTable = MysteryReplacementInfo->FirstChildElement("ReplacementInstructionTableList")->FirstChildElement("ReplacementInstructionTable");
			while (ReplacementInstructionTable != nullptr)
			{
				if (ReplacementInstructionTableID == ReplacementInstructionTable->Attribute("replacementInstructionTableID"))
				{
					tinyxml2::XMLElement* ReplacementInstruction = ReplacementInstructionTable->FirstChildElement("ReplacementInstruction");
					while (ReplacementInstruction != nullptr)
					{
						string symbol = ReplacementInstruction->FirstChildElement("DoReplace")->Attribute("newSymbol");
						values.push_back(magic_enum::enum_cast<Symbols>(symbol).value());
						ReplacementInstruction = ReplacementInstruction->NextSiblingElement("ReplacementInstruction");
					}
					break;
				}
				ReplacementInstructionTable = ReplacementInstructionTable->NextSiblingElement("ReplacementInstructionTable");
			}

			// Cycle through ReplacementWeightTables to grab the weights
			tinyxml2::XMLElement* ReplacementWeightTable = MysteryReplacementInfo->FirstChildElement("ReplacementWeightTableList")->FirstChildElement("ReplacementWeightTable");
			while (ReplacementWeightTable != nullptr)
			{
				if (ReplacementWeightTableID == ReplacementWeightTable->Attribute("replacementWeightTableID"))
				{
					tinyxml2::XMLElement* Weight = ReplacementWeightTable->FirstChildElement("Weight");
					while (Weight != nullptr)
					{
						string weight = Weight->GetText();
						weights.push_back(stoll(weight));
						Weight = Weight->NextSiblingElement("Weight");
					}
					break;
				}
				ReplacementWeightTable = ReplacementWeightTable->NextSiblingElement("ReplacementWeightTable");
			}

			// Add New Weight Table
			weightTables.emplace(replacementSequenceID, WeightTable(weights, values));
			ReplacementSequence = ReplacementSequence->NextSiblingElement("ReplacementSequence");
		}
	}
}

void SlotGame::ReadXMLReels(string xmlName)
{
	// Open the XML document
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(xmlName.c_str()) != tinyxml2::XML_SUCCESS)
	{
		cout << "Error reading xml file: " << doc.ErrorStr() << endl;
		return;
	}

	// Loop Through Reel Sets
	tinyxml2::XMLElement* ReelStripSet = doc.FirstChildElement("GameMath")->FirstChildElement("ReelStripSetList")->FirstChildElement("ReelStripSet");
	while (ReelStripSet != nullptr)
	{
		// Get Name
		string Identifier = ReelStripSet->FirstChildElement("Identifier")->GetText();

		// Loop Through ReelStripIDs
		vector<string> reelStripNames;
		tinyxml2::XMLElement* ReelStripID = ReelStripSet->FirstChildElement("ReelStripIDList")->FirstChildElement("ReelStripID");
		while (ReelStripID != nullptr)
		{
			reelStripNames.push_back(ReelStripID->GetText());
			ReelStripID = ReelStripID->NextSiblingElement("ReelStripID");
		}

		// Loop Through Reel Strips Names
		vector<vector<int>> reelSet;
		vector<vector<int>> reelSetWeights;
		for (string reelStripName : reelStripNames)
		{
			tinyxml2::XMLElement* ReelStrip = doc.FirstChildElement("GameMath")->FirstChildElement("ReelStripList")->FirstChildElement("ReelStrip");

			// Loop Through XML Reel Strips to Find Matching Name
			vector<int> reelSymbols;
			vector<int> reelWeights;
			while (ReelStrip != nullptr)
			{
				if (ReelStrip->FirstChildElement("Identifier")->GetText() == reelStripName)
				{
					tinyxml2::XMLElement* WeightedElement = ReelStrip->FirstChildElement("WeightedElementList")->FirstChildElement("WeightedElement");

					// Grab each of the symbols and weights
					while (WeightedElement != nullptr)
					{
						string symbolText = WeightedElement->FirstChildElement("StringValue")->GetText();
						reelSymbols.push_back(magic_enum::enum_cast<Symbols>(symbolText).value());

						string symbolWeight = WeightedElement->FirstChildElement("Weight")->GetText();
						reelWeights.push_back(stoi(symbolWeight));

						WeightedElement = WeightedElement->NextSiblingElement("WeightedElement");
					}
					break;
				}
				ReelStrip = ReelStrip->NextSiblingElement("ReelStrip");
			}
			reelSet.push_back(reelSymbols);
			reelSetWeights.push_back(reelWeights);
		}
		reelSets.emplace(Identifier, SlotReels(reelSet, reelSetWeights));
		ReelStripSet = ReelStripSet->NextSiblingElement("ReelStripSet");
	}
}

void SlotGame::ReadXMLCombos(string xmlName)
{
	// Open the XML document
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(xmlName.c_str()) != tinyxml2::XML_SUCCESS)
	{
		cout << "Error reading xml file: " << doc.ErrorStr() << endl;
		return;
	}

	// Loop through each combo set
	tinyxml2::XMLElement* PaylineComboSet = doc.FirstChildElement("GameMath")->FirstChildElement("ComboSetList")->FirstChildElement("PaylineComboSet");
	while (PaylineComboSet != nullptr)
	{
		string comboSetName = PaylineComboSet->FirstChildElement("Identifier")->GetText();
		//SymbolCombos combos = SymbolCombos(numReels, numSymbols, symbolSubstitutions, symbolMultipliers); // NOTE: Might have to change the number of reels per combo set if there are different number of reels 

		// Loop through each combo
		tinyxml2::XMLElement* PaylineCombo = PaylineComboSet->FirstChildElement("PaylineComboList")->FirstChildElement("PaylineCombo");
		while (PaylineCombo != nullptr)
		{
			// Loop through the symbols to construct the combo
			vector<int> combo;
			tinyxml2::XMLElement* Symbol = PaylineCombo->FirstChildElement("SymbolList")->FirstChildElement("Symbol");
			while (Symbol != nullptr)
			{
				combo.push_back(magic_enum::enum_cast<Symbols>(Symbol->GetText()).value());
				Symbol = Symbol->NextSiblingElement("Symbol");
			}

			// Get the Pay and add it to the symbol combos
			double pay = stod(PaylineCombo->FirstChildElement("Value")->GetText());
			int bonusCode = 0;
			tinyxml2::XMLElement* BonusCode = PaylineCombo->FirstChildElement("BonusCode");
			if (BonusCode != nullptr) bonusCode = stoi(BonusCode->GetText());
			//combos.SetCombo(combo, pay, bonusCode);
			PaylineCombo = PaylineCombo->NextSiblingElement("PaylineCombo");
		}
		//comboSets.emplace(comboSetName, combos);
		PaylineComboSet = PaylineComboSet->NextSiblingElement("PaylineComboSet");
	}
}