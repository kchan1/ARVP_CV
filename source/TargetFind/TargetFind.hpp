#include "imgformat.h"
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

TargetData* getTarget(gsl_matrix* img)
{
  int i;
  gsl_matrix* img_buff = new gsl_matrix();
  for(i=0;i<img->size1;i++)
  {
    if()
    {
    }
  }
}

#endif
