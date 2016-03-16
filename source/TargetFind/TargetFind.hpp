#include "imgformat.hpp"
#include "LinkedList.hpp"
#include <gsl/gsl_blas.h>
#ifndef _TARGETFIND_H_
#define _TARGETFIND_H_

const int RED_THRES = 100;
const int GRN_THRES = 200;
const int BLU_THRES = 150;

class TargetData
{
public:
  int width,height;
  float centroid_x;
  float centroid_y;
  float circum_radius;
  //Telem* telem;
private:
};

LinkedList<TargetData> getTargets(gsl_matrix* img)
{
  size_t i,j;
  gsl_matrix* img_buff = gsl_matrix_calloc(img->size1,img->size2);
  //j scans down the rows, i scans down the columns
  for(j=0;j<img->size1;j++)
    for(i=0;i<img->size2;i++)
    {
      ulong newPx = 0;
      //red threshold
      if(getChanR(gsl_matrix_get(img,j,i))>=RED_THRES)
	newPx |= 0xFF<<OFFSET_R;
      if(getChanG(gsl_matrix_get(img,j,i))>=GREEN_THRES)
	newPx |= 0xFF<<OFFSET_G;
      if(getChanG(gsl_matrix_get(img,j,i))>=GREEN_THRES)
	newPx |= 0xFF<<OFFSET_B;
      gsl_matrix_set(img_buff,j,i,newPx);
    }
}

#endif
