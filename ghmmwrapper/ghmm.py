#!/usr/bin/env python2.3
################################################################################
#
#       This file is part of GHMM (General Hidden Markov Model library) 
#
#       file:    ghmm.py
#       authors: Benjamin Georgi, Wasinee Rungsarityotin, Alexander Schliep
#
#       Copyright (C) 2003-2004, Alexander Schliep and MPI Molekulare Genetik, Berlin
#                                   
#       Contact: schliep@molgen.mpg.de         
#
#       Information: http://ghmm.org
#
#       This library is free software; you can redistribute it and/or
#       modify it under the terms of the GNU Library General Public
#       License as published by the Free Software Foundation; either
#       version 2 of the License, or (at your option) any later version.
#
#       This library is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#       Library General Public License for more details.
#
#       You should have received a copy of the GNU Library General Public
#       License along with this library; if not, write to the Free
#       Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#
#
#
#       This file is version $Revision$ 
#                       from $Date$
#             last change by $Author$.
#
################################################################################

"""

The Design of ghmm.py 

HMMs are stochastic models which encode a probability density over
sequences of symbols. These symbols can be discrete letters (A,C,G and
T for DNA; 1,2,3,4,5,6 for dice), real numbers (weather measurement
over time: temperature) or vectors of either or the combination
thereof (weather again: temperature, pressure, percipitation).

Note: We will always talk about emissions, emission sequence and so
forth when we refer to the sequence of symbols. Another name
for the same object is observation resp. observation sequence.


The objects one has to deal with in HMM modelling are the following

1) The domain the emissions come from: the EmissionDomain. Domain
   is to be understood mathematically and to encompass both discrete,
   finite alphabets and fields such as the real numbers or intervals
   of the reals.

   For technical reasons there can be two representations of an emission
   symbol: an external and an internal. The external representation
   is the view of the application using ghmm.py. The internal one
   is what is used in both ghmm.py and the ghmm C-library. Representations
   can coincide, but this is not guaranteed. Most prominently discrete
   alphabets of size k are represented as [0,1,2,...,k] internally.
   It is the domain objects job to provide a mapping between representations
   in both directions.

   NOTE: Do not make assumptions about the internal representations. It might
   change.
   

2) Every domain has to afford a distribution, which is usually
   parameterized. A distribution associated with a domain
   should allow us to compute $\Prob[x| distribution parameters]$
   efficiently.

   The distribution defines the *type* of distribution which
   we will use to model emissions in *every state* of the HMM.
   The *type* of distribution will be identical for all states,
   their *parameterizations* will differ from state to state. 
   

3) We will consider a Sequence of emissions from the same emission
   domain and very often sets of such sequences: SequenceSet


4)
   The HMM: The HMM consists of two major components: A Markov chain
   over states (implemented as a weighted directed graph with
   adjacency and inverse-adjacency lists) and the emission
   distributions per-state. For reasons of efficiency the HMM itself
   is *static*, as far as the topology of the underlying Markov chain
   (and obviously the EmissionDomain) are concerned. You cannot add or
   delete transitions in an HMM.

   Transition probabilities and the parameters of the per-state
   emission distributions can be easily modified. Particularly,
   Baum-Welch reestimation is supported.  While a transition cannot be
   deleted from the graph, you can set the transition probability to
   zero, which has the same effect from the theoretical point of
   view. However, the corresponding edge in the graph is still
   traversed in the computation. 

   States in HMMs are referred to by their integer index. State sequences
   are simply list of integers.

   If you want to store application specific data for each state you have to
   do it yourself.

   Subclasses of HMM implement specific types of HMM. The type depends
   on the EmissionDomain, the Distribution used, the specific
   extensions to the 'standard' HMMs and so forth

   
5) HMMFactory: This provides a way of constucting HMMs. Classes derived
   from HMMFactory allow to read HMMs from files, construct them explicitly
   from, for a discrete alphabet, transition matrix, emission matrix and prior
   or serve as the basis for GUI-based model building.

   There are two ways of using the HMMFactory.

   Static construction:

   HMMOpen(fileName) # Calls an object of type HMMOpen instantiated in ghmm
   HMMOpen(fileName, type=HMM.FILE_XML)
   HMMFromMatrix(emission_domain, distribution, A, B, pi) # B is a list of distribution parameters

   # XXX do we need dsitribution here?

   Dynamic construction: Providing a context for dynamically
   editing existing HMMs or creating them from scratch

   HMMEditingContext() # Create an object
   HMMEditingContext(hmm) # Create an object from existing HMM
   
   Examples:

   hmm = HMMOpen('some-hmm.xml')

   hmm_context = HMMEditingContext(hmm) # just reads hmm
   hmm_context.addTransition(4,5, 0.3) # normalization will occurr
   hmm_context.addTransition(5,6, 0.1)

   hmm = hmm_context() # Creates a new hmm 

   hmm.bla ....

   

"""


#-------------------------------------------------------------------------------
# Exceptions
#
#

class GHMMError(Exception):
    """Base class for exceptions in this module."""

    def __init__(self, expression, message):
        self.expression = expression
        self.message = message


    

class EmissionDomain:
    """ Abstract base class for emissions produced by a HMM.

        There can be two representations for emissions:
        1) An internal, used in ghmm.py and the ghmm C-library
        2) An external, used in your particular application

        Example:
        The underlying library represents symbols from a finite,
        discrete domain as integers (see Alphabet).
    """

    def internal(self, emission):
        """ Given a emission return the internal representation
        """
        return emission


    def internalSequence(self, emissionSequence):
        """ Given a emissionSequence return the internal representation
        """
        return emissionSequence


    def external(self, internal):
        """ Given an internal representation return the
            external representation
        """
        return internal

    def externalSequence(self, internalSequence):
        """ Given a sequence with the internal representation return the
            external representation
        """
        return internalSequence


    def isAdmissable(self, emission):
        """ Check whether emission is admissable (contained in) the domain
            raises GHMMOutOfDomain else
        """
        return None


class Alphabet(EmissionDomain):
    """ Discrete, finite alphabet
    
    """
    def __init__(self, listOfCharacters):
        """ Creates an alphabet out of a listOfCharacters """
        self.listOfCharacters = listOfCharacters
        self.index = {} # Which index belongs to which character
        i = 0
        for c in self.listOfCharacters:
            self.index[c] = i
            i += 1

    def internal(self, emission):
        """ Given a emission return the internal representation
        """
        return self.index[emission]


    def internalSequence(self, emissionSequence):
        """ Given a emission_sequence return the internal representation
        """
        result = copy.deepcopy(emissionSequence)
        for i in xrange(len(result)):
            result[i] = self.index[result[i]]
        return result


    def external(self, internal):
        """ Given an internal representation return the
            external representation
        """
        return self.listOfCharacters[internal]

    def external_sequence(self, internalSequence):
        """ Given a sequence with the internal representation return the
            external representation
        """
        result = copy.deepcopy(internalSequence)
        for i in xrange(len(result)):
            result[i] = self.listOfCharacters[result[i]]
        return result

    def isadmissable(self, emission):
        """ Check whether emission is admissable (contained in) the domain
            raises GHMMOutOfDomain else
        """
        return emission in self.listOfCharacters


DNA = Alphabet(['A','C','G','T'])
#AminoAcids20 = Alphabet([A,C,G,T,])

def IntegerRange(a,b):
    return Alphabet(range(a,b))

    
class Distribution:
    """ Abstract base class for distribution over EmissionDomains
    """

    # From Spass (S. Rahmann):
    # add density, mass, cumuliative dist, quantils, sample, fit pars,
    # moments

    def set(self, parameters):
        """ Set all parameters defining the Distribution """
        pass

    def get(self):
        """ Return a tuple with all the parameters of the Distribution """


class DiscreteDistribution(Distribution):
    """ A DiscreteDistribution over an Alphabet """

    def __init__(self, alphabet, probability_vector):
        self.emissionDomain = alphabet
        self.prob = probability_vector #XXX Need a copy here, NumArray???

        if not len(probability_vector) == alphabet.size():
            print "GHMMIncompatibleXXX"
        
    def __get__(self,i):
        return self.prob[i]
    
    def set(self, probability_vector):
        self.prob = probability_vector

    def get(self):
        return self.prob
        

class ContinousDistribution(Distribution):
    pass

class NormalDistribution(ContinousDistribution):

    def __init__(self, domain, (mu, sigma2)):
        self.emissionDomain = domain
        self.mu = mu
        self.sigma2 = sigma2

    def set(self, (mu, sigma2)):
        self.mu = mu
        self.sigma2 = sigma2

    def get(self):
        return (self.mu, self.sigma2)
        

class MixtureContinousDistribution(Distribution):
    pass


class EmissionSequence(list):
    """ An EmissionSequence contains the *internal* representation of
        a sequence of emissions. It also contains a reference to the
        domain where the emission orginate
    """

    def __init__(self, emissionDomain, fromList = None):
        pass

#    def sequenceSet(self):
#        """ Make a one-element SequenceSet out of me """


class SequenceSet:
    pass
    

def SequenceSetOpen(fileName):
    pass

class HMMFactory:
    """ A HMMFactory is the base class of HMM factories.
        A HMMFactory has just a constructor and a () method
    """


GHMM_FILETYPE_SMO = 'smo'
GHMM_FILETYPE_XML = 'xml'

class HMMOpenFactory(HMMFactory):

    def __init__(self, defaultFileType=None):
        if defaultFileType:
            self.defaultFileType = defaultFileType

    def __call__(self, fileName, fileType=None):
        hmmClass = self.determineHMMClass("bla.xml") # Return proper class
        #m = hmmClass("bla.xml")
        #return m

    def determineHMMClass(self, fileName):
        pass 

HMMOpen = HMMOpenFactory(GHMM_FILETYPE_SMO)

class HMMFromMatrices(HMMFactory):
    def __init__(self, emissionDomain, distribution, A, B, pi):

        if isinstance(emissionDomain,Alphabet):

            if isinstance(distribution,DiscreteDistribution):
                # Know we know that HMM has discrete emissions
                m = 'FIX ME'
                return m
            else:
                # HMM with continous emissions
                raise GHMMError


class HMM:

    def __init__(self):
        # Do not do anything a subclass of HMM gets constructed by a HMMFactory
        pass

    def loglikelihood(self, emissionSequences):
        """ Compute log( P[emissionSequences| model]) using the forward algorithm

            emission_sequences can either be a SequenceSet or a Sequence

            Result: log( P[emissionSequences| model]) of type float
                    numarray vector of floats
            
            Note: The implementation will not compute the full forward matrix 
        """

    def rankLoglikelihood(self, emission_sequences):
        """


           XXX Should that be here?
        """
        pass
        


    ## Further Marginals ...



    def logprob(self, emissionSequence, stateSequence):
        """ log P[ emissionSequence, stateSequence| m] """
        pass
        

    def baumWelch(self, trainingSequences, nrSteps, loglikelihoodCutoff):
        """ Reestimate the model parameters given the training_sequences.
            Perform at most nr_steps until the improvement in likelihood
            is below likelihood_cutoff
        
            training_sequences can either be a SequenceSet or a Sequence
  
            Result: Final loglikelihood
        """
        self.baumWelchSetup(trainingSequences)
        (steps_made, loglikelihood_array, scale_array) = self.baumWelchStep(nrSteps,
                                                                            loglikelihoodCutoff)
        return loglikelihood_array[-1]

    def baumWelchSetup(self, trainingSequences):
        """ Setup necessary temporary variables for Baum-Welch-reestimation.
            Use baum_welch_setup and baum_welch_step if you want more control
            over the training, compute diagnostics or do noise-insertion

            training_sequences can either be a SequenceSet or a Sequence
        """
        pass
    
    def baumWelchStep(self, nrSteps, loglikelihoodCutoff):
        """ Setup necessary temporary variables for Baum-Welch-reestimation.
            Use baum_welch_setup and baum_welch_step if you want more control
            over the training, compute diagnostics or do noise-insertion

            training_sequences can either be a SequenceSet or a Sequence
        """
        pass
    
    def baumWelchDelete(self):
        """ Delete the necessary temporary variables for Baum-Welch-reestimation """
        # Needed ?
        pass

    def forward(self, emissionSequence):
        """

            Result: the (N x T)-matrix containing the forward-variables
                    and the scaling vector
        """
        pass

    def backward(self, emissionSequence, scalingVector):
        """

            Result: the (N x T)-matrix containing the backward-variables
        """

    

    def viterbi(self, emissionSequences):
        """ Compute the Viterbi-path for each sequence in emission_sequences

            emission_sequences can either be a SequenceSet or a Sequence

            Result: [q_0, ..., q_T] the viterbi-path if emission_sequences is a Sequence
                    [[q_0^0, ..., q_T^0], ..., [q_0^k, ..., q_T^k]} for a k-sequence
                    SequenceSet
        """

    
    def sample(self, T):
        """ Sample emission sequences 


        """

    def sampleSingle(self, T):
        """ Sample a single emission sequence of length at most T.
            Returns a Sequence object.
        """

    def state(self, stateLabel):
        """ Given a stateLabel return the integer index to the state """
        pass


    def getTransition(self, i, j):
        """ Accessor function for the transition a_ij """
        pass

    def setTransition(self, i, j, prob):
        """ Accessor function for the transition a_ij """
        pass

    def getEmission(self, i):
        """ Accessor function for the  """
        pass

    def setEmission(self, i, j, distribution):
        """ Accessor function for the """
        pass

    def normalize(self):
        """ Normalize transition probs, emission probs (if applicable) """
        pass
        
    def randomize(self, noise_level):
        """ """
        pass
    


#m = HMMOpen("test.smo", modelIndex = 3) # Pick 3-rd model out of the smo fiel
m = HMMOpen("test.smo")

seqs = SequenceSetOpen('test.sqd')
l = m.baumWelch(seqs, 100, 0.001)
print l
