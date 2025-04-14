
#include "GameName/source/GameName.h"

int main(int argc, char* argv[])
{
    vector<string> args(argv, argv + argc);
    
    GameName* game = new GameName("math.40.937.xml", 40, 1);  // If the total bet is not baseBet*betMult it can be set with the fourth argument
    game->SetupGame();

    game->RunSims(100'000'000, args);            // Runs a sim; Files output to current directory
    //game->FreePlay();                            // Will allow you to play the game in the console
    //game->CyclePositions();                      // Will cycle through all positions of a reel set and create a histogram of possible scores
}
