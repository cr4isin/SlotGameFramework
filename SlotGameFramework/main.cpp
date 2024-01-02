
#include "SlotGame.h"

int main()
{
    SlotGame game = SlotGame();
    game.SetBetScheme(1, 1);
    //game.DoSomething();
    game.RunSims(1, 100'000'000);
    //game.FreePlay();
}

// to do:
// clean up custom classes and add getters so variables can be private
// add weight tables/manager
// add game pays
// add median spins calculator
