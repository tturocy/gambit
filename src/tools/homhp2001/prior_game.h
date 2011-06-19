/*
 * prior_game.h
 *
 *  Created on: May 28, 2011
 *      Author: jbecker
 */

#ifndef PRIOR_GAME_H_
#define PRIOR_GAME_H_

#include <tr1/memory>
#include "libgambit/libgambit.h"
#include "strategy_support_with_consec_nums.h"
#include "obj_needing_helper.h"


#include <iostream>
template<typename T>
static void print_array (const Gambit::Array<T> & array) {
	for (int i = array.First (); i <= array.Last (); i++) {
		std::cout << " " << array [i] << " ";
	}
}

class PriorGameHelperTool;

class PriorGame : public ObjNeedingHelper<PriorGameHelperTool> {
private:
	std::tr1::shared_ptr<const Gambit::MixedStrategyProfile<double> > prior;


public:
	PriorGame (
			std::tr1::shared_ptr<class Helper> theHelper,
			std::tr1::shared_ptr<const Gambit::MixedStrategyProfile<double> > thePrior
			);
	virtual ~PriorGame ();

	double getPayoffFromStrategyConsec (int consec) const;
	double getPayoffFromStrategy (const Gambit::GameStrategy &) const;

	double getDerivFromStrategiesConsec (int consec, int derivConsec) const;
	double getDerivFromStrategies (const Gambit::GameStrategy &, const Gambit::GameStrategy &) const;

	static std::tr1::shared_ptr<class Helper> createHelper (
			std::tr1::shared_ptr<const Gambit::StrategySupport> support
			);

	static std::tr1::shared_ptr<class Helper> createHelper (
			std::tr1::shared_ptr<const StrategySupportWithConsecNums> suppWcn
			);


	int getNumPlayers () const;

	int getBestResponse (int pl) const;

};

#endif /* PRIOR_GAME_H_ */
