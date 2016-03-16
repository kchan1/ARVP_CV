#include "LinkedList.hpp"
#include <gsl/gsl_blas.h>
#include <iostream>
int main(int argc,char*argv[])
{
  std::cout<<"--Testing basic Linked List functions--\n";

  gsl_matrix * m0 = gsl_matrix_calloc(1,1);
  gsl_matrix * m1 = gsl_matrix_calloc(2,2);
  gsl_matrix * m2 = gsl_matrix_calloc(3,3);
  gsl_matrix * m3 = gsl_matrix_calloc(4,4);

  LinkedList<gsl_matrix>* list = new LinkedList<gsl_matrix>();
  list->push(m0);
  list->push(m1);
  list->push(m2);
  list->push(m3);
  while(list->hasNext())
  {
    gsl_matrix* e = list->pop();
    printf("Matrix size (%lu,%lu)\n",e->size2,e->size1);
  }
  gsl_matrix_free(m0);
  gsl_matrix_free(m1);
  gsl_matrix_free(m2);
  gsl_matrix_free(m3);
}
