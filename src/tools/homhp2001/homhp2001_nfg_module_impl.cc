/*
 * homhp2001_nfg_module_impl.cc
 *
 *  Created on: May 19, 2011
 *      Author: jbecker
 */

#include <memory>
#include <tr1/memory>

#include <iostream>
#include <map>
#include "libgambit/libgambit.h"
#include "strategy_support_with_consec_nums.h"
#include "prior_game.h"
#include "homhp2001_nfg_module.h"
#include "homhp2001_nfg_module_impl.h"
#include "homotopy_solver.h"
#include "homhp2001_rho.h"


using std::tr1::shared_ptr;





Homhp2001NFGModuleImpl::Homhp2001NFGModuleImpl (shared_ptr<const Gambit::StrategySupport> theSupport)
: support (theSupport) {
}



namespace {

class CallbackObj : public HomotopySolver::CallbackObj {
private:
	Homhp2001Rho * rhoObj;
	std::auto_ptr<Homhp2001Rho::SLAM> rhoData;
	int lengthAlpha;

public:
	CallbackObj (Homhp2001Rho * theRhoObj)
	: rhoObj (theRhoObj), rhoData (NULL), lengthAlpha (-1) {
	}

	virtual ~CallbackObj () { }

	// const Homhp2001Rho::Homhp2001SLAM & getData () { return *rhoData; }
	std::auto_ptr<Homhp2001Rho::SLAM> giveMeData () { return rhoData; }

private:
	virtual void doInitializeCalculation (double & t, double * v) {
		if (rhoData.get () == NULL) { rhoData = rhoObj->createSLAM (); }

		lengthAlpha = rhoData->getTotalNumStrategies ();

		t = 0.0;
		rhoObj->initializeSLAMWithStartpoint (*rhoData);
		rhoData->copyAlphaToVec(& (v [0]));
		rhoData->copyMuToVec (& (v [lengthAlpha]));

/*
		std::cout << "Sigma:" << std::endl; print_array<double> (rhoData->getSigma ()); std::cout << std::endl;
		std::cout << "Lambda:" << std::endl; print_array<double> (rhoData->getLambda ()); std::cout << std::endl;
		std::cout << "Mu:" << std::endl; print_array<double> (rhoData->getMu ()); std::cout << std::endl;
		std::cout << "Alpha:" << std::endl; print_array<double> (rhoData->getAlpha ()); std::cout << std::endl;
*/

	}

	virtual void doCalcRhoJacobian (
			double * a, double t, double * x, double * v
			)
	{
		rhoData->setFromAlphaMuVec (& (x [0]), & (x [lengthAlpha]));

		/*
		std::cout << "Sigma:" << std::endl; print_array<double> (rhoData->getSigma ()); std::cout << std::endl;
		std::cout << "Lambda:" << std::endl; print_array<double> (rhoData->getLambda ()); std::cout << std::endl;
		std::cout << "Mu:" << std::endl; print_array<double> (rhoData->getMu ()); std::cout << std::endl;
		std::cout << "Alpha:" << std::endl; print_array<double> (rhoData->getAlpha ()); std::cout << std::endl;
*/

		rhoObj->calcRhoJacobian (v, t, *rhoData);
	}

	virtual void doFinalizeCalculation (double t, double * v) {
		rhoData->setFromAlphaMuVec (& (v [0]), & (v [lengthAlpha]));
	}

};

} // end anonymous namespace


void Homhp2001NFGModuleImpl::doCalculation () {

	shared_ptr<const StrategySupportWithConsecNums> suppWcn (new StrategySupportWithConsecNums (support));
	shared_ptr<PriorGame::Helper> helper = PriorGame::createHelper (suppWcn);

	shared_ptr<const Gambit::MixedStrategyProfile<double> >
		prior (new Gambit::MixedStrategyProfile<double> (*support));

	shared_ptr<const PriorGame> priorGame (new PriorGame (helper, prior));


	const int numPlayers = suppWcn->getNumPlayers();
	const int numstrats = suppWcn->getTotalNumStrategies ();



	shared_ptr<Homhp2001Rho> rhoObj (new Homhp2001Rho (suppWcn, priorGame));
	CallbackObj * callbackObj = new CallbackObj (rhoObj.get ());


	HomotopySolver::solve (numstrats + numPlayers, callbackObj );

	std::auto_ptr<Homhp2001Rho::SLAM> rhoData = callbackObj->giveMeData ();

	delete callbackObj; callbackObj = NULL;


/*
	for (int i = 1; i <= numstrats; ++i) {
		alpha [i] = Y [i];
	}

	Gambit::Array<double> mu (numPlayers);

	for (int pl = 1; pl <= numPlayers; ++pl) {
		mu [pl] = Y [numstrats + pl - 1];
	}

	//transform alpha into sigma and lambda
	for (int i = 1; i <= numstrats; ++i) {
		if (alpha [i] >= 0) {
			sigma [i] = alpha [i] * alpha [i];
			lambda [i] = 0.0;
		} else {
			sigma [i] = 0.0;
			lambda [i] = alpha [i] * alpha [i];
		}
	}
*/

	// print
	for (int pl = 1; pl <= numPlayers; ++pl) {
		std::cout << "Strategy Player " << pl << std::endl;
		const int firststrat = suppWcn->getFirstConsecForPlayer (pl);
		const int laststrat = suppWcn->getLastConsecForPlayer (pl);
		for (int strat = firststrat; strat <= laststrat; ++strat) {
			std::cout << " " << (rhoData->getSigma ()) [strat] << " ";
		}
		std::cout << std::endl;
	}
	for (int pl = 1; pl <= numPlayers; ++pl) {
		std::cout << "Payoff Player " << pl << ": "<< (rhoData->getMu ()) [ pl ] << std::endl;
	}


}

