
#include "SlotGame.h"

// ============================== Constructor and Destructor ==============================
SlotGame::SlotGame()
{
	// Create pointers here
	baseReelSet = new SlotReels(baseReels, baseReelWeights);
	baseGrid = new SlotGrid(numReels, numRows);
	symbolComboInfo = new SymbolComboInfo(numReels, numSymbols, paytable, symbolSubstitutions, symbolMultipliers, true);
}

SlotGame::~SlotGame()
{
	// Delete pointers here
	delete baseReelSet;
	delete baseGrid;
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
	baseGrid->SetLines(CustomLines, 1);
	baseGrid->SetWays(numSymbols, paytable, symbolSubstitutions, symbolMultipliers);
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

	vector<int> positions = baseReelSet->GenerateRandomPositions();
	baseGrid->FillGrid(positions, baseReelSet);
	if (printComboInfo)
	{
		baseGrid->PrintGrid();
	}
	//score += baseGrid->EvaluateGridLines(symbolComboInfo);
	score += baseGrid->EvaluateGridWays();

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
	vector<long long> weights = {1,0,2,0,3,0,4,0,5,0,0,0,1};
	WeightTable weightTable(weights);
	int index = 0;
	long long weight = 0;
	double value = 0;
	for (int i = 0; i < 30; i++)
	{
		weightTable.Call(weight, index, value);
		cout << weight << "\t" << index << "\t" << value << "\n";
	}
}

void SlotGame::RunSims(int numTrials, int trialSize)
{
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
	baseGrid->SetPrintComboInfo(true);
	string input;
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

void SlotGame::CyclePositions()
{
	// Will cycle through all positions of a reel set and create a histogram of every possible score
	map<double, size_t> hist;
	vector<int> positions(numReels,0);
	double maxScore = 0;
	vector<int> maxPositions(numReels, 0);
	CyclePositionsRecursive(hist, positions, maxScore, maxPositions);

	ofstream outputFile("CycleStopsOutput.txt");
	outputFile << "Max Pay Stops:\t";
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
	// Function used to run CycleStops()
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
		double score = baseGrid->EvaluateGridWays();
		int combos = 1;
		for (int iReel=0; iReel < positions.size(); iReel++)
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