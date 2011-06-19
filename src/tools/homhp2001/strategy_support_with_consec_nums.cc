/*
 * strategy_support_with_consec_nums.cc
 *
 *  Created on: 27.05.2011
 *      Author: jbecker
 */

#include <tr1/memory>
#include <map>
#include <iostream>
#include "libgambit/libgambit.h"
#include "strategy_support_const_extension.h"
#include "strategy_support_with_consec_nums.h"

static int calcTotalNumStrategies (const Gambit::Array<int> numStrategies) {
	int totalNumStrategies = 0;
	for (int i = numStrategies.First (); i <= numStrategies.Last (); i++) {
		totalNumStrategies += numStrategies [i];
	}
	return totalNumStrategies;
}

StrategySupportWithConsecNums::StrategySupportWithConsecNums (
		std::tr1::shared_ptr<const Gambit::StrategySupport> theSupport
		)
: StrategySupportConstExtension (theSupport),
  numPlayers (theSupport->GetGame ()->NumPlayers ()),
  numStrategiesPerPlayer (theSupport->NumStrategies()),
  totalNumStrategies (calcTotalNumStrategies (numStrategiesPerPlayer)),
  globalToAssigned (),
  assignedToGlobal (totalNumStrategies),
  firstAssignedForPlayer (numPlayers + 1),
  playerForAssigned (totalNumStrategies)
{

    int runningIdx = 1;
    for (Gambit::GamePlayerIterator player = theSupport->Players ();
	 !player.AtEnd ();
	 player++) {
    	int nPlayer = player->GetNumber ();
    	firstAssignedForPlayer [nPlayer] = runningIdx;
    	for (Gambit::SupportStrategyIterator strategy = theSupport->Strategies (player);
    			!strategy.AtEnd ();
    			strategy++) {
    		int id = strategy->GetId ();
    		globalToAssigned [id] = runningIdx;
    		assignedToGlobal [runningIdx] = id;
    		playerForAssigned [runningIdx] = nPlayer;
    		runningIdx++;
    	}
    }
    firstAssignedForPlayer [numPlayers + 1] = runningIdx;
}
