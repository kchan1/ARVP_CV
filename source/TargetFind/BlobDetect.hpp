#ifndef _BLOBDETECT_HPP_
#define _BLOBDETECT_HPP_
#include "../ARVP_JPEG.hpp"
#include "../imgformat.hpp"
#include "../imgops.hpp"
#include "../LinkedList.hpp"
#include <gsl/gsl_blas.h>
#include "TargetData.hpp"
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
//small helper class to stuff into linked lists
class BlobCandidate
{
 public:
  int scale,ypos,xpos,intensity;
  BlobCandidate(int scale,int ypos,int xpos,int intensity)
  {
    this->scale = scale;
    this->ypos = ypos;
    this->xpos = xpos;
    this->intensity = intensity;
  }
};

//detects targets at a specified scale
LinkedList<BlobCandidate>*detectScale(ARVP_Image*src_img,int init_scale,int max_scale, int threshold)
{
  printf("detectScale();\n");
  int scale,j,i;
  ARVP_Image*gradient_img = new ARVP_Image(src_img->height,src_img->width);
  ARVP_Image*scale_space_img = new ARVP_Image(src_img->height,src_img->width);
  //this is what is returned
  LinkedList<BlobCandidate>*candidate_list = new LinkedList<BlobCandidate>();
  gsl_matrix*filter;

  //perform gradient, the blob is found as a low-gradient area
  printf("Performing gradient operation\n");
  gradient_RGB(src_img,gradient_img,true);
  ///perform canny edge detection, blob is found as edge-less region
  //printf("Beginning Canny Edge Detection\n");
  //cannyEdgeDetection(src_img,gradient_img,1,10,2);
  //gradient_img->debugPrint();
  //printf("Beginning the scales\n");
  for(scale = init_scale;scale<=max_scale;scale++)
  {
    //perform gaussian blur for scale space
    int blur_size = (int)ceil(sqrt(scale)*6);
    if(scale>0)
    {
      filter = gsl_matrix_calloc(blur_size,blur_size);
      gaussian(filter,scale);
    }
    else
    {
      filter = gsl_matrix_calloc(1,1);
      gsl_matrix_set(filter,0,0,1);
    }
    printf("Doing Gaussian\n");
    convolution_RGB(gradient_img,scale_space_img,filter,blur_size/2,blur_size/2);
    gsl_matrix_free(filter);
    //perform laplacian operator
    //strong positive reponse for blobs of size sqrt(2*scale)
    //filter = gsl_matrix_calloc(blur_size,blur_size);
    //laplace(filter);
    int smoothsize = 9;
    filter = gsl_matrix_calloc(smoothsize,smoothsize);
    for(j=0;j<smoothsize;j++)
      for(i=0;i<smoothsize;i++)
	gsl_matrix_set(filter,j,i,1.0/pow(smoothsize,2));
    convolution_RGB(scale_space_img,scale_space_img,filter,smoothsize/2,smoothsize/2); 
    /*
    for(j=0;j<(int)filter->size1;j++)
    {
      for(i=0;i<(int)filter->size2;i++)
	printf("FILTER %i,%i = %f, ",j,i,gsl_matrix_get(filter,j,i));
      printf("\n");
    }
    */
    //printf("Filter");
    //printf("Doing Laplacian\n");
    //convolution_RGB(scale_space_img,scale_space_img,filter,1,1);
    //gsl_matrix_free(filter);
    //gather candidate results with the scale,position,and intensity
    printf("Getting Candidates\n");
    for(j=0;j<(int)scale_space_img->height;j++)
      for(i=0;i<(int)scale_space_img->width;i++)
      {
	//if the pixel is above the threshold, add it to the candidate list
	//later add the maxima condition maybe
	int intensity = (int)scale_space_img->get_ch(0,j,i);
	//if(j==i)
	//  printf("INTENSITY %i,%i = %i\n",j,i,intensity);
	if(intensity<=threshold)
	{
	  //printf("Got one!\n");
	  //candidate_list->push(new BlobCandidate(init_scale,j,i,intensity));
	  scale_space_img->set_ch(1,j,i,255);
	}
      }
    
    printf("Finished scale %i\n",scale);
    char scale_str[2];
    char filename_head[] = "img_";
    char filename_tail[] = ".jpg";
    sprintf(scale_str,"%i",scale);
    const char* filename = strcat(filename_head,strcat(scale_str,filename_tail));
    saveARVP_Image(scale_space_img,filename);
  }
  printf("Finished detecting, now deleing\n");
  delete gradient_img;
  delete scale_space_img;
  return candidate_list;
}

#endif
