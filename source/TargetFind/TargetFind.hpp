#ifndef _TARGETFIND_HPP_
#define _TARGETFIND_HPP_
#include "../imgformat.hpp"
#include "../LinkedList.hpp"
#include <gsl/gsl_blas.h>
#include "TargetData.hpp"
#include <iostream>
const int RED_THRES = 100;
const int GRN_THRES = 200;
const int BLU_THRES = 150;

LinkedList<TargetData> getTargets(gsl_matrix* img)
{
  size_t i,j;
  gsl_matrix* img_buff = gsl_matrix_calloc(img->size1,img->size2);
  double px_buff;
  unsigned long current_px;
  //j scans down the rows, i scans down the columns
  for(j=0;j<img->size1;j++)
    for(i=0;i<img->size2;i++)
    {
      ulong newPx = 0;
      //red threshold
      if(getChanR(gsl_matrix_get(img,j,i))>=RED_THRES)
	newPx |= 0xFF<<OFFSET_R;
      //green threshold
      if(getChanG(gsl_matrix_get(img,j,i))>=GREEN_THRES)
	newPx |= 0xFF<<OFFSET_G;
      //blue threshold
      if(getChanG(gsl_matrix_get(img,j,i))>=GREEN_THRES)
	newPx |= 0xFF<<OFFSET_B;
      //write into the buffer
      gsl_matrix_set(img_buff,j,i,newPx);
    }
  //do canny edge detection
  //use a 5x5 to blur, stdev = 1 
  gsl_matrix*gauss = gsl_matrix_alloc(5,1);
  gaussian(gauss,2);
  //blur the red
  convolution(img_buff,gauss,7/2,7/2,CHAN_R);
  //blur the green
  convolution(img_buff,gauss,7/2,7/2,CHAN_G);
  //blur the blue 
  convolution(img_buff,gauss,7/2,7/2,CHAN_B);
  //deallocate the gaussian filter because it never gets used again
  gsl_matrix_free(gausXSs);
  //flatten back to grayscale
  for(j=0;j<img_buff->size1;j++)
    for(i=0;i<img_buff->size2;i++)
    {
      px_buff = 0;
      current_px = (unsigned long)gsl_matrix_get(img_buff,j,i);
      px_buff += 0.2989*((current_px & CHAN_R) >> OFFSET_R);
      px_buff += 0.5870*((current_px & CHAN_G) >> OFFSET_G);
      px_buff += 0.1140*((current_px & CHAN_B) >> OFFSET_B);
      //For the sake of convenience, store intensity in Red
      gsl_matrix_set(img_buff,j,i,(((double)px_buff)&0xFF)<<OFFSET_R);
    }
  //find gradient
  
  gsl_matrix * derivX = gsl_matrix_calloc(3,3);
  derivX = 
  gsl_matrix * derivY = gsl_matrix_calloc(3,3);
  //store Gx in Green, Gy in Blue
  
  //store (a scaled) theta in Data

  //find boundaries in list
  //for every boundary...
  //  calculate area and perimeter to estimate roundness
  gsl_matrix_free(gauss7_2);
}

#endif
