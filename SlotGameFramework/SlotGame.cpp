
#include "SlotGame.h"

// ============================== Constructor and Destructor ==============================
SlotGame::SlotGame()
{
	// Create pointers here
	baseReelSet = new SlotReels(baseReels);
	freeReelSet = new SlotReels(freeReels);
	baseGrid = new SlotGrid(numReels, numRows);
	symbolComboInfo = new SymbolComboInfo(numReels, numSymbols, paytable, symbolSubstitutions, symbolMultipliers, true);
}

SlotGame::~SlotGame()
{
	// Delete pointers here
	delete baseReelSet;
	delete freeReelSet;
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
	baseGrid->SetLines(CustomLines, 5);
	baseGrid->SetWays(numSymbols, paytable, symbolSubstitutions, symbolMultipliers);
}

void SlotGame::SetupReels()
{
}

void SlotGame::SetupWeightTables()
{
}
// ============================== Game Functions ==============================
double SlotGame::PlayGame()
{
	double score = 0;

	// Generate positions and fill grid
	vector<int> positions = baseReelSet->GenerateRandomPositions();
	baseGrid->FillGrid(positions, baseReelSet);

	// Expand wilds
	for (int iReel = 0; iReel < 3; iReel++)
	{
		if (baseGrid->IsSymbolOnReel(WILD, 1 + iReel))
		{
			baseGrid->FillReelWithSymbol(WILD, 1 + iReel);
		}
	}
	if (printComboInfo)
	{
		baseGrid->PrintGrid();
	}
	score += baseGrid->EvaluateGridLines(symbolComboInfo);

	numBonus = baseGrid->CountSymbolOnGrid(BONUS);
	if (numBonus >= 3)
	{
		score += PlayBonus();
	}

	return score;
}
double SlotGame::PlayBonus()
{
	double score = 0;
	vector<int> positions;
	int spinsRemaining = numFreeGames[numBonus];

	while (spinsRemaining > 0)
	{
		positions = freeReelSet->GenerateRandomPositions();
		baseGrid->FillGrid(positions, freeReelSet);

		for (int iReel = 0; iReel < 3; iReel++)
		{
			if (!wildReels[iReel] && baseGrid->IsSymbolOnReel(WILD, 1 + iReel))
			{
				wildReels[iReel] = true;
			}
			if (wildReels[iReel])
			{
				baseGrid->FillReelWithSymbol(WILD, 1 + iReel);
			}
		}

		score += baseGrid->EvaluateGridLines(symbolComboInfo);
		spinsRemaining--;
	}
	for (int iReel = 0; iReel < 3; iReel++)
	{
		wildReels[iReel] = false;
	}
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

void SlotGame::TestStops()
{
	vector<vector<int>> stops = {
		{ 14, 15, 16, 17, 56, 57, 58, 59 },
		{ 20, 21, 22, 23, 40, 41, 42, 43 },
		{ 7, 8, 9, 10, 24, 25, 26, 27 },
		{ 29, 30, 31, 32 },
		{ 30, 31, 32, 33, 62, 63, 64, 65 },
	};

	double maxPay = 0;
	vector<int> maxStops = {};

	for (int i1 = 0; i1 < stops[0].size(); i1++)
	{
		baseGrid->FillGridReel(0, stops[0][i1], baseReelSet);


		for (int i2 = 0; i2 < stops[1].size(); i2++)
		{
			baseGrid->FillGridReel(1, stops[1][i2], baseReelSet);


			for (int i3 = 0; i3 < stops[2].size(); i3++)
			{
				baseGrid->FillGridReel(2, stops[2][i3], baseReelSet);


				for (int i4 = 0; i4 < stops[3].size(); i4++)
				{
					baseGrid->FillGridReel(3, stops[3][i4], baseReelSet);


					for (int i5 = 0; i5 < stops[4].size(); i5++)
					{
						baseGrid->FillGridReel(4, stops[4][i5], baseReelSet);
						double score = baseGrid->EvaluateGridWays();
						cout << score << endl;
						if (score > maxPay)
						{
							maxPay = score;
							maxStops = { i1, i2, i3, i4, i5 };
						}
					}
				}
			}
		}
	}
	cout << "MAX: " << maxPay << endl;
	cout << "STOPS: " << maxStops[0] << maxStops[1] << maxStops[2] << maxStops[3] << maxStops[4] << endl;
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

	ofstream outputFile("CyclePositionsOutput.txt");
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
		for (int i = 0; i < freeReelSet->GetReelSize(currentReel); i++)
		{
			if (currentReel == 0)
			{
				cout << i << " / " << freeReelSet->GetReelSize(currentReel) << "\n";
			}
			positions.at(currentReel) = i;
			baseGrid->FillGridReel(currentReel, positions[currentReel], freeReelSet);
			CyclePositionsRecursive(hist, positions, maxScore, maxPositions, currentReel + 1);
		}
	}
	else
	{
		double score = baseGrid->EvaluateGridLines(symbolComboInfo);
		int combos = 1;
		for (int iReel=0; iReel < positions.size(); iReel++)
		{
			combos *= freeReelSet->GetWeight(iReel, positions[iReel]);
		}
		hist[score] += combos;
		if (score > maxScore)
		{
			maxPositions = positions;
			maxScore = score;
		}
	}
}