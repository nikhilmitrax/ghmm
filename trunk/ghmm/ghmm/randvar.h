/*-----------------------------------------------------------------------------
  author       : Bernhard Knab
  filename     : ghmm/ghmm/randvar.h
  created      : TIME: 16:43:38     DATE: Wed 17. February 1999
  $Id$

__copyright__

------------------------------------------------------------------------------*/

#ifndef RANDVAR_H
#define RANDVAR_H

/**@name Help functions for random values */
/*@{ (Doc++-Group: randvar) */


#define PI 3.141592653589793116

#ifdef __cplusplus
extern "C" {
#endif

/**
   Calculates the one dimensional density function phi( mean, u ) for the
   normal distribution at point x.
   @return       function value
   @param x:      point value
   @param mean:   mean value for the normal distribution
   @param u:      variance for the normal distribution ($\sigma^2$)
   */
double randvar_normal_density(double x, double mean, double u);

/**
   Determinates the value of the one dimensional density function 
   phi( mean, u ) for the normal distribution at point x. The value is
   got from a previoously calculated list, which is made in first call
   to this function.
   @return       function value
   @param x:      point value
   @param mean:   mean value for the normal distribution
   @param u:      variance for the normal distribution ($\sigma^2$)
   */
double randvar_normal_density_approx(double x, double mean, double u);

/**
   Calculates the one dimensional Gauss density function phi( mean, u ) 
   at point x. In this case: phi = 0 for x < 0. 
   @return       function value
   @param x:      point value
   @param mean:   mean value for the normal distribution
   @param u:      variance for the normal distribution ($\sigma^2$)
   */
double randvar_normal_density_pos(double x, double mean, double u);

/**
   Calculates the one dimensional Gauss density function phi( mean, u ) 
   at point x. In this case: phi = 0 for x on the interval ($-\infty$, a).
   @return       function value
   @param x:      point value
   @param mean:   mean value for the normal distribution
   @param u:      variance for the normal distribution ($sigma^2$)
   @param a:      left limit for the density function
   */
double randvar_normal_density_trunc(double x, double mean, double u, double a);

/** 
   Generates a Uniform( 0, K-1 ) distributed random integer. 
   @return          random integer
   @param seed:      1: initialize only the Uniform( 0, 1 ) generator, 
                     0: return a uniform distributed integer 
   @param K:         right limit for uniform distribution
  */
double randvar_uniform_int(int seed, int K);

/** 
   Generates a N( 0, 1 ) distributed random number.
   @return          random value
   @param seed:      1: only initialize the U(0,1) generator,
                    0: returns a standard normal distributed random number 
  */
double randvar_std_normal(int seed);

/**
   Generates a N( mue, u ) distributed random number.
   @return           random number
   @param mue:       mean value for the normal distribution 
   @param u:         variance for the random distribution
   @param seed:      1: only initialization
                     0: returns a standard normal distributed random number 
  */
double randvar_normal(double mue, double u, int seed);

/** 
   Generates a positive, N( mue, u ) distributed random number.
   @return          random number
   @param mue:       mean value for the normal distribution 
   @param u:         variance for the random distribution
   @param seed:      1: only initialize the N( 0, 1 ) generator
                     0: returns a standard normal distributed random number 
  */
double randvar_normal_pos(double mue, double u, int seed);

/**
   Determinates the N( 0, 1 ) distribution function at point x.
   The distribution is read in as a table and points between the
   sampling points are generated with interpolation.
   @return           function value
   @param x:         point value
   */
double randvar_get_PHI(double x);

/**
   Calculates scaling factor 1/a for the truncated normal distribution.
   a corresponds to the value of the integral from x to infinity over
   the normal density function.
   @return           1/a
   @param x:         left limit for integral
   @param mean:      mean value for the normal distribution
   @param u:         variance for the normal distribution
   */
double randvar_get_1overa(double x, double mean, double u);

/**
   Determinates the first sampling point x, for which PHI(x) = 1 for the first
   time.
   @return          x with PHI(x)==1, PHI(y) < 1 for all y < x
   */
double randvar_get_xPHIless1();

#if 0
/**
   Determinates the sampling point x, for which PHI(x) = PHI(y) for 
   the first time, where x and y consecutive.
   @return          x with PHI(x)==PHI(y), where PHI(x')<PHI(y') f.a.x',y'<x,y
   */
double randvar_get_xPHIxgleichPHIy();
#endif

/**                                 
   Cumalative distribution function F(x;mean,u) for the N(mean, u).
   @return           F(x)
   @param x:         point value
   @param mean:      mean value for the normal distribution
   @param u:         variance for the normal distribution
*/
double randvar_normal_cdf(double x, double mean, double u);

/**                                 _
   Cumalative distribution function F(x;mean,u) for the truncated N(mean, u).
   @return          F(x)
   @param x:         point value
   @param mean:      mean value for the normal distribution
   @param u:         variance for the normal distribution
*/
double randvar_normal_pos_cdf(double x, double mean, double u);

  /**
   Small help function for interpolation, returns a constant.
  */
double randvar_get_xfaktphi();

/**
   Small help function for interpolation, returns a constant.
  */
double randvar_get_xstepphi();

  /** 
   Small help function for interpolation, returns a constant.
  */
double randvar_get_philen();

#ifdef __cplusplus
}
#endif


#endif /* RANDVAR_H */

/*@} (Doc++-Group: randvar) */