
#include "SlotGame.h"

// ============================== Constructor and Destructor ==============================
SlotGame::SlotGame()
{
	// Create pointers here
	baseReelSet = new SlotReels(baseReels);
	baseGrid = new SlotGrid(numReels, numRows);
	multiSymbolComboInfo = new MultiSymbolComboInfo(numReels, numSymbols, paytable, symbolSubstitutions, symbolMultipliers);
	symbolComboInfo = new SymbolComboInfo(numReels, numSymbols, paytable, symbolSubstitutions, symbolMultipliers);
}

SlotGame::~SlotGame()
{
	// Delete pointers here
	delete baseReelSet;
	delete baseGrid;
	delete multiSymbolComboInfo;
	delete symbolComboInfo;
}
// ============================== Setup ==============================
void SlotGame::SetupGame()
{
	baseGrid->SetLines(CustomLines, 5);
	//baseReelSet->ReplaceSymbolOnReel(1, MM, SE);
	//baseReelSet->ReplaceSymbolOnReel(2, MM, SE);
	//baseReelSet->ReplaceSymbolOnReel(3, MM, BE);
}
// ============================== Game Functions ==============================
double SlotGame::PlayGame()
{
	double score = 0;
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
	vector<int> stops = { 0,1,2,3,4 };
	vector<int> stops2 = stops;

	baseGrid->FillGrid(stops, baseReelSet);
	baseGrid->PrintGrid(symbolStrings);
	double score = baseGrid->EvaluateGrid(multiSymbolComboInfo);
	cout << "SCORE: " << score << endl;
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
		double score = baseGrid->EvaluateGrid(multiSymbolComboInfo);
		int combos = 1;
		for (int i=0; i<stops.size(); i++)
		{
			combos *= baseReelSet->m_reelWeights[i][stops[i]];
		}
		if (!hist.contains(score))
		{
			hist[score] = 0;
		}
		hist[score] += combos;
	}
}