/*
 * strategy_support_with_consec_nums.h
 *
 *  Created on: 27.05.2011
 *      Author: jbecker
 */

#ifndef STRATEGY_SUPPORT_WITH_CONSEC_NUMS_H_
#define STRATEGY_SUPPORT_WITH_CONSEC_NUMS_H_

#include <tr1/memory>
#include <map>
#include "libgambit/libgambit.h"
#include "strategy_support_const_extension.h"


class StrategySupportWithConsecNums
: public StrategySupportConstExtension {
private:
	const int numPlayers;
	const Gambit::Array<int> numStrategiesPerPlayer;

	// must be declared after numStrategiesPerPlayer, otherwise constructor will not properly initialize it
	const int totalNumStrategies;

	mutable std::map<int,int> globalToAssigned;

	// must be declared after totalNumStrategies, otherwise constructor will not properly initialize it
	mutable Gambit::Array<int> assignedToGlobal;

	// must be declared after numPlayers, otherwise constructor will not properly initialize it
	mutable Gambit::Array<int> firstAssignedForPlayer;

	// must be declared after totalNumStrategies, otherwise constructor will not properly initialize it
	mutable Gambit::Array<int> playerForAssigned;

public:
	StrategySupportWithConsecNums (std::tr1::shared_ptr<const Gambit::StrategySupport> theSupport);
	virtual ~StrategySupportWithConsecNums () {  }

	int getIdFromConsec (int number) const { return assignedToGlobal [number]; }
	Gambit::GameStrategy getStrategyFromConsec (int number) const {
		return getSupport ()->GetGame ()->GetStrategy (getIdFromConsec (number));
	}
	int getConsec (int id) const { return globalToAssigned [id]; }
	int getConsec (const Gambit::GameStrategy & strategy) const {
		return getConsec (strategy->GetId ());
	}
	int getTotalNumStrategies () const { return totalNumStrategies; }
	int getNumStrategiesForPlayer (int pl) const { return numStrategiesPerPlayer [pl]; }
	int getFirstConsecForPlayer (int pl) const { return firstAssignedForPlayer [pl]; }
	int getLastConsecForPlayer (int pl) const { return firstAssignedForPlayer [pl + 1] - 1; }
	int getPlayerForConsec (int strat) const { return playerForAssigned [strat]; }
	int getNumPlayers () const { return numPlayers; }
};

#endif /* STRATEGY_SUPPORT_WITH_CONSEC_NUMS_H_ */
