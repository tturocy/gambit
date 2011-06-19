/*
 * homhp2001_rho.cc
 *
 *  Created on: Jun 17, 2011
 *      Author: jbecker
 */

#include "homhp2001_rho.h"

#include <memory>
#include <tr1/memory>
#include <libgambit/libgambit.h>
#include "prior_game.h"
#include "strategy_support_with_consec_nums.h"


using std::auto_ptr;
using std::tr1::shared_ptr;




Homhp2001Rho :: Homhp2001Rho (
		shared_ptr<const StrategySupportWithConsecNums> theSupportWithConsecNums,
		shared_ptr<const PriorGame> priorGame
		)
: suppWcn (theSupportWithConsecNums),
  helper1 (PriorGame::createHelper (theSupportWithConsecNums)),
  mixed1 (new Gambit::MixedStrategyProfile<double> (* (theSupportWithConsecNums->getSupport ()))),
  pgp (priorGame) {
}




auto_ptr<Homhp2001Rho::SLAM>
Homhp2001Rho :: createSLAM () {

	auto_ptr<SLAM>   slam ( new SLAM (
			suppWcn->getTotalNumStrategies (), suppWcn->getNumPlayers ()
	));

	return slam;

}




void
Homhp2001Rho :: initializeSLAMWithStartpoint (SLAM & slam) {

	Gambit::Array<double> & sigma = slam.getSigmaForWriting ();
	Gambit::Array<double> & lambda = slam.getLambdaForWriting ();
	Gambit::Array<double> & mu = slam.getMuForWriting ();

	// initialize vectors sigma and lambda
	const int numPlayers = suppWcn->getNumPlayers ();

	for (int pl = 1; pl <= numPlayers; ++pl) {
		const int firststrat = suppWcn->getFirstConsecForPlayer (pl);
		const int laststrat = suppWcn->getLastConsecForPlayer (pl);
		const int bestResponse = pgp->getBestResponse (pl);
		for (int strat = firststrat; strat <= laststrat; ++strat) {
			sigma [strat] = 0.0;
			lambda [strat] = pgp->getPayoffFromStrategyConsec (bestResponse) -
					pgp->getPayoffFromStrategyConsec (strat);
		}
		sigma [bestResponse] = 1.0; lambda [bestResponse] = 0.0;
	}

	for (int pl = 1; pl <= numPlayers; ++pl) {
		mu [pl] = pgp->getPayoffFromStrategyConsec ( pgp->getBestResponse (pl) );
	}

	slam.finalizeMuWriting ();
	slam.finalizeLambdaWriting ();
	slam.finalizeSigmaWriting ();

}




void
Homhp2001Rho :: calcRho (
		double * v,
		double t,
		const SLAM & slam)
{

	const Gambit::Array<double> & lambda = slam.getLambda ();
	const Gambit::Array<double> & sigma = slam.getSigma ();
	const Gambit::Array<double> & mu = slam.getMu ();

	const int totalNumStrategies = suppWcn->getTotalNumStrategies ();
	const int numPlayers = suppWcn->getNumPlayers ();

	for (int strat = 1; strat <= totalNumStrategies; ++strat) {
		(*mixed1) [ suppWcn->getIdFromConsec (strat) ] = sigma [strat];
	}

	PriorGame pgs (helper1, mixed1);

	for (int pl = 1; pl <= numPlayers; ++pl) {
		const int firststrat = suppWcn->getFirstConsecForPlayer (pl);
		const int laststrat = suppWcn->getLastConsecForPlayer (pl);

		v [totalNumStrategies + pl] = - 1.0;
		for (int strat = firststrat; strat <= laststrat; ++strat) {
			v [strat] = t * pgs.getPayoffFromStrategyConsec (strat) +
					(1.0 - t) * pgp->getPayoffFromStrategyConsec (strat) +
					lambda [strat] - mu [ pl ];

			v [totalNumStrategies + pl] += sigma [strat];
		}
	}

}




void
Homhp2001Rho :: calcRhoJacobian (
		double * vmatrix,
		double t,
		const SLAM & slam)
{

	/* This subroutine  returns in the vector V the */
	/* Jacobian matrix [D RHO/D LAMBDA, D RHO/ D X] evaluated at the */
	/* point (LAMBDA, X) */

	const Gambit::Array<double> & alpha = slam.getAlpha ();
	const Gambit::Array<double> & sigma = slam.getSigma ();

	const int totalNumStrategies = suppWcn->getTotalNumStrategies ();
	const int numPlayers = suppWcn->getNumPlayers ();

	for (int strat = 1; strat <= totalNumStrategies; ++strat) {
		(*mixed1) [ suppWcn->getIdFromConsec (strat) ] = sigma [strat];
	}

	PriorGame pgs (helper1, mixed1);

	double * v = vmatrix;

	// k = 1
	for (int strat = 1; strat <= totalNumStrategies; ++strat) {
		v [strat - 1] = pgs.getPayoffFromStrategyConsec (strat) - pgp->getPayoffFromStrategyConsec (strat);
	}
	for (int pl = 1; pl <= numPlayers; ++pl) {
		v [totalNumStrategies + pl - 1] = 0.0;
	}

	for (int k = 2; k <= totalNumStrategies + 1; ++k) {
		v += totalNumStrategies + numPlayers;

		const int playerForK = suppWcn->getPlayerForConsec (k-1);

		for (int pl = 1; pl <= numPlayers; ++pl) {
			const int firststrat = suppWcn->getFirstConsecForPlayer (pl);
			const int laststrat = suppWcn->getLastConsecForPlayer (pl);

			v [totalNumStrategies + pl - 1] = 0.0;

			if (pl == playerForK) {
				for (int strat = firststrat; strat <= laststrat; ++strat) {
					v [strat - 1] = 0.0;
				}
				v [k - 2] = (alpha [k - 1] <= 0.0) ? 2.0 * alpha [k - 1] : 0.0;
				v [totalNumStrategies + playerForK - 1] = (alpha [k-1] >= 0) ? 2.0 * alpha [k-1] : 0.0;
			} else {
				for (int strat = firststrat; strat <= laststrat; ++strat) {
					v [strat - 1] = (alpha [k-1] >= 0.0) ?
							t * 2.0 * alpha [k-1] * pgs.getDerivFromStrategiesConsec (strat, k - 1)
							: 0.0;
				}
			}
		}
	}

	for (int k = totalNumStrategies + 2; k <= totalNumStrategies + numPlayers + 1; ++k) {
		v += totalNumStrategies + numPlayers;

		for (int i = 0; i < totalNumStrategies + numPlayers; ++i) {
			v [i] = 0.0;
		}
		const int playerK = k - 1 - totalNumStrategies;
		const int firststrat = suppWcn->getFirstConsecForPlayer (playerK);
		const int laststrat = suppWcn->getLastConsecForPlayer (playerK);
		for (int strat = firststrat; strat <= laststrat; ++strat) {
			v [strat - 1] = - 1.0;
		}
	}
/*
	std::cout << "k = " << k << ": ";
    for (int i = 0; i < totalNumStrategies + numPlayers; ++i) {
    	std::cout << " " << v [i] << " ";
    }
    std::cout << std::endl;
    */
}

/*
void Homhp2001Rho::doCalcRhoJacobianAlt (
		double * a, double t, double * x, double * v, int k
		)
{

/* This subroutine  returns in the vector V the Kth column of the */
/* Jacobian matrix [D RHO/D LAMBDA, D RHO/ D X] evaluated at the */
/* point (LAMBDA, X) */

/* Here the Jacobian is computed using a two-sided difference */
/* approximation */
/*
	const int totalNumStrategies = suppWcn->getTotalNumStrategies ();
	const int numPlayers = suppWcn->getNumPlayers ();

    /* Function Body */
/*
    const double h = 1e-7;

    if (k == 1) {
    	t -= 0.5 * h;
    	doCalcRho (t, x, v1);
    	t += h;
    	doCalcRho (t, x, v2);
    } else {
    	x [k - 2] -= 0.5 * h;
    	doCalcRho (t, x, v1);
    	x [k - 2] += h;
    	doCalcRho (t, x, v2);
    	x [k - 2] -= 0.5 * h;
    }
    for (int i = 0; i < totalNumStrategies + numPlayers; ++i) {
    	v [i] = (v2 [i + 1] - v1 [i + 1]) / h;
    }

	std::cout << "k = " << k << ": ";
    for (int i = 0; i < totalNumStrategies + numPlayers; ++i) {
    	std::cout << " " << v [i] << " ";
    }
    std::cout << std::endl;
}

*/
