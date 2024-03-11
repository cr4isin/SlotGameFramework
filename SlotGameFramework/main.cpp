
#include "SlotGame.h"

int main()
{
    SlotGame game = SlotGame();
    game.SetBetScheme(10, 1);       // Set the bet here, arguments are lines bet and bet mult. If total bet is not baseBet*betMult it can be set with the 3rd argument.
    game.SetupGame();

    game.RunSims(1, 100'000'000);   // Runs a sim. First argument is number of trials, second is number of games per trial. Files output to current directory
    //game.FreePlay();              // Will allow you to play the game in the console
    //game.CyclePositions();        // Will cycle through all positions of a reel set and create a histogram of every possible score
}

// to do:
// add game pays
// add median spins calculator
