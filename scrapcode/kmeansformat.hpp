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
