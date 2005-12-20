/*******************************************************************************
*
*       This file is part of the General Hidden Markov Model Library,
*       GHMM version __VERSION__, see http://ghmm.org
*
*       Filename: ghmm/ghmm/smodel.h
*       Authors:  Bernhard Knab, Benjamin Georgi
*
*       Copyright (C) 1998-2004 Alexander Schliep 
*       Copyright (C) 1998-2001 ZAIK/ZPR, Universitaet zu Koeln
*	Copyright (C) 2002-2004 Max-Planck-Institut fuer Molekulare Genetik, 
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
#ifndef GHMM_SMODEL_H
#define GHMM_SMODEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ghmm.h"
#include "scanner.h"


/**@name SHMM-Modell */
/*@{ (Doc++-Group: smodel) */

/** Continuous HMM. Structures and function. 
    ghmm_cmodel includes continuous ghmm_dmodel with one transition matrix 
    (COS  is set to 1) and an extension for
    models with several matrices
    (COS is set to a value greater than 1). In the latter case
    a suitable (depending on the spezific application) function 
    sequence\_get\_class has to be defined */

/**
   typedef density\_t fuer ghmm_cmodel.
*/
  typedef enum {
    normal,
    normal_right,
    normal_approx,
    normal_left,
    uniform,
    density_number
  } ghmm_density_t;


/** @name ghmm_cstate
    Structure for one state.
*/
  struct ghmm_cstate {
  /** Number of output densities per state */
    int M;
  /** initial prob. */
    double pi;
  /** IDs of successor states */
    int *out_id;
  /** IDs of predecessor states */
    int *in_id;
  /** transition probs to successor states. It is a
   matrix in case of mult. transition matrices (COS > 1)*/
    double **out_a;
  /** transition probs from predecessor states. It is a
   matrix in case of mult. transition matrices (COS > 1) */
    double **in_a;
  /** number of  successor states */
    int out_states;
  /** number of  predecessor states */
    int in_states;
  /** weight vector for output function components */
    double *c;
  /** mean vector for output functions (normal density and truncated normal
      density) or max value for uniform distribution */
    double *mue;
  /** variance vector for output functions or min value for uniform distribution */
    double *u;
  /** value where the normal is truncated (only for truncated distributions) */
    double *a;
  /** flag for fixation of parameter. If fix = 1 do not change parameters of
      output functions, if fix = 0 do normal training. Default is 0. */
    int fix;
  /** Flag for density function for each component of the mixture
      0: normal density, 1: truncated normal (right side) 
      density, 2: approximated normal density, 3: truncated normal (left side)
      4: uniform distribution */
    ghmm_density_t *density;
  /**  array of flags for fixing mixture components in the reestimation
        mixture_fix[i] = 1 means mu and sigma of component i are fixed.  **/
    int *mixture_fix;
  /** x coordinate position for graph representation plotting **/
    int xPosition;
  /** y coordinate position for graph representation plotting **/
    int yPosition;
  };
  typedef struct ghmm_cstate ghmm_cstate;

  struct ghmm_cmodel;

  struct ghmm_c_class_change_context {

    /* Names of class change module/function (for python callback) */
    char *python_module;
    char *python_function;

    /* index of current sequence */
    int k;

    /** pointer to class function */
    int (*get_class) (struct ghmm_cmodel *, double *, int, int);


    /* space for any data necessary for class switch, USER is RESPONSIBLE */
    void *user_data;

  };
  typedef struct ghmm_c_class_change_context ghmm_c_class_change_context;

/** @name ghmm_cmodel
    continous HMM    
*/
  struct ghmm_cmodel {
  /** Number of states */
    int N;
  /** Maximun number of components in the states */
    int M;
  /** ghmm_cmodel includes continuous model with one transition matrix 
      (cos  is set to 1) and an extension for models with several matrices
      (cos is set to a positive integer value > 1).*/
    int cos;
  /** prior for a priori prob. of the model. -1 means no prior specified (all
      models have equal prob. a priori. */
    double prior;
  /** All states of the model. Transition probs are part of the states. */
    ghmm_cstate *s;

    /* pointer to a ghmm_c_class_change_context struct necessary for multiple transition
       classes */
    ghmm_c_class_change_context *class_change;

  };
  typedef struct ghmm_cmodel ghmm_cmodel;



/* don't include this earlier: in sequence.h ghmm_cmodel has to be known */
#include "sequence.h"

  int ghmm_c_class_change_alloc (ghmm_cmodel * smo);

/** Alloc model
    @return allocated cmodel, -1: error
    @param N number of states in the model 
    @param modeltype type of the model
*/
  ghmm_cmodel * ghmm_cmodel_calloc( int N, int modeltype);

/** Free memory ghmm_cmodel 
    @return 0: success, -1: error
    @param smo  pointer pointer of ghmm_cmodel */
  int ghmm_c_free (ghmm_cmodel ** smo);

/**
   Copies one smodel. Memory alloc is here.
   @return pointer to ghmm_cmodel copy
   @param smo   ghmm_cmodel to be copied  */
  ghmm_cmodel *ghmm_c_copy (const ghmm_cmodel * smo);

/**
   Checks if ghmm_cmodel is well definded. E.g. sum pi = 1, only positive values 
   etc.
   @return 0 if ghmm_cmodel is ok, -1 for error
   @param smo   ghmm_cmodel for  checking
*/
  int ghmm_c_check (const ghmm_cmodel * smo);

/**
   For a vector of smodels: check that the number of states and the number
   of output function components are the same in each smodel.
   @return 0 if smodels are  ok, -1 for error
   @param smo:            vector of smodels for checking
   @param smodel_number:  number of smodels
 */
  int ghmm_c_check_compatibility (ghmm_cmodel ** smo, int smodel_number);

/**
   Generates random symbol.
   Generates one random number for a specified state and specified
   output component of the given smodel.
   @return           random number
   @param smo:       ghmm_cmodel
   @param state:     state
   @param m:         index of output component
*/
  double ghmm_c_get_random_var (ghmm_cmodel * smo, int state, int m);


/** 
    Produces sequences to a given model. All memory that is needed for the 
    sequences is allocated inside the function. It is possible to define
    the length of the sequences global (global_len > 0) or it can be set 
    inside the function, when a final state in the model is reach (a state
    with no output). If the model has no final state, the sequences will
    have length MAX_SEQ_LEN.
    @return             pointer to an array of sequences
    @param smo:         model
    @param seed:        initial parameter for the random value generator
                        (an integer). If seed == 0, then the random value
			generator is not initialized.
    @param global_len:  length of sequences (=0: automatically via final states)
    @param seq_number:  number of sequences
    @param label:       label tag
    @param Tmax:        maximal sequence length, set to MAX_SEQ_LEN if -1 
*/

  ghmm_cseq *ghmm_c_generate_sequences (ghmm_cmodel * smo, int seed,
                                           int global_len, long seq_number,
                                           long label, int Tmax);

/** 
    Computes sum over all sequence of
    seq_w * log( P ( O|lambda ) ). If a sequence can't be generated by smo
    error cost of seq_w * PRENALTY_LOGP are imposed.
   @return       n: number of evaluated sequences, -1: error
   @param smo   ghmm_cmodel
   @param sqd    sequence struct
   @param log\_p  evaluated log likelihood
*/
  int ghmm_c_likelihood (ghmm_cmodel * smo, ghmm_cseq * sqd, double *log_p);

/** 
    Computes log likelihood for all sequence of
    seq_w * log( P ( O|lambda ) ). If a sequence can't be generated by smo
    error cost of seq_w * PRENALTY_LOGP are imposed.
   @return       n: number of evaluated sequences, -1: error
   @param smo   ghmm_cmodel
   @param sqd    sequence struct
   @param log\_p array of evaluated likelihoods
*/
  int ghmm_c_individual_likelihoods (ghmm_cmodel * smo, ghmm_cseq * sqd,
                                     double *log_ps);


/** Reads an XML file with specifications for one or more smodels.
    All parameters in matrix or vector form.
   @return vector of read smodels
   @param filename   input xml file
   @param smo_number  number of smodels to read*/
  ghmm_cmodel **ghmm_c_xml_read (const char *filename, int *smo_number);

/**
   Prints one ghmm_cmodel in matrix form.
   @param file     output file
   @param smo   ghmm_cmodel
*/
  void ghmm_c_print (FILE * file, ghmm_cmodel * smo);

/**
   Prints one ghmm_cmodel with only one transition Matrix A (=Ak\_0).
   @param file     output file
   @param smo   ghmm_cmodel
*/
  void ghmm_c_print_oneA (FILE * file, ghmm_cmodel * smo);

/**
   Prints transition matrix of specified class.
   @param file       output file
   @param smo     ghmm_cmodel
   @param k          transition class
   @param tab      format: leading tab
   @param separator  format: seperator
   @param ending     format: end of data in line
*/
  void ghmm_c_Ak_print (FILE * file, ghmm_cmodel * smo, int k, char *tab,
                        char *separator, char *ending);

/**
   Prints weight matrix of output functions of an smodel.
   @param file       output file
   @param smo     ghmm_cmodel
   @param tab      format: leading tab
   @param separator  format: seperator
   @param ending     format: end of data in line
*/
  void ghmm_c_C_print (FILE * file, ghmm_cmodel * smo, char *tab, char *separator,
                       char *ending);

/**
   Prints mean matrix of output functions of an smodel.
   @param file       output file
   @param smo     ghmm_cmodel
   @param tab      format: leading tab
   @param separator  format: seperator
   @param ending     format: end of data in line
*/
  void ghmm_c_Mue_print (FILE * file, ghmm_cmodel * smo, char *tab,
                         char *separator, char *ending);
/**
   Prints variance matrix of output functions of an smodel.
   @param file       output file
   @param smo     ghmm_cmodel
   @param tab      format: leading tab
   @param separator  format: seperator
   @param ending     format: end of data in line
*/
  void ghmm_c_U_print (FILE * file, ghmm_cmodel * smo, char *tab, char *separator,
                       char *ending);
/**
   Prints initial prob vector of an smodel.
   @param file       output file
   @param smo     ghmm_cmodel
   @param tab      format: leading tab
   @param separator  format: seperator
   @param ending     format: end of data in line
*/
  void ghmm_c_Pi_print (FILE * file, ghmm_cmodel * smo, char *tab, char *separator,
                        char *ending);
/**
   Prints vector of fix\_states.
   @param file       output file
   @param smo     ghmm_cmodel
   @param tab      format: leading tab
   @param separator  format: seperator
   @param ending     format: end of data in line
*/
  void ghmm_c_fix_print (FILE * file, ghmm_cmodel * smo, char *tab,
                         char *separator, char *ending);

/** Computes the density of one symbol (omega) in a given state and a 
    given output component
    @return calculated density
    @param smo ghmm_cmodel
    @param state state 
    @param m output component
    @param omega given symbol
*/
  double ghmm_c_calc_cmbm (ghmm_cmodel * smo, int state, int m, double omega);

/** Computes the density of one symbol (omega) in a given state (sums over
    all output components
    @return calculated density
    @param smo ghmm_cmodel
    @param state state 
    @param omega given symbol
*/
  double ghmm_c_calc_b (ghmm_cmodel * smo, int state, double omega);

/** Computes probabilistic distance of two models
    @return the distance
    @param cm0  ghmm_cmodel used for generating random output
    @param cm   ghmm_cmodel to compare with
    @param maxT  maximum output length (for HMMs with absorbing states multiple
                 sequences with a toal length of at least maxT will be 
		 generated)
    @param symmetric  flag, whether to symmetrize distance (not implemented yet)
    @param verbose  flag, whether to monitor distance in 40 steps. 
                    Prints to stdout (yuk!)
*/
  double ghmm_c_prob_distance (ghmm_cmodel * cm0, ghmm_cmodel * cm, int maxT,
                               int symmetric, int verbose);

/** 
    Computes value of distribution function for a given symbol omega, a given
    ghmm_dstate and a given output component.
    @return   value of distribution function
    @param smo   ghmm_cmodel
    @param state  state
    @param m      component
    @param omega symbol
*/
  double ghmm_c_calc_cmBm (ghmm_cmodel * smo, int state, int m, double omega);

/** 
    Computes value of distribution function for a given symbol omega and
    a given  state. Sums over all components.
    @return   value of distribution function
    @param smo   ghmm_cmodel
    @param state  state
    @param omega symbol
*/
  double ghmm_c_calc_B (ghmm_cmodel * smo, int state, double omega);

/** Computes the number of free parameters in an array of
   smodels. E.g. if the number of parameter from pi is N - 1.
   Counts only those parameters, that can be changed during  
   training. If pi[i] = 0 it is not counted, since it can't be changed.
   @return number of free parameters
   @param smo ghmm_cmodel
   @param smo\_number number of smodels
*/
  int ghmm_c_count_free_parameter (ghmm_cmodel ** smo, int smo_number);


/*============================================================================*/

/* keep the following functions for first distribution???
   --> BK ? 
*/


/** Generates interval(a,b) with  B(a) < 0.01, B(b) > 0.99
    @param smo    continous HMM
    @param state  given state
    @param a      return-value: left side
    @param b      return-value: right side
*/
  void ghmm_c_get_interval_B (ghmm_cmodel * smo, int state, double *a, double *b);


#ifdef __cplusplus
}
#endif
#endif
/*@} (Doc++-Group: smodel) */
