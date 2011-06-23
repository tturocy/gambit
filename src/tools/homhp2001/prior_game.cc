/*
 * prior_game.cc
 *
 *  Created on: May 28, 2011
 *      Author: jbecker
 */

#include <tr1/memory>
#include <iostream>
#include "libgambit/libgambit.h"
#include "prior_game.h"
#include "prior_game_helper_tool.h"

using std::tr1::shared_ptr;

PriorGame::~PriorGame () {
}

shared_ptr<PriorGame::Helper>
PriorGame::createHelper (shared_ptr<const StrategySupportWithConsecNums> suppWcn) {
	shared_ptr<PriorGameHelperTool> t (new PriorGameHelperTool (suppWcn));
	shared_ptr<PriorGame::Helper> p =
			ObjNeedingHelper<PriorGameHelperTool>::createHelper (t);
	return p;
}

shared_ptr<PriorGame::Helper>
PriorGame::createHelper (shared_ptr<const Gambit::StrategySupport> theSupport) {
	shared_ptr<const StrategySupportWithConsecNums> suppWcn (new StrategySupportWithConsecNums (theSupport));
	return PriorGame::createHelper (suppWcn);
}

static int
determineBest (int firstsn, int lastsn, const Gambit::Array<double> & eu) {
	int best = firstsn;
	double bestu = eu [firstsn];
	for (int sn = firstsn + 1; sn <= lastsn; ++sn) {
		if (eu [sn] > bestu) {
			bestu = eu [sn];
			best = sn;
		}
	}
	return best;
}

PriorGame::PriorGame (shared_ptr<PriorGame::Helper> theHelper,
		shared_ptr<const Gambit::MixedStrategyProfile<double> > thePrior)
: ObjNeedingHelper<PriorGameHelperTool> (theHelper), prior (thePrior)
{
	shared_ptr<const StrategySupportWithConsecNums> supportWithConsecNums =
			getHelperTool ()->getSupportWithConsecNums ();
	const int totalNumStrategies = supportWithConsecNums->getTotalNumStrategies ();

	for (int s = 1; s <= totalNumStrategies; s++) {
		Gambit::GameStrategy strategy = supportWithConsecNums->getStrategyFromConsec (s);
		const int nPlayer = strategy->GetPlayer ()->GetNumber ();
		getHelperTool ()->setExpectedUtility (s, prior->GetPayoffDeriv (nPlayer, strategy));
	}

    // const Gambit::Array<int> numStrategiesArray = helper.getSuppStratNumberer.getNumStrategies ();
    const int totalNumOutcomes = getHelperTool ()->getNumOutcomes ();

    // std::cout << "eup: "; print_array<double> (getHelperTool ()->getExpectedUtilities ()); std::cout << std::endl;

    /* determine best responses */
    const int numPlayers = supportWithConsecNums->getNumPlayers ();


    for (int pln = 1; pln <= numPlayers; ++pln) {
    	int best = determineBest (
    			supportWithConsecNums->getFirstConsecForPlayer (pln),
    			supportWithConsecNums->getLastConsecForPlayer (pln),
    			getHelperTool ()->getExpectedUtilities ()
    	);
    	getHelperTool ()->setBestResponse (pln, best);
    	getHelperTool ()->setBestResponseEU (pln, getHelperTool ()->getExpectedUtility (best));
    }

    // std::cout << "br: "; print_array<int> (getHelperTool()->getBestResponses ()); std::cout << std::endl;
    // std::cout << "mu: "; print_array<double> (getHelperTool ()->getBestResponsesEU ()); std::cout << std::endl;

}

double PriorGame::getPayoffFromStrategyConsec (int consec) const {
	return getHelperTool ()->getExpectedUtility ( consec );
}

double PriorGame::getPayoffFromStrategy (const Gambit::GameStrategy & strategy) const {
	return getPayoffFromStrategyConsec ( getHelperTool ()->getSupportWithConsecNums ()->getConsec (strategy) );
}

double PriorGame :: getDerivFromStrategiesConsec (int consec, int derivConsec) const {
	shared_ptr<const StrategySupportWithConsecNums> supportWithConsecNums =
				getHelperTool ()->getSupportWithConsecNums ();
	return getDerivFromStrategies ( supportWithConsecNums->getStrategyFromConsec (consec),
			supportWithConsecNums->getStrategyFromConsec (derivConsec) );
}

double PriorGame :: getDerivFromStrategies (
		const Gambit::GameStrategy & strategy, const Gambit::GameStrategy & derivStrategy
		) const
{
	return prior->GetPayoffDeriv (strategy->GetPlayer ()->GetNumber (), strategy, derivStrategy);
}


int PriorGame::getNumPlayers () const {
	return getHelperTool ()->getNumPlayers ();
}

int PriorGame::getBestResponse (int pl) const {
	return getHelperTool ()->getBestResponse (pl);
}

