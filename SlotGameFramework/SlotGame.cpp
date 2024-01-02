
#include "SlotGame.h"

// ============================== Constructor and Destructor ==============================
SlotGame::SlotGame()
{
	// Create pointers here
	baseReelSet = new SlotReels(baseReels);
	baseGrid = new SlotGrid(numReels, numRows);
	//multiSymbolComboInfo = new MultiSymbolComboInfo(numReels, numSymbols, paytable, symbolSubstitutions, symbolMultipliers);
	symbolComboInfo = new SymbolComboInfo(numReels, numSymbols, paytable, symbolSubstitutions, symbolMultipliers, true);
}

SlotGame::~SlotGame()
{
	// Delete pointers here
	delete baseReelSet;
	delete baseGrid;
	//delete multiSymbolComboInfo;
	delete symbolComboInfo;
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
	baseGrid->SetSymbolStrings(symbolStrings);
	baseGrid->SetPrintComboInfo(printComboInfo);
	baseGrid->SetLines(CustomLines, 5);
	//baseGrid->SetWays(numSymbols, paytable, symbolSubstitutions, symbolMultipliers);
}

void SlotGame::SetupReels()
{
	//baseReelSet->ReplaceSymbolOnReel(1, MM, SE);
	//baseReelSet->ReplaceSymbolOnReel(2, MM, SE);
	//baseReelSet->ReplaceSymbolOnReel(3, MM, BE);
}

void SlotGame::SetupWeightTables()
{
}
// ============================== Game Functions ==============================
double SlotGame::PlayGame()
{
	double score = 0;

	vector<int> stops = baseReelSet->GenerateRandomStops();
	baseGrid->FillGrid(stops, baseReelSet);
	if (printComboInfo)
	{
		baseGrid->PrintGrid();
	}
	score += baseGrid->EvaluateGridLines(symbolComboInfo);
	//score += baseGrid->EvaluateGridWays();

	return score;
}
double SlotGame::PlayBonus()
{
	double score = 0;
	return score;
}
// ============================== Other Functions ==============================
void SlotGame::DoSomething()
{
	// Blank function used for testing

	/*for (int i = 0; i <= 30; i++)
	{
		cout << i << endl;
		vector<int> output = ChangeBase(i, 5, 5);
		PrintVec(output);
	}*/


	printComboInfo = true;
	SetupGame();
	double score = PlayGame();

	cout << "SCORE: " << score << endl;
}

void SlotGame::RunSims(int numTrials, int trialSize)
{
	SetupGame();
	map<double, size_t> hist;
	int percentile = trialSize / 100;
	double coinIn = 0;
	double coinOut = 0;

	for (int iTrial = 0; iTrial < numTrials; iTrial++)
	{
		for (int iGame = 1; iGame <= trialSize; iGame++)
		{
			double score = PlayGame();
			coinOut += score;
			coinIn += totalBet;
			hist[PlayGame()]++;
			if (iGame % percentile == 0)
			{
				cout << iGame / percentile << "/100\t" << coinOut / coinIn << endl;
			}
		}
	}

	ofstream outputFile("SimsOutput.txt");
	for (auto const& [score, combo] : hist)
	{
		outputFile << score << "\t" << combo << "\n";
	}
	outputFile.close();
}

void SlotGame::FreePlay()
{
	printComboInfo = true;
	string input;
	SetupGame();
	cout << "Press Enter to Play!";
	cin.get();

	double coinIn = 0;
	double coinOut = 0;
	while (true)
	{
		system("cls");
		cout << "\n";
		double score = PlayGame();
		coinIn += totalBet;
		coinOut += score;
		cout << "\nScore: " << score << "\nCoin In:\t" << coinIn << "\nCoin Out:\t" << coinOut << "\n\nPress Enter to Play again... ";
		cin.get();

	}
}

void SlotGame::CycleStops()
{
	// Will cycle through all stops of a reel set and create a histogram of every possible score
	map<double, size_t> hist;
	vector<int> stops(numReels,0);
	CycleStopsRecursive(hist, stops);

	ofstream outputFile("CycleStopsOutput.txt");
	for (auto const& [score, combo] : hist)
	{
		outputFile << score << "\t" << combo << "\n";
	}
	outputFile.close();
}

void SlotGame::CycleStopsRecursive(map<double, size_t>& hist, vector<int>& stops, int currentReel)
{
	// Function used to run CycleStops()
	// Might need to edit your grid, reels, and evaluation type being used
	if (currentReel < stops.size())
	{
		for (int i = 0; i < baseReelSet->m_reelSizes[currentReel]; i++)
		{
			if (currentReel == 0)
			{
				cout << i << " / " << baseReelSet->m_reelSizes[currentReel] << "\n";
			}
			stops.at(currentReel) = i;
			baseGrid->FillGridColumn(stops, baseReelSet, currentReel);
			CycleStopsRecursive(hist, stops, currentReel + 1);
		}
	}
	else
	{
		double score = baseGrid->EvaluateGridLines(symbolComboInfo);
		int combos = 1;
		for (int i=0; i<stops.size(); i++)
		{
			combos *= baseReelSet->m_reelWeights[i][stops[i]];
		}
		hist[score] += combos;
	}
}