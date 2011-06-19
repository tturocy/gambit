/*
 * prior_game_helper_tool.cc
 *
 *  Created on: May 28, 2011
 *      Author: jbecker
 */

#include "libgambit/libgambit.h"
#include "strategy_support_with_consec_nums.h"
#include "prior_game_helper_tool.h"

using std::tr1::shared_ptr;

static int calcNumOutcomes (const Gambit::Array<int> numStrategies) {
	int numOutcomes = 1;
	for (int i = numStrategies.First (); i <= numStrategies.Last (); i++) {
		numOutcomes *= numStrategies [i];
	}
	return numOutcomes;
}



PriorGameHelperTool::PriorGameHelperTool (
		shared_ptr<const StrategySupportWithConsecNums> theSupportWithConsecNums)
: supportWithConsecNums (theSupportWithConsecNums),
  numOutcomes (calcNumOutcomes (theSupportWithConsecNums->getSupport ()->NumStrategies ())),
  minUtility (theSupportWithConsecNums->getSupport ()->GetGame ()->GetMinPayoff ()),
  expectedUtilities (theSupportWithConsecNums->getTotalNumStrategies ()),
  bestResponse (theSupportWithConsecNums->getNumPlayers ()),
  bestResponseEU (theSupportWithConsecNums->getNumPlayers ())
{
}
