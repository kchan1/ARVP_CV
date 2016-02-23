#include "disp.h"
#include "imgformat.h"
#include <math.h>
void calcSingleDispSAD(gsl_matrix* imgL,
		       gsl_matrix* imgR, 
		       gsl_matrix* imgTgt,
		       int maxDisp,
		       int windowSize,
		       int locationX,
		       int locationY)
{
  int i,j,u,v;
  //int mincostindex = -1;
  //int mincost = -1;
  int costs[maxDisp+1];
  //set costs that are within bounds to 0
  for(j=0;j<maxDisp+1 && (locationX-j)>=0;j++)
  {
    costs[j] = 0;
  }
  //set costs that are out of bounds to ridiculous number
  for(j++;j<maxDisp+1;j++)
  {
    costs[j] = 0xFF*3;
  }
  //iterate over disparities, bound by both maxDisp and the image boundary
  for(i=0;i<=maxDisp && (locationX-i)>=0;i++)
  {
    //iterate over the windows both L and R
    for(u=-(windowSize/2);u<=windowSize/2;u++)
      for(v=-(windowSize/2);v<=windowSize/2;v++)
      {
	ulong Lpx = getBoundPixel(imgL,locationX+u  ,locationY+v);
	ulong Rpx = getBoundPixel(imgR,locationX+u-i,locationY+v);
	//TODO: put abs and the other channels too
	cost[i] += abs(getChanR(Lpx) - getChanR(Rpx)) +
	  abs(getChanG(Lpx) - getChanG(Rpx)) +
	  abs(getChanB(Lpx) - getChanB(Rpx));
	
      }
  }
  //find min cost
  for(i=1,j=0;i<=maxDisp;i++)
    if(cost[i]<cost[j])
      j=i;
  
}
