#include "LinkedList.hpp"
#include <gsl/gsl_blas.h>
#include <iostream>
int main(int argc,char*argv[])
{
  std::cout<<"--Testing basic Linked List functions--\n";

  //gsl_matrix * m0 = gsl_matrix_calloc(1,1);
  //gsl_matrix * m1 = gsl_matrix_calloc(2,2);
  //gsl_matrix * m2 = gsl_matrix_calloc(3,3);
  //gsl_matrix * m3 = gsl_matrix_calloc(4,4);
  int* m0 = new int(0);
  int* m1 = new int(1);
  int* m2 = new int(2);
  int* m3 = new int(3);
  
  //LinkedList<gsl_matrix>* list = new LinkedList<gsl_matrix>();
  LinkedList<int>* list = new LinkedList<int>();
  list->push(m0);
  list->push(m1);
  list->push(m2);
  list->push(m3);
  while(list->hasNext())
  {
    //gsl_matrix* e = list->pop();
    //printf("Matrix size (%lu,%lu)\n",e->size2,e->size1);
    int* val = list->pop();
    printf("Popped: %i\n",*val);
  }
  
  //gsl_matrix_free(m0);
  //gsl_matrix_free(m1);
  //gsl_matrix_free(m2);
  //gsl_matrix_free(m3);
  delete m0;
  delete m1;
  delete m2;
  delete m3;
  delete list;
}
