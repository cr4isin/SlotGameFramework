#include "GameTemplate.h"

GameTemplate::GameTemplate(string mathXMLFileName, int baseBet, int betMult, int totalBet, bool useCurrentDirectory)
	: SlotGame("GameTemplate", mathXMLFileName, baseBet, betMult, totalBet, useCurrentDirectory)
{
	// Grids
	

	// Symbol Sets
	

	// Combos


	// Reels


	// Weight and Value Tables

}

double GameTemplate::PlayGame() 
{
	double score = 0;

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
