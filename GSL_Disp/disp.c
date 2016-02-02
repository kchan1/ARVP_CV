#include disp.h
#include imgformat.h
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
  for(j=0;j<maxDisp+1;j++)
  {
    costs[j] = 0;
  }
  //set costs that are out of bounds to ridiculous number
  for(j=0;j<maxDisp+1;j++)
  {
    costs[j] = 0xFF*3;
  }
  //calculate costs
  for(i=0;i<=maxDisp && (i+locationX)<imgL.size2;i++)
  {
    //i is now the new x-location
    //calculate match cost
    for(u=-(windowSize/2);u<=windowSize/2;u++)
      for(v=-(windowSize/2);v<=windowSize/2;v++)
      {
	int Lpx = gsl_matrix_get(imgL,locationX+u  ,locationY+v);
	int Rpx = gsl_matrix_get(imgR,locationX+u+i,locationY+v);
	//TODO: put abs and the other channels too
	cost[i] += Lpx&CHAN_R - Rpx&CHAN_R;
      }
  }
}
