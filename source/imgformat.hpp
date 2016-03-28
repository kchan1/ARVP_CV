#include <gsl/gsl_blas.h>
#ifndef _IMGFORMAT_HPP_
#define _IMGFORMAT_HPP_
//this lets us slice up 
typedef struct pixel_RGB{unsigned char ch[3];} pixel_RGB;
const pixel_RGB COLOR_WHITE = {.ch = {0xFF,0xFF,0xFF}};

class ARVP_Image
{
private:
  
public:
  int CHANNELS;
  unsigned char * data;
  pixel_RGB * px_view;
  size_t width,height,channels;
  ARVP_Image(unsigned int height,unsigned int width,unsigned char* data)
  {
    this->CHANNELS = 3;
    this->width = width;
    this->height = height;
    this->data = data;
    this->px_view = (pixel_RGB*)(void*)this->data;
    //would have liked this concept but raspicam disallows it
    /*
    this->data = (unsigned long**)calloc(height,sizeof(unsigned long*)*height);
    for(i=0;i<(int)height;i++)
      this->data[i] = (unsigned long*)calloc(width,sizeof(unsigned long)*width);
    */
  }
  ARVP_Image(unsigned int height, unsigned int width)
  {
    this->CHANNELS = 3;
    this->width = width;
    this->height = height;
    this->data = new unsigned char[width*height*this->CHANNELS];
    this->px_view = (pixel_RGB*)(void*)this->data;
  }
  ~ARVP_Image()
  {
    delete[] this->data;
    /*
    int i;
    for(i=0;i<(int)height;i++)
      free(this->data[i]);
    free(this->data);
    */
  }
  void set(unsigned int row, unsigned int col,pixel_RGB val)
  { 
    this->px_view[row*width+col] = val;
  }
  void set_ch(unsigned int ch,unsigned int row, unsigned int col,unsigned char val)
  { 
    this->data[(row*width+col)*this->CHANNELS+ch] = val;
  }
  inline pixel_RGB get(unsigned int row, unsigned int col)
  {
    return this->px_view[row*width+col];
  }
  inline unsigned char get_ch(unsigned int ch, unsigned int row, unsigned int col)
  { 
    return this->data[(row*width+col)*this->CHANNELS+ch];
  }
};

bool isInImage(ARVP_Image* img,int coordY,int coordX)
{
  if(coordX<int(img->width) && coordX>=0 && coordY<int(img->height) && coordY >=0)
    return true;
  else
    return false;
}
//gets a pixel bounded by the image border
pixel_RGB getBoundPixel(ARVP_Image* img,int coordY,int coordX)
{
  if(!isInImage(img,coordY,coordX))
  {
    if(coordX<0)
      coordX=0;
    else if(coordX>=int(img->width))
      coordX=img->width-1;

    if(coordY<0)
      coordY=0;
    else if(coordY>=int(img->height))
      coordY=img->height-1;
  }
  return img->get((unsigned int)(coordY),(unsigned int)(coordX));
}
unsigned char getBoundChannel(ARVP_Image* img,unsigned int ch,int targY,int targX)
{
  int coordY=targY;
  int coordX=targX;
  if(!isInImage(img,coordY,coordX))
  {
    if(coordX<0)
      coordX=0;
    else if(coordX>=int(img->width))
      coordX=img->width-1;

    if(coordY<0)
      coordY=0;
    else if(coordY>=int(img->height))
      coordY=img->height-1;
  }
  
  return img->get_ch(ch,(unsigned int)(coordY),(unsigned int)(coordX));
}
#endif
