#include <gsl/gsl_blas.h>
#ifndef _IMGFORMAT_HPP_
#define _IMGFORMAT_HPP_
//this lets us slice up 
typedef struct pixel_RGB{unsigned char ch[3];} pixel_RGB;
const pixel_RGB COLOR_WHITE = {.ch = {0xFF,0xFF,0xFF}};

class ARVP_Image
{
private:
  size_t p_width,p_height,p_channels;
public:
  unsigned char * data;
  pixel_RGB * px_view;
  ARVP_Image(unsigned int height,unsigned int width,unsigned char* data)
  {
    this->p_channels = 3;
    this->p_width = width;
    this->p_height = height;
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
    this->p_channels = 3;
    this->p_width = width;
    this->p_height = height;
    this->data = new unsigned char[width*height*this->p_channels];
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
  void debugPrint(bool sign0=false,bool sign1=false,bool sign2=false)
  {
    pixel_RGB px;
    printf("Image Diagonal:\n");
    for(unsigned int i=0;i<this->height() && i<this->width();i+=1)
    {
      px = this->get(i,i);
      printf("DEBUG: img[%i,%i]=(%i,%i,%i)\n",
	     (int)i,(int)i,
	     sign0?(signed char)px.ch[0]:(unsigned char)px.ch[0],
	     sign1?(signed char)px.ch[1]:(unsigned char)px.ch[1],
	     sign2?(signed char)px.ch[2]:(unsigned char)px.ch[2]);
    }
  }
  void set(unsigned int row, unsigned int col,pixel_RGB val)
  { 
    this->px_view[row*this->width()+col] = val;
  }
  void set_ch(unsigned int ch,unsigned int row, unsigned int col,unsigned char val)
  { 
    this->data[(row*this->width()+col)*this->channels()+ch] = val;
  }
  pixel_RGB get(unsigned int row, unsigned int col)
  {
    return this->px_view[row*this->width()+col];
  }
  unsigned char get_ch(unsigned int ch, unsigned int row, unsigned int col)
  { 
    return this->data[(row*this->width()+col)*this->channels()+ch];
  }
  size_t width(){return this->p_width;}
  size_t height(){return this->p_height;}
  size_t channels(){return this->p_channels;}
};

bool isInImage(ARVP_Image* img,int coordY,int coordX)
{
  if(coordX<int(img->width()) && coordX>=0 && 
     coordY<int(img->height()) && coordY >=0)
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
    else if(coordX>=int(img->width()))
      coordX=img->width()-1;

    if(coordY<0)
      coordY=0;
    else if(coordY>=int(img->height()))
      coordY=img->height()-1;
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
    else if(coordX>=int(img->width()))
      coordX=img->width()-1;

    if(coordY<0)
      coordY=0;
    else if(coordY>=int(img->height()))
      coordY=img->height()-1;
  }
  
  return img->get_ch(ch,(coordY),(coordX));
}
#endif
