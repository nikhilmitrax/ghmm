/*
  author: Achim Gaedke
  created: 9. Juli 2001
  file: xmlio/examples/ghmm++/ghmm_alphabet.cpp
  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include "ghmm_alphabet.h"

#ifdef HAVE_NAMESPACES
using namespace std;
#endif

ghmm_alphabet::ghmm_alphabet():
  XMLIO_ElementArrayElement<ghmm_symbol>()
{
  init_members();
}

ghmm_alphabet::ghmm_alphabet(const string& tag, XMLIO_Attributes &attributes):
  XMLIO_ElementArrayElement<ghmm_symbol>(tag,attributes)
{
  init_members();
}

const char* ghmm_alphabet::toString() const
{
  return "ghmm_alphabet";
}

void ghmm_alphabet::print() const
{
  cout<<toString()<<":"<<endl;
  for (XMLIO_ElementArrayElement<ghmm_symbol>::const_iterator pos=begin();pos!=end();++pos)
    if (*pos!=NULL)
      (*pos)->print();

}

void ghmm_alphabet::init_members()
{
  set_element_name("Symbol");
}


