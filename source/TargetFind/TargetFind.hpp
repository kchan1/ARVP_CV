#ifndef _TARGETFIND_HPP_
#define _TARGETFIND_HPP_
#include "../imgformat.hpp"
#include "../LinkedList.hpp"
#include <gsl/gsl_blas.h>
#include "TargetData.hpp"
#include <iostream>
LinkedList<TargetData> getTargets(ARVP_img* img)
{
  //do canny edge detection
  //find boundaries in list
  
  //for every boundary...
  //  calculate area and perimeter to estimate roundness
  return NULL;
}


#endif
