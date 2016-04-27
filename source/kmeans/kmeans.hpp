#ifndef _KMEANS_HPP_
#define _KMEANS_HPP_
#include "../ARVP_JPEG.hpp"
#include "kmeansformat.hpp"
ClusterProfile* getMeans(ARVP_Image * src_img, ARVP_image * ctrl_img)
{
  ClusterProfile * img_profile = new ClusterProfile(3,8);
  int k,j,i;
  for(k=0;k<src_img->channels;k++)
    for(j=0;j<src_img->height;j++)
      for(i=0;i<src_img->width;i++)
      {
	img_profile->channel_list[k]->
      }
}
#endif
