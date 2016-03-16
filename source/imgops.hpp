#ifndef _IMAGEOPS_HPP_
#define _IMAGEOPS_HPP_
#include "imgformat.hpp"
#include <math.h>
#include <gsl/gsl_blas.h>

void gaussian(gsl_matrix*filter,float stdev)
{
  unsigned int i,j;
  float org_x,org_y;
  double value = 0;
  org_x = filter->size1/2;
  org_y = filter->size2/2;
  for(j=0;j<filter->size1;j++)
    for(i=0;i<filter->size2;i++)
    {
      value = 1.0/(2.0*M_PI*pow(stdev,2))
	*exp(-(pow(i-org_x,2)+
	       pow(j-org_y,2))/(2.0*pow(stdev,2)));
      //printf("GAUSS VALUE (%i,%i): %f",i,j,value);
      gsl_matrix_set(filter,j,i,value);
    }
}

void convolution(gsl_matrix* img, gsl_matrix*filter, int filter_x, int filter_y,ulong channel)
{
  size_t i,j,u,v;
  //create a buffer to store convolution results
  gsl_matrix* img_buff = gsl_matrix_calloc(img->size1,img->size2);
  //j scans down the rows, i scans across the columns
  for(j=0;j<img->size1;j++)
    for(i=0;i<img->size2;i++)
    {
      double newPx = 0;
      //v scans down the rows, u scans across the columns
      for(v=0;v<filter->size1;v++)
	for(u=0;u<filter->size2;u++)
	{
	  //apply filter element to image element
	  newPx += ((unsigned long)getBoundPixel(img,
				  j-filter->size1/2+v,
				  i-filter->size2/2+u)
		    &channel)
	    *gsl_matrix_get(filter,v,u);
	}
      gsl_matrix_set(img_buff,j,i,newPx);
    }
  //copy back in the buffer
  for(j=0;j<img->size1;j++)
    for(i=0;i<img->size2;i++)
    {
      gsl_matrix_set(img,j,i,(unsigned long)gsl_matrix_get(img_buff,j,i));
    }
  return;
}

#endif
