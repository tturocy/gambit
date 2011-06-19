/*
 * homhp2001_nfg_solver_impl.h
 *
 *  Created on: May 17, 2011
 *      Author: jbecker
 */

#ifndef HOMHP2001_NFG_SOLVER_IMPL_H_
#define HOMHP2001_NFG_SOLVER_IMPL_H_

#include "homhp2001_nfg_solver_impl.h"
#include <tr1/memory>

class Homhp2001NFGSolverImpl
: public Homhp2001NFGSolver {
public:
	class Environment;

private:
	std::tr1::shared_ptr<const Environment> env;
	const Gambit::Game & game;

public:
	Homhp2001NFGSolverImpl (
			std::tr1::shared_ptr<const Environment>, const Gambit::Game &);
	virtual ~Homhp2001NFGSolverImpl () { };

private:
	virtual void doSolve () const;
};
#endif /* HOMHP2001_NFG_SOLVER_IMPL_H_ */
