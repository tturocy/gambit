/*
 * homotopy_solver.cc
 *
 *  Created on: May 30, 2011
 *      Author: jbecker
 */

#include <iostream>
#include "homotopy_solver.h"

/* ************************************************************************** */
extern "C" {

#include "f2c.h"

int fixpdf_(integer *n, doublereal *y, integer *iflag,
	doublereal *arctol, doublereal *eps, integer *trace, doublereal *a,
	integer *ndima, integer *nfe, doublereal *arclen, doublereal *yp,
	doublereal *ypold, doublereal *qr, doublereal *alpha, doublereal *tz,
	integer *pivot, doublereal *wt, doublereal *phi, doublereal *p,
	void * param);
}
/* end extern "C" */
/* ************************************************************************** */

extern "C"
int rhojac_ (
		doublereal * a,
		doublereal * lambda,
		doublereal * x,
		doublereal * v,
		// integer * k,
		void * param
		)
{
	static_cast<HomotopySolver::CallbackObj *> (param) -> rhoJacobian (a, *lambda, x, v);
	return 0;
}


void HomotopySolver::solve (int n,
		HomotopySolver::CallbackObj * callback) {
	/* dimension of y is totalNumStrategies + numPlayers + 1 */

	integer theN = n;
	integer iflag = -2; // tells FIXPDF to do homotopy path-following
	double arctol = 1.0e-8;
	double eps = 1.0e-12;
	integer trace = 1; // means that all points along the homotopy path are printed
	double* a = new double [n];
	integer ndima = n;
	integer nfe; // result: number of function evaluations
	double arclen; // result: length of homotopy path
	double* qr = new double [n * (n+1)]; // work array
	double* qalpha = new double [n]; // work array
	double* tz = new double [n+1]; // work array
	integer* pivot = new integer [n+1]; // work array
	double* wt = new double [n+1];
	double* phi = new double [(n+1)*16];
	double* p = new double [n+1];


	double* ypold = new double [n + 1];
	double* yp = new double [n + 1];

	ypold [0] = 1.0;
	for (int j = 1; j <= n; ++j)  ypold [j] = 0.0;

	yp [0] = 1.0;
	for (int j = 1; j <= n; ++j)  yp [j] = 0.0;

	double * y = new double [n + 1];

	callback->initializeCalculation (y [0], & (y [1]));


	fixpdf_ (&theN, y, &iflag, &arctol, &eps, &trace, a, &ndima,
			&nfe, &arclen, yp, ypold, qr, qalpha, tz, pivot,
			wt, phi, p, callback);

	callback->finalizeCalculation (y [0], & (y [1]));

	delete [] y;

	delete [] ypold;
	delete [] yp;


	delete [] p;
	delete [] phi;
	delete [] wt;

	delete [] pivot;
	delete [] tz;
	delete [] qalpha;

	delete [] qr;

	delete [] a;
}

