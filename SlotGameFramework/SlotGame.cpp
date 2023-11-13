
#include "SlotGame.h"

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

void SlotGame::SetupGame()
{
	baseGrid->SetTwoWayLines(CustomLines, 5);
	//baseReelSet->ReplaceSymbolOnReel(1, MM, SE);
	//baseReelSet->ReplaceSymbolOnReel(2, MM, SE);
	//baseReelSet->ReplaceSymbolOnReel(3, MM, BE);
}

void SlotGame::DoSomething()
{
	vector<int> stops = { 6,7,6,6,9 };
	vector<int> stops2 = stops;

	baseGrid->FillGrid(stops, baseReelSet);
	baseGrid->PrintGrid();
	double score = baseGrid->EvaluateTwoWayGrid(multiSymbolComboInfo);
	cout << "SCORE: " << score << endl;
}

void SlotGame::CycleStops()
{
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
		double score = baseGrid->EvaluateTwoWayGrid(multiSymbolComboInfo);
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