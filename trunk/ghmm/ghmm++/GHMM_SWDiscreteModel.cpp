/*
  created: 2. April 2003 by Wasinee
  authors: Wasinee Rungsarityotin (rungsari@molgen.mpg.de)
  file   : $Source$
  $Id$

  __copyright__

*/
#include "ghmm/ghmm.h"
#include "ghmm/viterbi.h"
#include "ghmm/mes.h"
#include "ghmm/foba.h"
#include "ghmm/matrix.h"
#include "ghmm/model.h"
#include "ghmm/vector.h"

#include <xmlio/XMLIO_Definitions.h>
#include <xmlio/XMLIO_Document.h>

#include "ghmm++/GHMM_IntVector.h"
#include "ghmm++/GHMM_DoubleVector.h"
#include "ghmm++/GHMM_Sequences.h"
#include "ghmm++/GHMM_DoubleMatrix.h"
#include "ghmm++/GHMM_Alphabet.h"
#include "ghmm++/GHMM_GMLEmission.h"
#include "ghmm++/GHMM_GMLAlphabet.h"

#include <ghmm++/GHMM_AbstractModelT.hh> // Template
#include "ghmm++/GHMM_SWDiscreteModel.h"
#include <iostream>
#include <queue>
#include <vector>
// #include "ghmm/reestimate.h"


#ifdef HAVE_NAMESPACES
using namespace std;
#endif

GHMM_SWDiscreteModel::GHMM_SWDiscreteModel(GHMM_Alphabet* my_alphabet) 
{  
  init(my_alphabet);
  alphabet = my_alphabet;
}


GHMM_SWDiscreteModel::GHMM_SWDiscreteModel(GHMM_Alphabet* my_alphabet, int no_klass)
{  
  init(my_alphabet);
  alphabet = my_alphabet;
  no_classes   = no_klass;
  c_model->cos = no_klass;
}

/** Constructor. */
GHMM_SWDiscreteModel::GHMM_SWDiscreteModel( sdmodel* my_model) 
{
  init();
  c_model = my_model;
  buildCppData();
}


GHMM_SWDiscreteModel::~GHMM_SWDiscreteModel() 
{
  /* frees model. */  
  sdmodel_free(&c_model);
  if (own_alphabet)
    SAFE_DELETE(alphabet);
  cleanCPP();
}

void GHMM_SWDiscreteModel::setNodeTag(const string& tag)
{ GHMM_AbstractModelT<GHMM_GMLState, GHMM_GMLTransition>::setNodeTag( tag ); }

void GHMM_SWDiscreteModel::setTransitionTag(const string& tag)
{ GHMM_AbstractModelT<GHMM_GMLState, GHMM_GMLTransition>::setTransitionTag( tag ); }

const char* GHMM_SWDiscreteModel::toString() const 
{
  return "GHMM_SWDiscreteModel";
}


/*
int GHMM_SWDiscreteModel::check() const {
  return model_check(c_model);
}*/


/**
   Produces a model, which generates the given sequence with probability 1.
   The model is a strict left-right model with one state for each element 
   in the sequence and the output in state i is the i-th value in the sequence 
   with probability 1. The model also has a final state, a state with no output.
   @param seq:      sequence
   @param seq_len:  length of the sequence
   @param anz_symb: number of symbols in the sequence
*/
//GHMM_SWDiscreteModel::GHMM_SWDiscreteModel(const int *seq, int seq_len, int anz_symb) {
//  c_model = model_generate_from_sequence(seq,seq_len,anz_symb);
//}


GHMM_Sequences* GHMM_SWDiscreteModel::generate_sequences(int seed, int global_len, long seq_number, int Tmax) const {

  return new GHMM_Sequences(sdmodel_generate_sequences(c_model,seed,global_len,seq_number, Tmax));
}


/**
   Calculates the sum log( P( O | lambda ) ).
   Sequences, that can not be generated from the given model, are neglected.
   @return    log(P)
   @param mo model
   @param sq sequences       
*/
//double GHMM_SWDiscreteModel::likelihood(sequence_t *sq) {
//  return model_likelihood(c_model,sq);
//}


/**
   Writes the model in matrix format.
   @param file: output file
*/
void GHMM_SWDiscreteModel::print(FILE *file)
{
  fprintf(stderr, "GHMM_SWDiscreteModel::print method not implemented\n");
}


void GHMM_SWDiscreteModel::A_print(FILE *file, char *tab, char *separator, char *ending) const {
  for(int k=0; k < c_model->cos; k++)
    sdmodel_Ak_print(file,c_model,k,tab,separator,ending);
}


void GHMM_SWDiscreteModel::B_print(FILE *file, char *tab, char *separator, char *ending) const {
  sdmodel_B_print(file,c_model,tab,separator,ending);
}


void GHMM_SWDiscreteModel::Pi_print(FILE *file, char *tab, char *separator, char *ending) const {
  sdmodel_Pi_print(file,c_model,tab,separator,ending);
}


//void GHMM_SWDiscreteModel::states_print(FILE *file) {
//  sdmodel_states_print(file,c_model);
//}


//double GHMM_SWDiscreteModel::prob_distance(GHMM_SWDiscreteModel* m, int maxT, int symmetric, int verbose) {
//  return model_prob_distance(c_model,m->c_model,maxT,symmetric,verbose);
//}

/*
GHMM_DoubleMatrix* GHMM_SWDiscreteModel::foba_forward(GHMM_Sequences* seq, int index, 
						    GHMM_DoubleVector *scale, double *log_p) const {
  int len = seq->getLength(index);
  GHMM_DoubleMatrix *alpha = new GHMM_DoubleMatrix(len,c_model->N);

  bool delete_scale = false;
  if (! scale) {
    scale        = new GHMM_DoubleVector();
    delete_scale = true;
  }

  scale->resize(len);

  int result = ::foba_forward(c_model,seq->getIntSequence(index),len,alpha->c_matrix,scale->c_vector,log_p);

  if (result == -1)
    SAFE_DELETE(alpha);

  if (delete_scale)
    SAFE_DELETE(scale);

  return alpha;
  }*/

/*
int GHMM_SWDiscreteModel::foba_backward(GHMM_Sequences* seq, int index, double **beta, 
				      const double *scale) const {
  return ::foba_backward(c_model,seq->getIntSequence(index),seq->getLength(index),beta,scale);
  }*/

/*
int GHMM_SWDiscreteModel::foba_logp(GHMM_Sequences* seq, int index, double *log_p) const {
  return ::foba_logp(c_model,seq->getIntSequence(index),seq->getLength(index),log_p);
  }*/


/*
sdstate* GHMM_SWDiscreteModel::getCState(int index) const {
  if (index >= c_model->N) {
    fprintf(stderr,"GHMM_SWDiscreteModel::getCState(int):\n");
    fprintf(stderr,"State no. %d does not exist. Model has %d states.\n",index,c_model->N);
    exit(1);
  }

  return &c_model->s[index];
  }*/


int GHMM_SWDiscreteModel::getNumberOfTransitionMatrices() const {
  return c_model->cos;
}


//int GHMM_SWDiscreteModel::reestimate_baum_welch(GHMM_Sequences* seq) {
//  return ::reestimate_baum_welch(c_model,seq->c_i_sequences);
//}

void GHMM_SWDiscreteModel::buildCppData()
{
  /* Create C++ wrapper for all states and fill C states with usefull 
     information. */
  int i;
  for (i = 0; i < c_model->N; ++i) {
    GHMM_GMLState* state = new GHMM_GMLState(this,i,&c_model->s[i]);
    states.push_back(state);
  }
}


void GHMM_SWDiscreteModel::cleanCPP()
{
  unsigned int i;

  for (i = 0; i < states.size(); ++i) {
    SAFE_DELETE(states[i]);
    if (edge_classes) delete edge_classes[i];
  }
  if (edge_classes) delete edge_classes;
  states.clear();
}


void GHMM_SWDiscreteModel::init() {
  attributes.clear();
  tag                = "graph";  
  setNodeTag("node");
  setTransitionTag("edge");
  alphabet           = NULL;
  c_model            = NULL;
  own_alphabet       = false;
  no_classes         = 1;
}


void GHMM_SWDiscreteModel::init(GHMM_Alphabet *my_alphabet) {
  attributes.clear();
  tag                = "graph";  
  setNodeTag("node");
  setTransitionTag("edge");
  alphabet           = my_alphabet;
  c_model            = NULL;
  own_alphabet       = true; 
  no_classes         = 1;

  c_model = (sdmodel*) calloc(1,sizeof(sdmodel));
  if (!c_model) {
    fprintf(stderr,"GHMM_SWDiscreteModel::GHMM_SWDiscreteModel() could not allocate c_model\n");
    exit(1);
  }

  c_model->N       = 0;
  c_model->M       = alphabet->size();
  c_model->cos     = no_classes;
  c_model->prior   = -1;
  c_model->s       = NULL;
  c_model->model_type = 0;
  c_model->silent  = NULL;
}


void GHMM_SWDiscreteModel::init(int number_of_states, int my_M, double my_prior) {
  init();

  int i;
  int j;

  c_model = (sdmodel*) calloc(1,sizeof(sdmodel));
  if (!c_model) {
    fprintf(stderr,"GHMM_SWDiscreteModel::GHMM_SWDiscreteModel() could not allocate c_model\n");
    exit(1);
  }
  c_model->model_type = 0;
  c_model->N       = number_of_states;
  c_model->M       = my_M;
  c_model->prior   = my_prior;
  c_model->cos     = no_classes;
  c_model->s       = (sdstate*) malloc(sizeof(sdstate) * max(c_model->N,1));
  /* initialize all states. */
  c_model->silent  = NULL;

  for (i = 0; i < number_of_states; ++i) {
    c_model->s[i].pi         = 0;
    c_model->s[i].b          = (double*) malloc(sizeof(double) * my_M);
    /* output probabilities are initialized with 0. */
    for (j = 0; j < my_M; ++j)
      c_model->s[i].b[j] = 0;
    c_model->s[i].out_id     = NULL;
    c_model->s[i].in_id      = NULL;
    c_model->s[i].out_a      = NULL;
    c_model->s[i].in_a       = NULL;
    c_model->s[i].out_states = 0;
    c_model->s[i].in_states  = 0;
    c_model->s[i].fix        = 0;
  }

  buildCppData();
}


void GHMM_SWDiscreteModel::DFSVisit(int nodev, int &timevisit, int *parents, DFSCOLORS *colors)
{
  int i,w;
  colors[nodev]=GRAY;
  ++timevisit;
  for(i=0; i < c_model->s[nodev].out_states; i++) { 
    w=c_model->s[nodev].out_id[i]; // Explore edge (v,w)
    if ( edge_classes[nodev][w] == NONE ) { // First exploration
      edge_classes[nodev][w] = colors[w];
      //fprintf(stderr, " %d edge (%s, %s)\n", (int) colors[w], c_model->s[nodev].label, 
      //	      c_model->s[w].label);
      fflush(stderr);
    }
    if ( colors[w] == WHITE ) {
      parents[w]=nodev;
      DFSVisit(w, timevisit, parents, colors);
    } 
  }
  colors[nodev]=BLACK; // finished  
  ++timevisit;
}

void GHMM_SWDiscreteModel::DFS()
{
  int i,j;
  DFSCOLORS *colors= new DFSCOLORS[c_model->N];
  int *parents = new int[c_model->N];
  edge_classes = new DFSCOLORS*[c_model->N];

  for(i=0; i < c_model->N; i++) {
    colors[i]=WHITE;
    parents[i]=-1;
    edge_classes[i]= new DFSCOLORS[c_model->N];
    for(j=0; j < c_model->N; j++) {
      edge_classes[i][j] = NONE;
    }
  }

  int timevisit=0;
  for(i=0; i < c_model->N; i++) {
    if ( colors[i] == WHITE ) {
      DFSVisit(i,timevisit,parents, colors);
    }
  }

  delete colors;
  delete parents;
}

void GHMM_SWDiscreteModel::topological_sort()
{
  /*
   * Topological sort with cycle detection (WR)
   *
   * Calculate indegrees for nodes (using Map from node-key to int in-degree).
   * Create a queue q and enqueue all nodes of indegree 0.
   * Create an empty list t for top-sorted nodes
   * Set loopcount to 0                           
   * Loop while queue is not empty
   *     Increment loopcount 
   *     Dequeue from q a node v and append it to list t
   *     Loop over nodes w adjacent to v
   *         Decrement w's indegree (i.e. decrement value in Map)
   *         If w's indegree is 0, enqueue w in q
   * If loopcount < number of nodes in graph, return cycle-in-graph  
   * Return top-sorted list t
   *
   */

  std::queue< int > toporder_queue;
  std::vector< int > toporder;

  if (c_model->model_type == kSilentStates) {
    cerr << "GHMM_SWDiscreteModel: GHMM will order silent states" << endl;
  } else
    return;

  int i,j,v,w;
  int loopcount  = 0;
  int *indegrees = new int[c_model->N];

  DFS(); // Classify edges as tree or back-edges
  for(i=0; i < c_model->N; i++) {
    indegrees[i]=c_model->s[i].in_states;
  }
  for(i=0; i < c_model->N; i++) { // don't consider back edges in top sort 
    for(j=0; j < c_model->N; j++) {
      if (edge_classes[i][j] == GRAY) {
	indegrees[j]--;
	fprintf(stderr, "BACK edge (%s, %s)\n", c_model->s[i].label, c_model->s[j].label);
      }
    }
  }
  // Create a queue q and enqueue all nodes of indegree 0.
  for(i=0; i < c_model->N; i++) {
    if ( indegrees[i] == 0 ) toporder_queue.push(i);
  }

  loopcount = 0;
  while( !toporder_queue.empty() ) {
    loopcount++;
    v=toporder_queue.front(); 
    toporder_queue.pop();  // dequeue a node v
    toporder.push_back(v);     // append it to the list
    for(i=0; i < c_model->s[v].out_states; i++) {
      w=c_model->s[v].out_id[i];
      if ( edge_classes[v][w] != GRAY ) {
	indegrees[w]--;
	if (w != v && indegrees[w]==0) {
	  toporder_queue.push(w);
	}
      }
    }
  }

  if ( c_model->topo_order != NULL ) {
    free(c_model->topo_order);
    c_model->topo_order=NULL;
    c_model->topo_order_length=0;
  }
  vector< int > dels_order;
  cout << "Topological ordering of DELs:";
  for(i=0; i < toporder.size(); i++) {
    if ( c_model->silent[toporder[i]] ) {
      cout << c_model->s[toporder[i]].label << " ,";
      dels_order.push_back(toporder[i]);
    }
  }
  // copying ordering of silent states into struct sdmodel
  c_model->topo_order_length=dels_order.size();
  if (!(c_model->topo_order=(int*)malloc(dels_order.size()*sizeof(int)))) {
    fprintf(stderr, "GHMM_SWDiscreteModel: Cannot allocate memory in heap\n");
    return;
  }
  for(i=0; i < c_model->topo_order_length; i++) {
    c_model->topo_order[i]=dels_order[i];
  }
  cout << endl;
  if ( loopcount < c_model->N ) {
    cerr << "!!! Cycle detection !!!!" << endl;
  }
  delete indegrees;
}


/* Returns state with given index. */
sdstate* GHMM_SWDiscreteModel::getCState(int index) const
{
  if (index >= c_model->N) {
    fprintf(stderr,"GHMM_SWDiscreteModel::getCState(int):\n");
    fprintf(stderr,"State no. %d does not exist. Model has %d states.\n",index,c_model->N);
    exit(1);
  }  
  return &c_model->s[index]; 
}


model *GHMM_SWDiscreteModel::create_cmodel(int kclass) {
  model *new_model;
  if ( kclass <  c_model->cos ) {
    new_model = sdmodel_to_model(c_model, kclass);
    return new_model;
  }
  return NULL;
}


GHMM_IntVector* GHMM_SWDiscreteModel::viterbi(GHMM_Sequences* sequences, int index, double* log_p) {
  double my_logp;

  if (!log_p)
    log_p = &my_logp;

  int len = sequences->getLength(index);
  int *seq = sdviterbi(c_model,sequences->getIntSequence(index),len,log_p);
  return new GHMM_IntVector(seq,len);
}


void GHMM_SWDiscreteModel::XMLIO_finishedReading() {
  unsigned int i;

  if (c_model == NULL)
    {
      c_model = (sdmodel*) calloc(1,sizeof(sdmodel));

      if (!c_model) {
	fprintf(stderr,"GHMM_ContinuousModel::XMLIO_finishedReading() could not allocate c_model\n");
	exit(1);
      }
    }

  int alphabet_size = 0;
  if (getAlphabet())
    alphabet_size = getAlphabet()->size();
  else
    alphabet_size = states[0]->emission->weights.size();

  c_model->N       = states.size();
  c_model->M       = alphabet_size;
  c_model->prior   = -1;
  c_model->s       = (sdstate*) malloc(sizeof(sdstate) * c_model->N);
  c_model->cos     = no_classes;

  /* fill states. */
  for (i = 0; i < states.size(); ++i)
    states[i]->fillState(&c_model->s[i]);

  /* check whether sum of initial probs is 1. */
  //if (check() == -1)  to allow silent states
  //  exit(1);

  /* we dont need the transitions any more. */
  cleanTransitions();
}

GHMM_Alphabet *GHMM_SWDiscreteModel::getAlphabet() const
{
  return alphabet;
}

GHMM_ModelType GHMM_SWDiscreteModel::getModelType() const {
  return GHMM_DISCRETE;
}

GHMM_SWDiscreteModel* GHMM_SWDiscreteModel::copy()
{ return new GHMM_SWDiscreteModel(sdmodel_copy(c_model)); }

void GHMM_SWDiscreteModel::createTransitions() {
  unsigned int i;
  int edge;

  for (i = 0; i < states.size(); ++i)
    {
      printf("GHMM_SWDiscreteModel::createTransitions: %d out-going\n", states[i]->getOutEdges());
      for (edge = 0; edge < states[i]->getOutEdges(); ++edge)
      {
	transitions.push_back(states[i]->createTransition(edge, c_model));
      }
    }
}

XMLIO_Element* GHMM_SWDiscreteModel::XMLIO_startTag(const string& tag, XMLIO_Attributes &attrs) 
{
  if ( node_tag == "node" && edge_tag == "edge" ) {
    if (tag == node_tag) {
      GHMM_GMLState* ghmm_state = new GHMM_GMLState(this,states.size(),attrs);
      states.push_back(ghmm_state);
    
      state_by_id[ghmm_state->id] = states.size() - 1;
      
      /* Pass all nested elements to this state. */
      return ghmm_state;
    }

    if (tag == edge_tag ) {
      GHMM_GMLTransition* transition = new GHMM_GMLTransition(attrs);
      transitions.push_back(transition);
    
      return transition;
    }
  }
  else
  {
	  fprintf(stderr,"XMLIO_startTag: wrong tag %s found \n", tag.c_str());
	  exit(-1);
  }
  return NULL;
}

const int GHMM_SWDiscreteModel::XMLIO_writeContent(XMLIO_Document& writer) 
{
  int total_bytes = 0;
  int result;
  unsigned int i;

  writer.changeIndent(2);
  result = writer.writeEndl();

  if (result < 0)
    return result;
  total_bytes += result;


  /* write states */
  for (i = 0; i < states.size(); ++i) {
    if (i > 0) {
      result = writer.writeEndl();

      if (result < 0)
	return result;
      total_bytes += result;
    }

    result = writer.writeElement(states[i]);
  
    if (result < 0)
      return result;
    total_bytes += result;

    result = writer.writeEndl();

    if (result < 0)
      return result;
    total_bytes += result;
  }

  
  /* write transitions */

  createTransitions();
  for (i = 0; i < transitions.size(); ++i) {
    writer.writeEndl();
    writer.writeElement(transitions[i]);
    writer.writeEndl();
  }
  cleanTransitions();

  return 0;
}

