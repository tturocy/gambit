/* fixpdf.f -- translated by f2c (version 20090411).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

/* translated from fixpdf.for by f2c
 * manual changes:
 *   * replaced par, ipar by param
 *   * rhojac_ calculates full matrix instead of only first derivative
 */

#include <stdlib.h>

#include <f2c.h>

/* Common Block Declarations */

struct {
    integer cray1[38];
} d9mach_;

#define d9mach_1 d9mach_

/* Table of constant values */

static integer c__1 = 1;
static integer c__4 = 4;
static doublereal c_b74 = 1.;
static integer c__8285 = 8285;
static integer c_b187 = 8388608;
static integer c__0 = 0;
static integer c__24574 = 24574;
static integer c_b190 = 16777215;
static integer c_b194 = 16777214;
static integer c__16291 = 16291;
static integer c__16292 = 16292;
static integer c__16383 = 16383;
static integer c_b202 = 10100890;
static integer c_b203 = 8715215;
static integer c_b205 = 16226447;
static integer c_b206 = 9001388;
static integer c__9 = 9;
static integer c__3 = 3;

/* Subroutine */ int fixpdf_(integer *n, doublereal *y, integer *iflag, 
	doublereal *arctol, doublereal *eps, integer *trace, doublereal *a, 
	integer *ndima, integer *nfe, doublereal *arclen, doublereal *yp, 
	doublereal *ypold, doublereal *qr, doublereal *alpha, doublereal *tz, 
	integer *pivot, doublereal *wt, doublereal *phi, doublereal *p,
	void * param)
{
    /* Format strings */
    static char fmt_117[] = "(/\002 STEP\002,i5,3x,\002NFE =\002,i5,3x,\002A"
	    "RC LENGTH =\002,f9.4,3x,\002LAMBDA =\002,f7.4,5x,\002X vector"
	    ":\002/1p,(1x,6e12.4))";

    /* System generated locals */
    integer phi_dim1, phi_offset, qr_dim1, qr_offset, i__1, i__2;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

    /* Local variables */
    extern /* Subroutine */ int f_(doublereal *, doublereal *);
    static doublereal g[13], h__;
    static integer j, k;
    static doublereal s, w[12], gi[11], sa, sb;
    static integer iv[10], jw;
    static doublereal s99;
    static integer np1, kgi, ivc;
    static logical st99;
    static integer nfec;
    extern /* Subroutine */ int fode_();
    static doublereal aold, hold;
    static integer kold;
    extern /* Subroutine */ int rhoa_(doublereal *, doublereal *, doublereal *
	    , void *);
    static integer iter;
    static doublereal xold, epst;
    static integer judy;
    extern /* Subroutine */ int root_(doublereal *, doublereal *, doublereal *
	    , doublereal *, doublereal *, doublereal *, integer *);
    static doublereal sout, sqnp1;
    static integer lcode;
    static logical crash;
    static integer limit;
    static logical start;
    extern /* Subroutine */ int steps_(U_fp, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, logical *,
	     doublereal *, integer *, integer *, logical *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *, integer *, doublereal *, integer *, integer *, 
	    integer *, doublereal *, doublereal *, doublereal *, doublereal *,
	     doublereal *, doublereal *, integer *, integer *, integer *, 
	    void *);
    static doublereal cursw;
    static integer iflagc;
    static doublereal y1sout, alphas[12], curtol;
    static integer ksteps;
    static doublereal epsstp;
    extern /* Subroutine */ int sintrp_(doublereal *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, integer *, integer *, 
	    doublereal *, integer *, integer *, integer *, doublereal *, 
	    doublereal *, doublereal *, doublereal *, doublereal *, 
	    doublereal *);

    /* Fortran I/O blocks */
    static cilist io___31 = { 0, 0, 0, fmt_117, 0 };



/* SUBROUTINE  FIXPDF  FINDS A FIXED POINT OR ZERO OF THE */
/* N-DIMENSIONAL VECTOR FUNCTION F(X), OR TRACKS A ZERO CURVE */
/* OF A GENERAL HOMOTOPY MAP RHO(A,LAMBDA,X).  FOR THE FIXED */
/* POINT PROBLEM F(X) IS ASSUMED TO BE A C2 MAP OF SOME BALL */
/* INTO ITSELF.  THE EQUATION  X = F(X)  IS SOLVED BY */
/* FOLLOWING THE ZERO CURVE OF THE HOMOTOPY MAP */

/*  LAMBDA*(X - F(X)) + (1 - LAMBDA)*(X - A)  , */

/* STARTING FROM LAMBDA = 0, X = A.  THE CURVE IS PARAMETERIZED */
/* BY ARC LENGTH S, AND IS FOLLOWED BY SOLVING THE ORDINARY */
/* DIFFERENTIAL EQUATION  D(HOMOTOPY MAP)/DS = 0  FOR */
/* Y(S) = (LAMBDA(S), X(S)). */

/* FOR THE ZERO FINDING PROBLEM F(X) IS ASSUMED TO BE A C2 MAP */
/* SUCH THAT FOR SOME R > 0,  X*F(X) >= 0  WHENEVER NORM(X) = R. */
/* THE EQUATION  F(X) = 0  IS SOLVED BY FOLLOWING THE ZERO CURVE */
/* OF THE HOMOTOPY MAP */

/*   LAMBDA*F(X) + (1 - LAMBDA)*(X - A) */

/* EMANATING FROM LAMBDA = 0, X = A. */

/*  A  MUST BE AN INTERIOR POINT OF THE ABOVE MENTIONED BALLS. */

/* FOR THE CURVE TRACKING PROBLEM RHO(A,LAMBDA,X) IS ASSUMED TO */
/* BE A C2 MAP FROM E**M X [0,1) X E**N INTO E**N, WHICH FOR */
/* ALMOST ALL PARAMETER VECTORS A IN SOME NONEMPTY OPEN SUBSET */
/* OF E**M SATISFIES */

/*  RANK [D RHO(A,LAMBDA,X)/D LAMBDA , D RHO(A,LAMBDA,X)/DX] = N */

/* FOR ALL POINTS (LAMBDA,X) SUCH THAT RHO(A,LAMBDA,X)=0.  IT IS */
/* FURTHER ASSUMED THAT */

/*           RANK [ D RHO(A,0,X0)/DX ] = N  . */

/* WITH A FIXED, THE ZERO CURVE OF RHO(A,LAMBDA,X) EMANATING */
/* FROM  LAMBDA = 0, X = X0  IS TRACKED UNTIL  LAMBDA = 1  BY */
/* SOLVING THE ORDINARY DIFFERENTIAL EQUATION */
/* D RHO(A,LAMBDA(S),X(S))/DS = 0  FOR  Y(S) = (LAMBDA(S), X(S)), */
/* WHERE S IS ARC LENGTH ALONG THE ZERO CURVE.  ALSO THE HOMOTOPY */
/* MAP RHO(A,LAMBDA,X) IS ASSUMED TO BE CONSTRUCTED SUCH THAT */

/*              D LAMBDA(0)/DS > 0  . */

/* THIS CODE IS BASED ON THE ALGORITHM IN L. T. WATSON, A */
/* GLOBALLY CONVERGENT ALGORITHM FOR COMPUTING FIXED POINTS OF */
/* C2 MAPS, APPL. MATH. COMPUT., 5 (1979) 297-311. */


/* FOR THE FIXED POINT AND ZERO FINDING PROBLEMS, THE USER */
/* MUST SUPPLY A SUBROUTINE  F(X,V)  WHICH EVALUATES F(X) AT X */
/* AND RETURNS THE VECTOR F(X) IN V, AND A SUBROUTINE  FJAC(X,V,K) */
/* WHICH RETURNS IN V THE KTH COLUMN OF THE JACOBIAN MATRIX OF */
/* F(X) EVALUATED AT X.  FOR THE CURVE TRACKING PROBLEM, THE USER MUST */
/* SUPPLY A SUBROUTINE  RHOA(V,LAMBDA,X,PAR,IPAR)  WHICH GIVEN */
/* (LAMBDA,X) RETURNS A PARAMETER VECTOR A IN V SUCH THAT */
/* RHO(A,LAMBDA,X)=0, AND A SUBROUTINE  RHOJAC(A,LAMBDA,X,V,K,PAR,IPAR) */
/* WHICH RETURNS IN V THE KTH COLUMN OF THE N X (N+1) JACOBIAN */
/* MATRIX [D RHO/D LAMBDA, D RHO/DX] EVALUATED AT (A,LAMBDA,X). */
/*  FIXPDF  DIRECTLY OR INDIRECTLY USES THE SUBROUTINES */
/*  STEPS , SINTRP , ROOT , FODE , F (OR  RHOA ), */
/* FJAC (OR  RHOJAC ), DCPOSE , D1MACH , AND THE BLAS FUNCTIONS */
/* DDOT  AND  DNRM2 .  ONLY  D1MACH  CONTAINS MACHINE */
/* DEPENDENT CONSTANTS.  NO OTHER MODIFICATIONS BY THE USER ARE */
/* REQUIRED. */

/* ***WARNING:  THIS SUBROUTINE IS GENERALLY MORE ROBUST THAN  FIXPNF */
/* AND  FIXPQF , BUT MAY BE SLOWER THAN THOSE SUBROUTINES BY A */
/* FACTOR OF TWO. */


/* ON INPUT: */

/* N  IS THE DIMENSION OF X, F(X), AND RHO(A,LAMBDA,X). */

/* Y  IS AN ARRRAY OF LENGTH  N + 1.  (Y(2),...,Y(N+1)) = A  IS THE */
/*    STARTING POINT FOR THE ZERO CURVE FOR THE FIXED POINT AND */
/*    ZERO FINDING PROBLEMS.  (Y(2),...,Y(N+1)) = X0  FOR THE CURVE */
/*    TRACKING PROBLEM. */

/* IFLAG  CAN BE -2, -1, 0, 2, OR 3.  IFLAG  SHOULD BE 0 ON THE */
/*    FIRST CALL TO  FIXPDF  FOR THE PROBLEM  X=F(X), -1 FOR THE */
/*    PROBLEM  F(X)=0, AND -2 FOR THE PROBLEM  RHO(A,LAMBDA,X)=0. */
/*    IN CERTAIN SITUATIONS  IFLAG  IS SET TO 2 OR 3 BY  FIXPDF, */
/*    AND  FIXPDF  CAN BE CALLED AGAIN WITHOUT CHANGING  IFLAG. */

/* ARCTOL  IS THE LOCAL ERROR ALLOWED THE ODE SOLVER WHEN */
/*    FOLLOWING THE ZERO CURVE.  IF  ARCTOL .LE. 0.0  ON INPUT */
/*    IT IS RESET TO  .5*DSQRT(EPS).  NORMALLY  ARCTOL  SHOULD */
/*    BE CONSIDERABLY LARGER THAN  EPS. */

/* EPS  IS THE LOCAL ERROR ALLOWED THE ODE SOLVER WHEN VERY */
/*    NEAR THE FIXED POINT(ZERO).  EPS  IS APPROXIMATELY THE */
/*    MIXED ABSOLUTE AND RELATIVE ERROR IN THE COMPUTED FIXED */
/*    POINT(ZERO). */

/* TRACE  IS AN INTEGER SPECIFYING THE LOGICAL I/O UNIT FOR */
/*    INTERMEDIATE OUTPUT.  IF  TRACE .GT. 0  THE POINTS COMPUTED ON */
/*    THE ZERO CURVE ARE WRITTEN TO I/O UNIT  TRACE . */

/* A(1:NDIMA) CONTAINS THE PARAMETER VECTOR  A .  FOR THE FIXED POINT */
/*    AND ZERO FINDING PROBLEMS, A  NEED NOT BE INITIALIZED BY THE */
/*    USER, AND IS ASSUMED TO HAVE LENGTH  N.  FOR THE CURVE */
/*    TRACKING PROBLEM, A  HAS LENGTH  NDIMA  AND MUST BE INITIALIZED */
/*    BY THE USER. */

/* NDIMA  IS THE DIMENSION OF  A, AND IS USED ONLY FOR THE CURVE */
/*    TRACKING PROBLEM. */

/* YP(1:N+1) IS A WORK ARRAY CONTAINING THE CURRENT TANGENT */
/*    VECTOR TO THE ZERO CURVE. */

/* YPOLD(1:N+1) IS A WORK ARRAY CONTAINING THE PREVIOUS TANGENT */
/*    VECTOR TO THE ZERO CURVE. */

/* QR(1:N,1:N+1), ALPHA(1:N), TZ(1:N+1), AND PIVOT(1:N+1) ARE */
/*    ALL WORK ARRAYS USED BY  FODE  TO CALCULATE THE TANGENT */
/*    VECTOR YP. */

/* WT(1:N+1), PHI(1:N+1,1:16), AND P(1:N+1) ARE ALL WORK ARRAYS */
/*    USED BY THE ODE SUBROUTINE  STEPS  . */

/* PAR(1:*) AND IPAR(1:*) ARE ARRAYS FOR (OPTIONAL) USER PARAMETERS, */
/*    WHICH ARE SIMPLY PASSED THROUGH TO THE USER WRITTEN SUBROUTINES */
/*    RHOA, RHOJAC. */

/* Y, ARCTOL, EPS, ARCLEN, NFE, AND IFLAG SHOULD ALL BE */
/* VARIABLES IN THE CALLING PROGRAM. */


/* ON OUTPUT: */

/* N  AND  TRACE  ARE UNCHANGED. */

/* Y(1) = LAMBDA, (Y(2),...,Y(N+1)) = X, AND Y IS AN APPROXIMATE */
/*    ZERO OF THE HOMOTOPY MAP.  NORMALLY LAMBDA = 1 AND X IS A */
/*    FIXED POINT(ZERO) OF F(X).  IN ABNORMAL SITUATIONS LAMBDA */
/*    MAY ONLY BE NEAR 1 AND X IS NEAR A FIXED POINT(ZERO). */

/* IFLAG = */
/*  -2   CAUSES  FIXPDF  TO INITIALIZE EVERYTHING FOR THE PROBLEM */
/*       RHO(A,LAMBDA,X) = 0 (USE ON FIRST CALL). */

/*  -1   CAUSES  FIXPDF  TO INITIALIZE EVERYTHING FOR THE PROBLEM */
/*       F(X) = 0 (USE ON FIRST CALL). */

/*   0   CAUSES  FIXPDF  TO INITIALIZE EVERYTHING FOR THE PROBLEM */
/*       X = F(X) (USE ON FIRST CALL). */

/*   1   NORMAL RETURN. */

/*   2   SPECIFIED ERROR TOLERANCE CANNOT BE MET.  EPS HAS BEEN */
/*       INCREASED TO A SUITABLE VALUE.  TO CONTINUE, JUST CALL */
/*       FIXPDF  AGAIN WITHOUT CHANGING ANY PARAMETERS. */

/*   3   STEPS  HAS BEEN CALLED 1000 TIMES.  TO CONTINUE, CALL */
/*       FIXPDF  AGAIN WITHOUT CHANGING ANY PARAMETERS. */

/*   4   JACOBIAN MATRIX DOES NOT HAVE FULL RANK.  THE ALGORITHM */
/*       HAS FAILED (THE ZERO CURVE OF THE HOMOTOPY MAP CANNOT BE */
/*       FOLLOWED ANY FURTHER). */

/*   5   EPS  (OR  ARCTOL ) IS TOO LARGE.  THE PROBLEM SHOULD BE */
/*       RESTARTED BY CALLING  FIXPDF  WITH A SMALLER  EPS  (OR */
/*       ARCTOL ) AND  IFLAG = 0 (-1, -2). */

/*   6   I - DF(X)  IS NEARLY SINGULAR AT THE FIXED POINT (DF(X) IS */
/*       NEARLY SINGULAR AT THE ZERO, OR  D RHO(A,LAMBDA,X)/DX  IS */
/*       NEARLY SINGULAR AT  LAMBDA = 1 ).  ANSWER MAY NOT BE */
/*       ACCURATE. */

/*   7   ILLEGAL INPUT PARAMETERS, A FATAL ERROR. */

/* ARCTOL = EPS AFTER A NORMAL RETURN (IFLAG = 1). */

/* EPS  IS UNCHANGED AFTER A NORMAL RETURN (IFLAG = 1).  IT IS */
/*    INCREASED TO AN APPROPRIATE VALUE ON THE RETURN IFLAG = 2. */

/* A  WILL (NORMALLY) HAVE BEEN MODIFIED. */

/* NFE  IS THE NUMBER OF FUNCTION EVALUATIONS (= NUMBER OF */
/*    JACOBIAN EVALUATIONS). */

/* ARCLEN  IS THE LENGTH OF THE PATH FOLLOWED. */



/* *****  ARRAY DECLARATIONS.  ***** */

/* ARRAYS NEEDED BY THE ODE SUBROUTINE  STEPS . */

/* ARRAYS NEEDED BY  FIXPDF , FODE , AND  DCPOSE . */

/* *****  END OF DIMENSIONAL INFORMATION.  ***** */


/* LIMITD  IS AN UPPER BOUND ON THE NUMBER OF STEPS.  IT MAY BE */
/* CHANGED BY CHANGING THE FOLLOWING PARAMETER STATEMENT: */


/* :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  :  : */
    /* Parameter adjustments */
    --p;
    phi_dim1 = *n + 1;
    phi_offset = 1 + phi_dim1;
    phi -= phi_offset;
    --wt;
    --pivot;
    --tz;
    --alpha;
    qr_dim1 = *n;
    qr_offset = 1 + qr_dim1;
    qr -= qr_offset;
    --ypold;
    --yp;
    --a;
    --y;

    /* Function Body */
    if (*n <= 0 || *eps <= 0.f) {
	*iflag = 7;
    }
    if (*iflag >= -2 && *iflag <= 0) {
	goto L10;
    }
    if (*iflag == 2) {
	goto L35;
    }
    if (*iflag == 3) {
	goto L30;
    }
/* ONLY VALID INPUT FOR  IFLAG  IS -2, -1, 0, 2, 3. */
    *iflag = 7;
    return 0;

/* *****  INITIALIZATION BLOCK.  ***** */

L10:
    *arclen = 0.f;
    s = 0.f;
    if (*arctol <= 0.f) {
	*arctol = sqrt(*eps) * .5f;
    }
    nfec = 0;
    iflagc = *iflag;
    np1 = *n + 1;
    sqnp1 = sqrt((doublereal) np1);

/* SWITCH FROM THE TOLERANCE  ARCTOL  TO THE (FINER) TOLERANCE  EPS  IF */
/* THE CURVATURE OF ANY COMPONENT OF  Y  EXCEEDS  CURSW. */

    cursw = 10.f;

    st99 = FALSE_;
    start = TRUE_;
    crash = FALSE_;
    hold = 1.f;
    h__ = .1f;
    epsstp = *arctol;
    ksteps = 0;
/* SET INITIAL CONDITIONS FOR ORDINARY DIFFERENTIAL EQUATION. */
    ypold[1] = 1.f;
    yp[1] = 1.f;
    y[1] = 0.f;
    i__1 = np1;
    for (j = 2; j <= i__1; ++j) {
	ypold[j] = 0.f;
	yp[j] = 0.f;
/* L20: */
    }
/* LOAD  A  FOR THE FIXED POINT AND ZERO FINDING PROBLEMS. */
    if (iflagc >= -1) {
	i__1 = np1;
	for (j = 2; j <= i__1; ++j) {
	    a[j - 1] = y[j];
/* L23: */
	}
    }
L30:
    limit = 1000000;

/* *****  END OF INITIALIZATION BLOCK.  ***** */


/* *****  MAIN LOOP.  ***** */

L35:
    i__1 = limit;
    for (iter = 1; iter <= i__1; ++iter) {
	if (y[1] < 0.f) {
/* L40: */
	    *arclen += s;
	    *iflag = 5;
	    return 0;
	}
/* L50: */
	if (s <= sqnp1 * 7.f) {
	    goto L80;
	}
/* ARC LENGTH IS GETTING TOO LONG, THE PROBLEM WILL BE */
/* RESTARTED WITH A DIFFERENT  A  VECTOR. */
	*arclen += s;
	s = 0.f;
L60:
	start = TRUE_;
	crash = FALSE_;
/* COMPUTE A NEW  A  VECTOR. */
	if (iflagc == -2) {
	    i__2 = *ndima;
	    for (jw = 1; jw <= i__2; ++jw) {
		qr[jw + qr_dim1] = a[jw];
/* L63: */
	    }
	    rhoa_(&a[1], &y[1], &y[2], param);
	    i__2 = *ndima;
	    for (jw = 1; jw <= i__2; ++jw) {
		aold = qr[jw + qr_dim1];
/* IF NEW AND OLD  A  DIFFER BY TOO MUCH, TRACKING SHOULD NOT CONTINUE. */
		if ((d__1 = a[jw] - aold, abs(d__1)) > abs(aold) + 1.f) {
		    *arclen += s;
		    *iflag = 5;
		    return 0;
		}
/* L65: */
	    }
	} else {
	    f_(&y[2], &yp[1]);
	    i__2 = *n;
	    for (jw = 1; jw <= i__2; ++jw) {
		aold = a[jw];
		if (iflagc == -1) {
		    a[jw] = y[1] * yp[jw] / (1.f - y[1]) + y[jw + 1];
		} else {
		    a[jw] = (y[jw + 1] - y[1] * yp[jw]) / (1.f - y[1]);
		}
/* IF NEW AND OLD  A  DIFFER BY TOO MUCH, TRACKING SHOULD NOT CONTINUE. */
		if ((d__1 = a[jw] - aold, abs(d__1)) > abs(aold) + 1.f) {
		    *arclen += s;
		    *iflag = 5;
		    return 0;
		}
/* L70: */
	    }
	}
	goto L100;
L80:
	if (y[1] <= .99f || st99) {
	    goto L100;
	}
/* WHEN LAMBDA REACHES .99, THE PROBLEM WILL BE RESTARTED WITH */
/* A NEW  A  VECTOR. */
L90:
	st99 = TRUE_;
	epsstp = *eps;
	*arctol = *eps;
	goto L60;

/* SET DIFFERENT ERROR TOLERANCE FOR HIGH CURVATURE COMPONENTS OF THE */
/* TRAJECTORY Y(S). */
L100:
	curtol = cursw * hold;
	epst = *eps / epsstp;
	i__2 = np1;
	for (jw = 1; jw <= i__2; ++jw) {
	    if ((d__1 = yp[jw] - ypold[jw], abs(d__1)) <= curtol) {
		wt[jw] = (d__1 = y[jw], abs(d__1)) + 1.f;
	    } else {
		wt[jw] = ((d__1 = y[jw], abs(d__1)) + 1.f) * epst;
	    }
/* L110: */
	}

/* TAKE A STEP ALONG THE CURVE. */
	steps_((U_fp)fode_, &np1, &y[1], &s, &h__, &epsstp, &wt[1], &start, &
		hold, &k, &kold, &crash, &phi[phi_offset], &p[1], &yp[1], 
		alphas, w, g, &ksteps, &xold, &ivc, iv, &kgi, gi, &ypold[1], &
		a[1], &qr[qr_offset], &alpha[1], &tz[1], &pivot[1], &nfec, &
		iflagc, param);
/* PRINT LATEST POINT ON CURVE IF REQUESTED. */
	if (*trace > 0) {
	    io___31.ciunit = *trace;
	    s_wsfe(&io___31);
	    do_fio(&c__1, (char *)&iter, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&nfec, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&s, (ftnlen)sizeof(doublereal));
	    do_fio(&c__1, (char *)&y[1], (ftnlen)sizeof(doublereal));
	    i__2 = np1;
	    for (jw = 2; jw <= i__2; ++jw) {
		do_fio(&c__1, (char *)&y[jw], (ftnlen)sizeof(doublereal));
	    }
	    e_wsfe();
	}
	*nfe = nfec;
/* CHECK IF THE STEP WAS SUCCESSFUL. */
	if (iflagc == 4) {
	    *arclen += s;
	    *iflag = 4;
	    return 0;
	}
/* L120: */
	if (crash) {
/* RETURN CODE FOR ERROR TOLERANCE TOO SMALL. */
	    *iflag = 2;
/* CHANGE ERROR TOLERANCES. */
	    *eps = epsstp;
	    if (*arctol < *eps) {
		*arctol = *eps;
	    }
/* CHANGE LIMIT ON NUMBER OF ITERATIONS. */
	    limit -= iter;
	    return 0;
	}

/* L130: */
	if (y[1] >= 1.f) {
	    if (st99) {
		goto L160;
	    }

/* IF LAMBDA .GE. 1.0 BUT THE PROBLEM HAS NOT BEEN RESTARTED */
/* WITH A NEW  A  VECTOR, BACK UP AND RESTART. */

	    s99 = s - hold * .5f;
/* GET AN APPROXIMATE ZERO Y(S) WITH  Y(1)=LAMBDA .LT. 1.0  . */
L135:
	    sintrp_(&s, &y[1], &s99, &wt[1], &yp[1], &np1, &kold, &phi[
		    phi_offset], &ivc, iv, &kgi, gi, alphas, g, w, &xold, &p[
		    1]);
	    if (wt[1] < 1.f) {
		goto L140;
	    }
	    s99 = (s - hold + s99) * .5f;
	    goto L135;

L140:
	    i__2 = np1;
	    for (judy = 1; judy <= i__2; ++judy) {
		y[judy] = wt[judy];
		ypold[judy] = yp[judy];
/* L144: */
	    }
	    s = s99;
	    goto L90;
	}

/* L150: */
    }

/* *****  END OF MAIN LOOP.  ***** */

/* LAMBDA HAS NOT REACHED 1 IN 1000 STEPS. */
    *iflag = 3;
    return 0;


/* USE INVERSE INTERPOLATION TO GET THE ANSWER AT LAMBDA = 1.0 . */

L160:
    sa = s - hold;
    sb = s;
    lcode = 1;
L170:
    root_(&sout, &y1sout, &sa, &sb, eps, eps, &lcode);
/* ROOT  FINDS S SUCH THAT Y(1)(S) = LAMBDA = 1 . */
    if (lcode > 0) {
	goto L190;
    }
    sintrp_(&s, &y[1], &sout, &wt[1], &yp[1], &np1, &kold, &phi[phi_offset], &
	    ivc, iv, &kgi, gi, alphas, g, w, &xold, &p[1]);
    y1sout = wt[1] - 1.f;
    goto L170;
L190:
    *iflag = 1;
/* SET IFLAG = 6 IF  ROOT  COULD NOT GET  LAMBDA = 1.0  . */
    if (lcode > 2) {
	*iflag = 6;
    }
    *arclen += sa;
/* LAMBDA(SA) = 1.0 . */
    sintrp_(&s, &y[1], &sa, &wt[1], &yp[1], &np1, &kold, &phi[phi_offset], &
	    ivc, iv, &kgi, gi, alphas, g, w, &xold, &p[1]);

    i__1 = np1;
    for (j = 1; j <= i__1; ++j) {
/* L210: */
	y[j] = wt[j];
    }
    return 0;
} /* fixpdf_ */






/* Subroutine */ int f_(doublereal *x, doublereal *v)
{

/* EVALUATE  F(X)  AND RETURN IN THE VECTOR  V . */

    /* Parameter adjustments */
    --v;
    --x;

    /* Function Body */
    return 0;
} /* f_ */






/* Subroutine */ int fjac_(doublereal *x, doublereal *v, integer *k)
{

/* RETURN IN  V  THE KTH COLUMN OF THE JACOBIAN MATRIX OF */
/* F(X) EVALUATED AT  X . */

    /* Parameter adjustments */
    --v;
    --x;

    /* Function Body */
    return 0;
} /* fjac_ */






/* Subroutine */ int rhoa_(doublereal *a, doublereal *lambda, doublereal *x, 
	void * param)
{

/* PAR(1:*) AND IPAR(1:*) ARE ARRAYS FOR (OPTIONAL) USER PARAMETERS, */
/*    WHICH ARE SIMPLY PASSED THROUGH TO THE USER WRITTEN SUBROUTINES */
/*    RHOA, RHOJS. */

/* CALCULATE AND RETURN IN  A  THE VECTOR Z SUCH THAT */
/*  RHO(Z,LAMBDA,X) = 0 . */

    /* Parameter adjustments */
    --x;
    --a;

    /* Function Body */
    return 0;
} /* rhoa_ */






/* Subroutine */ int fode_(doublereal *s, doublereal *y, doublereal *yp, 
	doublereal *ypold, doublereal *a, doublereal *qr, doublereal *alpha, 
	doublereal *tz, integer *pivot, integer *nfe, integer *n, integer *
	iflag, void * param)
{
    /* System generated locals */
    integer qr_dim1, qr_offset, i__1, i__2;

    /* Local variables */
    extern /* Subroutine */ int f_(doublereal *, doublereal *);
    static integer i__, j, k, ik, lw, kp1, np1;
    static doublereal sum;
    extern /* Subroutine */ int fjac_(doublereal *, doublereal *, integer *);
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    static integer ierr, kpiv;
    extern doublereal dnrm2_(integer *, doublereal *, integer *);
    extern /* Subroutine */ int rhojac_(doublereal *, doublereal *, 
	    doublereal *, doublereal *, void *),
	    dcpose_(integer *, integer *, doublereal *, doublereal *, integer 
	    *, integer *, doublereal *, doublereal *);
    static doublereal ypnorm;


/* SUBROUTINE  FODE  IS USED BY SUBROUTINE  STEPS  TO SPECIFY THE */
/* ORDINARY DIFFERENTIAL EQUATION  DY/DS = G(S,Y) , WHOSE SOLUTION */
/* IS THE ZERO CURVE OF THE HOMOTOPY MAP.  S = ARC LENGTH, */
/* YP = DY/DS, AND  Y(S) = (LAMBDA(S), X(S)) . */

/* PAR(1:*) AND IPAR(1:*) ARE ARRAYS FOR (OPTIONAL) USER PARAMETERS, */
/*    WHICH ARE SIMPLY PASSED THROUGH TO THE USER WRITTEN SUBROUTINES */
/*    RHOA, RHOJAC. */


/* *****  ARRAY DECLARATIONS.  ***** */


/* ARRAYS FOR COMPUTING THE JACOBIAN MATRIX AND ITS KERNEL. */

/* *****  END OF DIMENSIONAL INFORMATION.  ***** */


    /* Parameter adjustments */
    --pivot;
    --tz;
    --alpha;
    qr_dim1 = *n;
    qr_offset = 1 + qr_dim1;
    qr -= qr_offset;
    --a;
    --ypold;
    --yp;
    --y;

    /* Function Body */
    np1 = *n + 1;
    ++(*nfe);
/* NFE CONTAINS THE NUMBER OF JACOBIAN EVALUATIONS. */
/*   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   * */

/* COMPUTE THE JACOBIAN MATRIX, STORE IT IN QR. */

    if (*iflag == -2) {

/*  QR = ( D RHO(A,LAMBDA,X)/D LAMBDA , D RHO(A,LAMBDA,X)/DX )  . */

	i__1 = np1;
	rhojac_(&a[1], &y[1], &y[2], &qr[qr_dim1 + 1], param);
/* L30: */
    } else {
	f_(&y[2], &tz[1]);
	if (*iflag == 0) {

/*      QR = ( A - F(X), I - LAMBDA*DF(X) )  . */

	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* L100: */
		qr[j + qr_dim1] = a[j] - tz[j];
	    }
	    i__1 = *n;
	    for (k = 1; k <= i__1; ++k) {
		fjac_(&y[2], &tz[1], &k);
		kp1 = k + 1;
		i__2 = *n;
		for (j = 1; j <= i__2; ++j) {
/* L110: */
		    qr[j + kp1 * qr_dim1] = -y[1] * tz[j];
		}
/* L120: */
		qr[k + kp1 * qr_dim1] += 1.f;
	    }
	} else {

/*   QR = ( F(X) - X + A, LAMBDA*DF(X) + (1 - LAMBDA)*I ) . */

/* L140: */
	    i__1 = *n;
	    for (j = 1; j <= i__1; ++j) {
/* L150: */
		qr[j + qr_dim1] = tz[j] - y[j + 1] + a[j];
	    }
	    i__1 = *n;
	    for (k = 1; k <= i__1; ++k) {
		fjac_(&y[2], &tz[1], &k);
		kp1 = k + 1;
		i__2 = *n;
		for (j = 1; j <= i__2; ++j) {
/* L160: */
		    qr[j + kp1 * qr_dim1] = y[1] * tz[j];
		}
/* L170: */
		qr[k + kp1 * qr_dim1] = 1.f - y[1] + qr[k + kp1 * qr_dim1];
	    }
	}
    }

/*   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   *   * */
/* REDUCE THE JACOBIAN MATRIX TO UPPER TRIANGULAR FORM. */
/* L210: */
    dcpose_(n, n, &qr[qr_offset], &alpha[1], &pivot[1], &ierr, &tz[1], &yp[1])
	    ;
    if (ierr == 0) {
	goto L220;
    }
    *iflag = 4;
    return 0;
/* COMPUTE KERNEL OF JACOBIAN, WHICH SPECIFIES YP=DY/DS. */
L220:
    tz[np1] = 1.f;
    i__1 = *n;
    for (lw = 1; lw <= i__1; ++lw) {
	i__ = np1 - lw;
	ik = i__ + 1;
	sum = 0.f;
	i__2 = np1;
	for (j = ik; j <= i__2; ++j) {
/* L230: */
	    sum += qr[i__ + j * qr_dim1] * tz[j];
	}
/* L240: */
	tz[i__] = -sum / alpha[i__];
    }
    ypnorm = dnrm2_(&np1, &tz[1], &c__1);
    i__1 = np1;
    for (k = 1; k <= i__1; ++k) {
	kpiv = pivot[k];
/* L260: */
	yp[kpiv] = tz[k] / ypnorm;
    }
    if (ddot_(&np1, &yp[1], &c__1, &ypold[1], &c__1) >= 0.f) {
	goto L280;
    }
    i__1 = np1;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L270: */
	yp[i__] = -yp[i__];
    }

/* SAVE CURRENT DERIVATIVE (= TANGENT VECTOR) IN  YPOLD . */
L280:
    i__1 = np1;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L290: */
	ypold[i__] = yp[i__];
    }
    return 0;
} /* fode_ */






/* Subroutine */ int root_(doublereal *t, doublereal *ft, doublereal *b, 
	doublereal *c__, doublereal *relerr, doublereal *abserr, integer *
	iflag)
{
    /* System generated locals */
    doublereal d__1, d__2;

    /* Builtin functions */
    double d_sign(doublereal *, doublereal *);

    /* Local variables */
    static doublereal a, p, q, u, ae, fa, fb, fc;
    static integer ic;
    static doublereal re, fx, cmb, tol, acmb, acbs;
    extern doublereal d1mach_(integer *);
    static integer kount;


/*  ROOT COMPUTES A ROOT OF THE NONLINEAR EQUATION F(X)=0 */
/*  WHERE F(X) IS A CONTINOUS REAL FUNCTION OF A SINGLE REAL */
/*  VARIABLE X.  THE METHOD USED IS A COMBINATION OF BISECTION */
/*  AND THE SECANT RULE. */

/*  NORMAL INPUT CONSISTS OF A CONTINUOS FUNCTION F AND AN */
/*  INTERVAL (B,C) SUCH THAT F(B)*F(C).LE.0.0.  EACH ITERATION */
/*  FINDS NEW VALUES OF B AND C SUCH THAT THE INTERVAL(B,C) IS */
/*  SHRUNK AND F(B)*F(C).LE.0.0.  THE STOPPING CRITERION IS */

/*          DABS(B-C).LE.2.0*(RELERR*DABS(B)+ABSERR) */

/*  WHERE RELERR=RELATIVE ERROR AND ABSERR=ABSOLUTE ERROR ARE */
/*  INPUT QUANTITIES.  SET THE FLAG, IFLAG, POSITIVE TO INITIALIZE */
/*  THE COMPUTATION.  AS B,C AND IFLAG ARE USED FOR BOTH INPUT AND */
/*  OUTPUT, THEY MUST BE VARIABLES IN THE CALLING PROGRAM. */

/*  IF 0 IS A POSSIBLE ROOT, ONE SHOULD NOT CHOOSE ABSERR=0.0. */

/*  THE OUTPUT VALUE OF B IS THE BETTER APPROXIMATION TO A ROOT */
/*  AS B AND C ARE ALWAYS REDEFINED SO THAT DABS(F(B)).LE.DABS(F(C)). */

/*  TO SOLVE THE EQUATION, ROOT MUST EVALUATE F(X) REPEATEDLY. THIS */
/*  IS DONE IN THE CALLING PROGRAM.  WHEN AN EVALUATION OF F IS */
/*  NEEDED AT T, ROOT RETURNS WITH IFLAG NEGATIVE.  EVALUATE FT=F(T) */
/*  AND CALL ROOT AGAIN.  DO NOT ALTER IFLAG. */

/*  WHEN THE COMPUTATION IS COMPLETE, ROOT RETURNS TO THE CALLING */
/*  PROGRAM WITH IFLAG POSITIVE= */

/*     IFLAG=1  IF F(B)*F(C).LT.0 AND THE STOPPING CRITERION IS MET. */

/*          =2  IF A VALUE B IS FOUND SUCH THAT THE COMPUTED VALUE */
/*              F(B) IS EXACTLY ZERO.  THE INTERVAL (B,C) MAY NOT */
/*              SATISFY THE STOPPING CRITERION. */

/*          =3  IF DABS(F(B)) EXCEEDS THE INPUT VALUES DABS(F(B)), */
/*              DABS(F(C)).  IN THIS CASE IT IS LIKELY THAT B IS CLOSE */
/*              TO A POLE OF F. */

/*          =4  IF NO ODD ORDER ROOT WAS FOUND IN THE INTERVAL.  A */
/*              LOCAL MINIMUM MAY HAVE BEEN OBTAINED. */

/*          =5  IF TOO MANY FUNCTION EVALUATIONS WERE MADE. */
/*              (AS PROGRAMMED, 500 ARE ALLOWED.) */

/*  THIS CODE IS A MODIFICATION OF THE CODE ZEROIN WHICH IS COMPLETELY */
/*  EXPLAINED AND DOCUMENTED IN THE TEXT  NUMERICAL COMPUTING:  AN */
/*  INTRODUCTION,  BY L. F. SHAMPINE AND R. C. ALLEN. */

/*  CALLS  D1MACH . */


    if (*iflag >= 0) {
	goto L100;
    }
    *iflag = abs(*iflag);
    switch (*iflag) {
	case 1:  goto L200;
	case 2:  goto L300;
	case 3:  goto L400;
    }
L100:
    u = d1mach_(&c__4);
    re = max(*relerr,u);
    ae = max(*abserr,0.);
    ic = 0;
    acbs = (d__1 = *b - *c__, abs(d__1));
    a = *c__;
    *t = a;
    *iflag = -1;
    return 0;
L200:
    fa = *ft;
    *t = *b;
    *iflag = -2;
    return 0;
L300:
    fb = *ft;
    fc = fa;
    kount = 2;
/* Computing MAX */
    d__1 = abs(fb), d__2 = abs(fc);
    fx = max(d__1,d__2);
L1:
    if (abs(fc) >= abs(fb)) {
	goto L2;
    }

/*  INTERCHANGE B AND C SO THAT ABS(F(B)).LE.ABS(F(C)). */

    a = *b;
    fa = fb;
    *b = *c__;
    fb = fc;
    *c__ = a;
    fc = fa;
L2:
    cmb = (*c__ - *b) * .5f;
    acmb = abs(cmb);
    tol = re * abs(*b) + ae;

/*  TEST STOPPING CRITERION AND FUNCTION COUNT. */

    if (acmb <= tol) {
	goto L8;
    }
    if (kount >= 500) {
	goto L12;
    }

/*  CALCULATE NEW ITERATE EXPLICITLY AS B+P/Q */
/*  WHERE WE ARRANGE P.GE.0.  THE IMPLICIT */
/*  FORM IS USED TO PREVENT OVERFLOW. */

    p = (*b - a) * fb;
    q = fa - fb;
    if (p >= 0.f) {
	goto L3;
    }
    p = -p;
    q = -q;

/*  UPDATE A, CHECK IF REDUCTION IN THE SIZE OF BRACKETING */
/*  INTERVAL IS SATISFACTORY.  IF NOT BISECT UNTIL IT IS. */

L3:
    a = *b;
    fa = fb;
    ++ic;
    if (ic < 4) {
	goto L4;
    }
    if (acmb * 8.f >= acbs) {
	goto L6;
    }
    ic = 0;
    acbs = acmb;

/*  TEST FOR TOO SMALL A CHANGE. */

L4:
    if (p > abs(q) * tol) {
	goto L5;
    }

/*  INCREMENT BY TOLERANCE */

    *b += d_sign(&tol, &cmb);
    goto L7;

/*  ROOT OUGHT TO BE BETWEEN B AND (C+B)/2 */

L5:
    if (p >= cmb * q) {
	goto L6;
    }

/*  USE SECANT RULE. */

    *b += p / q;
    goto L7;

/*  USE BISECTION. */

L6:
    *b = (*c__ + *b) * .5f;

/*  HAVE COMPLETED COMPUTATION FOR NEW ITERATE B. */

L7:
    *t = *b;
    *iflag = -3;
    return 0;
L400:
    fb = *ft;
    if (fb == 0.f) {
	goto L9;
    }
    ++kount;
    if (d_sign(&c_b74, &fb) != d_sign(&c_b74, &fc)) {
	goto L1;
    }
    *c__ = a;
    fc = fa;
    goto L1;

/* FINISHED.  SET IFLAG. */

L8:
    if (d_sign(&c_b74, &fb) == d_sign(&c_b74, &fc)) {
	goto L11;
    }
    if (abs(fb) > fx) {
	goto L10;
    }
    *iflag = 1;
    return 0;
L9:
    *iflag = 2;
    return 0;
L10:
    *iflag = 3;
    return 0;
L11:
    *iflag = 4;
    return 0;
L12:
    *iflag = 5;
    return 0;
} /* root_ */






/* Subroutine */ int sintrp_(doublereal *x, doublereal *y, doublereal *xout, 
	doublereal *yout, doublereal *ypout, integer *neqn, integer *kold, 
	doublereal *phi, integer *ivc, integer *iv, integer *kgi, doublereal *
	gi, doublereal *alpha, doublereal *g, doublereal *w, doublereal *xold,
	 doublereal *p)
{
    /* System generated locals */
    integer phi_dim1, phi_offset, i__1, i__2;

    /* Local variables */
    static doublereal c__[13], h__;
    static integer i__, j, l, m;
    static doublereal hi;
    static integer iq, jq, iw;
    static doublereal xi;
    static integer kp1, kp2;
    static doublereal gdi, alp, hmu, xiq, rmu, xim1, gdif, temp1, temp2, 
	    temp3, gamma, sigma, gtemp[13], wtemp[13];


/* ***BEGIN PROLOGUE  SINTRP */
/* ***DATE WRITTEN   740101   (YYMMDD) */
/* ***REVISION DATE  840201   (YYMMDD) */
/* ***CATEGORY NO.  D2A2 */
/* ***KEYWORDS  INITIAL VALUE ORDINARY DIFFERENTIAL EQUATIONS, */
/*             VARIABLE ORDER ADAMS METHODS, SMOOTH INTERPOLANT FOR */
/*             DEABM IN THE DEPAC PACKAGE */
/* ***AUTHOR  SHAMPINE, L.F.,  SNLA */
/*           GORDON, M.K. */
/*             MODIFIED BY H.A. WATTS */
/* ***PURPOSE  APPROXIMATES THE SOLUTION AT XOUT BY EVALUATING THE */
/*            POLYNOMIAL COMPUTED IN STEPS AT XOUT.  MUST BE USED IN */
/*            CONJUNCTION WITH STEPS. */
/* ***DESCRIPTION */

/*   WRITTEN BY L. F. SHAMPINE AND M. K. GORDON */

/*   ABSTRACT */


/*   THE METHODS IN SUBROUTINE  STEPS  APPROXIMATE THE SOLUTION NEAR  X */
/*   BY A POLYNOMIAL.  SUBROUTINE  SINTRP  APPROXIMATES THE SOLUTION AT */
/*   XOUT  BY EVALUATING THE POLYNOMIAL THERE.  INFORMATION DEFINING THIS */
/*   POLYNOMIAL IS PASSED FROM  STEPS  SO  SINTRP  CANNOT BE USED ALONE. */

/*   THIS CODE IS COMPLETELY EXPLAINED AND DOCUMENTED IN THE TEXT, */
/*   COMPUTER SOLUTION OF ORDINARY DIFFERENTIAL EQUATIONS, THE INITIAL */
/*   VALUE PROBLEM  BY L. F. SHAMPINE AND M. K. GORDON. */
/*   FURTHER DETAILS ON USE OF THIS CODE ARE AVAILABLE IN *SOLVING */
/*   ORDINARY DIFFERENTIAL EQUATIONS WITH ODE, STEP, AND INTRP*, */
/*   BY L. F. SHAMPINE AND M. K. GORDON, SLA-73-1060. */

/*   INPUT TO SINTRP -- */

/*   THE USER PROVIDES STORAGE IN THE CALLING PROGRAM FOR THE ARRAYS IN */
/*   THE CALL LIST */
/*      DIMENSION Y(NEQN),YOUT(NEQN),YPOUT(NEQN),PHI(NEQN,16),P(NEQN), */
/*                ALPHA(12),G(13),W(12),GI(11),IV(10) */
/*   AND DEFINES */
/*      XOUT -- POINT AT WHICH SOLUTION IS DESIRED. */
/*   THE REMAINING PARAMETERS ARE DEFINED IN  STEPS  AND PASSED TO */
/*   SINTRP  FROM THAT SUBROUTINE. */

/*   OUTPUT FROM  SINTRP -- */

/*      YOUT(*) -- SOLUTION AT  XOUT */
/*      YPOUT(*) -- DERIVATIVE OF SOLUTION AT  XOUT */

/*   THE REMAINING PARAMETERS ARE RETURNED UNALTERED FROM THEIR INPUT */
/*   VALUES.  INTEGRATION WITH  STEPS  MAY BE CONTINUED. */

/* ***REFERENCES  SHAMPINE L.F., GORDON M.K., *SOLVING ORDINARY */
/*                 DIFFERENTIAL EQUATIONS WITH ODE, STEP, AND INTRP*, */
/*                 SLA-73-1060, SANDIA LABORATORIES, 1973. */
/*               WATTS H.A., SHAMPINE L.F., *A SMOOTHER INTERPOLANT FOR */
/*                 DE/STEP,INTRP : II*, SAND84-0293, SANDIA LABORATORIES, */
/*                 1984. */
/* ***ROUTINES CALLED  (NONE) */
/* ***END PROLOGUE  SINTRP */



/* ***FIRST EXECUTABLE STATEMENT */
    /* Parameter adjustments */
    --p;
    phi_dim1 = *neqn;
    phi_offset = 1 + phi_dim1;
    phi -= phi_offset;
    --ypout;
    --yout;
    --y;
    --iv;
    --gi;
    --alpha;
    --g;
    --w;

    /* Function Body */
    kp1 = *kold + 1;
    kp2 = *kold + 2;

    hi = *xout - *xold;
    h__ = *x - *xold;
    xi = hi / h__;
    xim1 = xi - 1.f;

/*   INITIALIZE WTEMP(*) FOR COMPUTING GTEMP(*) */

    xiq = xi;
    i__1 = kp1;
    for (iq = 1; iq <= i__1; ++iq) {
	xiq = xi * xiq;
	temp1 = (doublereal) (iq * (iq + 1));
/* L10: */
	wtemp[iq - 1] = xiq / temp1;
    }

/*   COMPUTE THE DOUBLE INTEGRAL TERM GDI */

    if (*kold <= *kgi) {
	goto L50;
    }
    if (*ivc > 0) {
	goto L20;
    }
    gdi = 1.f / temp1;
    m = 2;
    goto L30;
L20:
    iw = iv[*ivc];
    gdi = w[iw];
    m = *kold - iw + 3;
L30:
    if (m > *kold) {
	goto L60;
    }
    i__1 = *kold;
    for (i__ = m; i__ <= i__1; ++i__) {
/* L40: */
	gdi = w[kp2 - i__] - alpha[i__] * gdi;
    }
    goto L60;
L50:
    gdi = gi[*kold];

/*   COMPUTE GTEMP(*) AND C(*) */

L60:
    gtemp[0] = xi;
    gtemp[1] = xi * .5f * xi;
    c__[0] = 1.f;
    c__[1] = xi;
    if (*kold < 2) {
	goto L90;
    }
    i__1 = *kold;
    for (i__ = 2; i__ <= i__1; ++i__) {
	alp = alpha[i__];
	gamma = xim1 * alp + 1.f;
	l = kp2 - i__;
	i__2 = l;
	for (jq = 1; jq <= i__2; ++jq) {
/* L70: */
	    wtemp[jq - 1] = gamma * wtemp[jq - 1] - alp * wtemp[jq];
	}
	gtemp[i__] = wtemp[0];
/* L80: */
	c__[i__] = gamma * c__[i__ - 1];
    }

/*   DEFINE INTERPOLATION PARAMETERS */

L90:
    sigma = (wtemp[1] - xim1 * wtemp[0]) / gdi;
    rmu = xim1 * c__[kp1 - 1] / gdi;
    hmu = rmu / h__;

/*   INTERPOLATE FOR THE SOLUTION -- YOUT */
/*   AND FOR THE DERIVATIVE OF THE SOLUTION -- YPOUT */

    i__1 = *neqn;
    for (l = 1; l <= i__1; ++l) {
	yout[l] = 0.f;
/* L100: */
	ypout[l] = 0.f;
    }
    i__1 = *kold;
    for (j = 1; j <= i__1; ++j) {
	i__ = kp2 - j;
	gdif = g[i__] - g[i__ - 1];
	temp2 = gtemp[i__ - 1] - gtemp[i__ - 2] - sigma * gdif;
	temp3 = c__[i__ - 1] - c__[i__ - 2] + rmu * gdif;
	i__2 = *neqn;
	for (l = 1; l <= i__2; ++l) {
	    yout[l] += temp2 * phi[l + i__ * phi_dim1];
/* L110: */
	    ypout[l] += temp3 * phi[l + i__ * phi_dim1];
	}
/* L120: */
    }
    i__1 = *neqn;
    for (l = 1; l <= i__1; ++l) {
	yout[l] = (1.f - sigma) * p[l] + sigma * y[l] + h__ * (yout[l] + (
		gtemp[0] - sigma * g[1]) * phi[l + phi_dim1]);
/* L130: */
	ypout[l] = hmu * (p[l] - y[l]) + (ypout[l] + (c__[0] + rmu * g[1]) * 
		phi[l + phi_dim1]);
    }

    return 0;
} /* sintrp_ */






/* Subroutine */ int steps_(S_fp f, integer *neqn, doublereal *y, doublereal *
	x, doublereal *h__, doublereal *eps, doublereal *wt, logical *start, 
	doublereal *hold, integer *k, integer *kold, logical *crash, 
	doublereal *phi, doublereal *p, doublereal *yp, doublereal *alpha, 
	doublereal *w, doublereal *g, integer *ksteps, doublereal *xold, 
	integer *ivc, integer *iv, integer *kgi, doublereal *gi, doublereal *
	fpwa1, doublereal *fpwa2, doublereal *fpwa3, doublereal *fpwa4, 
	doublereal *fpwa5, integer *ifpwa1, integer *ifpc1, integer *ifpc2, 
	void * param)
{
    /* Initialized data */

    static doublereal two[13] = { 2.,4.,8.,16.,32.,64.,128.,256.,512.,1024.,
	    2048.,4096.,8192. };
    static doublereal gstr[13] = { .5,.0833,.0417,.0264,.0188,.0143,.0114,
	    .00936,.00789,.00679,.00592,.00524,.00468 };

    /* System generated locals */
    integer phi_dim1, phi_offset, fpwa3_dim1, fpwa3_offset, i__1, i__2;
    doublereal d__1, d__2, d__3;

    /* Builtin functions */
    double d_sign(doublereal *, doublereal *), sqrt(doublereal), pow_dd(
	    doublereal *, doublereal *);

    /* Local variables */
    static integer i__, j, l;
    static doublereal r__, v[12];
    static integer iq, jv, ns, im1, km1, ip1, km2, kp1, kp2;
    static doublereal erk, sig[13], err, rho, tau, psi[12], sum;
    static integer nsm2, nsp1, nsp2;
    static doublereal beta[12], absh, hnew;
    static integer knew;
    static doublereal twou, erkm1, erkm2, erkp1, temp1, temp2, temp3, temp4, 
	    temp5, temp6, p5eps;
    static integer ifail;
    static doublereal reali;
    static logical nornd;
    static doublereal round;
    static integer kprev;
    extern doublereal d1mach_(integer *);
    static doublereal fouru;
    static logical phase1;
    static integer limit1, limit2;
    static doublereal realns;




/*   WRITTEN BY L. F. SHAMPINE AND M. K. GORDON */

/*   ABSTRACT */

/*   SUBROUTINE  STEPS  IS NORMALLY USED INDIRECTLY THROUGH SUBROUTINE */
/*   DEABM .  BECAUSE  DEABM  SUFFICES FOR MOST PROBLEMS AND IS MUCH */
/*   EASIER TO USE, USING IT SHOULD BE CONSIDERED BEFORE USING  STEPS */
/*   ALONE. */

/*   SUBROUTINE STEPS INTEGRATES A SYSTEM OF  NEQN  FIRST ORDER ORDINARY */
/*   DIFFERENTIAL EQUATIONS ONE STEP, NORMALLY FROM X TO X+H, USING A */
/*   MODIFIED DIVIDED DIFFERENCE FORM OF THE ADAMS PECE FORMULAS.  LOCAL */
/*   EXTRAPOLATION IS USED TO IMPROVE ABSOLUTE STABILITY AND ACCURACY. */
/*   THE CODE ADJUSTS ITS ORDER AND STEP SIZE TO CONTROL THE LOCAL ERROR */
/*   PER UNIT STEP IN A GENERALIZED SENSE.  SPECIAL DEVICES ARE INCLUDED */
/*   TO CONTROL ROUNDOFF ERROR AND TO DETECT WHEN THE USER IS REQUESTING */
/*   TOO MUCH ACCURACY. */

/*   THIS CODE IS COMPLETELY EXPLAINED AND DOCUMENTED IN THE TEXT, */
/*   COMPUTER SOLUTION OF ORDINARY DIFFERENTIAL EQUATIONS, THE INITIAL */
/*   VALUE PROBLEM  BY L. F. SHAMPINE AND M. K. GORDON. */
/*   FURTHER DETAILS ON USE OF THIS CODE ARE AVAILABLE IN *SOLVING */
/*   ORDINARY DIFFERENTIAL EQUATIONS WITH ODE, STEP, AND INTRP*, */
/*   BY L. F. SHAMPINE AND M. K. GORDON, SLA-73-1060. */


/*   THE PARAMETERS REPRESENT -- */
/*      F -- SUBROUTINE TO EVALUATE DERIVATIVES */
/*      NEQN -- NUMBER OF EQUATIONS TO BE INTEGRATED */
/*      Y(*) -- SOLUTION VECTOR AT X */
/*      X -- INDEPENDENT VARIABLE */
/*      H -- APPROPRIATE STEP SIZE FOR NEXT STEP.  NORMALLY DETERMINED BY */
/*           CODE */
/*      EPS -- LOCAL ERROR TOLERANCE */
/*      WT(*) -- VECTOR OF WEIGHTS FOR ERROR CRITERION */
/*      START -- LOGICAL VARIABLE SET .TRUE. FOR FIRST STEP,  .FALSE. */
/*           OTHERWISE */
/*      HOLD -- STEP SIZE USED FOR LAST SUCCESSFUL STEP */
/*      K -- APPROPRIATE ORDER FOR NEXT STEP (DETERMINED BY CODE) */
/*      KOLD -- ORDER USED FOR LAST SUCCESSFUL STEP */
/*      CRASH -- LOGICAL VARIABLE SET .TRUE. WHEN NO STEP CAN BE TAKEN, */
/*           .FALSE. OTHERWISE. */
/*      YP(*) -- DERIVATIVE OF SOLUTION VECTOR AT  X  AFTER SUCCESSFUL */
/*           STEP */
/*      KSTEPS -- COUNTER ON ATTEMPTED STEPS */

/*   THE VARIABLES X,XOLD,KOLD,KGI AND IVC AND THE ARRAYS Y,PHI,ALPHA,G, */
/*   W,P,IV AND GI ARE REQUIRED FOR THE INTERPOLATION SUBROUTINE SINTRP. */
/*   THE ARRAYS FPWA* AND IFPWA1 AND INTEGER CONSTANTS IFPC* ARE */
/*   WORKING STORAGE PASSED DIRECTLY THROUGH TO  FODE.  THE ARRAYS */
/*   PAR AND IPAR ARE USER PARAMETERS PASSED THROUGH TO RHOA AND RHOJAC. */

/*   INPUT TO STEPS */

/*      FIRST CALL -- */

/*   THE USER MUST PROVIDE STORAGE IN HIS CALLING PROGRAM FOR ALL ARRAYS */
/*   IN THE CALL LIST, NAMELY */

/*     DIMENSION Y(NEQN),WT(NEQN),PHI(NEQN,16),P(NEQN),YP(NEQN), */
/*    1  ALPHA(12),W(12),G(13),GI(11),IV(10),   FPWA1(NEQN), */
/*    2  FPWA2(NEQN-1),FPWA3(NEQN-1,NEQN),FPWA4(NEQN-1), */
/*    3  FPWA5(NEQN),IFPWA1(NEQN) */
/*                              --                --    **NOTE** */

/*   THE USER MUST ALSO DECLARE  START  AND  CRASH */
/*   LOGICAL VARIABLES AND  F  AN EXTERNAL SUBROUTINE, SUPPLY THE */
/*   SUBROUTINE  F(X,Y,YP,FPWA1,FPWA2,FPWA3,FPWA4,FPWA5,IFPWA1,IFPC1, */
/*                 NEQN-1,IFPC2,PAR,IPAR) TO EVALUATE */
/*      DY(I)/DX = YP(I) = F(X,Y(1),Y(2),...,Y(NEQN)) */
/*   AND INITIALIZE ONLY THE FOLLOWING PARAMETERS. */
/*      NEQN -- NUMBER OF EQUATIONS TO BE INTEGRATED */
/*      Y(*) -- VECTOR OF INITIAL VALUES OF DEPENDENT VARIABLES */
/*      X -- INITIAL VALUE OF THE INDEPENDENT VARIABLE */
/*      H -- NOMINAL STEP SIZE INDICATING DIRECTION OF INTEGRATION */
/*           AND MAXIMUM SIZE OF STEP.  MUST BE VARIABLE */
/*      EPS -- LOCAL ERROR TOLERANCE PER STEP.  MUST BE VARIABLE */
/*      WT(*) -- VECTOR OF NON-ZERO WEIGHTS FOR ERROR CRITERION */
/*      START -- .TRUE. */
/*      KSTEPS -- SET KSTEPS TO ZERO */
/*   DEFINE U TO BE THE MACHINE UNIT ROUNDOFF QUANTITY BY CALLING */
/*   THE FUNCTION ROUTINE  D1MACH,  U = D1MACH(3), OR BY */
/*   COMPUTING U SO THAT U IS THE SMALLEST POSITIVE NUMBER SUCH */
/*   THAT 1.0+U .GT. 1.0. */

/*   STEPS  REQUIRES THAT THE L2 NORM OF THE VECTOR WITH COMPONENTS */
/*   LOCAL ERROR(L)/WT(L)  BE LESS THAN  EPS  FOR A SUCCESSFUL STEP.  THE */
/*   ARRAY  WT  ALLOWS THE USER TO SPECIFY AN ERROR TEST APPROPRIATE */
/*   FOR HIS PROBLEM.  FOR EXAMPLE, */
/*      WT(L) = 1.0  SPECIFIES ABSOLUTE ERROR, */
/*            = ABS(Y(L))  ERROR RELATIVE TO THE MOST RECENT VALUE OF THE */
/*                 L-TH COMPONENT OF THE SOLUTION, */
/*            = ABS(YP(L))  ERROR RELATIVE TO THE MOST RECENT VALUE OF */
/*                 THE L-TH COMPONENT OF THE DERIVATIVE, */
/*            = MAX(WT(L),ABS(Y(L)))  ERROR RELATIVE TO THE LARGEST */
/*                 MAGNITUDE OF L-TH COMPONENT OBTAINED SO FAR, */
/*            = ABS(Y(L))*RELERR/EPS + ABSERR/EPS  SPECIFIES A MIXED */
/*                 RELATIVE-ABSOLUTE TEST WHERE  RELERR  IS RELATIVE */
/*                 ERROR,  ABSERR  IS ABSOLUTE ERROR AND  EPS = */
/*                 MAX(RELERR,ABSERR) . */

/*      SUBSEQUENT CALLS -- */

/*   SUBROUTINE  STEPS  IS DESIGNED SO THAT ALL INFORMATION NEEDED TO */
/*   CONTINUE THE INTEGRATION, INCLUDING THE STEP SIZE  H  AND THE ORDER */
/*   K , IS RETURNED WITH EACH STEP.  WITH THE EXCEPTION OF THE STEP */
/*   SIZE, THE ERROR TOLERANCE, AND THE WEIGHTS, NONE OF THE PARAMETERS */
/*   SHOULD BE ALTERED.  THE ARRAY  WT  MUST BE UPDATED AFTER EACH STEP */
/*   TO MAINTAIN RELATIVE ERROR TESTS LIKE THOSE ABOVE.  NORMALLY THE */
/*   INTEGRATION IS CONTINUED JUST BEYOND THE DESIRED ENDPOINT AND THE */
/*   SOLUTION INTERPOLATED THERE WITH SUBROUTINE  SINTRP .  IF IT IS */
/*   IMPOSSIBLE TO INTEGRATE BEYOND THE ENDPOINT, THE STEP SIZE MAY BE */
/*   REDUCED TO HIT THE ENDPOINT SINCE THE CODE WILL NOT TAKE A STEP */
/*   LARGER THAN THE  H  INPUT.  CHANGING THE DIRECTION OF INTEGRATION, */
/*   I.E., THE SIGN OF  H , REQUIRES THE USER SET  START = .TRUE. BEFORE */
/*   CALLING  STEPS  AGAIN.  THIS IS THE ONLY SITUATION IN WHICH  START */
/*   SHOULD BE ALTERED. */

/*   OUTPUT FROM STEPS */

/*      SUCCESSFUL STEP -- */

/*   THE SUBROUTINE RETURNS AFTER EACH SUCCESSFUL STEP WITH  START  AND */
/*   CRASH  SET .FALSE. .  X  REPRESENTS THE INDEPENDENT VARIABLE */
/*   ADVANCED ONE STEP OF LENGTH  HOLD  FROM ITS VALUE ON INPUT AND  Y */
/*   THE SOLUTION VECTOR AT THE NEW VALUE OF  X .  ALL OTHER PARAMETERS */
/*   REPRESENT INFORMATION CORRESPONDING TO THE NEW  X  NEEDED TO */
/*   CONTINUE THE INTEGRATION. */

/*      UNSUCCESSFUL STEP -- */

/*   WHEN THE ERROR TOLERANCE IS TOO SMALL FOR THE MACHINE PRECISION, */
/*   THE SUBROUTINE RETURNS WITHOUT TAKING A STEP AND  CRASH = .TRUE. . */
/*   AN APPROPRIATE STEP SIZE AND ERROR TOLERANCE FOR CONTINUING ARE */
/*   ESTIMATED AND ALL OTHER INFORMATION IS RESTORED AS UPON INPUT */
/*   BEFORE RETURNING.  TO CONTINUE WITH THE LARGER TOLERANCE, THE USER */
/*   JUST CALLS THE CODE AGAIN.  A RESTART IS NEITHER REQUIRED NOR */
/*   DESIRABLE. */
/* ***REFERENCES  SHAMPINE L.F., GORDON M.K., *SOLVING ORDINARY */
/*                 DIFFERENTIAL EQUATIONS WITH ODE, STEP, AND INTRP*, */
/*                 SLA-73-1060, SANDIA LABORATORIES, 1973. */



/*   ALL LOCAL VARIABLES ARE SAVED, RATHER THAN PASSED, IN THIS */
/*   SPECIALIZED VERSION OF STEPS. */



    /* Parameter adjustments */
    --ifpwa1;
    --fpwa5;
    --fpwa4;
    fpwa3_dim1 = *neqn - 1;
    fpwa3_offset = 1 + fpwa3_dim1;
    fpwa3 -= fpwa3_offset;
    --fpwa2;
    --fpwa1;
    --yp;
    --p;
    phi_dim1 = *neqn;
    phi_offset = 1 + phi_dim1;
    phi -= phi_offset;
    --wt;
    --y;
    --alpha;
    --w;
    --g;
    --iv;
    --gi;

    /* Function Body */


/*       ***     BEGIN BLOCK 0     *** */
/*   CHECK IF STEP SIZE OR ERROR TOLERANCE IS TOO SMALL FOR MACHINE */
/*   PRECISION.  IF FIRST STEP, INITIALIZE PHI ARRAY AND ESTIMATE A */
/*   STARTING STEP SIZE. */
/*                   *** */

/*   IF STEP SIZE IS TOO SMALL, DETERMINE AN ACCEPTABLE ONE */

/* ***FIRST EXECUTABLE STATEMENT */
    twou = 2.f * d1mach_(&c__4);
    fouru = twou + twou;
    *crash = TRUE_;
    if (abs(*h__) >= fouru * abs(*x)) {
	goto L5;
    }
    d__1 = fouru * abs(*x);
    *h__ = d_sign(&d__1, h__);
    return 0;
L5:
    p5eps = *eps * .5f;

/*   IF ERROR TOLERANCE IS TOO SMALL, INCREASE IT TO AN ACCEPTABLE VALUE */

    round = 0.f;
    i__1 = *neqn;
    for (l = 1; l <= i__1; ++l) {
/* L10: */
/* Computing 2nd power */
	d__1 = y[l] / wt[l];
	round += d__1 * d__1;
    }
    round = twou * sqrt(round);
    if (p5eps >= round) {
	goto L15;
    }
    *eps = round * 2.f * (fouru + 1.f);
    return 0;
L15:
    *crash = FALSE_;
    g[1] = 1.f;
    g[2] = .5f;
    sig[0] = 1.f;
    if (! (*start)) {
	goto L99;
    }

/*   INITIALIZE.  COMPUTE APPROPRIATE STEP SIZE FOR FIRST STEP */

    i__1 = *neqn - 1;
    (*f)(x, &y[1], &yp[1], &fpwa1[1], &fpwa2[1], &fpwa3[fpwa3_offset], &fpwa4[
	    1], &fpwa5[1], &ifpwa1[1], ifpc1, &i__1, ifpc2, param)
	    ;
    if (*ifpc2 > 0) {
	return 0;
    }
    sum = 0.f;
    i__1 = *neqn;
    for (l = 1; l <= i__1; ++l) {
	phi[l + phi_dim1] = yp[l];
	phi[l + (phi_dim1 << 1)] = 0.f;
/* L20: */
/* Computing 2nd power */
	d__1 = yp[l] / wt[l];
	sum += d__1 * d__1;
    }
    sum = sqrt(sum);
    absh = abs(*h__);
    if (*eps < sum * 16.f * *h__ * *h__) {
	absh = sqrt(*eps / sum) * .25f;
    }
/* Computing MAX */
    d__2 = absh, d__3 = fouru * abs(*x);
    d__1 = max(d__2,d__3);
    *h__ = d_sign(&d__1, h__);

/* *      U = D1MACH(3) */
/* *      BIG = SQRT(D1MACH(2)) */
/* *      CALL HSTART (F,NEQN,X,X+H,Y,YP,WT,1,U,BIG, */
/* *     1             PHI(1,3),PHI(1,4),PHI(1,5),PHI(1,6),RPAR,IPAR,H) */

    *hold = 0.f;
    *k = 1;
    *kold = 0;
    kprev = 0;
    *start = FALSE_;
    phase1 = TRUE_;
    nornd = TRUE_;
    if (p5eps > round * 100.f) {
	goto L99;
    }
    nornd = FALSE_;
    i__1 = *neqn;
    for (l = 1; l <= i__1; ++l) {
/* L25: */
	phi[l + phi_dim1 * 15] = 0.f;
    }
L99:
    ifail = 0;
/*       ***     END BLOCK 0     *** */

/*       ***     BEGIN BLOCK 1     *** */
/*   COMPUTE COEFFICIENTS OF FORMULAS FOR THIS STEP.  AVOID COMPUTING */
/*   THOSE QUANTITIES NOT CHANGED WHEN STEP SIZE IS NOT CHANGED. */
/*                   *** */

L100:
    kp1 = *k + 1;
    kp2 = *k + 2;
    km1 = *k - 1;
    km2 = *k - 2;

/*   NS IS THE NUMBER OF STEPS TAKEN WITH SIZE H, INCLUDING THE CURRENT */
/*   ONE.  WHEN K.LT.NS, NO COEFFICIENTS CHANGE */

    if (*h__ != *hold) {
	ns = 0;
    }
    if (ns <= *kold) {
	++ns;
    }
    nsp1 = ns + 1;
    if (*k < ns) {
	goto L199;
    }

/*   COMPUTE THOSE COMPONENTS OF ALPHA(*),BETA(*),PSI(*),SIG(*) WHICH */
/*   ARE CHANGED */

    beta[ns - 1] = 1.f;
    realns = (doublereal) ns;
    alpha[ns] = 1.f / realns;
    temp1 = *h__ * realns;
    sig[nsp1 - 1] = 1.f;
    if (*k < nsp1) {
	goto L110;
    }
    i__1 = *k;
    for (i__ = nsp1; i__ <= i__1; ++i__) {
	im1 = i__ - 1;
	temp2 = psi[im1 - 1];
	psi[im1 - 1] = temp1;
	beta[i__ - 1] = beta[im1 - 1] * psi[im1 - 1] / temp2;
	temp1 = temp2 + *h__;
	alpha[i__] = *h__ / temp1;
	reali = (doublereal) i__;
/* L105: */
	sig[i__] = reali * alpha[i__] * sig[i__ - 1];
    }
L110:
    psi[*k - 1] = temp1;

/*   COMPUTE COEFFICIENTS G(*) */

/*   INITIALIZE V(*) AND SET W(*). */

    if (ns > 1) {
	goto L120;
    }
    i__1 = *k;
    for (iq = 1; iq <= i__1; ++iq) {
	temp3 = (doublereal) (iq * (iq + 1));
	v[iq - 1] = 1.f / temp3;
/* L115: */
	w[iq] = v[iq - 1];
    }
    *ivc = 0;
    *kgi = 0;
    if (*k == 1) {
	goto L140;
    }
    *kgi = 1;
    gi[1] = w[2];
    goto L140;

/*   IF ORDER WAS RAISED, UPDATE DIAGONAL PART OF V(*) */

L120:
    if (*k <= kprev) {
	goto L130;
    }
    if (*ivc == 0) {
	goto L122;
    }
    jv = kp1 - iv[*ivc];
    --(*ivc);
    goto L123;
L122:
    jv = 1;
    temp4 = (doublereal) (*k * kp1);
    v[*k - 1] = 1.f / temp4;
    w[*k] = v[*k - 1];
    if (*k != 2) {
	goto L123;
    }
    *kgi = 1;
    gi[1] = w[2];
L123:
    nsm2 = ns - 2;
    if (nsm2 < jv) {
	goto L130;
    }
    i__1 = nsm2;
    for (j = jv; j <= i__1; ++j) {
	i__ = *k - j;
	v[i__ - 1] -= alpha[j + 1] * v[i__];
/* L125: */
	w[i__] = v[i__ - 1];
    }
    if (i__ != 2) {
	goto L130;
    }
    *kgi = ns - 1;
    gi[*kgi] = w[2];

/*   UPDATE V(*) AND SET W(*) */

L130:
    limit1 = kp1 - ns;
    temp5 = alpha[ns];
    i__1 = limit1;
    for (iq = 1; iq <= i__1; ++iq) {
	v[iq - 1] -= temp5 * v[iq];
/* L135: */
	w[iq] = v[iq - 1];
    }
    g[nsp1] = w[1];
    if (limit1 == 1) {
	goto L137;
    }
    *kgi = ns;
    gi[*kgi] = w[2];
L137:
    w[limit1 + 1] = v[limit1];
    if (*k >= *kold) {
	goto L140;
    }
    ++(*ivc);
    iv[*ivc] = limit1 + 2;

/*   COMPUTE THE G(*) IN THE WORK VECTOR W(*) */

L140:
    nsp2 = ns + 2;
    kprev = *k;
    if (kp1 < nsp2) {
	goto L199;
    }
    i__1 = kp1;
    for (i__ = nsp2; i__ <= i__1; ++i__) {
	limit2 = kp2 - i__;
	temp6 = alpha[i__ - 1];
	i__2 = limit2;
	for (iq = 1; iq <= i__2; ++iq) {
/* L145: */
	    w[iq] -= temp6 * w[iq + 1];
	}
/* L150: */
	g[i__] = w[1];
    }
L199:
/*       ***     END BLOCK 1     *** */

/*       ***     BEGIN BLOCK 2     *** */
/*   PREDICT A SOLUTION P(*), EVALUATE DERIVATIVES USING PREDICTED */
/*   SOLUTION, ESTIMATE LOCAL ERROR AT ORDER K AND ERRORS AT ORDERS K, */
/*   K-1, K-2 AS IF CONSTANT STEP SIZE WERE USED. */
/*                   *** */

/*   INCREMENT COUNTER ON ATTEMPTED STEPS */

    ++(*ksteps);

/*   CHANGE PHI TO PHI STAR */

    if (*k < nsp1) {
	goto L215;
    }
    i__1 = *k;
    for (i__ = nsp1; i__ <= i__1; ++i__) {
	temp1 = beta[i__ - 1];
	i__2 = *neqn;
	for (l = 1; l <= i__2; ++l) {
/* L205: */
	    phi[l + i__ * phi_dim1] = temp1 * phi[l + i__ * phi_dim1];
	}
/* L210: */
    }

/*   PREDICT SOLUTION AND DIFFERENCES */

L215:
    i__1 = *neqn;
    for (l = 1; l <= i__1; ++l) {
	phi[l + kp2 * phi_dim1] = phi[l + kp1 * phi_dim1];
	phi[l + kp1 * phi_dim1] = 0.f;
/* L220: */
	p[l] = 0.f;
    }
    i__1 = *k;
    for (j = 1; j <= i__1; ++j) {
	i__ = kp1 - j;
	ip1 = i__ + 1;
	temp2 = g[i__];
	i__2 = *neqn;
	for (l = 1; l <= i__2; ++l) {
	    p[l] += temp2 * phi[l + i__ * phi_dim1];
/* L225: */
	    phi[l + i__ * phi_dim1] += phi[l + ip1 * phi_dim1];
	}
/* L230: */
    }
    if (nornd) {
	goto L240;
    }
    i__1 = *neqn;
    for (l = 1; l <= i__1; ++l) {
	tau = *h__ * p[l] - phi[l + phi_dim1 * 15];
	p[l] = y[l] + tau;
/* L235: */
	phi[l + (phi_dim1 << 4)] = p[l] - y[l] - tau;
    }
    goto L250;
L240:
    i__1 = *neqn;
    for (l = 1; l <= i__1; ++l) {
/* L245: */
	p[l] = y[l] + *h__ * p[l];
    }
L250:
    *xold = *x;
    *x += *h__;
    absh = abs(*h__);
    i__1 = *neqn - 1;
    (*f)(x, &p[1], &yp[1], &fpwa1[1], &fpwa2[1], &fpwa3[fpwa3_offset], &fpwa4[
	    1], &fpwa5[1], &ifpwa1[1], ifpc1, &i__1, ifpc2, param)
	    ;
    if (*ifpc2 > 0) {
	return 0;
    }

/*   ESTIMATE ERRORS AT ORDERS K,K-1,K-2 */

    erkm2 = 0.f;
    erkm1 = 0.f;
    erk = 0.f;
    i__1 = *neqn;
    for (l = 1; l <= i__1; ++l) {
	temp3 = 1.f / wt[l];
	temp4 = yp[l] - phi[l + phi_dim1];
	if (km2 < 0) {
	    goto L265;
	} else if (km2 == 0) {
	    goto L260;
	} else {
	    goto L255;
	}
L255:
/* Computing 2nd power */
	d__1 = (phi[l + km1 * phi_dim1] + temp4) * temp3;
	erkm2 += d__1 * d__1;
L260:
/* Computing 2nd power */
	d__1 = (phi[l + *k * phi_dim1] + temp4) * temp3;
	erkm1 += d__1 * d__1;
L265:
/* Computing 2nd power */
	d__1 = temp4 * temp3;
	erk += d__1 * d__1;
    }
    if (km2 < 0) {
	goto L280;
    } else if (km2 == 0) {
	goto L275;
    } else {
	goto L270;
    }
L270:
    erkm2 = absh * sig[km1 - 1] * gstr[km2 - 1] * sqrt(erkm2);
L275:
    erkm1 = absh * sig[*k - 1] * gstr[km1 - 1] * sqrt(erkm1);
L280:
    temp5 = absh * sqrt(erk);
    err = temp5 * (g[*k] - g[kp1]);
    erk = temp5 * sig[kp1 - 1] * gstr[*k - 1];
    knew = *k;

/*   TEST IF ORDER SHOULD BE LOWERED */

    if (km2 < 0) {
	goto L299;
    } else if (km2 == 0) {
	goto L290;
    } else {
	goto L285;
    }
L285:
    if (max(erkm1,erkm2) <= erk) {
	knew = km1;
    }
    goto L299;
L290:
    if (erkm1 <= erk * .5f) {
	knew = km1;
    }

/*   TEST IF STEP SUCCESSFUL */

L299:
    if (err <= *eps) {
	goto L400;
    }
/*       ***     END BLOCK 2     *** */

/*       ***     BEGIN BLOCK 3     *** */
/*   THE STEP IS UNSUCCESSFUL.  RESTORE  X, PHI(*,*), PSI(*) . */
/*   IF THIRD CONSECUTIVE FAILURE, SET ORDER TO ONE.  IF STEP FAILS MORE */
/*   THAN THREE TIMES, CONSIDER AN OPTIMAL STEP SIZE.  DOUBLE ERROR */
/*   TOLERANCE AND RETURN IF ESTIMATED STEP SIZE IS TOO SMALL FOR MACHINE */
/*   PRECISION. */
/*                   *** */

/*   RESTORE X, PHI(*,*) AND PSI(*) */

    phase1 = FALSE_;
    *x = *xold;
    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	temp1 = 1.f / beta[i__ - 1];
	ip1 = i__ + 1;
	i__2 = *neqn;
	for (l = 1; l <= i__2; ++l) {
/* L305: */
	    phi[l + i__ * phi_dim1] = temp1 * (phi[l + i__ * phi_dim1] - phi[
		    l + ip1 * phi_dim1]);
	}
/* L310: */
    }
    if (*k < 2) {
	goto L320;
    }
    i__1 = *k;
    for (i__ = 2; i__ <= i__1; ++i__) {
/* L315: */
	psi[i__ - 2] = psi[i__ - 1] - *h__;
    }

/*   ON THIRD FAILURE, SET ORDER TO ONE.  THEREAFTER, USE OPTIMAL STEP */
/*   SIZE */

L320:
    ++ifail;
    temp2 = .5f;
    if ((i__1 = ifail - 3) < 0) {
	goto L335;
    } else if (i__1 == 0) {
	goto L330;
    } else {
	goto L325;
    }
L325:
    if (p5eps < erk * .25f) {
	temp2 = sqrt(p5eps / erk);
    }
L330:
    knew = 1;
L335:
    *h__ = temp2 * *h__;
    *k = knew;
    ns = 0;
    if (abs(*h__) >= fouru * abs(*x)) {
	goto L340;
    }
    *crash = TRUE_;
    d__1 = fouru * abs(*x);
    *h__ = d_sign(&d__1, h__);
    *eps += *eps;
    return 0;
L340:
    goto L100;
/*       ***     END BLOCK 3     *** */

/*       ***     BEGIN BLOCK 4     *** */
/*   THE STEP IS SUCCESSFUL.  CORRECT THE PREDICTED SOLUTION, EVALUATE */
/*   THE DERIVATIVES USING THE CORRECTED SOLUTION AND UPDATE THE */
/*   DIFFERENCES.  DETERMINE BEST ORDER AND STEP SIZE FOR NEXT STEP. */
/*                   *** */
L400:
    *kold = *k;
    *hold = *h__;

/*   CORRECT AND EVALUATE */

    temp1 = *h__ * g[kp1];
    if (nornd) {
	goto L410;
    }
    i__1 = *neqn;
    for (l = 1; l <= i__1; ++l) {
	temp3 = y[l];
	rho = temp1 * (yp[l] - phi[l + phi_dim1]) - phi[l + (phi_dim1 << 4)];
	y[l] = p[l] + rho;
	phi[l + phi_dim1 * 15] = y[l] - p[l] - rho;
/* L405: */
	p[l] = temp3;
    }
    goto L420;
L410:
    i__1 = *neqn;
    for (l = 1; l <= i__1; ++l) {
	temp3 = y[l];
	y[l] = p[l] + temp1 * (yp[l] - phi[l + phi_dim1]);
/* L415: */
	p[l] = temp3;
    }
L420:
    i__1 = *neqn - 1;
    (*f)(x, &y[1], &yp[1], &fpwa1[1], &fpwa2[1], &fpwa3[fpwa3_offset], &fpwa4[
	    1], &fpwa5[1], &ifpwa1[1], ifpc1, &i__1, ifpc2, param)
	    ;
    if (*ifpc2 > 0) {
	return 0;
    }

/*   UPDATE DIFFERENCES FOR NEXT STEP */

    i__1 = *neqn;
    for (l = 1; l <= i__1; ++l) {
	phi[l + kp1 * phi_dim1] = yp[l] - phi[l + phi_dim1];
/* L425: */
	phi[l + kp2 * phi_dim1] = phi[l + kp1 * phi_dim1] - phi[l + kp2 * 
		phi_dim1];
    }
    i__1 = *k;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = *neqn;
	for (l = 1; l <= i__2; ++l) {
/* L430: */
	    phi[l + i__ * phi_dim1] += phi[l + kp1 * phi_dim1];
	}
/* L435: */
    }

/*   ESTIMATE ERROR AT ORDER K+1 UNLESS: */
/*     IN FIRST PHASE WHEN ALWAYS RAISE ORDER, */
/*     ALREADY DECIDED TO LOWER ORDER, */
/*     STEP SIZE NOT CONSTANT SO ESTIMATE UNRELIABLE */

    erkp1 = 0.f;
    if (knew == km1 || *k == 12) {
	phase1 = FALSE_;
    }
    if (phase1) {
	goto L450;
    }
    if (knew == km1) {
	goto L455;
    }
    if (kp1 > ns) {
	goto L460;
    }
    i__1 = *neqn;
    for (l = 1; l <= i__1; ++l) {
/* L440: */
/* Computing 2nd power */
	d__1 = phi[l + kp2 * phi_dim1] / wt[l];
	erkp1 += d__1 * d__1;
    }
    erkp1 = absh * gstr[kp1 - 1] * sqrt(erkp1);

/*   USING ESTIMATED ERROR AT ORDER K+1, DETERMINE APPROPRIATE ORDER */
/*   FOR NEXT STEP */

    if (*k > 1) {
	goto L445;
    }
    if (erkp1 >= erk * .5f) {
	goto L460;
    }
    goto L450;
L445:
    if (erkm1 <= min(erk,erkp1)) {
	goto L455;
    }
    if (erkp1 >= erk || *k == 12) {
	goto L460;
    }

/*   HERE ERKP1 .LT. ERK .LT. MAX(ERKM1,ERKM2) ELSE ORDER WOULD HAVE */
/*   BEEN LOWERED IN BLOCK 2.  THUS ORDER IS TO BE RAISED */

/*   RAISE ORDER */

L450:
    *k = kp1;
    erk = erkp1;
    goto L460;

/*   LOWER ORDER */

L455:
    *k = km1;
    erk = erkm1;

/*   WITH NEW ORDER DETERMINE APPROPRIATE STEP SIZE FOR NEXT STEP */

L460:
    hnew = *h__ + *h__;
    if (phase1) {
	goto L465;
    }
    if (p5eps >= erk * two[*k]) {
	goto L465;
    }
    hnew = *h__;
    if (p5eps >= erk) {
	goto L465;
    }
    temp2 = (doublereal) (*k + 1);
    d__1 = p5eps / erk;
    d__2 = 1.f / temp2;
    r__ = pow_dd(&d__1, &d__2);
/* Computing MAX */
    d__1 = .5, d__2 = min(.9,r__);
    hnew = absh * max(d__1,d__2);
/* Computing MAX */
    d__2 = hnew, d__3 = fouru * abs(*x);
    d__1 = max(d__2,d__3);
    hnew = d_sign(&d__1, h__);
L465:
    *h__ = hnew;
    return 0;
/*       ***     END BLOCK 4     *** */
} /* steps_ */






/* Subroutine */ int dcpose_(integer *ndim, integer *n, doublereal *qr, 
	doublereal *alpha, integer *pivot, integer *ierr, doublereal *y, 
	doublereal *sum)
{
    /* System generated locals */
    integer qr_dim1, qr_offset, i__1, i__2, i__3;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i__, j, k, kp1, np1;
    static doublereal beta;
    static integer jbar;
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *, 
	    integer *);
    static doublereal qrkk, sigma, alphak;


/* SUBROUTINE  DCPOSE  IS A MODIFICATION OF THE ALGOL PROCEDURE */
/* DECOMPOSE  IN P. BUSINGER AND G. H. GOLUB, LINEAR LEAST */
/* SQUARES SOLUTIONS BY HOUSEHOLDER TRANSFORMATIONS, */
/* NUMER. MATH. 7 (1965) 269-276. */

    /* Parameter adjustments */
    qr_dim1 = *ndim;
    qr_offset = 1 + qr_dim1;
    qr -= qr_offset;
    --alpha;
    --pivot;
    --y;
    --sum;

    /* Function Body */
    *ierr = 0;
    np1 = *n + 1;
    i__1 = np1;
    for (j = 1; j <= i__1; ++j) {
	sum[j] = ddot_(n, &qr[j * qr_dim1 + 1], &c__1, &qr[j * qr_dim1 + 1], &
		c__1);
/* L20: */
	pivot[j] = j;
    }
    i__1 = *n;
    for (k = 1; k <= i__1; ++k) {
	sigma = sum[k];
	jbar = k;
	kp1 = k + 1;
	i__2 = np1;
	for (j = kp1; j <= i__2; ++j) {
	    if (sigma >= sum[j]) {
		goto L40;
	    }
	    sigma = sum[j];
	    jbar = j;
L40:
	    ;
	}
	if (jbar == k) {
	    goto L70;
	}
	i__ = pivot[k];
	pivot[k] = pivot[jbar];
	pivot[jbar] = i__;
	sum[jbar] = sum[k];
	sum[k] = sigma;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    sigma = qr[i__ + k * qr_dim1];
	    qr[i__ + k * qr_dim1] = qr[i__ + jbar * qr_dim1];
	    qr[i__ + jbar * qr_dim1] = sigma;
/* L50: */
	}
/*   END OF COLUMN INTERCHANGE. */
L70:
	i__2 = *n - k + 1;
	sigma = ddot_(&i__2, &qr[k + k * qr_dim1], &c__1, &qr[k + k * qr_dim1]
		, &c__1);
	if (sigma != 0.f) {
	    goto L60;
	}
	*ierr = 1;
	return 0;
L60:
	if (k == *n) {
	    goto L500;
	}
	qrkk = qr[k + k * qr_dim1];
	alphak = -sqrt(sigma);
	if (qrkk < 0.f) {
	    alphak = -alphak;
	}
	alpha[k] = alphak;
	beta = 1.f / (sigma - qrkk * alphak);
	qr[k + k * qr_dim1] = qrkk - alphak;
	i__2 = np1;
	for (j = kp1; j <= i__2; ++j) {
/* L80: */
	    i__3 = *n - k + 1;
	    y[j] = beta * ddot_(&i__3, &qr[k + k * qr_dim1], &c__1, &qr[k + j 
		    * qr_dim1], &c__1);
	}
	i__3 = np1;
	for (j = kp1; j <= i__3; ++j) {
	    i__2 = *n;
	    for (i__ = k; i__ <= i__2; ++i__) {
		qr[i__ + j * qr_dim1] -= qr[i__ + k * qr_dim1] * y[j];
/* L90: */
	    }
/* Computing 2nd power */
	    d__1 = qr[k + j * qr_dim1];
	    sum[j] -= d__1 * d__1;
/* L100: */
	}
L500:
	;
    }
    alpha[*n] = qr[*n + *n * qr_dim1];
    return 0;
} /* dcpose_ */






doublereal dnrm2_(integer *n, doublereal *x, integer *incx)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal ret_val, d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer ix;
    static doublereal ssq, norm, scale, absxi;

/*     .. Scalar Arguments .. */
/*     .. Array Arguments .. */
/*     .. */

/*  DNRM2 returns the euclidean norm of a vector via the function */
/*  name, so that */

/*     DNRM2 := sqrt( x'*x ) */



/*  -- This version written on 25-October-1982. */
/*     Modified on 14-October-1993 to inline the call to DLASSQ. */
/*     Sven Hammarling, Nag Ltd. */


/*     .. Parameters .. */
/*     .. Local Scalars .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Executable Statements .. */
    /* Parameter adjustments */
    --x;

    /* Function Body */
    if (*n < 1 || *incx < 1) {
	norm = 0.;
    } else if (*n == 1) {
	norm = abs(x[1]);
    } else {
	scale = 0.;
	ssq = 1.;
/*        The following loop is equivalent to this call to the LAPACK */
/*        auxiliary routine: */
/*        CALL DLASSQ( N, X, INCX, SCALE, SSQ ) */

	i__1 = (*n - 1) * *incx + 1;
	i__2 = *incx;
	for (ix = 1; i__2 < 0 ? ix >= i__1 : ix <= i__1; ix += i__2) {
	    if (x[ix] != 0.) {
		absxi = (d__1 = x[ix], abs(d__1));
		if (scale < absxi) {
/* Computing 2nd power */
		    d__1 = scale / absxi;
		    ssq = ssq * (d__1 * d__1) + 1.;
		    scale = absxi;
		} else {
/* Computing 2nd power */
		    d__1 = absxi / scale;
		    ssq += d__1 * d__1;
		}
	    }
/* L10: */
	}
	norm = scale * sqrt(ssq);
    }

    ret_val = norm;
    return ret_val;

/*     End of DNRM2. */

} /* dnrm2_ */






doublereal ddot_(integer *n, doublereal *dx, integer *incx, doublereal *dy, 
	integer *incy)
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val;

    /* Local variables */
    static integer i__, m, ix, iy, mp1;
    static doublereal dtemp;


/*     forms the dot product of two vectors. */
/*     uses unrolled loops for increments equal to one. */
/*     jack dongarra, linpack, 3/11/78. */
/*     modified 12/3/93, array(1) declarations changed to array(*) */


    /* Parameter adjustments */
    --dy;
    --dx;

    /* Function Body */
    ret_val = 0.;
    dtemp = 0.;
    if (*n <= 0) {
	return ret_val;
    }
    if (*incx == 1 && *incy == 1) {
	goto L20;
    }

/*        code for unequal increments or equal increments */
/*          not equal to 1 */

    ix = 1;
    iy = 1;
    if (*incx < 0) {
	ix = (-(*n) + 1) * *incx + 1;
    }
    if (*incy < 0) {
	iy = (-(*n) + 1) * *incy + 1;
    }
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dtemp += dx[ix] * dy[iy];
	ix += *incx;
	iy += *incy;
/* L10: */
    }
    ret_val = dtemp;
    return ret_val;

/*        code for both increments equal to 1 */


/*        clean-up loop */

L20:
    m = *n % 5;
    if (m == 0) {
	goto L40;
    }
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	dtemp += dx[i__] * dy[i__];
/* L30: */
    }
    if (*n < 5) {
	goto L60;
    }
L40:
    mp1 = m + 1;
    i__1 = *n;
    for (i__ = mp1; i__ <= i__1; i__ += 5) {
	dtemp = dtemp + dx[i__] * dy[i__] + dx[i__ + 1] * dy[i__ + 1] + dx[
		i__ + 2] * dy[i__ + 2] + dx[i__ + 3] * dy[i__ + 3] + dx[i__ + 
		4] * dy[i__ + 4];
/* L50: */
    }
L60:
    ret_val = dtemp;
    return ret_val;
} /* ddot_ */





/*
doublereal d1mach_(integer *i__)
{
    // Initialized data

    static integer sc = 0;

    // Format strings
    static char fmt_9000[] = "(/\002 Adjust D1MACH by uncommenting data stat"
	    "ements\002/\002 appropriate for your machine.\002)";

    // System generated locals
    doublereal ret_val;
    static doublereal equiv_4[5];

    // Builtin functions
    integer s_wsfe(cilist *), e_wsfe(void);
    int s_stop(char *, ftnlen);
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen), 
	    e_wsle(void);

    // Local variables
    static integer j;
// #define log10 ((integer *)equiv_4 + 8)
// #define dmach (equiv_4)
// #define large ((integer *)equiv_4 + 2)
// #define small ((integer *)equiv_4)
// #define diver ((integer *)equiv_4 + 6)
// #define right ((integer *)equiv_4 + 4)
    extern int i1mcry_(integer *, integer *, integer *,
	    integer *, integer *);

    // Fortran I/O blocks
    static cilist io___173 = { 0, 6, 0, fmt_9000, 0 };
    static cilist io___174 = { 0, 6, 0, fmt_9000, 0 };
    static cilist io___175 = { 0, 6, 0, 0, 0 };



//  DOUBLE-PRECISION MACHINE CONSTANTS
//  D1MACH( 1) = B**(EMIN-1), THE SMALLEST POSITIVE MAGNITUDE.
//  D1MACH( 2) = B**EMAX*(1 - B**(-T)), THE LARGEST MAGNITUDE.
//  D1MACH( 3) = B**(-T), THE SMALLEST RELATIVE SPACING.
//  D1MACH( 4) = B**(1-T), THE LARGEST RELATIVE SPACING.
//  D1MACH( 5) = LOG10(B)

//  THIS VERSION ADAPTS AUTOMATICALLY TO MOST CURRENT MACHINES.
//  R1MACH CAN HANDLE AUTO-DOUBLE COMPILING, BUT THIS VERSION OF
//  D1MACH DOES NOT, BECAUSE WE DO NOT HAVE QUAD CONSTANTS FOR
//  MANY MACHINES YET.
//  TO COMPILE ON OLDER MACHINES, ADD A C IN COLUMN 1
//  ON THE NEXT LINE
//  AND REMOVE THE C FROM COLUMN 1 IN ONE OF THE SECTIONS BELOW.
//  CONSTANTS FOR EVEN OLDER MACHINES CAN BE OBTAINED BY
//          mail netlib@research.bell-labs.com
//          send old1mach from blas
//  PLEASE SEND CORRECTIONS TO dmg OR ehg@bell-labs.com.

//     MACHINE CONSTANTS FOR THE HONEYWELL DPS 8/70 SERIES.
//      DATA SMALL(1),SMALL(2) / O402400000000, O000000000000 /
//      DATA LARGE(1),LARGE(2) / O376777777777, O777777777777 /
//      DATA RIGHT(1),RIGHT(2) / O604400000000, O000000000000 /
//      DATA DIVER(1),DIVER(2) / O606400000000, O000000000000 /
//      DATA LOG10(1),LOG10(2) / O776464202324, O117571775714 /, SC/987/

//     MACHINE CONSTANTS FOR PDP-11 FORTRANS SUPPORTING
//     32-BIT INTEGERS.
//      DATA SMALL(1),SMALL(2) /    8388608,           0 /
//      DATA LARGE(1),LARGE(2) / 2147483647,          -1 /
//      DATA RIGHT(1),RIGHT(2) /  612368384,           0 /
//      DATA DIVER(1),DIVER(2) /  620756992,           0 /
//      DATA LOG10(1),LOG10(2) / 1067065498, -2063872008 /, SC/987/

//     MACHINE CONSTANTS FOR THE UNIVAC 1100 SERIES.
//      DATA SMALL(1),SMALL(2) / O000040000000, O000000000000 /
//      DATA LARGE(1),LARGE(2) / O377777777777, O777777777777 /
//      DATA RIGHT(1),RIGHT(2) / O170540000000, O000000000000 /
//      DATA DIVER(1),DIVER(2) / O170640000000, O000000000000 /
//      DATA LOG10(1),LOG10(2) / O177746420232, O411757177572 /, SC/987/

//     ON FIRST CALL, IF NO DATA UNCOMMENTED, TEST MACHINE TYPES.
    if (sc != 987) {
	dmach[0] = 1e13;
	if (small[0] == 1117925532 && small[1] == -448790528) {
//           *** IEEE BIG ENDIAN ***
	    small[0] = 1048576;
	    small[1] = 0;
	    large[0] = 2146435071;
	    large[1] = -1;
	    right[0] = 1017118720;
	    right[1] = 0;
	    diver[0] = 1018167296;
	    diver[1] = 0;
	    log10[0] = 1070810131;
	    log10[1] = 1352628735;
	} else if (small[1] == 1117925532 && small[0] == -448790528) {
//           *** IEEE LITTLE ENDIAN ***
	    small[1] = 1048576;
	    small[0] = 0;
	    large[1] = 2146435071;
	    large[0] = -1;
	    right[1] = 1017118720;
	    right[0] = 0;
	    diver[1] = 1018167296;
	    diver[0] = 0;
	    log10[1] = 1070810131;
	    log10[0] = 1352628735;
	} else if (small[0] == -2065213935 && small[1] == 10752) {
//               *** VAX WITH D_FLOATING ***
	    small[0] = 128;
	    small[1] = 0;
	    large[0] = -32769;
	    large[1] = -1;
	    right[0] = 9344;
	    right[1] = 0;
	    diver[0] = 9472;
	    diver[1] = 0;
	    log10[0] = 546979738;
	    log10[1] = -805796613;
	} else if (small[0] == 1267827943 && small[1] == 704643072) {
//               *** IBM MAINFRAME ***
	    small[0] = 1048576;
	    small[1] = 0;
	    large[0] = 2147483647;
	    large[1] = -1;
	    right[0] = 856686592;
	    right[1] = 0;
	    diver[0] = 873463808;
	    diver[1] = 0;
	    log10[0] = 1091781651;
	    log10[1] = 1352628735;
	} else if (small[0] == 1120022684 && small[1] == -448790528) {
//           *** CONVEX C-1 ***
	    small[0] = 1048576;
	    small[1] = 0;
	    large[0] = 2147483647;
	    large[1] = -1;
	    right[0] = 1019215872;
	    right[1] = 0;
	    diver[0] = 1020264448;
	    diver[1] = 0;
	    log10[0] = 1072907283;
	    log10[1] = 1352628735;
	} else if (small[0] == 815547074 && small[1] == 58688) {
//           *** VAX G-FLOATING ***
	    small[0] = 16;
	    small[1] = 0;
	    large[0] = -32769;
	    large[1] = -1;
	    right[0] = 15552;
	    right[1] = 0;
	    diver[0] = 15568;
	    diver[1] = 0;
	    log10[0] = 1142112243;
	    log10[1] = 2046775455;
	} else {
	    dmach[1] = 1e27;
	    dmach[2] = 1e27;
	    large[1] -= right[1];
	    if (large[1] == 64 && small[1] == 0) {
		d9mach_1.cray1[0] = 67291416;
		for (j = 1; j <= 20; ++j) {
		    d9mach_1.cray1[j] = d9mach_1.cray1[j - 1] + 
			    d9mach_1.cray1[j - 1];
// L10:
		}
		d9mach_1.cray1[21] = d9mach_1.cray1[20] + 321322;
		for (j = 22; j <= 37; ++j) {
		    d9mach_1.cray1[j] = d9mach_1.cray1[j - 1] + 
			    d9mach_1.cray1[j - 1];
// L20:
		}
		if (d9mach_1.cray1[37] == small[0]) {
//                *** CRAY ***
		    i1mcry_(small, &j, &c__8285, &c_b187, &c__0);
		    small[1] = 0;
		    i1mcry_(large, &j, &c__24574, &c_b190, &c_b190);
		    i1mcry_(&large[1], &j, &c__0, &c_b190, &c_b194);
		    i1mcry_(right, &j, &c__16291, &c_b187, &c__0);
		    right[1] = 0;
		    i1mcry_(diver, &j, &c__16292, &c_b187, &c__0);
		    diver[1] = 0;
		    i1mcry_(log10, &j, &c__16383, &c_b202, &c_b203);
		    i1mcry_(&log10[1], &j, &c__0, &c_b205, &c_b206);
		} else {
		    s_wsfe(&io___173);
		    e_wsfe();
		    s_stop("779", (ftnlen)3);
		}
	    } else {
		s_wsfe(&io___174);
		e_wsfe();
		s_stop("779", (ftnlen)3);
	    }
	}
	sc = 987;
    }
//    SANITY CHECK
    if (dmach[3] >= 1.) {
	s_stop("778", (ftnlen)3);
    }
    if (*i__ < 1 || *i__ > 5) {
	s_wsle(&io___175);
	do_lio(&c__9, &c__1, "D1MACH(I): I =", (ftnlen)14);
	do_lio(&c__3, &c__1, (char *)&(*i__), (ftnlen)sizeof(integer));
	do_lio(&c__9, &c__1, " is out of bounds.", (ftnlen)18);
	e_wsle();
	s_stop("", (ftnlen)0);
    }
    ret_val = dmach[*i__ - 1];
    return ret_val;
*/
/* /+ Standard C source for D1MACH -- remove the * in column 1 +/ */
#include <stdio.h>
#include <float.h>
#include <math.h>
doublereal d1mach_ (integer * i)
{
	switch(*i){
	case 1: return DBL_MIN;
	case 2: return DBL_MAX;
	case 3: return DBL_EPSILON/FLT_RADIX;
	case 4: return DBL_EPSILON;
	case 5: return log10((double)FLT_RADIX);
	}
	fprintf(stderr, "invalid argument: d1mach(%ld)\n", *i);
	exit(1); return 0; /* some compilers demand return values */
}


#undef right
#undef diver
#undef small
#undef large
#undef dmach
#undef log10


/* Subroutine */ int i1mcry_(integer *a, integer *a1, integer *b, integer *
	c__, integer *d__)
{
/* *** SPECIAL COMPUTATION FOR OLD CRAY MACHINES **** */
    *a1 = (*b << 24) + *c__;
    *a = (*a1 << 24) + *d__;
    return 0;
} /* i1mcry_ */

