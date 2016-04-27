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
    this->buckets = buckets;
    this->bucket_list = new long[buckets];
    for(int i=0;i<buckets;i++)
      bucket_list[i] = 0;
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
    this->samples += amount;
    this->bucket_list[bucket] += amount;
  }
  
  virtual double getMean()
  {
    printf("GETTING MEAN WITH %u SAMPLES\n",(unsigned int)this->samples);
    double mean = 0;
    for(unsigned int i=0;i<this->buckets;i++)
    {
      long double bucket = (long double)(this->bucket_list[i]);
      long double num_samples = (long double)(this->samples);
      printf("BUCKET %i HAS %i / %u \n",(int)i,(int)this->bucket_list[i],(unsigned int)this->samples);
      printf("\tWEIGHT: %f\n",double(bucket/num_samples));
      mean += double(bucket*i/num_samples);
    }
    return mean;
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
};

class AggregateChannel : public ChannelProfile
{
public:
  double mean;
  //long samples;
  AggregateChannel(double mean,unsigned int samples) :
    ChannelProfile(0)
  {
    this->mean = mean;
    this->samples = samples;
  }
  AggregateChannel() :
    ChannelProfile(0)
  {
    this->mean = 0;
    this->samples = 0;
  }
  /*
  static AggregateChannel * combineProfiles( LinkedList<ChannelProfile> *profile_list)
  {
    double new_mean = 0;
    long new_samples = 0;
    for(int i=0;i<profile_list->getSize();i++)
    {
      ChannelProfile*cp = profile_list.get(i);
      new_mean += cp->getMean();
      new_samples += cp->channels;
    }
    return new AggregateChannel(new_mean,new_samples);
  }
  */
};

class AggregateProfile : public ClusterProfile
{
  AggregateChannel**channel_list;
  AggregateProfile(int channels)
  {
    this->samples = 0;
    this->channels = channels;
    this->buckets = 0;
    this->channel_list=new AggregateChannel*[channels];
  }
  AggregateProfile(ClusterProfile*prev_profile)
  {
    this->samples = 0;
    this->channels = prev_profile->channels;
    this->buckets = 0;
    this->channel_list=new AggregateChannel*[prev_profile->channels];
    for(unsigned int i=0;i<prev_profile->channels;i++)
      this->channel_list[i]=new AggregateChannel(prev_profile->channel_list[i]->getMean(),prev_profile->channel_list[i]->samples);
  }
  ~AggregateProfile()
  {
    for(unsigned int i=0;i<channels;i++)
      delete channel_list[i];
    delete[] channel_list;
  }
  virtual void addSample(unsigned int channel,unsigned int bucket, int amount)
  {return;}

  virtual double getChannelMean(unsigned int channel)
  {
    return this->channel_list[channel]->getMean();
  }
  /*
  static AggregateProfile*combineProfiles(LinkedList<ClusterProfile>*profile_list)
  {
    AggregateProfile*new_profile = new AggregateProfile(profile_list.get(0)->channels);
    LinkedList<ChannelProfile>*ch_profiles();
    for(int i=0;i<new_profile->channels;i++)
    {
      while(ch_profiles->hasNext())
	delete ch_profiles->pop();
      for(int j=0;j<profile_list->getSize();j++)
      {
	ch_profiles.push(profile_list->get(j)->channel_list[i])
      }
      new_profile->channel_list[i] = AggregateChannel::combineProfiles(ch_profiles);
    }
    return new_profile;
  }
  */
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
    /*
    ~ClusterSignature()
    {
      delete[] name;
    }
    */
    
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
};
#endif
