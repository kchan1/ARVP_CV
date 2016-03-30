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
  printf("Image of size w,h: %i,%i\n",(int)test_img->width(),(int)test_img->height());
  
  printf("Edge detecting the crisp image\n");
  cannyEdgeDetection(test_img,test_img);
  //gsl_matrix * blur = gsl_matrix_calloc(7,7);
  /*
  ARVP_Image*buffx_img = new ARVP_Image((unsigned int)test_img->height(),
					(unsigned int)test_img->width());
  ARVP_Image*buffy_img = new ARVP_Image((unsigned int)test_img->height(),
					(unsigned int)test_img->width());
					
  gsl_matrix * edg = gsl_matrix_calloc(3,3);
  sobel_x(edg);
  //convolution_RGB(test_img,buffx_img,edg,1,1);
  //convolution_single(test_img,0,buffx_img,0,edg,1,1);
  //convolution_single(test_img,1,buffx_img,1,edg,1,1);
  //convolution_single(test_img,2,buffx_img,2,edg,1,1);
  sobel_y(edg);
  //convolution_RGB(test_img,buffy_img,edg,1,1);
  //convolution_single(test_img,0,buffy_img,0,edg,1,1);
  //convolution_single(test_img,1,buffy_img,1,edg,1,1);
  //convolution_single(test_img,2,buffy_img,2,edg,1,1);
  printf("\nBUFFX\n");
  buffx_img->debugPrint(true,true,true);
  printf("\nBUFFY\n");
  buffy_img->debugPrint(true,true,true);
  double Gx,Gy,G;
  for(unsigned int k=0;k<3;k++)
    for(unsigned int j=0;j<test_img->height();j++)
      for(unsigned int i=0;i<test_img->width();i++)
	{
	  Gx = (signed char)buffx_img->get_ch(k,j,i);
	  Gy = (signed char)buffy_img->get_ch(k,j,i);
	  G = sqrt(pow(Gx,2) + pow(Gy,2));
	  test_img->set_ch(k,j,i,G);
	}
  */
  //gaussian(blur,2);
  //convolution_RGB(test_img,test_img,blur,7/2,7/2);
  //gsl_matrix_free(buffx_img);
  printf("Saving image as %s\n",outname);
  saveARVP_Image(test_img,outname);
  //gsl_matrix_free(blur);
  delete test_img;
  //delete buffx_img;
  //delete buffy_img;
  return 0;
}
