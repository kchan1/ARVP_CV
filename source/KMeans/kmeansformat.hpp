#ifndef _KMEANSFORMAT_HPP_
#define _KMEANSFORMAT_HPP_
#include "../imgformat.hpp"
#include "../LinkedList.hpp"
class ChannelProfile
{
public:
  unsigned long samples;
  unsigned int buckets;
  long*bucket_list;
  ChannelProfile(int buckets)
  {
    this->samples = 0;
    this->buckets = buckets;
    this->bucket_list = new long[buckets];
    for(int i=0;i<buckets;i++)
      this->bucket_list[i] = 0;
  }
  virtual ~ChannelProfile()
  {
    delete[] bucket_list;
  }

  virtual void addPixel(unsigned char px_ch)
  {
    this->addSamples(int(px_ch)/(256/buckets),1);
  }
  
  virtual void addSamples(unsigned int bucket, int amount)
  {
    //if(bucket ==0)
    //  printf("SAMPLES %lu -> %lu\n",this->samples,this->samples+amount);
    this->samples += amount;
    this->bucket_list[bucket] += amount;
  }
  
  virtual double getMean()
  {
    //printf("GETTING MEAN WITH %lu SAMPLES\n",this->samples);
    double mean = 0;
    for(unsigned int i=0;i<this->buckets;i++)
      mean += double(i * this->getBucketWeight(i) * 255/this->buckets); 
    return mean;
  }
  virtual double getBucketWeight(int bucket_index)
  {
    long double bucket = (long double)(this->bucket_list[bucket_index]);
    long double num_samples = (long double)(this->samples);
    return bucket/num_samples;
  }
};

class ClusterProfile
{
public:
  unsigned long samples;
  unsigned int channels;
  unsigned int buckets;
  ChannelProfile ** channel_list;
  
  ClusterProfile(int channels,int buckets)
  {
    this->samples = 0;
    this->channels = channels;
    this->buckets = buckets;
    this->channel_list = new ChannelProfile*[channels];
    for(unsigned int i=0;i<this->channels;i++)
      this->channel_list[i] = new ChannelProfile(buckets);
  }
  
  ClusterProfile()
  {
    this->samples = 0;
    this->channels = 3;
    this->buckets = 8;
    this->channel_list = new ChannelProfile*[channels];
    for(unsigned int i=0;i<this->channels;i++)
      this->channel_list[i] = new ChannelProfile(buckets);
  }

  virtual ~ClusterProfile()
  {
    for(unsigned int i=0;i<this->channels;i++)
      delete this->channel_list[i];
    delete[] this->channel_list;
  }

  void addPixel(pixel_RGB px)
  {
    for(int i=0;i<3;i++)
      this->channel_list[i]->addPixel(px.ch[i]);
  }
  
  virtual void addSamples(unsigned int channel,unsigned int bucket,int amount)
  {
    this->channel_list[channel]->addSamples(bucket,amount);
  }

  virtual double getChannelMean(unsigned int channel)
  {
    return this->channel_list[channel]->getMean();
  }
  
  virtual getDistance(Region blob)
  {
    for(int k=0;)
  }
};

class KMeansMap
{
private:
  class ClusterSignature
  {
  public:
    const char* name;
    pixel_RGB ID;
    ClusterSignature(const char* name,pixel_RGB ID)
    {
      this->name = name;
      this->ID = ID;
    }
  };
public:
  
  LinkedList<ClusterSignature>*cluster_list;

  KMeansMap()
  {
    cluster_list = new LinkedList<ClusterSignature>();
  }
  ~KMeansMap()
  {
    while(cluster_list->hasNext())
    {
      ClusterSignature * cs = cluster_list->pop();
      delete cs;
    }
    delete cluster_list;
  }
  unsigned int clusters()
  {
    return (unsigned int)cluster_list->getSize();
  }
  void addCluster(const char* name,pixel_RGB ID)
  {
    cluster_list->push(new ClusterSignature(name,ID));
  }
  unsigned int lookupCluster(pixel_RGB px)
  {
    for(int i=0;i<cluster_list->getSize();i++)
      if(cluster_list->get(i)->ID==px)
	return i;
    return 420;
  }
  pixel_RGB getClusterID(unsigned int cluster_index)
  {
    return this->cluster_list->get(cluster_index)->ID;
  }
  const char* getClusterName(unsigned int cluster_index)
  {
    return this->cluster_list->get(cluster_index)->name;
  }
};

class AggregateChannel:public ChannelProfile
{
  unsigned long samples;
  unsigned int buckets;
  double* bucket_weights;
  AggregateChannel(int buckets) : ChannelProfile(0)
  {
    this->samples = 0;
    this->buckets = buckets;
    this->bucket_weights = new double[buckets];
    for(int i=0;i<buckets;i++)
      this->bucket_weights[i] = 0;
  }
  AggregateChannel(ChannelProfile*cp) : ChannelProfile(0)
  {
    this->samples = cp->samples;
    this->buckets = cp->buckets;
    this->bucket_weights = new double[buckets];
    for(int i=0;i<(int)this->buckets;i++)
      this->bucket_weights[i] = cp->getBucketWeight(i);
  }
  AggregateChannel(ChannelProfile*cp1,ChannelProfile*cp2) : ChannelProfile(0)
  {
    this->samples = cp1->samples + cp2->samples;
    if(cp1->buckets != cp2->buckets)
      printf("ERROR BUCKET MISMATCH, DEFAULT TO cp1\n\t cp1:%i cp2:%i\n",cp1->buckets,cp2->buckets);
    this->buckets = cp1->buckets;
    this->bucket_weights = new double[this->buckets];
    for(int i=0;i<(int)this->buckets;i++)
    {
      //take a weighted average of the weighted averages
      double wt1 = cp1->getBucketWeight(i);
      double wt2 = cp2->getBucketWeight(i);
      this->bucket_weights[i] = (wt1*cp1->samples + wt2*cp2->samples)/(cp1->samples+cp2->samples);
    }
  }
};
#endif
