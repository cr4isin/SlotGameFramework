#include "GameTemplate.h"

GameTemplate::GameTemplate(string mathXMLFileName, int baseBet, int betMult, int totalBet, bool useCurrentDirectory)
	: SlotGame("GameTemplate", mathXMLFileName, baseBet, betMult, totalBet, useCurrentDirectory)
{
	// Grids
	grid = SlotGrid(numReels, numRows);
	//grid.SetLines(lines);

	// Symbol Sets
	MainSymbols = mathXML.GetSymbolSet("MainSymbols", symbolSubstitutions, symbolMultipliers, symbolColors);

	// Combos
	MainPaylineCombos = mathXML.GetPaylineComboSet("MainPaylineCombos", MainSymbols, numReels, baseBet, betMult);

	// Reels
	Reels_Main = mathXML.GetReelStripSet("Reels_Main", MainSymbols);

	// Weight and Value Tables
	mathXML.GetAllWeightTables(weightTables);
	mathXML.GetAllValueTables(valueTables);

	// Mystery Replacement
	MysterySequence = mathXML.GetMysteryReplacement("MysterySequence", MainSymbols);

}

double GameTemplate::PlayGame() 
{
	double score = 0;
	vector<int> positions(numReels);

	// Generate positions, fill grid, do mystery replacement
	Reels_Main.GenerateRandomPositions(positions);
	grid.FillGrid(positions, Reels_Main);
	grid.Replace(MysterySequence);

	// Evaluate Line pays
	grid.Evaluate(MainPaylineCombos);
	score += MainPaylineCombos.pay;

	// Print results if in Free Play
	if (inFreePlay)
	{
		cout << "=== Base Game ===\n";
		grid.SetHighlights(MainPaylineCombos);
		grid.PrintGridTimed(MainSymbols);
		MainPaylineCombos.PrintResults();
	}

	return score;
}

double GameTemplate::PlayBonus(int bonusCode)
{
	double score = 0;

	switch (bonusCode)
	{
		case 1:
		case 2:
		case 3: score += FreeSpins(bonusCode); break;
		default: 
		{
			cout << "No such bonus code: " << bonusCode << endl;
			throw runtime_error("Unknown Bonus Code used");
		}
	}

	return score;
}
