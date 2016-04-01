#include "ARVP_JPEG.hpp"
#include "imgformat.hpp"
#include "imgops.hpp"
#include <gsl/gsl_blas.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
int main(int argc,char*argv[])
{
  const char* filename = "img.jpg";
  const char* outname = "write.jpg";
  std::cout<<"--Testing has begun on opening an image--\n";
  ARVP_Image*test_img = openJPEG(filename);
  printf("Opening image %s\n",filename);
  printf("Image of size w,h: %i,%i\n",(int)test_img->width,(int)test_img->height);
  
  if(argc>=2 && strcmp(argv[1],"canny")==0)
  {
    printf("Edge detecting the crisp image\n");
    cannyEdgeDetection(test_img,test_img);
  }
  else if(argc>=2 && strcmp(argv[1],"gauss")==0)
  {
    printf("Blurring the crisp image\n");
    int f_size = 7;
    double stdev = 2;
    if(argc>=3)
      f_size = atoi(argv[2]);
    if(argc>=4)
      stdev = atof(argv[3]);
    
    gsl_matrix * blur = gsl_matrix_calloc(f_size,f_size);
    gaussian(blur,stdev);
    convolution_RGB(test_img,test_img,blur,f_size/2,f_size/2);
    gsl_matrix_free(blur);
  }
  else if(argc>=2 && strcmp(argv[1],"sobel")==0)
  {
    printf("Grading the crisp image\n");
    ARVP_Image*buffx_img = new ARVP_Image((unsigned int)test_img->height,
					  (unsigned int)test_img->width);
    ARVP_Image*buffy_img = new ARVP_Image((unsigned int)test_img->height,
					  (unsigned int)test_img->width);
    
    gsl_matrix * edgx = gsl_matrix_calloc(3,3);
    gsl_matrix * edgy = gsl_matrix_calloc(3,3);
    sobel_x(edgx);
    sobel_y(edgy);
    if(argc>=3 && strcmp(argv[2],"sngl")==0)
    {
      convolution_single(test_img,0,buffx_img,0,edgx,1,1);
      convolution_single(test_img,1,buffx_img,1,edgx,1,1);
      convolution_single(test_img,2,buffx_img,2,edgx,1,1);

      convolution_single(test_img,0,buffy_img,0,edgy,1,1);
      convolution_single(test_img,1,buffy_img,1,edgy,1,1);
      convolution_single(test_img,2,buffy_img,2,edgy,1,1);
    }
    else// if(argc>=3 && strcmp(argv[2],"RGB")==0)
    {
      convolution_RGB(test_img,buffx_img,edgx,1,1);
      convolution_RGB(test_img,buffy_img,edgy,1,1);
    }
    gsl_matrix_free(edgx);
    gsl_matrix_free(edgy);
    //printf("\nBUFFX\n");
    //buffx_img->debugPrint(true,true,true);
    //printf("\nBUFFY\n");
    //buffy_img->debugPrint(true,true,true);
    
    double Gx,Gy,G;
    for(unsigned int k=0;k<3;k++)
      for(unsigned int j=0;j<test_img->height;j++)
	for(unsigned int i=0;i<test_img->width;i++)
	  {
	    Gx = (signed char)buffx_img->get_ch(k,j,i);
	    Gy = (signed char)buffy_img->get_ch(k,j,i);
	    G = sqrt(pow(Gx,2) + pow(Gy,2));
	    test_img->set_ch(k,j,i,G);
	  }
    delete buffx_img;
    delete buffy_img;
  }
  else
  {
    printf("Usage: jpegtestmain [canny|gauss|sobel [sngl|RGB]]\n");
    delete test_img;
    return 0;
  }
  printf("Saving image as %s\n",outname);
  saveARVP_Image(test_img,outname);
  delete test_img;
  return 0;
}
