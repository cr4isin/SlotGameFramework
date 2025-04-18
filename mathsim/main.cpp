
#include "ExampleGame/source/ExampleGame.h"

int main(int argc, char* argv[])
{
    vector<string> args(argv, argv + argc);
    
    ExampleGame* game = new ExampleGame("math.40.937.xml", 40, 1);  // If the total bet is not baseBet*betMult it can be set with the fourth argument

    game->RunSims(100'000'000, args);                               // Runs a sim, use the 3rd argument to only sim a specific bonus
    //game->FreePlay();                                               // Will allow you to play the game in the console
}
