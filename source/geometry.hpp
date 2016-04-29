#ifndef _GEOMETRY_HPP_
#define _GEOMETRY_HPP_
typedef struct coord {
  unsigned int axis[2];
  bool operator ==(const coord& c)
  {return this->axis[0]==c->axis[0] && this->axis[1]==c->axis[1];}
} coord;

class Blob
{
  coord*coords;
  unsigned int num_coords;
  unsigned int capacity;
  Blob(int max_size)
  {
    this->capacity=max_size;
    this->num_coords = 0;
    this->coords = new coord[max_size];
  }
  void expandBlob()
  {
    unsigned int next_size = capacity + (capacity/2+1);
    coord*next_coords = new coord[next_size];
    for(int i=0;i<num_coords;i++)
    {
      next_coords[i]=this->coords[i];
    }
    delete[] this->coords;
    this->coords = next_coords;
    this->capacity = next_size;
  }
  void addCoord(coord c)
  {
    if(this->num_coords==this->capacity)
      expandBlob();
    this->coords[num_coords++]=c;
  }
  ~Blob()
  {
    delete[] coords;
  }
}

template<class T>
class Point2D
{
public:
  T xpos,ypos;
  Point2D(T xpos,T ypos)
  {
    this->xpos=xpos;
    this->ypos=ypos;
  }
  Point2D(coord c)
  {
    this->xpos=c.axis[0];
    this->ypos=c.axis[1];
  }
  coord toCoord()
  {
    coord c;
    c.axis[0]=xpos;
    c.axis[1]=ypos;
    return c;
  }
};
#endif
