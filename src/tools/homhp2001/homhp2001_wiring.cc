/*
 * homhp2001_wiring.cc
 *
 *  Created on: May 17, 2011
 *      Author: jbecker
 */

#include <memory>
#include <tr1/memory>
#include "libgambit/libgambit.h"
#include "libgambit/stratspt.h"
#include "homhp2001_nfg_solver.h"
#include "homhp2001_nfg_module.h"
#include "homhp2001_nfg_solver_env.h"
#include "homhp2001_nfg_module_impl.h"
#include "homhp2001_nfg_solver_impl.h"

using std::auto_ptr;
using std::tr1::shared_ptr;


// sealed
class StdNFGSolverEnv
: public Homhp2001NFGSolverImpl::Environment {
private:
	StdNFGSolverEnv () { };

	static shared_ptr<const StdNFGSolverEnv> singleton;

public:
	static shared_ptr<const Homhp2001NFGSolverImpl::Environment> getSingleton ();

	auto_ptr<Homhp2001NFGModule>
	createHomhp2001NFGModule (shared_ptr<const Gambit::StrategySupport>) const;
};



/* *******************************************************
 * StdNFGSolverEnv
 * *******************************************************/

shared_ptr<const StdNFGSolverEnv>
StdNFGSolverEnv::singleton (new StdNFGSolverEnv ());


shared_ptr<const Homhp2001NFGSolverImpl::Environment>
StdNFGSolverEnv::getSingleton () {
	return singleton;
}

auto_ptr<Homhp2001NFGModule>
StdNFGSolverEnv::createHomhp2001NFGModule (shared_ptr<const Gambit::StrategySupport> support) const {
	return std::auto_ptr<Homhp2001NFGModule> ( new Homhp2001NFGModuleImpl (support) );
}



/* ********************************************************
 * Global Functions
 * ********************************************************/

auto_ptr<Homhp2001NFGSolver>
createHomhp2001NFGSolver (const Gambit::Game & game) {
	return auto_ptr<Homhp2001NFGSolver> (
			new Homhp2001NFGSolverImpl (StdNFGSolverEnv::getSingleton (), game)
	);
}



