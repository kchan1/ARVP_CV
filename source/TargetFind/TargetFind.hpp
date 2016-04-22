#ifndef _TARGETFIND_HPP_
#define _TARGETFIND_HPP_
#include "../imgformat.hpp"
#include "../LinkedList.hpp"
#include <gsl/gsl_blas.h>
#include "TargetData.hpp"
#include <iostream>

LinkedList<TargetData> getTargets(ARVP_img* img)
{
  //do a sobel detector
  //do a 50x50 convo
  
  //for every boundary...
  //  calculate area and perimeter to estimate roundness
  return NULL;
}


#endif
