/*
 * homhp2001_nfg_solver_env.h
 *
 *  Created on: May 18, 2011
 *      Author: jbecker
 */

#ifndef HOMHP2001_NFG_SOLVER_ENV_H_
#define HOMHP2001_NFG_SOLVER_ENV_H_

#include "homhp2001_nfg_solver_impl.h"

class Homhp2001NFGSolverImpl::Environment {
public:
	virtual ~Environment () { };
	virtual std::auto_ptr<Homhp2001NFGModule> createHomhp2001NFGModule (
			std::tr1::shared_ptr<const Gambit::StrategySupport>) const = 0;
};

#endif /* HOMHP2001_NFG_SOLVER_ENV_H_ */
