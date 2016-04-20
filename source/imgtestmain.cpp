#include <iostream>
#include "imgformat.hpp"
#include "imgops.hpp"
#include <gsl/gsl_blas.h>
#include <jpeglib.h>
#include <cstddef>
#include "ARVP_JPEG.hpp"
int main(int argc,char*argv[])
{
  std::cout<<"--Testing basic image functions--\n";

  const int img_rows = 100;
  const int img_cols = 100;
  ARVP_Image* img = new ARVP_Image(img_rows,img_cols);
  for(unsigned int j=0;j<img->height;j++)
    for(unsigned int i=0;i<img->width;i++)
      for(unsigned int k=0;k<3;k++)
	img->set_ch(k,j,i,(i>50 && k==0)?255:0);
  std::cout<<"Image Diagonal:\n";
  for(unsigned int i=0;i<img->height&&i<img->width;i++)
  {
    pixel_RGB px = img->get(i,i);
    printf("\timg[%i,%i]=(%i,%i,%i)\n",(int)i,(int)i,(int)px.ch[0],(int)px.ch[1],(int)px.ch[2]);
  }
  std::cout<<"--Gaussian Test--\n";
  gsl_matrix*gauss = gsl_matrix_alloc(5,5);
  gaussian(gauss,0.84089642);
  
  std::cout<<"Gaussian:\n";
  for(unsigned int j=0;j<gauss->size1;j++)
  {
    for(unsigned int i=0;i<gauss->size2;i++)
      printf("%0.10f ",gsl_matrix_get(gauss,j,i));
    printf("\n");
  } 
  std::cout<<"--Convolution Test--\n";
  convolution_RGB(img,img,gauss,
	      gauss->size1/2,gauss->size2/2);
  std::cout<<"Image Diagonal:\n";
  for(unsigned int i=0;i<img->height&&i<img->width;i++)
  {
    pixel_RGB px = img->get(i,i);
    printf("\timg[%i,%i]=(%i,%i,%i)\n",(int)i,(int)i,(int)px.ch[0],(int)px.ch[1],(int)px.ch[2]);
  }
  saveARVP_Image(img,"img.jpg");
  delete img;
  gsl_matrix_free(gauss);
  return 0;
}
