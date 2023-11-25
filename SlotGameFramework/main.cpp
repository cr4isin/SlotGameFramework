// SlotGameFramework.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "SlotGame.h"

int main()
{
    SlotGame game = SlotGame();
    game.SetupGame();
    game.DoSomething();
}

// to do:
// have SetLines copy the lines array to a grid variable, and create the line elements
// this way users can use multi symbol combos or normal symbol combos
// add a function for set lines all ways which will also change the line counts to 1
// add an optional variable BothWays = false to creating symbol combos
// two evaluate grid functions, one that accepts symbol combos and one that accepts multi symbol combos
