/*
  created: 21 Jan 2002 by Peter Pipenbacher
  authors: Peter Pipenbacher (pipenb@zpr.uni-koeln.de)
  file   : $Source$
  $Id$

__copyright__

*/

#ifndef _GHMM_DISCRETEMODEL_H
#define _GHMM_DISCRETEMODEL_H 1

#include <vector>
#include <ghmm/model.h>
#include "ppghmm++/GHMM_State.h"
#include "ppghmm++/GHMM_AbstractModel.h"

#include <ppghmm++/begin_code.h>

#ifdef HAVE_NAMESPACES
namespace std {
#endif

class GHMM_DiscreteModel;
class GHMM_Sequences;
class GHMM_IntVector;
class GHMM_DoubleVector;
class GHMM_DoubleMatrix;

/** Discrete HMM model (model in C data structure). */
class GHMM_DiscreteModel: public GHMM_AbstractModel {

 public:
  
  /** Constructor. 
      @param number_of_states Number of the states.
      @param my_M Size of the alphabet. 
      @param my_prior Prior for the a priori probability for the model 
                      (-1 for none). */
  GHMM_DiscreteModel(int number_of_states, int my_M, double my_prior=-1);
  /** Constructor. Construct from c model. Object now is owner of this model. 
      @param my_model model as C data structure. */
  GHMM_DiscreteModel(model* my_model);
  /** Destructor. */
  virtual ~GHMM_DiscreteModel();

  /** Returns name of class. */
  virtual const char* toString() const;

  /** 
      Writes transition matrix of a model.
      @param file: output file
      @param tab:  format: leading tabs
      @param separator: format: seperator for columns
      @param ending:    format: end of a row 
  */
  void A_print(FILE *file, char *tab, char *separator, char *ending) const;
  /**
     Writes transposed transition matrix of the model.
     @param file: output file
     @param tab:  format: leading Tabs
     @param separator: format: seperator for columns
     @param ending:    format: end of a row  
  */
  void A_print_transp(FILE *file, char *tab, char *separator, char *ending) const;
  /**
     Writes output matrix of a model.
     @param file: output file
     @param tab:  format: leading tabs
     @param separator: format: seperator for columns
     @param ending:    format: end of a row  
  */
  void B_print(FILE *file, char *tab, char *separator, char *ending) const;
  /**
     Writes transposed output matrix of a model.
     @param file: output file
     @param tab:  format: leading Tabs
     @param separator: format: seperator for columns
     @param ending:    format: end of a row  
  */   
  void B_print_transp(FILE *file, char *tab, char *separator, char *ending) const;
  /**
     Tests if all standardization requirements of model are fulfilled. 
     (That is, if the sum of the probabilities is 1).
     @return 0 on succes; -1 on error. 
  */
  virtual int check() const;
  /**
     Copies a given model. Allocates the necessary memory.
     @return copy of the model
  */
  GHMM_DiscreteModel* copy() const;
  /** 
      Backward-Algorithm. 
      Calculates beta[t][i] given an integer sequence and a model. Scale factors 
      given as parameter (come from sfoba\_forward).
      @param seq:     sequences
      @param index:   index of sequence to take
      @param beta     beta[t][i]
      @param scale    scale factors
      @return 0 for success, -1 for error
  */
  int fobaBackward(GHMM_Sequences* seq, int index, double **beta, const double *scale) const;
  /** Forward-Algorithm.
      Calculates alpha[t][i], scaling factors scale[t] and log( P(O|lambda) ) for
      a given double sequence and a given model.
      @param seq:    sequences
      @param index:  index of sequence to take
      @param alpha:  alpha[t][i]
      @param scale:  scale factors (return value, if scale is NULL this vector will not be returned)
      @param log\_p:  log likelihood log( P(O|lambda) )
      @return NULL on error, alpha matrix on success
  */
  GHMM_DoubleMatrix* fobaForward(GHMM_Sequences* seq, int index, GHMM_DoubleVector* scale, 
				 double *log_p) const;
  /**
     Calculation of  log( P(O|lambda) ). 
     Done by calling sfoba\_forward. Use this function if only the
     log likelihood and not alpha[t][i] is needed.
     @param seq:      sequences
     @param index:    index of sequence to take
     @param log\_p    log likelihood log( P(O|lambda) )
     @return 0 for success, -1 for error
  */
  int fobaLogp(GHMM_Sequences* seq, int index, double *log_p) const;
  /**
     Writes fix vector of a matrix.
     @param file: output file
     @param tab:  format: leading Tabs
     @param separator: format: seperator for columns
     @param ending:    format: end of a row  
  */
  void fix_print(FILE *file, char *tab, char *separator, char *ending) const;
  /** 
      Produces sequences to a given model. All memory that is needed for the 
      sequences is allocated inside the function. It is possible to define
      the length of the sequences global (global_len > 0) or it can be set 
      inside the function, when a final state in the model is reach (a state
      with no output). If the model has no final state, the sequences will
      have length MAX_SEQ_LEN.
      @return             pointer to an array of sequences
      @param seed:        initial parameter for the random value generator
                          (an integer). If seed == 0, then the random value
			  generator is not initialized.
      @param global_len:  length of sequences (=0: automatically via final states)
      @param seq_number:  number of sequences
  */
  GHMM_Sequences* generate_sequences(int seed, int global_len, long seq_number) const;
  /** */
  virtual int getNumberOfTransitionMatrices() const;
  /* Returns state with given index. */
  state* getCState(int index) const;
  /**
     Writes initial allocation vector of a matrix.
     @param file: output file
     @param tab:  format: leading Tabs
     @param separator: format: seperator for columns
     @param ending:    format: end of a row  
  */
  void Pi_print(FILE *file, char *tab, char *separator, char *ending) const;
  /**
     Writes the model in matrix format.
     @param file: output file
  */
  virtual void print(FILE *file) const;
  /**
     Viterbi algorithm. Calculates the Viterbi path (the optimal path trough
     the model) and the Viterbi probability to a given model and a given 
     sequence.
     @return Viterbi path
     @param sequences  sequences structure
     @param index      index of sequence to take
     @param log_p      probability of the sequence in the Viterbi path 
                       (return value).
  */
  GHMM_IntVector* viterbi(GHMM_Sequences* sequences, int index, double *log_p) const;
  /**
     Baum-Welch Algorithm for HMMs.
     Training of model parameter with multiple integer sequences (incl. scaling).
     New parameters set directly in hmm (no storage of previous values!).
     @return           0/-1 success/error
     @param seq        sequences used for training.
  */
  int reestimate_baum_welch(GHMM_Sequences* seq);

  /** C Model. */
  model* c_model;


 private:

  /** Build c++ data from c_model. */
  void buildCppData();
};

#ifdef HAVE_NAMESPACES
}
#endif

#include <ppghmm++/close_code.h>

#endif /* _GHMM_DISCRETEMODEL_H */
