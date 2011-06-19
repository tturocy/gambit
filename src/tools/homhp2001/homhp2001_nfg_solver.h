/*
 * homhp2001_nfg_solver.h
 *
 *  Created on: May 17, 2011
 *      Author: jbecker
 */

#ifndef HOMHP2001_NFG_SOLVER_H_
#define HOMHP2001_NFG_SOLVER_H_

class Homhp2001NFGSolver {
public:
	virtual ~Homhp2001NFGSolver () { };
	void solve () const { doSolve (); }

private:
	virtual void doSolve () const = 0;
};

#endif /* HOMHP2001_NFG_SOLVER_H_ */
