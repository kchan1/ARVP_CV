#ifndef _KMEANSFORMAT_HPP_
#define _KMEANSFORMAT_HPP_
class ClusterProfile
{
public:
  class ChannelProfile
  {
  public:
    double*levels;
    ChannelProfile(int buckets)
    {
      this->levels = new double[buckets];
      for(int i=0;i<buckets;i++)
	levels[i] = 0;
    }
  };

  int buckets;
  int channels;
  ChannelProfile ** channel_list;

  ClusterProfile(int channels,int buckets)
  {
    this->channels = channels;
    this->buckets = buckets;
    this->channel_list = new ChannelProfile[channels];
    for(int i=0;i<channels;i++)
      this->channel_list[i] = new ChannelProfile(buckets);
  }

  ClusterProfile() : ClusterProfile(3,8)
  {;}

  ~ClusterProfile()
  {
    for(int i=0;i<this->channels;i++)
      delete this->channel_list[i];
    delete[] this->channel_list;
  }
};
#endif
