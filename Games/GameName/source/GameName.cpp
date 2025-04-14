#include "GameName.h"

void GameName::SetupGame() {
    // Symbol Sets
    MainSymbols = mathXML.GetSymbolSet("MainSymbols", symbolSubstitutions, symbolMultipliers, symbolColors);

    // Combos
    MainPaylineCombos = mathXML.GetPaylineComboSet("MainPaylineCombos", MainSymbols, numReels, betMult);
    BonusScatters = mathXML.GetScatterComboSet("BonusScatters", MainSymbols, numReels, totalBet);

    // Reels
    Reels_Main = mathXML.GetReelStripSet("Reels_Main", MainSymbols);
    Reels_FG = mathXML.GetReelStripSet("Reels_FG", MainSymbols);

    // Grids
    grid = SlotGrid(numReels, numRows);
    grid.SetLines(lines, baseBet);

    // Weight and Value Tables
    mathXML.LoadAllWeightTables(weightTables);
    mathXML.LoadAllValueTables(valueTables);
}

double GameName::PlayGame() {
	if (inFreePlay) cout << "=== Base Game ===\n";
	double score = 0;
	vector<int> positions(numReels);

	// Generate positions and fill grid
	Reels_Main.GenerateRandomPositions(positions);
	grid.FillGrid(positions, Reels_Main);

	// Evaluate Lines
	score += grid.Evaluate(MainPaylineCombos).totalPay;

	// Evaluate Scatter pays
	ComboResults scatterPays = grid.Evaluate(BonusScatters);
	score += scatterPays.totalPay;

	AddToTracker("BaseGame", score);

	// Trigger free games
	if (scatterPays.bonusCodes.size() > 0)
	{
		score += PlayBonus(scatterPays.bonusCodes[0]);
	}

	AddToHistogram("score", score);
	return score;
}

double GameName::PlayBonus(int bonusCode) {
	double score = 0;
	vector<int> positions(numReels);
	int spinsRemaining = valueTables["FSNumSpins"][bonusCode - 1];
	int spinNumber = 0;

	while (spinsRemaining > 0)
	{
		spinNumber++;
		if (inFreePlay) cout << "=== Free Spin " << spinNumber << " ===\n";

		double spinScore = 0;

		// Generate positions and fill grid
		Reels_FG.GenerateRandomPositions(positions);
		grid.FillGrid(positions, Reels_FG);

		// Determine which 2 reels to fill with WILDs
		int wildPattern = weightTables["FSWildReels"].DrawValue();
		vector<int> wildReels = ChangeBase(wildPattern, 2, numReels);
		for (int iReel = 0; iReel < numReels; iReel++)
		{
			if (wildReels[iReel] == 1)
			{
				grid.FillReelWithSymbol(MainSymbols.GetSymbolIndex("WILD"), iReel);
			}
		}

		// Evaluate Lines
		spinScore += grid.Evaluate(MainPaylineCombos).totalPay;

		// Evaluate Scatter pays and retriggers
		ComboResults scatterPays = grid.Evaluate(BonusScatters);
		score += scatterPays.totalPay;
		if (scatterPays.bonusCodes.size() > 0)
		{
			score += spinsRemaining += valueTables["FSNumSpins"][scatterPays.bonusCodes[0] - 1];
		}

		AddToTracker("FreeGame", spinScore);

		score += spinScore;
		spinsRemaining--;
	}

	return score;
}
