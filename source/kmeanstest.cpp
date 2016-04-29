#include "KMeans/kmeans.hpp"
#include "KMeans/kmeansformat.hpp"
#include "ARVP_JPEG.hpp"
#include "imgformat.hpp"
#include "imgops.hpp"
#include <iostream>
#include <stdio.h>

int main(int argc,char*argv[])
{
  const char* src_name = "sky.jpg";
  const char* ctrl_name = "ctrl.jpg";
  //const char* outname = "write.jpg";
  std::cout << "--Testing has begun on opening an image--\n";

  ARVP_Image*src_img_raw = openJPEG(src_name);
  //ARVP_Image_Resize*src_img_scaled = new ARVP_Image_Resize(src_img_raw->height/4,
  //						     src_img_raw->width/4,
  //						     src_img_raw);
  //ARVP_Image*src_img = (ARVP_Image*)src_img_scaled;

  ARVP_Image*ctrl_img_raw = openJPEG(ctrl_name);

  ARVP_Image*src_img = src_img_raw;
  ARVP_Image*ctrl_img = ctrl_img_raw;
  printf("Opening image %s\n",src_name);
  printf("Original Image of size w,h: %i,%i\n",(int)src_img_raw->width,(int)src_img_raw->height);
  printf("Image of size w,h: %i,%i\n",(int)src_img->width,(int)src_img->height);
  printf("Making the map\n");
  KMeansMap * skymap = new KMeansMap();
  printf("Adding clusters to the map\n");
  //our sky is white, ground is black
  const char*skylabel = "sky";
  const char*gndlabel = "gnd";
  skymap->addCluster(skylabel,COLOR_WHITE);
  skymap->addCluster(gndlabel,COLOR_BLACK);
  
  printf("Doing the algorithm (oh boy)\n");
  LinkedList<ClusterProfile>*profiles = getProfiles(src_img,ctrl_img,skymap);
  printf("Results:\n");
  for(int i=0;i<profiles->getSize();i++)
  {
    printf("Result %s %f,%f,%f\n",
	   skymap->getClusterName(i),
	   profiles->get(i)->getChannelMean(0),
	   profiles->get(i)->getChannelMean(1),
	   profiles->get(i)->getChannelMean(2));
  }
  printf("Deleting the results\n");
  while(profiles->hasNext())
  {
    ClusterProfile*next = profiles->pop();
    delete next;
  }
  delete profiles;
  printf("Deleting the map\n");
  delete skymap;
  //delete src_img_raw;
  delete src_img;
  
  //delete ctrl_img_raw;
  delete ctrl_img;
}
