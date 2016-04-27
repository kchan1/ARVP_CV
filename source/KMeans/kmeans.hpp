#ifndef _KMEANS_HPP_
#define _KMEANS_HPP_
#include "../ARVP_JPEG.hpp"
#include "kmeansformat.hpp"
#include "../imgformat.hpp"
//used for training
LinkedList<ClusterProfile>*getProfiles(ARVP_Image * src_img, ARVP_Image * ctrl_img, KMeansMap*map)
{
  unsigned int k,j,i;
  LinkedList<ClusterProfile> * profiles = new LinkedList<ClusterProfile>();
  printf("Creating Profiles...\n");
  for(k=0;k<map->clusters();k++)
    profiles->push(new ClusterProfile(3,8));
  printf("Sampling...\n");
  for(j=0;j<src_img->height;j++)
    for(i=0;i<src_img->width;i++)
    {
      //get the ctrl signal
      pixel_RGB ctrl = ctrl_img->get(j,i);
      //get the pixel
      pixel_RGB px = src_img->get(j,i);
      //add a pixel to the corresponding ctrl signal
      int cluster=map->lookupCluster(ctrl);
      if(cluster!=420)
      {
	//printf("px %i,%i: cluster %i\n",(int)j,(int)i,cluster);
	profiles->get(cluster)->addPixel(px);
      }
      else
      {
	printf("Not Found!\n");
      }
    }
  printf("\n");
  return profiles;
}
#endif
