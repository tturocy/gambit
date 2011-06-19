/*
 * homhp2001_nfg_solver_impl.cc
 *
 *  Created on: May 18, 2011
 *      Author: jbecker
 */

#include <memory>
#include <tr1/memory>
#include <iostream>
#include "libgambit/libgambit.h"
#include "libgambit/stratspt.h"
#include "homhp2001_nfg_solver.h"
#include "homhp2001_nfg_module.h"
#include "homhp2001_nfg_solver_impl.h"
#include "homhp2001_nfg_solver_env.h"

using std::auto_ptr;
using std::tr1::shared_ptr;


Homhp2001NFGSolverImpl::Homhp2001NFGSolverImpl (
		shared_ptr<const Homhp2001NFGSolverImpl::Environment> theEnv,
		const Gambit::Game & theGame
		)
: env (theEnv), game (theGame) {
}

void Homhp2001NFGSolverImpl::doSolve () const {

	shared_ptr<Gambit::StrategySupport> support (new Gambit::StrategySupport (game));

	auto_ptr<Homhp2001NFGModule> module = env->createHomhp2001NFGModule (support);
	module->performCalculation ();
}
