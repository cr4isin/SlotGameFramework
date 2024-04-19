
#include "SlotGame.h"

int main(int argc, char* argv[])
{
    vector<string> args(argv, argv + argc);
    
    SlotGame game = SlotGame();
    game.SetBetScheme(10, 1);                // If the total bet is not baseBet*betMult it can be set with the third argument
    game.SetConfig("EXAMPLE");
    game.SetupGame();

    game.RunSims(100'000'000, args, true);   // Runs a sim; Files output to current directory
    //game.FreePlay();                         // Will allow you to play the game in the console
    //game.CyclePositions();                   // Will cycle through all positions of a reel set and create a histogram of possible scores
}
