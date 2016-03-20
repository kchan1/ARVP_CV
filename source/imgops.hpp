#ifndef _IMAGEOPS_HPP_
#define _IMAGEOPS_HPP_
#include "imgformat.hpp"
#include "LinkedList.hpp"
#include <math.h>
#include <gsl/gsl_blas.h>

class ConvResult
{
private:
  LinkedList<gsl_matrix> result_data;
  LinkedList<int> result_row;
  size_t width;
public:
  ConvResult(int width)
  {
    this->width = width;
  }
  
  //once a convolution reaches the end of a row,
  //it will call this with the topmost row that it has cleared.
  void writeClear(gsl_matrix*dst,int cleared_row)
  {
    unsigned int i;
    //if the row the convolution has cleared is equal to the topmost row, which it should be usually
    if(cleared_row == *result_row.get(result_row.getSize()-1))
    {
      //get the topmost row's data and index
      gsl_matrix * to_copy = result_data.pop();
      int* dst_row = result_row.pop();
      //copy the buffer (could optimize by doing a pointer replacement maybe)
      for(i=0;i<width;i++)
	gsl_matrix_set(dst,*dst_row,i,gsl_matrix_get(to_copy,0,i));
      delete dst_row;
      result_data.unshift(to_copy);
      result_row.unshift(new int());
      *result_row.get(0) = -1;
    }
  }
  //buffers a single result, may create a new buffer
  void bufferResult(double result,int row,int column)
  {
    //case of needing to add a new row, when there are no rows, or the rows do not match
    if(result_row.getSize()==0 || (*(result_row.get(0))!=row && *(result_row.get(0))>=0))
    {
      //printf("CONVRES:ADDING BUFFER!!!\n");
      //add new data row
      result_data.unshift(gsl_matrix_calloc(1,width));
      //add the new row's index
      result_row.unshift(new int());
      *result_row.get(0) = row;
    }
    //this allows us to recycle the multi-kilobyte buffers
    else if((*result_row.get(0))<0)
    {
      //printf("CONVRES:RECYCLING!!!\n");
      delete result_row.shift();
      result_row.unshift(new int());
      *result_row.get(0) = row;
    }
    //the case when we're mid-row
    else
    {
      //printf("SET AT: (%i,%i)\n",(int)row,(int)column);
      //set the frontmost data row's column to be the result
      gsl_matrix_set(result_data.get(0),0,column,result);
    }
  }
  //last thing to be called during a convolution!!!!
  //deallocates the kilobyte buffers 
  void flush(gsl_matrix*dst_img)
  {
    int i;
    gsl_matrix* to_copy;
    int* dst_row;
    //iterate through all the result data buffers
    while(result_data.hasNext())
    {
      //get the highest buffer and its row
      to_copy = result_data.pop();
      dst_row = result_row.pop();
      //if its row is within bounds, write it to the image
      if(*dst_row > 0 && *dst_row < (int)dst_img->size1)
	for(i=0;i<(int)width;i++)
	  gsl_matrix_set(dst_img,*dst_row,i,gsl_matrix_get(to_copy,0,i));
      //printf("CONVRES:BUFFER DELETED IN FLUSH!!!\n");
      gsl_matrix_free(to_copy);
      delete dst_row;
    }
  }
};

void gaussian(gsl_matrix*filter,float stdev)
{
  unsigned int i,j;
  float org_x,org_y;
  double value = 0;
  org_x = filter->size1/2;
  org_y = filter->size2/2;
  for(j=0;j<filter->size1;j++)
    for(i=0;i<filter->size2;i++)
    {
      value = 1.0/(2.0*M_PI*pow(stdev,2))
	*exp(-(pow(i-org_x,2)+
	       pow(j-org_y,2))/(2.0*pow(stdev,2)));
      //printf("GAUSS VALUE (%i,%i): %f",i,j,value);
      gsl_matrix_set(filter,j,i,value);
    }
}

//TODO: implement convolutions without a (huge) buffer, via buffering values,
//  until they are ready to be wrirtten
//  ready to write when convolution sweeps it on the upper left corner
//  flush also at the end
//  buffer should be filter-center tall, image-width wide.
//  buffer should be implemented as a circular queue.
//convolution that returns a new image
void convolution(gsl_matrix* src_img, gsl_matrix* dst_img, gsl_matrix*filter,
		 int filter_y, int filter_x,
		 unsigned long src_channel_width, int src_channel_offset,
		 unsigned long dst_channel_width, int dst_channel_offset)
{
  if(src_img->size1 != dst_img->size1 || src_img->size2 != dst_img->size2)
    return;
  size_t i,j,u,v;
  //create a buffer to store convolution results that cannot yet be written
  ConvResult result_buff(src_img->size2);
  //j scans down the rows, i scans across the columns
  //printf("Starting convolution\n");
  for(j=0;j<src_img->size1;j++)
  {
    //printf("  Row %i\n",(int)j);
    for(i=0;i<src_img->size2;i++)
    {
      //printf("    Px (%i,%i)\n",(int)j,(int)i);
      double newPx = 0;
      //v scans down the rows, u scans across the columns
      for(v=0;v<filter->size1;v++)
	for(u=0;u<filter->size2;u++)
	{
	  //apply filter element to image element
	  newPx += (((int)getBoundPixel(src_img,
					j-filter_y+v,
					i-filter_x+u)
		     >> src_channel_offset) & src_channel_width)
	    *gsl_matrix_get(filter,v,u);
	}
      //buffer the final result
      result_buff.bufferResult((((int)newPx)&dst_channel_width)<<dst_channel_offset,j,i);
    }
    //write a (if any) rows the convolution will never read again
    result_buff.writeClear(dst_img,j-filter_y);
  }
  //flush the buffer
  result_buff.flush(dst_img);
}


//void convolution(gsl_matrix* img, gsl_matrix*filter, int filter_x, int filter_y,ulong channel)
//{
//  return;
//}

//convolution, overwites destination on the channel
//convolution in place, overwrites image
void convolution_buffered(gsl_matrix* src_img, gsl_matrix* dst_img, gsl_matrix*filter,
		 int filter_x, int filter_y,
		 unsigned long src_channel_width, int src_channel_offset,
		 unsigned long dst_channel_width, int dst_channel_offset)
{
  size_t i,j,u,v;
  //create a buffer to store convolution results
  gsl_matrix* img_buff = gsl_matrix_calloc(src_img->size1,src_img->size2);
  //j scans down the rows, i scans across the columns
  for(j=0;j<src_img->size1;j++)
    for(i=0;i<src_img->size2;i++)
    {
      double newPx = 0;
      //v scans down the rows, u scans across the columns
      for(v=0;v<filter->size1;v++)
	for(u=0;u<filter->size2;u++)
	{
	  //apply filter element to image element
	  newPx += (((int)getBoundPixel(src_img,
					j-filter_y+v,
					i-filter_x+u)
		     >> src_channel_offset) & src_channel_width)
	    *gsl_matrix_get(filter,v,u);
	}
      gsl_matrix_set(img_buff,j,i,(((int)newPx)&dst_channel_width)<<dst_channel_offset);
    }
  //copy the buffer into dst_img
  for(j=0;j<img_buff->size1;j++)
    for(i=0;i<img_buff->size2;i++)
    {
      gsl_matrix_set(dst_img,j,i,(unsigned long)gsl_matrix_get(img_buff,j,i));
    }
  return;
}
#endif
