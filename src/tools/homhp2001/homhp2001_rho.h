/*
 * homhp2001_rho.h
 *
 *  Created on: Jun 17, 2011
 *      Author: jbecker
 */

#ifndef HOMHP2001_RHO_H_
#define HOMHP2001_RHO_H_

#include <exception>
#include <memory>
#include <tr1/memory>
#include "libgambit/libgambit.h"
#include "strategy_support_with_consec_nums.h"
#include "prior_game.h"



class Homhp2001Rho {
public:
	class SLAM;

	Homhp2001Rho (
			std::tr1::shared_ptr<const StrategySupportWithConsecNums> theSupportWithConsecNums,
			std::tr1::shared_ptr<const PriorGame> priorGame
			);
	virtual ~Homhp2001Rho () { };

	std::auto_ptr<SLAM> createSLAM ();
	void initializeSLAMWithStartpoint (SLAM & slam);

	void calcRho (double * v, double t, const SLAM & slam);
	void calcRhoJacobian (double * v, double t, const SLAM & slam);

private:
	class SLAMBase;

private:
	std::tr1::shared_ptr<const StrategySupportWithConsecNums> suppWcn;

	std::tr1::shared_ptr<PriorGame::Helper> helper1;
//	std::tr1::shared_ptr<PriorGame::Helper> helper2;

	std::tr1::shared_ptr<Gambit::MixedStrategyProfile<double> > mixed1;
//	std::tr1::shared_ptr<Gambit::MixedStrategyProfile<double> > mixed2;

	std::tr1::shared_ptr<const PriorGame> pgp;
};


class Homhp2001Rho::SLAMBase {
public:
	class MissingInitializationException : public std::exception {
	};

	virtual ~SLAMBase () { }

	const Gambit::Array<double> & getSigma () const {
		if (!sigmaValid)   initializeSigmaLambdaFromAlpha ();
		return sigma;
	}
	const Gambit::Array<double> & getLambda () const {
		if (!lambdaValid)   initializeSigmaLambdaFromAlpha ();
		return lambda;
	}
	const Gambit::Array<double> & getMu () const {
		if (!muValid)   throw MissingInitializationException ();
		return mu;
	}
	const Gambit::Array<double> & getAlpha () const {
		if (!alphaValid)   initializeAlphaFromSigmaLambda ();
		return alpha;
	}

	const int getTotalNumStrategies () const { return totalNumStrategies; }
	const int getNumPlayers () const { return numPlayers; }

	void setFromAlphaMuVec (double * vecAlpha, double * vecMu) {
		for (int i = 1; i <= totalNumStrategies; ++i) {
			alpha [i] = vecAlpha [i-1];
		}
		alphaValid = true;

		for (int i = 1; i <= numPlayers; ++i) {
			mu [i] = vecMu [i-1];
		}
		muValid = true,

		sigmaValid = false;
		lambdaValid = false;
	}

	void copyAlphaToVec (double * vec) const {
		const Gambit::Array<double> & _alpha = getAlpha ();

		for (int i = 1; i <= totalNumStrategies; ++i) {
			vec [i - 1] = _alpha [i];
		}
	}

	void copyMuToVec (double * vec)  const {
		const Gambit::Array<double> & _mu = getMu ();

		for (int pl = 1; pl <= numPlayers; ++pl) {
			vec [pl - 1] = _mu [pl];
		}
	}


protected:
	SLAMBase (int theTotalNumStrategies, int theNumPlayers)
	: totalNumStrategies (theTotalNumStrategies), numPlayers (theNumPlayers),
	  sigmaValid (false), lambdaValid (false),
	  alphaValid (false), muValid (false),
	  sigma (theTotalNumStrategies), lambda (theTotalNumStrategies),
	  alpha (theTotalNumStrategies), mu (theNumPlayers) {
	}

	Gambit::Array<double> & getSigmaForWriting () {
		sigmaValid = false; alphaValid = false; return sigma;
	}
	Gambit::Array<double> & getLambdaForWriting () {
		lambdaValid = false; alphaValid = false; return lambda;
	}
	Gambit::Array<double> & getMuForWriting () {
		muValid = false; return mu;
	}

	void finalizeSigmaWriting () { sigmaValid = true; alphaValid = false; }
	void finalizeLambdaWriting () { lambdaValid = true; alphaValid = false; }
	void finalizeMuWriting () { muValid = true; }

private:
	const int totalNumStrategies;
	const int numPlayers;

	mutable bool sigmaValid, lambdaValid, alphaValid, muValid;
	mutable Gambit::Array<double> sigma;
	mutable Gambit::Array<double> lambda;
	mutable Gambit::Array<double> alpha;
	mutable Gambit::Array<double> mu;

private:
	void initializeAlphaFromSigmaLambda () const {
		if (! (sigmaValid && lambdaValid)) throw MissingInitializationException ();

		for (int strat = 1; strat <= totalNumStrategies; ++strat) {
			alpha [strat] = (sigma [strat] > 0.0) ? sqrt (sigma [strat]) : - sqrt (lambda [strat]);
		}
		alphaValid = true;
	}

	void initializeSigmaLambdaFromAlpha () const {
		if (!alphaValid) throw MissingInitializationException ();

		for (int strat = 1; strat <= totalNumStrategies; ++strat) {
			double d = alpha [strat];
			if (d >= 0.0) {
				sigma [strat] = d * d;
				lambda [strat] = 0.0;
			} else {
				sigma [strat] = 0.0;
				lambda [strat] = d * d;
			}
		}
		sigmaValid = true;
		lambdaValid = true;
	}
};

class Homhp2001Rho::SLAM
: public Homhp2001Rho::SLAMBase {

	friend class Homhp2001Rho;

private:
	SLAM (int totalNumStrategies, int numPlayers)
	: SLAMBase (totalNumStrategies, numPlayers) {
	}

public:
	virtual ~SLAM () { }

};


#endif /* HOMHP2001_RHO_H_ */
