#ifndef _IMGFORMAT_HPP_
#define _IMGFORMAT_HPP_
#include <gsl/gsl_blas.h>

const unsigned long CHAN_R = 0xFF000000;
const int OFFSET_R = 4*6;
const unsigned long CHAN_G = 0x00FF0000;
const int OFFSET_G = 4*4;
const unsigned long CHAN_B = 0x0000FF00;
const int OFFSET_B = 4*2;
const unsigned long CHAN_D = 0x000000FF;
const int OFFSET_D = 4*0;
const unsigned long COLOR_WHITE = 0xFFFFFFFF;

class ARVP_Image
{
private:
  
public:
  size_t width,height;
  unsigned long ** data;
  ARVP_Image(unsigned width, unsigned height)
  {
    int i;
    this->width = width;
    this->height = height;
    this->data = (unsigned long**)calloc(height,sizeof(unsigned long*)*height);
    for(i=0;i<height;i++)
      this->data[i] = (unsigned long*)calloc(width,sizeof(unsigned long)*width);
  }
  ~ARVP_Image()
  {
    for(i=0;i<height;i++)
      free(this->data[i]);
    free(this->data);
  }
  void set(unsigned int row, unsigned int col,unsigned long val)
  {
    this->data[row][col] = val;
  }
  unsigned long get(unsigned int row, unsigned int col)
  {
    return this->data[row][col];
  }
}

bool isInImage(gsl_matrix* img,int coordY,int coordX)
{
  if(coordX<int(img->size2) && coordX>=0 && coordY<int(img->size1) && coordY >=0)
    return true;
  else
    return false;
}

inline short int getChanR(unsigned long pixel)
{
  return (pixel>>OFFSET_R)&0xFF;
}
inline short int getChanG(unsigned long pixel)
{
  return (pixel>>OFFSET_G)&0xFF;
}
inline short int getChanB(unsigned long pixel)
{
  return (pixel>>OFFSET_B)&0xFF;
}
inline short int getChanD(unsigned long pixel)
{
  return (pixel>>OFFSET_D)&0xFF;
}

//gets a pixel bounded by the image border
double getBoundPixel(ARVP_Image* img,int coordY,int coordX)
{
  if(!isInImage(img,coordY,coordX))
  {
    if(coordX<0)
      coordX=0;
    else if(coordX>=int(img->size2))
      coordX=img->size2-1;

    if(coordY<0)
      coordY=0;
    else if(coordY>=int(img->size1))
      coordY=img->size1-1;
  }
  /*
  printf("COORDS: (%i,%i) = %f \n",
	 (unsigned int)coordY,
	 (unsigned int)coordX,
	 gsl_matrix_get(img,(unsigned int)(coordY),(unsigned int)(coordX)));
  */
  //return gsl_matrix_get(img,(unsigned int)(coordY),(unsigned int)(coordX));
  return img->get((unsigned int)(coordY),(unsigned int)(coordX));
}
#endif
