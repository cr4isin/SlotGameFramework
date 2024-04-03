
#include "SlotGame.h"

int main(int argc, char* argv[])
{
    vector<string> args(argv, argv + argc);

    SlotGame game = SlotGame();
    game.SetBetScheme(10, 1);                // Set the bet here, baseBet is used for the number of lines bet by default. If total bet is not baseBet*betMult it can be set with the third argument.
    game.SetupGame();

    game.RunSims(100'000'000, args, true);   // Runs a sim. First argument is number of trials, second is number of games per trial. Files output to current directory
    //game.FreePlay();                         // Will allow you to play the game in the console
    //game.CyclePositions();                   // Will cycle through all positions of a reel set and create a histogram of every possible score
}
