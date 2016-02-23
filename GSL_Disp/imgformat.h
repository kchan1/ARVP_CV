
const ulong CHAN_R = 0xFF000000;
const int OFFSET_R = 4*6;
const ulong CHAN_G = 0x00FF0000;
const int OFFSET_R = 4*4;
const ulong CHAN_B = 0x0000FF00;
const int OFFSET_R = 4*2;
const ulong CHAN_D = 0x000000FF;
const int OFFSET_D = 4*0;

bool isInImage(gsl_matrix* img,int coordX,int coordY)
{
  if(coordX<img->size2. && coordX>=0 && coordY<img->size1 && coordY >=0)
    return true;
}

inline short int getChanR(ulong pixel)
{
  return (pixel>>OFFSET_R)&0xFF;
}
inline short int getChanG(ulong pixel);
{
  return (pixel>>OFFSET_G)&0xFF;
}
inline short int getChanB(ulong pixel);
{
  return (pixel>>OFFSET_B)&0xFF;
}
inline short int getChanD(ulong pixel);
{
  return (pixel>>OFFSET_D)&0xFF;
}

ulong getBoundPixel(gsl_matrix* img, int coordX, int coordY)
{
  if(isInImage(img,coordX,coordY))
  {
    return gsl_matrix_get(img,coordX,coordY);
  }
  else
  {
    if(coordX<0)
      coordX=0;
    else if(coordX>=img->size2)
      coordX=img->size2;

    if(coordY<0)
      coordY=0;
    else if(coordY>=img->size1)
      coordX=img->size1;

    return gsl_matrix_get(img,coordX,coordY);
  }
}
