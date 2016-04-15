#include <gsl/gsl_blas.h>
#include <math.h>
#ifndef _IMGFORMAT_HPP_
#define _IMGFORMAT_HPP_
//this lets us slice up 
typedef struct pixel_RGB{unsigned char ch[3];} pixel_RGB;
const pixel_RGB COLOR_WHITE = {.ch = {0xFF,0xFF,0xFF}};

class ARVP_Image
{
private:
public:
  size_t width,height,channels;
  unsigned char * data;
  pixel_RGB * px_view;

  ARVP_Image(unsigned int height,unsigned int width,unsigned char* data)
  {
    this->channels = 3;
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
    this->channels = 3;
    this->width = width;
    this->height = height;
    this->data = new unsigned char[width*height*this->channels];
    this->px_view = (pixel_RGB*)(void*)this->data;
  }

  virtual ~ARVP_Image()
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
    for(unsigned int i=0;i<this->height && i<this->width;i+=1)
    {
      px = this->get(i,i);
      printf("DEBUG: img[%i,%i]=(%i,%i,%i)\n",
	     (int)i,(int)i,
	     sign0?(signed char)px.ch[0]:(unsigned char)px.ch[0],
	     sign1?(signed char)px.ch[1]:(unsigned char)px.ch[1],
	     sign2?(signed char)px.ch[2]:(unsigned char)px.ch[2]);
    }
  }

  virtual void set(unsigned int row, unsigned int col,pixel_RGB val)
  { 
    this->px_view[row*this->width+col] = val;
  }

  virtual void set_ch(unsigned int ch,unsigned int row, unsigned int col,unsigned char val)
  { 
    this->data[(row*this->width+col)*this->channels+ch] = val;
  }

  virtual pixel_RGB get(unsigned int row, unsigned int col)
  {
    return this->px_view[row*this->width+col];
  }

  virtual unsigned char get_ch(unsigned int ch, unsigned int row, unsigned int col)
  {
    //printf("CALLING GET_CH ON ORIGINAL (%i,%i,%i)\n",(int)ch,(int)row,(int)col);
    return this->data[(row*this->width+col)*this->channels+ch];
  }
};

class ARVP_Image_Resize : public ARVP_Image
{
private:
  ARVP_Image*original;
public:
  //size_t width,height,channels;
  
  ARVP_Image_Resize(unsigned int height,unsigned int width,ARVP_Image* original) :
    ARVP_Image(height,width,NULL)
  {
    //this->channels = 3;
    //this->width = width;
    //this->height = height;
    this->original=original;
  }

  int scale_x(int org_x)
  {
    //printf("Scaling X %i down to %i",org_x,(int)round(((float)org_x)/original->width * this->width));
    return round(((float)org_x)/original->width * this->width);
  }
  int unscale_x(int new_x)
  {
    //printf("Unscaling X %i back to %i",new_x,(int)round(((float)new_x)/this->width * original->width));
    return round(((float)new_x)/this->width * original->width);
  }
  
  int scale_y(int org_y)
  {
    //printf("Scaling Y %i down to %i",org_y,(int)round(((float)org_y)/original->height * this->height));
    return round(((float)org_y)/original->height * this->height);
  }
  int unscale_y(int new_y)
  {
    //printf("Unscaling Y %i back to %i",new_y,(int)round(((float)new_y)/this->height * original->height));
    return round(((float)new_y)/this->height * original->height);
  }

  //overwritten member functions
  void set(unsigned int row, unsigned int col,pixel_RGB val)
  { 
    //this->px_view[row*this->width+col] = val;
    this->original->set(unscale_y(row),unscale_x(col),val);
  }

  void set_ch(unsigned int ch,unsigned int row, unsigned int col,unsigned char val)
  {
    //this->data[(row*this->width+col)*this->channels+ch] = val;
    this->original->set_ch(ch,unscale_y(row),unscale_x(col),val);
  }

  pixel_RGB get(unsigned int row, unsigned int col)
  {
    //return this->px_view[row*this->width+col];
    return this->original->get(unscale_y(row),unscale_x(col));
  }

  unsigned char get_ch(unsigned int ch, unsigned int row, unsigned int col)
  { 
    //printf("CALLING GET_CH ON ORIGINAL (%i,%i,%i)\n",(int)ch,(int)unscale_y(row),(int)unscale_x(col));
    //return this->data[(row*this->width+col)*this->channels+ch];
    return this->original->get_ch(ch,unscale_y(row),unscale_x(col));
  }
};

//verify a set of coordinates is inside an image
bool isInImage(ARVP_Image* img,int coordY,int coordX)
{
  if(coordX<int(img->width) && coordX>=0 && 
     coordY<int(img->height) && coordY >=0)
    return true;
  else
    return false;
}

//gets a pixel bounded by the image border
pixel_RGB getBoundPixel(ARVP_Image* img,int coordY,int coordX)
{
  //printf("GETTING BOUND PIXEL (%i,%i)\n",coordY,coordX);
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

//get a pixel but don't go out of bounds
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
  //printf("GETTING BOUND CHANNEL (%i,%i,%i)\n",(int)ch,coordY,coordX);
  return img->get_ch(ch,(coordY),(coordX));
}
#endif
