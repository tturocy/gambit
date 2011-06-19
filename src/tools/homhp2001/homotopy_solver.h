/*
 * homotopy_solver.h
 *
 *  Created on: May 30, 2011
 *      Author: jbecker
 */

#ifndef HOMOTOPY_SOLVER_H_
#define HOMOTOPY_SOLVER_H_


class HomotopySolver {
public:
	class CallbackObj {
	public:
		virtual ~CallbackObj () {  };

		void initializeCalculation (double & t, double * v) {
			doInitializeCalculation (t, v);
		}

		void rhoJacobian (double * a, double lambda, double * x, double * v) {
			// doCalcRhoJacobianAlt (a, lambda, x, v, k);
			doCalcRhoJacobian (a, lambda, x, v);
		}

		void finalizeCalculation (double t, double * v) {
			doFinalizeCalculation (t, v);
		}

	private:
		virtual void doInitializeCalculation (double & t, double * v) = 0;

		virtual void doCalcRhoJacobian (
				double * a, double lambda, double * x, double * v
				) = 0;

		virtual void doFinalizeCalculation (double t, double * v) = 0;

/*		virtual void doCalcRhoJacobianAlt (
				double * a, double lambda, double * x, double * v, int k
				) = 0; */
	};

	static void solve (int n, CallbackObj * callback);
};

#endif /* HOMOTOPY_SOLVER_H_ */
