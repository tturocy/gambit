/*
 * prior_game_helper.h
 *
 *  Created on: May 28, 2011
 *      Author: jbecker
 */

#ifndef PRIOR_GAME_HELPER_TOOL_H_
#define PRIOR_GAME_HELPER_TOOL_H_

#include "libgambit/libgambit.h"
#include "strategy_support_with_consec_nums.h"

class PriorGameHelperTool {
private:
	std::tr1::shared_ptr<const StrategySupportWithConsecNums> supportWithConsecNums;
	const int numOutcomes;
	const double minUtility;
	mutable Gambit::Array<double> expectedUtilities;
	mutable Gambit::Array<int> bestResponse;
    mutable Gambit::Array<double> bestResponseEU;

public:
	PriorGameHelperTool (std::tr1::shared_ptr<const StrategySupportWithConsecNums> theSupportWithConsecNums);

	// Getters
	std::tr1::shared_ptr<const Gambit::StrategySupport>
	getSupport () const {
		return supportWithConsecNums->getSupport ();
	}

	std::tr1::shared_ptr<const StrategySupportWithConsecNums>
	getSupportWithConsecNums () const {
		return supportWithConsecNums;
	}

	int getNumPlayers () const { return supportWithConsecNums->getNumPlayers (); }
	// const Gambit::Array<int> getNumStrategies () const { return numStrategies; }
	// const int getTotalNumStrategies () const { return totalNumStrategies; }
	const int getNumOutcomes () const { return numOutcomes; }
	const double getMinUtility () const { return minUtility; }

	const Gambit::Array<double> & getExpectedUtilities () const { return expectedUtilities; }
	double getExpectedUtility (int consec) const { return expectedUtilities [consec]; }
	void setExpectedUtility (int consec, double value) { expectedUtilities [consec] = value; }
	void setBestResponse (int pl, int consec) { bestResponse [pl] = consec; }
	int getBestResponse (int pl) const { return bestResponse [pl]; }
	const Gambit::Array<int> & getBestResponses () const { return bestResponse; }
	const Gambit::Array<double> & getBestResponsesEU () const { return bestResponseEU; }
	void setBestResponseEU (int pl, int value) { bestResponseEU [pl] = value; }
};

#endif /* PRIOR_GAME_HELPER_TOOL_H_ */
