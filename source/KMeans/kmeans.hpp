#ifndef _KMEANS_HPP_
#define _KMEANS_HPP_
#include "../ARVP_JPEG.hpp"
#include "kmeansformat.hpp"
#include "../imgformat.hpp"
#include "../LinkedList.hpp"
//used for training
LinkedList<ClusterProfile> * getProfiles(ARVP_Image * src_img, ARVP_Image * ctrl_img, KMeansMap*map)
{
  unsigned int k,j,i;
  LinkedList<ClusterProfile> * profiles = new LinkedList<ClusterProfile>();
  printf("Creating Profiles...\n");
  for(k=0;k<map->clusters();k++)
    profiles->push(new ClusterProfile(3,8));
  printf("Sampling...\n");
  //long unsigned int count = 0;
  //long unsigned int uncount = 0;
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
	//count++;
      }
      /*
      else
      {
	printf("Not Found!\n");
	uncount++;
      }
      */
    }
  //printf("END: count: %lu uncount: %lu\n",count,uncount);
  return profiles;
}

ClusterProfile * blobToCluster(ARVP_Image*src_img,Blob* blob)
{
  unsigned int i;
  ClusterProfile * profile = new ClusterProfile(3,8);
  printf("Sampling...\n");
  for(i=0;i<blob->num_coords;i++)
  {
    //get the coordinates
    coord c = blob->coords[i];
    //get the pixel
    pixel_RGB px = src_img->get(c.axis[0],c.axis[1]);
    //add the pixel to the cluster
    profile->addPixel(px);
  }
  return profile;
}

//ultimate culmination of the kmeans toolset
class KMeans
{
private:
  LinkedList<ClusterProfile>*profiles;
  KMeansMap*map;
public:
  KMeans()
  {
    this->profiles = NULL;
    this->map = NULL;
  }
  KMeans(LinkedList<ClusterProfile>*profiles,KMeansMap*map)
  {
    this->profiles = profiles;
    this->map = map;
  }
  ~KMeans()
  {
    while(this->profiles->hasNext())
    {
      ClusterProfile*next = profiles->pop();
      delete next;
    }
    delete this->profiles;
    delete this->map;
  }
  unsigned int matchPixel(pixel_RGB px)
  {
    if(this->profiles->getSize()<2)
    {
      printf("!!! ERROR IN MATCHPIXEL!!!\n");
      return 0;
    }
    //int min_match=0;
    //int min_dist=this->profiles->get(0)->get;
    for(int i=1;i<this->profiles->getSize();i++)
    {
      ;
    }
    return 0;
  }
  void saveKMeans(const char* filename)
  {
    ;
  }
  void loadKMeans(const char* filename)
  {
    ;
  }
};
#endif
