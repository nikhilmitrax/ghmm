/*******************************************************************************
*
*       This file is part of the General Hidden Markov Model Library,
*       GHMM version __VERSION__, see http://ghmm.org
*
*       Filename: ghmm/ghmm/ghmm.h
*       Authors:  Alexander Schliep, Janne Grunau
*
*       Copyright (C) 1998-2004 Alexander Schliep
*       Copyright (C) 1998-2001 ZAIK/ZPR, Universitaet zu Koeln
*       Copyright (C) 2002-2004 Max-Planck-Institut fuer Molekulare Genetik,
*                               Berlin
*
*       Contact: schliep@ghmm.org
*
*       This library is free software; you can redistribute it and/or
*       modify it under the terms of the GNU Library General Public
*       License as published by the Free Software Foundation; either
*       version 2 of the License, or (at your option) any later version.
*
*       This library is distributed in the hope that it will be useful,
*       but WITHOUT ANY WARRANTY; without even the implied warranty of
*       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*       Library General Public License for more details.
*
*       You should have received a copy of the GNU Library General Public
*       License along with this library; if not, write to the Free
*       Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*
*       This file is version $Revision$
*                       from $Date$
*             last change by $Author$.
*
*******************************************************************************/

#ifndef GHMM_H
#define GHMM_H

#ifdef __cplusplus
extern "C" {
#endif

/**@name GHMM-Globals */
/*@{ (Doc++-Group: globals) */

/** @name type_constants
    Constants giving model variations */
#define GHMM_kNotSpecified (0)

/** Model is a left-right */
#define GHMM_kLeftRight (1)

/** Model contains silent states (i.e., states without emissions) */
#define GHMM_kSilentStates (1 << 2)

/** Model has states with tied emission probabilities */
#define GHMM_kTiedEmissions (1 << 3)
#define GHMM_kUntied (-1)

/** Model has states emission probabilities conditioned on previous orders */
#define GHMM_kHigherOrderEmissions (1 << 4)

/** Model has background distributions */
#define GHMM_kBackgroundDistributions (1 << 5)
#define GHMM_kNoBackgroundDistribution (-1)

/** Model is a class HMM with labeled states */
#define GHMM_kLabeledStates (1 << 6)

/*@} (Doc++-Group: GHMM-Globals) */



/*@{ (Doc++-Group: Konstanten) */
/** 
    Convergence: Halt criterium for Baum-Welch reestimation if the difference
    of log(P) in two consecutive iterations is smaller than (EPS\_ITER\_BW * log(P))..
*/
#define GHMM_EPS_ITER_BW      0.0001

/**
  If the absolute difference of two numbers is smaller the EPS_PREC, then the
  numbers are equal. (Instead of using zero )
  */
#define GHMM_EPS_PREC         1E-8

/**
  Minimum value for U
  */
#define GHMM_EPS_U            1E-4

/**
  Maximum number of iterations in reestimate
  */
#define GHMM_MAX_ITER_BW      500

/**
  Maximum length of a sequence
  */
#define GHMM_MAX_SEQ_LEN 1000000

/**
  Maximum number of sequences 
  */
#define GHMM_MAX_SEQ_NUMBER   1500000

/**
   A value that is put in for log_p in the calculation of
   the objective function if ghmm_c_logp returns -1 (error).
*/
#define GHMM_PENALTY_LOGP -500.0

/**
   The left limit for the normal density
*/
#define GHMM_EPS_NDT  0.1

/*@} (Doc++-Group: Constants) */


/*@{ (Doc++-Group: Logging) */
void ghmm_set_logfunc(void (* fptr)(int, const char *, void *), void * clientdata);

void ghmm_set_loglevel(int level);
/*@} (Doc++-Group: Logging) */

#ifdef __cplusplus
}
#endif

#endif /* GHMM_H */
