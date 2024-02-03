
#include "SlotGame.h"

int main()
{
    SlotGame game = SlotGame();
    game.SetBetScheme(5, 1);
    game.SetupGame();

    game.CyclePositions();
    //game.DoSomething();
    //game.RunSims(1, 100'000'000);
    //game.FreePlay();
}

// to do:
// add game pays
// add median spins calculator
