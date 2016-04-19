#ifndef _BLOBDETECT_HPP_
#define _BLOBDETECT_HPP_
#include "../imgformat.hpp"
#include "../imgops.hpp"
#include "../LinkedList.hpp"
#include <gsl/gsl_blas.h>
#include "TargetData.hpp"
#include <math.h>
#include <iostream>
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
  gsl_matrix*filter3;
  //perform gradient, since the blob is found as a low-gradient area
  printf("performing gradient operation\n");
  gradient_RGB(src_img,gradient_img);
  printf("Beginning the scales\n");
  for(scale = init_scale;scale<=max_scale;scale++)
  {
    //perform gaussian blur for scale space
    int blur_size = (int)ceil(sqrt(scale)*6);
    filter3 = gsl_matrix_alloc(blur_size,blur_size);
    gaussian(filter3,scale);
    printf("Doing Gaussian\n");
    convolution_RGB(gradient_img,scale_space_img,filter3,1,1);
    //perform laplacian operator
    //strong positive reponse for blobs of size sqrt(2*scale)
    laplace(filter3);
    printf("Doing Laplacian\n");
    convolution_RGB(scale_space_img,scale_space_img,filter3,1,1);
    //gather candidate results with the scale,position,and intensity
    for(j=0;j<(int)scale_space_img->height;j++)
      for(i=0;i<(int)scale_space_img->width;i++)
      {
	//if the pixel is above the threshold, add it to the candidate list
	//later add the maxima condition maybe
	int intensity = (signed int)scale_space_img->get_ch(0,j,i);
	if(intensity>threshold)
	  candidate_list->push(new BlobCandidate(init_scale,j,i,intensity));	
      }
    printf("Finished scale %i\n",scale);
  }
  printf("Finished detecting, now deleing\n");
  delete gradient_img;
  delete scale_space_img;
  return candidate_list;
}

#endif
