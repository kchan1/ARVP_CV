#ifndef _GEOMETRY_HPP_
#define _GEOMETRY_HPP_
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
};
#endif
