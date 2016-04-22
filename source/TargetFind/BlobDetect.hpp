#ifndef _BLOBDETECT_HPP_
#define _BLOBDETECT_HPP_
#include "../ARVP_JPEG.hpp"
#include "../imgformat.hpp"
#include "../imgops.hpp"
#include "../LinkedList.hpp"
#include "../geometry.hpp"
#include <gsl/gsl_blas.h>
#include "TargetData.hpp"
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>

class pointGroup
{
private:
  Point2D<double>*centroid;
  double circum_radius;
  LinkedList<Point2D<int> >*members;
public:
  pointGroup(Point2D<int>*init_pt)
  {
    this->members = new LinkedList<Point2D<int> >();
    members->push(init_pt);
    this->circum_radius = 1.0;
  }
  ~pointGroup()
  {
    delete this->centroid;
    delete this->members;
  }
  double getRadius()
  {
    return this->circum_radius;
  }
  Point2D<double>*getCentroid()
  {
    return this->centroid;
  }
  void addPoints(LinkedList<Point2D<int> >*pts)
  {
    //continuously recalculate the centroid
    while(pts->hasNext())
    {
      Point2D<int>*pt=pts->pop();
      members->push(pt);
      //update the centroid
      double xcent = this->centroid->xpos;
      double ycent = this->centroid->ypos;
      xcent = xcent*(members->getSize()-1)/members->getSize() + pt->xpos/members->getSize();
      ycent = ycent*(members->getSize()-1)/members->getSize() + pt->ypos/members->getSize();
      this->centroid->xpos = xcent;
      this->centroid->ypos = ycent;
    }
    
    //update the radius
    double maxsqrdist=1;
    for(int i=0;i<members->getSize();i++)
    {
      double sqrdist=pow(this->centroid->xpos-members->get(i)->xpos,2)+pow(this->centroid->ypos-members->get(i)->ypos,2);
      if(sqrdist>maxsqrdist)
	maxsqrdist=sqrdist;
    }
    this->circum_radius = sqrt(maxsqrdist);
  }
  friend void absorbGroup(pointGroup*group1,pointGroup*group2)
  {
    group1->addPoints(group2->members);
    group2->circum_radius=0;
  }
};

bool canGrab(pointGroup*group1,pointGroup*group2)
{
  Point2D<double>*cent1 = group1->getCentroid();
  Point2D<double>*cent2 = group2->getCentroid();
  double sqrdist = pow(cent1->xpos-cent2->xpos,2)+pow(cent1->ypos-cent2->ypos,2);
  if(sqrdist<=pow(group1->getRadius()*1.5,2))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void growGroups(LinkedList<pointGroup>*groups)
{
  LinkedList<pointGroup>*ready=groups;
  LinkedList<pointGroup>*done=new LinkedList<pointGroup>;
  while(ready->hasNext())
  {
    pointGroup*group = ready->pop();
    bool hasRegrouped = false;
    for(int i=0;i<ready->getSize();i++)
    {
      if(canGrab(group,ready->get(i)))
      {
	hasRegrouped = true;
	pointGroup*food = ready->remove(i);
	absorbGroup(group,food);
	delete food;
	ready->push(group);
	break;
      }
    }
    if(hasRegrouped)
      continue;
    for(int i=0;i<done->getSize();i++)
    {
      if(canGrab(group,done->get(i)))
      {
	hasRegrouped = true;
	pointGroup*food = done->remove(i);
	absorbGroup(group,food);
	delete food;
	ready->push(group);
	break;
      }
    }
    if(!hasRegrouped)
    {
      done->push(group);
    }
  }
}

//detects targets at a specified scale
void detectLowGradeScale(ARVP_Image*scale_space_img,int scale,int threshold,LinkedList<Point2D<int> >*candidate_list,bool debug=false)
{
  gsl_matrix*filter;
  int j,i;
  //smooth the image
  int smoothsize = (1+scale-(scale)%2)*3;
  filter = gsl_matrix_calloc(smoothsize,smoothsize);
  for(j=0;j<smoothsize;j++)
    for(i=0;i<smoothsize;i++)
      gsl_matrix_set(filter,j,i,1.0/pow(smoothsize,2));
  convolution_RGB(scale_space_img,scale_space_img,filter,smoothsize/2,smoothsize/2); 
  //perform laplacian operator
  //strong positive reponse for blobs of size sqrt(2*scale)
  //filter = gsl_matrix_calloc(3,3);
  //laplace(filter);
  //printf("Doing Laplacian\n");
  //convolution_RGB(scale_space_img,scale_space_img,filter,1,1);
  //gsl_matrix_free(filter);

  //gather candidate results with the scale,position,and intensity
  printf("Getting Candidates\n");
  for(j=0;j<(int)scale_space_img->height;j++)
    for(i=0;i<(int)scale_space_img->width;i++)
    {
      //if the pixel is above the threshold, add it to the candidate list
      //later add the maxima/minima condition maybe
      //yes this is arbitrarily accessing the red channel
      int intensity = (int)scale_space_img->get_ch(0,j,i);
      if(intensity<=threshold)
      {
	//printf("Got one!\n");
	candidate_list->push(new Point2D<int>(i,j));
	if(debug)
	  scale_space_img->set_ch(1,j,i,255);
      }
    }
  if(debug)
  {
    printf("Finished scale %i\n",scale);
    char scale_str[2];
    char filename_head[] = "img_";
    char filename_tail[] = ".jpg";
    sprintf(scale_str,"%i",scale);
    const char* filename = strcat(filename_head,strcat(scale_str,filename_tail));
    saveARVP_Image(scale_space_img,filename);
  }
}



//detects targets at a specified scale as a grouping of edges
LinkedList<Point2D<int> >*cannyGroupDetect(ARVP_Image*src_img,int init_scale,int max_scale, int threshold,bool debug = false)
{
  ARVP_Image*edge_img = new ARVP_Image(src_img->height,src_img->width);
  //this is what is returned
  LinkedList<Point2D<int> >*candidate_list = new LinkedList<Point2D<int> >();
  //perform canny edge detection, blob is found as hi-edge region
  printf("Beginning Canny Edge Detection\n");
  cannyEdgeDetection(src_img,edge_img,init_scale,30,10);
  //put edges in groups
  //grow the groups
  printf("Finished detecting, now deleting\n");
  delete edge_img;
  return candidate_list;
}

//detects targets at a specified scale as a low-gradient section
LinkedList<Point2D<int> >*gradeSmoothDetect(ARVP_Image*src_img,int init_scale,int max_scale, int threshold,bool debug = false)
{
  int scale;
  ARVP_Image*gradient_img = new ARVP_Image(src_img->height,src_img->width);
  ARVP_Image*scale_space_img = new ARVP_Image(src_img->height,src_img->width);
  //this is what is returned
  LinkedList<Point2D<int> >*candidate_list = new LinkedList<Point2D<int> >();
  gsl_matrix*filter;

  //perform gradient, the blob is found as a low-gradient area
  printf("Performing gradient operation\n");
  gradient_RGB(src_img,gradient_img,true);
  //the image is evaluated at scales
  //printf("Beginning the scales\n");
  //for(scale = init_scale;scale<=max_scale;scale++)
  for(scale = init_scale;scale<=init_scale;scale++) //ignore max_scale for now
  {
    //perform gaussian blur for scale space
    int blur_size = (int)ceil(sqrt(scale)*6);
    if(scale>0)
    {
      filter = gsl_matrix_calloc(blur_size,blur_size);
      gaussian(filter,scale);
      printf("Doing Gaussian\n");
      convolution_RGB(gradient_img,scale_space_img,filter,blur_size/2,blur_size/2);
      gsl_matrix_free(filter);
    }
    //pass the scale space image to the detection
    detectLowGradeScale(gradient_img,scale,threshold,candidate_list,debug);
  }
  printf("Finished detecting, now deleting\n");
  delete gradient_img;
  delete scale_space_img;
  return candidate_list;
}

#endif
