
#include "SlotGame.h"

int main()
{
    SlotGame game = SlotGame();
    game.SetBetScheme(40, 1);
    game.SetupGame();

    game.RunSims(1, 100'000'000);
    //game.CyclePositions();
    //game.DoSomething();
    //game.FreePlay();
}

// to do:
// add game pays
// add median spins calculator
