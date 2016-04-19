#include "BlobDetect.hpp"
#include "../ARVP_JPEG.hpp"
#include "../imgformat.hpp"
#include "../imgops.hpp"
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
   ARVP_Image_Resize*test_img_scaled = new ARVP_Image_Resize(test_img_raw->height/4,
						     test_img_raw->width/4,
						     test_img_raw);
  ARVP_Image*test_img = (ARVP_Image*)test_img_scaled;
  printf("Opening image %s\n",filename);
  printf("Original Image of size w,h: %i,%i\n",(int)test_img_raw->width,(int)test_img_raw->height);
  printf("Image of size w,h: %i,%i\n",(int)test_img->width,(int)test_img->height);
  printf("Blobbing the crisp image\n");
  LinkedList<BlobCandidate>* candidate_list = detectScale(test_img,1,1,0);
  while (candidate_list->hasNext())
  {
    BlobCandidate*candidate = candidate_list->pop();
    printf("Candidate at S:%i, Y:%i, X:%i, I:%i\n",
	   candidate->scale,
	   candidate->ypos,
	   candidate->xpos,
	   candidate->intensity);
    delete candidate;
  }
  delete candidate_list;
  return 0;
}
