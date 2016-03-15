#ifndef _IMAGEOPS_HPP_
#define _IMAGEOPS_HPP_
#include "imgformat.hpp"
void convolution(gsl_matrix* img, gsl_matrix*filter, int filter_x, int filter_y,ulong channel)
{
  size_t i,j,u,v;
  gsl_matrix* img_buff = gsl_matrix_calloc(img->size1,img->size2);
  //j scans down the rows, i scans across the columns
  for(j=0;j<img->size1;j++)
    for(i=0;i<img->size2;i++)
    {
      float newPx = 0;
      for(v=0;v<filter->size1;v++)
	for(u=0;u<filter->size2;u++)
	{
	  //apply filter elements
	  newPx += (getBoundPixel(img_buff,
				 j-filter->size1/2+v,
				  i-filter->size2/2+u)
		    |channel)
	    *gsl_matrix_get(filter,v,u);
	}
      gsl_matrix_set(img_buff,j,i,ulong(newPx));
    }
  for(j=0;j<img->size1;j++)
    for(i=0;i<img->size2;i++)
    {
      gsl_matrix_set(img,j,i,gsl_matrix_get(img_buff,j,i));
    }
  return;
}
#endif
