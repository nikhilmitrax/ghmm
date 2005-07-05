/*
 * created: 21 Jan 2002 by Peter Pipenbacher
 * authors: Peter Pipenbacher (pipenb@zpr.uni-koeln.de)
 * file   : $Source$
 * $Id$
 *
 * __copyright__
 */

#include <xmlio/XMLIO_Definitions.h>
#include <xmlio/XMLIO_Document.h>
#include "ghmm/matrix.h"
#include "ghmm++/GHMM_Types.h"
#include "ghmm++/GHMM_State.h"
#include "ghmm++/GHMM_Transition.h"
#include "ghmm++/GHMM_ContinuousModel.h"
#include "ghmm++/GHMM_DiscreteModel.h"
#include "ghmm++/GHMM_Emission.h"
#include "ghmm++/GHMM_Toolkit.h"
#include "ghmm++/GHMM_Alphabet.h"

#include <iostream>

#ifdef HAVE_NAMESPACES
using namespace std;
#endif


GHMM_State::GHMM_State(GHMM_AbstractModel* my_model, int my_index, XMLIO_Attributes& attrs) {
  index             = my_index;
  c_state           = NULL; 
  c_sstate          = NULL;
  reading           = GHMM_STATE_NONE;
  cemission          = NULL;
  demission          = NULL;
  parent_model      = my_model;
  tag               = "state";
  xmlio_indent_type = XMLIO_INDENT_BOTH;

  /* by default take index as id. */
  id = attrs["id"];
  if (id == "")
    id = GHMM_Toolkit::toString(my_index);

  my_model->addStateID(id,index);

  attributes = attrs;
}


GHMM_State::GHMM_State(GHMM_AbstractModel* my_model, int my_index, sstate* my_state) {
  index             = my_index;
  c_sstate          = my_state;
  c_state           = NULL;
  c_sdstate          = NULL;
  reading           = GHMM_STATE_NONE;
  cemission          = NULL;
  demission          = NULL;
  parent_model      = my_model;
  tag               = "state";
  xmlio_indent_type = XMLIO_INDENT_BOTH;
  
  /* take index as id. */
  id = GHMM_Toolkit::toString(my_index);
  my_model->addStateID(id,index);
}


GHMM_State::GHMM_State(GHMM_AbstractModel* my_model, int my_index, state* my_state) {
  index             = my_index;
  c_sstate          = NULL;
  c_state           = my_state;
  c_sdstate         = NULL;
  reading           = GHMM_STATE_NONE;
  cemission          = NULL;
  demission          = NULL;
  parent_model      = my_model;
  tag               = "state";
  xmlio_indent_type = XMLIO_INDENT_BOTH;
  
  /* take index as id. */
  id = GHMM_Toolkit::toString(my_index);
  my_model->addStateID(id,index);
}


GHMM_State::GHMM_State(GHMM_AbstractModel* my_model, int my_index, sdstate* my_state) {
  index             = my_index;
  c_sstate          = NULL;
  c_state           = NULL;
  c_sdstate         = my_state;
  reading           = GHMM_STATE_NONE;
  demission          = NULL;
  cemission          = NULL;
  parent_model      = my_model;
  tag               = "state";
  xmlio_indent_type = XMLIO_INDENT_BOTH;
  
  /* take index as id. */
  id = GHMM_Toolkit::toString(my_index);
  my_model->addStateID(id,index);
}


GHMM_State::~GHMM_State() {
  SAFE_DELETE(cemission);
  SAFE_DELETE(demission);
}


const char* GHMM_State::toString() const {
  return "GHMM_State";
}


XMLIO_Element* GHMM_State::XMLIO_startTag(const string& tag, XMLIO_Attributes &attrs) {
  
  if (tag == "initial") {
    reading = GHMM_STATE_INITIAL;    
    return this;
  }

  if (tag == "emission") {

    if (getModelType() == GHMM_CONTINOUS) {
      SAFE_DELETE(cemission);
      return (cemission = new GHMM_CEmission(this));
    }
    if (getModelType() == GHMM_DISCRETE) {
      SAFE_DELETE(demission);
      return (demission = new GHMM_DEmission(this));
    }
  }

  fprintf(stderr,"tag '%s' not recognized in state element\n",tag.c_str());
  exit(1);
  
  return NULL;
}


void GHMM_State::XMLIO_endTag(const string& tag) {
  reading = GHMM_STATE_NONE;
}


void GHMM_State::XMLIO_getCharacters(const string& characters) {
  switch (reading) {

  case GHMM_STATE_INITIAL:
    initial = atof(characters.c_str());
    break;
    
  case GHMM_STATE_NONE:
    break;
  }
}


void GHMM_State::fillState(sstate* s) {
  GHMM_ContinuousModel* model = (GHMM_ContinuousModel*) parent_model;
  smodel* c_model             = model->c_model;

  /* store current c representation of state. */
  c_sstate = s;

  s->c   = (double*) malloc(sizeof(double) * c_model->M);
  s->mue = (double*) malloc(sizeof(double) * c_model->M);
  s->u   = (double*) malloc(sizeof(double) * c_model->M);

  vector<GHMM_Transition*> out_edges;
  vector<GHMM_Transition*> in_edges;
  unsigned int i;
  for (i = 0; i < model->transitions.size(); ++i) {
    if (model->transitions[i]->source == id)
      out_edges.push_back(model->transitions[i]);
    if (model->transitions[i]->target == id)
      in_edges.push_back(model->transitions[i]);
  }

  if (out_edges.size() > 0) {
    s->out_id = (int*) malloc(sizeof(int) * out_edges.size());
    s->out_a  = matrix_d_alloc(c_model->cos, out_edges.size());
  }

  if (in_edges.size() > 0) {
    s->in_id = (int*) malloc(sizeof(int) * in_edges.size());
    s->in_a  = matrix_d_alloc(c_model->cos, in_edges.size());
  }

  /* now fill with useful data. */
  s->pi = initial;

  for (i = 0; i < out_edges.size(); ++i)
    s->out_id[i] = model->getStateIndex(out_edges[i]->target);

  for (i = 0; i < in_edges.size(); ++i)
    s->in_id[i] = model->getStateIndex(in_edges[i]->source);

  int cos;
  for (cos = 0; cos < c_model->cos; ++cos) {
    for (i = 0; i < out_edges.size(); ++i)
      s->out_a[cos][i] = out_edges[i]->prob;

    for (i = 0; i < in_edges.size(); ++i)
      s->in_a[cos][i] = in_edges[i]->prob;
  }

  s->out_states = out_edges.size();
  s->in_states  = in_edges.size();

  if (c_model->M != 1) {
    fprintf(stderr,"M != 1 not yet supported in GHMM_State.cpp\n");
    exit(1);
  }

  
  if (c_model->M != (int) cemission->weights.size()) {
    fprintf(stderr,"M == %d, but just %d weights found in GHMM_State.cpp\n",c_model->M,(int) cemission->weights.size());
    exit(1);
  }

  for (i = 0; (int) i < c_model->M; ++i) {
    s->c[i]   = cemission->weights[i];
    s->mue[i] = cemission->mue[i];
    s->u[i]   = cemission->variance[i];
  }

  s->fix = 0;
}


void GHMM_State::fillState(state* s) {
  GHMM_DiscreteModel* m = (GHMM_DiscreteModel*) parent_model;
  model* c_model        = m->c_model;

  /* store current c representation of state. */
  c_state = s;

  s->b   = (double*) malloc(sizeof(double) * c_model->M);

  vector<GHMM_Transition*> out_edges;
  vector<GHMM_Transition*> in_edges;
  unsigned int i;
  for (i = 0; i < m->transitions.size(); ++i) {
    if (m->transitions[i]->source == id)
      out_edges.push_back(m->transitions[i]);
    if (m->transitions[i]->target == id)
      in_edges.push_back(m->transitions[i]);
  }

  if (out_edges.size() > 0) {
    s->out_id = (int*) malloc(sizeof(int) * out_edges.size());
    s->out_a  = (double*) malloc(sizeof(double) * out_edges.size());
  }

  if (in_edges.size() > 0) {
    s->in_id = (int*) malloc(sizeof(int) * in_edges.size());
    s->in_a  = (double*) malloc(sizeof(double) * in_edges.size());
  }

  /* now fill with useful data. */
  s->pi = initial;

  for (i = 0; i < out_edges.size(); ++i)
    s->out_id[i] = m->getStateIndex(out_edges[i]->target);

  for (i = 0; i < in_edges.size(); ++i)
    s->in_id[i] = m->getStateIndex(in_edges[i]->source);

  for (i = 0; i < out_edges.size(); ++i)
    {
      s->out_a[i] = out_edges[i]->prob;
      // printf( "\t State %d, out %g\n", i, s->out_a[i]);
    }

  for (i = 0; i < in_edges.size(); ++i)
    {
      s->in_a[i] = in_edges[i]->prob;
      // printf( "\t State %d, in %g\n", i, s->out_a[i]);
    }

  s->out_states = out_edges.size();
  s->in_states  = in_edges.size();

  if (c_model->M != (int) demission->weights.size()) {
    fprintf(stderr,"M == %d, but just %d weights found in GHMM_State.cpp\n",c_model->M,(int) demission->weights.size());
    exit(1);
  }

  for (i = 0; (int) i < c_model->M; ++i)
    s->b[i] = demission->weights[i];

  s->fix = 0;
}

void GHMM_State::fillState(sdstate* s) {
  fprintf(stderr, "GHMM_State::fillState. You should not be here\n");
  exit(1);
}

void GHMM_State::XMLIO_finishedReading() {
  if (!demission && !cemission) {
    fprintf(stderr,"state with id='%s' lacks emission element.\n",id.c_str());
    exit(1);
  }
}


void GHMM_State::changeOutEdge(int target, double prob) {
  changeOutEdge(0,target,prob);
}


void GHMM_State::changeInEdge(int target, double prob) {
  changeInEdge(0,target,prob);
}


void GHMM_State::changeOutEdge(int matrix_index, int target, double prob) {
  if (prob == 0) {
    removeOutEdge(target);
    return;
  }
  
  int i;
  if (c_sstate) {
    for (i = 0; i < c_sstate->out_states; ++i) {
      if (c_sstate->out_id[i] == target) {
	c_sstate->out_a[matrix_index][i] = prob;
	return;
      }
    }

    /* create new state. */
    c_sstate->out_states += 1;
    c_sstate->out_id = (int*) realloc(c_sstate->out_id,sizeof(int) * c_sstate->out_states);
    c_sstate->out_id[c_sstate->out_states - 1] = target;

    for (i = 0; i < parent_model->getNumberOfTransitionMatrices(); ++i) {
      c_sstate->out_a[i] = (double*) realloc(c_sstate->out_a[i],sizeof(double) * c_sstate->out_states);
      c_sstate->out_a[i][c_sstate->out_states - 1]  = 0;
    }
    c_sstate->out_a[matrix_index][c_sstate->out_states - 1]  = prob;
  }

  if (c_state) {
    for (i = 0; i < c_state->out_states; ++i)
      if (c_state->out_id[i] == target) {
	c_state->out_a[i] = prob;
	return;
      }

    /* create new state. */
    c_state->out_states += 1;
    c_state->out_id = (int*) realloc(c_state->out_id,sizeof(int) * c_state->out_states);
    c_state->out_id[c_state->out_states - 1] = target;
    
    c_state->out_a = (double*) realloc(c_state->out_a,sizeof(double) * c_state->out_states);
    c_state->out_a[c_state->out_states - 1] = prob;
  }
}


void GHMM_State::removeOutEdge(int target) {
  int i;

  if (c_sstate)
    for (i = 0; i < c_sstate->out_states; ++i)
      if (c_sstate->out_id[i] == target) {
	c_sstate->out_id[i] = c_sstate->out_id[c_sstate->out_states - 1];
	for (int m = 0; m < parent_model->getNumberOfTransitionMatrices(); ++m)
	  c_sstate->out_a[m][i]  = c_sstate->out_a[m][c_sstate->out_states - 1];
	c_sstate->out_states -= 1;
	return;
      }

  if (c_state)
    for (i = 0; i < c_state->out_states; ++i)
      if (c_state->out_id[i] == target) {
	c_state->out_id[i] = c_state->out_id[c_state->out_states - 1];
	c_state->out_a[i]  = c_state->out_a[c_state->out_states - 1];
	c_state->out_states -= 1;
	return;
      }
}


void GHMM_State::removeInEdge(int source) {
  int i;

  if (c_sstate)
    for (i = 0; i < c_sstate->in_states; ++i)
      if (c_sstate->in_id[i] == source) {
	c_sstate->in_id[i] = c_sstate->in_id[c_sstate->in_states - 1];
	for (int m = 0; m < parent_model->getNumberOfTransitionMatrices(); ++m)
	  c_sstate->in_a[m][i]  = c_sstate->in_a[m][c_sstate->in_states - 1];
	c_sstate->in_states -= 1;
	return;
      }

  if (c_state)
    for (i = 0; i < c_state->in_states; ++i)
      if (c_state->in_id[i] == source) {
	c_state->in_id[i] = c_state->in_id[c_state->in_states - 1];
	c_state->in_a[i]  = c_state->in_a[c_state->in_states - 1];
	c_state->in_states -= 1;
	return;
      }
}


void GHMM_State::changeInEdge(int matrix_index, int source, double prob) {
  if (prob == 0) {
    removeInEdge(source);
    return;
  }
  
  int i;
  if (c_sstate) {
    for (i = 0; i < c_sstate->in_states; ++i)
      if (c_sstate->in_id[i] == source) {
	c_sstate->in_a[matrix_index][i] = prob;
	return;
      }
    
    /* create new state. */
    c_sstate->in_states += 1;
    c_sstate->in_id = (int*) realloc(c_sstate->in_id,sizeof(int) * c_sstate->in_states);
    c_sstate->in_id[c_sstate->in_states - 1] = source;
    
    for (i = 0; i < parent_model->getNumberOfTransitionMatrices(); ++i) {
      c_sstate->in_a[i] = (double*) realloc(c_sstate->in_a[i],sizeof(double) * c_sstate->in_states);
      c_sstate->in_a[i][c_sstate->in_states - 1]  = 0;
    }
    c_sstate->in_a[matrix_index][c_sstate->in_states - 1]  = prob;
  }

  if (c_state) {
    for (i = 0; i < c_state->in_states; ++i)
      if (c_state->in_id[i] == source) {
	c_state->in_a[i] = prob;
	return;
      }
    
    /* create new state. */
    c_state->in_states += 1;
    c_state->in_id = (int*) realloc(c_state->in_id,sizeof(int) * c_state->in_states);
    c_state->in_id[c_state->in_states - 1] = source;
    
    c_state->in_a = (double*) realloc(c_state->in_a,sizeof(double) * c_state->in_states);
    c_state->in_a[c_state->in_states - 1] = prob;
  }
}


void GHMM_State::setOutputProbability(int index, double prob) {
  if (!c_state) {
    fprintf(stderr,"GHMM_State::setOutputProbability(int,double): object does not contain state.\n");
    exit(1);
  }
   
  GHMM_DiscreteModel* m = (GHMM_DiscreteModel*) parent_model;
  if (index >= m->c_model->M) {
    fprintf(stderr,"GHMM_State::setOutputProbability(int,double): symbol No. %d requested, but maximum number is %d.\n",index,m->c_model->M - 1);
    exit(1);
  }

  c_state->b[index] = prob;
}


void GHMM_State::setOutputProbability(const string& id, double prob) {
  setOutputProbability(((GHMM_DiscreteModel*)(parent_model))->alphabet->getIndex(id),prob);
}


void GHMM_State::setInitialProbability(double prob) {
  if (c_state)
    c_state->pi = prob;

  if (c_sstate)
    c_sstate->pi = prob;
}


XMLIO_Attributes& GHMM_State::XMLIO_getAttributes() {
  attributes["id"] = id;

  return attributes;
}


const int GHMM_State::XMLIO_writeContent(XMLIO_Document& writer) {
  int total_bytes = 0;
  int result;
  
  double initial = getInitial();

  writer.changeIndent(2);

  //  if (initial > 0) {
  result = writer.writeEndlIndent();
  
  if (result < 0)
    return result;
  total_bytes += result;
  
  result = writer.writef("<initial>%f</initial>",initial);
  
  if (result < 0)
    return result;
  total_bytes += result;
  //  }

  /* write emission */
  result = writer.writeEndl();
  
  if (result < 0)
    return result;
  total_bytes += result;
  
  if (getModelType() == GHMM_CONTINOUS) {
    GHMM_CEmission* my_emission = new GHMM_CEmission(this);
    result = writer.writeElement(my_emission);
    SAFE_DELETE(my_emission);
  }

  if (getModelType() == GHMM_DISCRETE) {
    GHMM_DEmission* my_emission = new GHMM_DEmission(this);
    result = writer.writeElement(my_emission);
    SAFE_DELETE(my_emission);
  }

  if (result < 0)
    return result;
  total_bytes += result;
  /* end write emission */

  result = writer.writeEndl();

  if (result < 0)
    return result;
  total_bytes += result;

  return total_bytes;
}


double GHMM_State::getInitial() const {
  if (c_sstate)
    return c_sstate->pi;

  if (c_state)
    return c_state->pi;

  fprintf(stderr,"%s::getInitial() state is empty\n",toString());
  exit(1);

  return 0;
}


GHMM_Transition* GHMM_State::createTransition(int edge_index) {
  if (c_state)
    return new GHMM_Transition(this,parent_model->getState(c_state->out_id[edge_index]),c_state->out_a[edge_index]);

  if (c_sstate)
    return new GHMM_Transition(this,parent_model->getState(c_sstate->out_id[edge_index]),c_sstate->out_a[0][edge_index]);

  return NULL;
}


void GHMM_State::setID(const string& my_id) {
  parent_model->stateIDChanged(id,my_id);

  id               = my_id;
  attributes["id"] = my_id;
}


GHMM_AbstractModel* GHMM_State::getModel() const {
  return parent_model;
}


int GHMM_State::getOutEdges() const {
  if (c_sstate)
    return c_sstate->out_states;

  if (c_state)
    return c_state->out_states;

  return 0;
}


GHMM_ModelType GHMM_State::getModelType() const {
  return getModel()->getModelType();
}