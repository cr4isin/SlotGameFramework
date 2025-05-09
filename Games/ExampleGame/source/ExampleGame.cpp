#include "ExampleGame.h"

ExampleGame::ExampleGame(string mathXMLFileName, int baseBet, int betMult, int totalBet, bool useCurrentDirectory)
	: SlotGame("ExampleGame", mathXMLFileName, baseBet, betMult, totalBet, useCurrentDirectory)
{
	// Grids
	grid = SlotGrid(numReels, numRows);
	grid.SetLines(lines, numLines);

	// Symbol Sets
	MainSymbols = mathXML.GetSymbolSet("MainSymbols", symbolSubstitutions, symbolMultipliers, symbolColors);

	// Combos
	MainPaylineCombos = mathXML.GetPaylineComboSet("MainPaylineCombos", MainSymbols, numReels, numLines, betMult);
	BonusScatters = mathXML.GetScatterComboSet("BonusScatters", MainSymbols, numReels, totalBet);

	// Reels
	Reels_Main = mathXML.GetReelStripSet("Reels_Main", MainSymbols);
	Reels_FG = mathXML.GetReelStripSet("Reels_FG", MainSymbols);

	// Weight and Value Tables
	mathXML.GetAllWeightTables(weightTables);
	mathXML.GetAllValueTables(valueTables);
}

double ExampleGame::PlayGame() 
{
	double score = 0;
	vector<int> positions(numReels);

	// Generate positions and fill grid
	Reels_Main.GenerateRandomPositions(positions);
	grid.FillGrid(positions, Reels_Main);

	// Evaluate Line pays
	grid.Evaluate(MainPaylineCombos);
	score += MainPaylineCombos.pay;

	// Evaluate Scatter pays
	grid.Evaluate(BonusScatters);
	score += BonusScatters.pay;

	AddToTracker("BaseGame", score);

	// Print results if in Free Play
	if (inFreePlay)
	{
		cout << "=== Base Game ===\n";
		grid.SetHighlights(MainPaylineCombos);
		grid.PrintGridTimed(MainSymbols);
		MainPaylineCombos.PrintResults();
		if (BonusScatters.bonusHit) PrintPauseMessage("Free Games Triggered! Press Enter to play...");
	}

	// Trigger free games
	if (BonusScatters.bonusHit)
	{
		score += PlayBonus(BonusScatters.bonusCode);
	}

	return score;
}

double ExampleGame::PlayBonus(int bonusCode)
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

double ExampleGame::FreeSpins(int bonusCode)
{
	double score = 0;
	vector<int> positions(numReels);
	int spinsRemaining = valueTables["FSNumSpins"][bonusCode - 1];
	int spinNumber = 0;

	while (spinsRemaining > 0)
	{
		spinNumber++;

		double spinScore = 0;

		// Generate positions and fill grid
		Reels_FG.GenerateRandomPositions(positions);
		grid.FillGrid(positions, Reels_FG);

		// Determine which 2 reels to fill with WILDs
		int wildPattern = weightTables.at("FSWildReels").DrawValue();
		vector<int> wildReels = ChangeBase(wildPattern, 2, numReels);
		for (int iReel = 0; iReel < numReels; iReel++)
		{
			if (wildReels[iReel] == 1)
			{
				grid.FillReelWithSymbol(MainSymbols["WILD"], iReel);
			}
		}

		// Evaluate Lines
		grid.Evaluate(MainPaylineCombos);
		spinScore += MainPaylineCombos.pay;

		// Evaluate Scatter pays and retriggers
		grid.Evaluate(BonusScatters);
		score += BonusScatters.pay;
		if (BonusScatters.bonusHit)
		{
			score += spinsRemaining += valueTables["FSNumSpins"][BonusScatters.bonusCode - 1];
		}

		AddToTracker("FreeSpin", spinScore);

		score += spinScore;
		spinsRemaining--;

		if (inFreePlay)
		{
			cout << "=== Free Spin " << spinNumber << " ===\n";
			grid.SetHighlights(MainPaylineCombos);
			grid.PrintGridTimed(MainSymbols);
			MainPaylineCombos.PrintResults();
			if (BonusScatters.bonusHit) cout << BonusScatters.combo.length << " BONUS retriggered Free Games! " << valueTables["FSNumSpins"][BonusScatters.bonusCode - 1] << " Free Games added\n\n";
			if (spinsRemaining > 0) PrintPauseMessage(to_string(spinsRemaining) + " Spins Remaining...", true);
		}
	}

	AddToHistogram("FreeSpinBonus", score);
	return score;
}
