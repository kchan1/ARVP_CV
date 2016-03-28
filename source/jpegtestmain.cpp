#include "ARVP_JPEG.hpp"
#include "imgformat.hpp"
#include "imgops.hpp"
#include <gsl/gsl_blas.h>
#include <iostream>
#include <stdio.h>
int main(int argc,char*argv[])
{
  const char* filename = "img.jpg";
  const char* outname = "write.jpg";
  std::cout<<"--Testing has begun on opening an image--\n";
  ARVP_Image*test_img = openJPEG(filename);
  printf("Opening image %s\n",filename);
  printf("Image of size w,h: %i,%i\n",(int)test_img->width,(int)test_img->height);
  /*
  printf("Image Diagonal:\n");
  for(int j=0;j<(int)test_img->height&&j<(int)test_img->width;j++)
  {
    pixel_RGB curr = test_img->get(j,j);
    printf("img[%i,%i] = {%i,%i,%i}\n",
	   j,j,
	   (int)curr.ch[0],(int)curr.ch[1],(int)curr.ch[2]);
  }
  */
  printf("Edge detecting the crisp image\n");
  cannyEdgeDetection(test_img,test_img);
  //gsl_matrix * blur = gsl_matrix_calloc(5,5);
  //gaussian(blur,0.84089642);
  //convolution_RGB(test_img,test_img,blur,5/2,5/2);
  printf("Saving image as %s\n",outname);
  saveARVP_Image(test_img,outname);
  //gsl_matrix_free(blur);
  delete test_img;
  return 0;
}
