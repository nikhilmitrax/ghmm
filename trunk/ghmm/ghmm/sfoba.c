/*******************************************************************************
  author       : Bernhard Knab
  filename     : ghmm/ghmm/sfoba.c
  created      : TIME: 16:45:09     DATE: Mon 15. November 1999
  $Id$

__copyright__

*******************************************************************************/

#include <math.h>
#include <float.h>
#include "mprintf.h"
#include "mes.h"
#include "sfoba.h"
#include "const.h"
#include "matrix.h"
#include "randvar.h"



/*----------------------------------------------------------------------------*/
static int sfoba_initforward(smodel *smo, double *alpha_1, double omega, 
			     double *scale, double **b) {
# define CUR_PROC "foba_initforward"
  int i;
  double c_0;
  scale[0] = 0.0;
  if (b == NULL)
    for (i = 0; i < smo->N; i++) {
      alpha_1[i] = smo->s[i].pi * smodel_calc_b(smo, i, omega);
      scale[0] += alpha_1[i];
    }
  else
    for (i = 0; i < smo->N; i++) {
      alpha_1[i] = smo->s[i].pi * b[i][smo->M];
      scale[0] += alpha_1[i];
    }
  if (scale[0] > DBL_MIN) { /* >= EPS_PREC */
    c_0 = 1/scale[0];
    for (i = 0; i < smo->N; i++) 
      alpha_1[i] *= c_0;
  }
  return(0); 
# undef CUR_PROC
} /* sfoba_initforward */

/*----------------------------------------------------------------------------*/
static double sfoba_stepforward(sstate *s, double *alpha_t, int osc, 
				double b_omega) {
  int i, id;
  double value = 0.0;
  for (i = 0; i < s->in_states; i++) {
    id = s->in_id[i];
    value += s->in_a[osc][i] * alpha_t[id];
  }
  value *= b_omega; /* b_omega outside the sum */
  return(value);
} /* sfoba_stepforward */


/*============================================================================*/
int sfoba_forward(smodel *smo, const double *O, int T, double ***b,
		  double **alpha, double *scale, double *log_p) {
# define CUR_PROC "sfoba_forward"
  int res = -1;  
  int i, t, osc =0 ;
  double c_t, osum = 0.0;

  /* calculate alpha and scale for t = 0 */
  if (b == NULL)
    sfoba_initforward(smo, alpha[0], O[0], scale, NULL);
  else
    sfoba_initforward(smo, alpha[0], O[0], scale, b[0]);
  if (scale[0] <= DBL_MIN) {
    /* means f(O[0], mue, u) << 0, first symbol very unlikely */
    /* mes_prot("scale[0] == 0.0!\n"); */
    goto STOP;
  }
  else {
    *log_p = - log(1/scale[0]);
    /* dummy function, returns 0 at the moment */
    osc = sequence_d_class(O, 0, &osum); 
    for (t = 1; t < T; t++) {
      scale[t] = 0.0;
      /* b not calculated yet */
      if (b == NULL) {
	for (i = 0; i < smo->N; i++) {
	  alpha[t][i] = sfoba_stepforward(&smo->s[i],alpha[t-1], osc,
					  smodel_calc_b(smo,i,O[t]));
	  scale[t] +=  alpha[t][i];
	}
      }
      /* b precalculated */
      else {
	for (i = 0; i < smo->N; i++) {
	  alpha[t][i] = sfoba_stepforward(&smo->s[i], alpha[t-1], osc,
					  b[t][i][smo->M]);
	  scale[t] +=  alpha[t][i];
	}
      }      
      if (scale[t] <= DBL_MIN) { /* seq. can't be build */
	goto STOP;
	break;
      }
      c_t = 1/scale[t];
      /* scale alpha */
      for (i = 0; i < smo->N; i++) 
	alpha[t][i] *= c_t;
      /* summation of log(c[t]) for calculation of log( P(O|lambda) ) */
      *log_p -= log(c_t);
      osc = sequence_d_class(O, t, &osum);
    }
  }
  /* log_p should not be smaller than value used for seqs. that 
     can't be build ???
     if (*log_p < (double)PENALTY_LOGP)
     *log_p = (double)PENALTY_LOGP;
     */
  return 0;
 STOP:
  *log_p = (double)-DBL_MAX; 
  return(res);
# undef CUR_PROC
} /* sfoba_forward */

/*============================================================================*/
int sfoba_backward(smodel *smo, const double *O, int T, double ***b,
		   double **beta, const double *scale) {
# define CUR_PROC "sfoba_backward"
  double *beta_tmp, sum, c_t, osum;
  int i, j, j_id, t, osc, t2;
  int res = -1;
  if (!m_calloc(beta_tmp, smo->N)) {mes_proc(); goto STOP;}

  for (t = 0; t < T; t++) {
    /* try differenent bounds here in case of problems 
       like beta[t] = NaN 
    */
    /* if (scale[t] < exp(-130)) { */
    /* if (scale[t] < exp(-230)) { */
    if (scale[t] <= DBL_MIN) {
	printf("backward scale(%d) = %e\n", t , scale[t]);
      goto STOP;
    }
  }
  /* initialize */
  c_t = 1/scale[T-1];
  for (i = 0; i < smo->N; i++) {
    beta[T-1][i] = 1;
    beta_tmp[i] = c_t;
  }
  /* Backward Step for t = T-2, ..., 0 */
  /* beta_tmp: Vector for storage of scaled beta in one time step */
  for (t = 0; t < T-1; t++) 
    osc = sequence_d_class(O, t, &osum);
  for (t = T-2; t >= 0; t--) {
    if (b == NULL)
      for (i = 0; i < smo->N; i++) {
	sum = 0.0;
	for (j = 0; j < smo->s[i].out_states; j++) {
	  j_id = smo->s[i].out_id[j];
	  sum += smo->s[i].out_a[osc][j] * smodel_calc_b(smo, j_id, O[t+1]) 
	    * beta_tmp[j_id];	  
	}
	beta[t][i] = sum;
      }
    else
      for (i = 0; i < smo->N; i++) {
	sum = 0.0;
	for (j = 0; j < smo->s[i].out_states; j++) {
	  j_id = smo->s[i].out_id[j];
	  sum += smo->s[i].out_a[osc][j] * b[t+1][j_id][smo->M]*beta_tmp[j_id];
	}
	beta[t][i] = sum;
      }
    c_t = 1/scale[t]; 
    for (i = 0; i < smo->N; i++) 
      beta_tmp[i] = beta[t][i] * c_t;
    for (t2 = 0; t2 < t; t2++)
      osc = sequence_d_class(O, t2, &osum);
  }
  res = 0;
STOP:
  m_free(beta_tmp);
  return(res);
# undef CUR_PROC
} /* sfoba_backward */

/*============================================================================*/
int sfoba_logp(smodel *smo, const double *O, int T, double *log_p) {
# define CUR_PROC "sfoba_logp"
  int res = -1;

  double **alpha, *scale = NULL;
  alpha = matrix_d_alloc(T, smo->N);
  if (!alpha) {mes_proc(); goto STOP;}
  if (!m_calloc(scale, T)) {mes_proc(); goto STOP;}
  /* run forward alg. */
  if (sfoba_forward(smo, O, T, NULL, alpha, scale, log_p) == -1 ) { 
    /* mes_proc(); */
    goto STOP;
  }
  res = 0;

STOP:
  matrix_d_free(&alpha, T);
  m_free(scale);
  return(res);
# undef CUR_PROC
} /* sfoba_logp */

