#include "TargetFind/BlobDetect.hpp"
#include "ARVP_JPEG.hpp"
#include "imgformat.hpp"
#include "imgops.hpp"
#include <gsl/gsl_blas.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
int main(void)
{
  std::cout<<"Testing Blob Detection\n";
  const char* filename = "img.jpg";
  //const char* outname = "write.jpg";
  std::cout<<"--Testing has begun on opening an image--\n";
  ARVP_Image*test_img_raw = openJPEG(filename);
  
  ARVP_Image_Resize*test_img_scaled = new ARVP_Image_Resize(test_img_raw->height/2,
  							    test_img_raw->width/2,
  							    test_img_raw);
  ARVP_Image*test_img = (ARVP_Image*)test_img_scaled;
  
  //ARVP_Image*test_img = test_img_raw;
  printf("Opening image %s\n",filename);
  printf("Original Image of size w,h: %i,%i\n",(int)test_img_raw->width,(int)test_img_raw->height);
  printf("Image of size w,h: %i,%i\n",(int)test_img->width,(int)test_img->height);
  printf("Blobbing the crisp image\n");
  LinkedList<Point2D<int> >* candidate_list = gradeSmoothDetect(test_img,0,4,15,220,true);
  while (candidate_list->hasNext())
  {
    Point2D<int>*candidate = candidate_list->pop();
    printf("Candidate at Y:%i, X:%i\n",
	   candidate->ypos,
	   candidate->xpos);
    delete candidate;
  }
  delete candidate_list;
  return 0;
}
