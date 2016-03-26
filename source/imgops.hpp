#ifndef _IMAGEOPS_HPP_
#define _IMAGEOPS_HPP_
#include "imgformat.hpp"
#include "LinkedList.hpp"
#include <math.h>
#include <gsl/gsl_blas.h>

class ConvResult
{
private:
  LinkedList<pixel_RGB> result_data;
  LinkedList<int> result_row;
  size_t width;
public:
  ConvResult(size_t width)
  {
    this->width = width;
  }
  
  //once a convolution reaches the end of a row,
  //it will call this with the topmost row that it has cleared.
  void writeClear(ARVP_Image*dst,int cleared_row)
  {
    unsigned int i;
    //if the row the convolution has cleared is equal to the topmost row, which it should be usually
    if(cleared_row == *result_row.get(result_row.getSize()-1))
    {
      //get the topmost row's data and index
      pixel_RGB * to_copy = result_data.pop();
      int* dst_row = result_row.pop();
      //copy it into the destination image
      for(i=0;i<width;i++)
      {
	//gsl_matrix_set(dst,*dst_row,i,gsl_matrix_get(to_copy,0,i));
	dst->set(*dst_row,i,to_copy[i]);
      }
      delete dst_row;
      result_data.unshift(to_copy);
      result_row.unshift(new int());
      *result_row.get(0) = -1;
    }
  }
  //buffers a single result, may create a new buffer
  void bufferResult(pixel_RGB result,int row,int column)
  {
    //case of needing to add a new row, when there are no rows, or the rows do not match
    if(result_row.getSize()==0 || (*(result_row.get(0))!=row && *(result_row.get(0))>=0))
    {
      //printf("CONVRES:ADDING BUFFER!!!\n");
      //add new data row
      //result_data.unshift((unsigned long*)calloc(width,(sizeof(unsigned long))*width));
      result_data.unshift(new pixel_RGB[width]);
      //add the new row's index
      result_row.unshift(new int());
      *result_row.get(0) = row;
      result_data.get(0)[column] = result;
    }
    //this allows us to recycle the multi-kilobyte buffers
    else if((*result_row.get(0))<0)
    {
      //printf("CONVRES:RECYCLING!!!\n");
      result_row.unshift(result_row.shift());
      *result_row.get(0) = row;
      result_data.get(0)[column] = result;
    }
    //the case when we're mid-row
    else
    {
      //printf("SET AT: (%i,%i)\n",(int)row,(int)column);
      //set the frontmost data row's column to be the result
      result_data.get(0)[column] = result;
    }
  }
  //last thing to be called during a convolution!!!!
  //deallocates the kilobyte buffers 
  void flush(ARVP_Image*dst_img)
  {
    int i;
    pixel_RGB* to_copy;
    int* dst_row;
    //iterate through all the result data buffers
    while(result_data.hasNext())
    {
      //get the highest buffer and its row
      to_copy = result_data.pop();
      dst_row = result_row.pop();
      //if its row is within bounds, write it to the image
      if(*dst_row > 0 && *dst_row < (int)dst_img->height)
      {
	for(i=0;i<(int)width;i++)
	{
	  dst_img->set(*dst_row,i,to_copy[i]);
	}
      }
      //printf("CONVRES:BUFFER DELETED IN FLUSH!!!\n");
      //gsl_matrix_free(to_copy);
      delete[] to_copy;
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

//convolution on a single channel
void convolution_single(ARVP_Image* src_img, ARVP_Image* dst_img, 
		 gsl_matrix*filter, int filter_y, int filter_x,
		 int channel)
{
  //error check that the images are aligned
  if(src_img->height != dst_img->height || src_img->width != dst_img->width)
    return;
  //i,j for image traverse, u,v for filter traverse
  size_t i,j,u,v;
  //newPx is to buffer the convolution result
  double new_ch;
  //create a buffer to store convolution results that cannot yet be written
  ConvResult result_buff(src_img->width);
  //j scans down the rows, i scans across the columns
  //printf("Starting convolution\n");
  for(j=0;j<src_img->width;j++)
  {
    //printf("  Row %i\n",(int)j);
    for(i=0;i<src_img->height;i++)
    {
      
      //printf("    Px (%i,%i)\n",(int)j,(int)i);
      new_ch = 0;
      pixel_RGB old_px = getBoundPixel(src_img,
				       j-filter_y+v,
				       i-filter_x+u);
      //v scans down the rows, u scans across the columns
      for(v=0;v<filter->size1;v++)
	for(u=0;u<filter->size2;u++)
	{
	  //apply filter element to image element
	  new_ch += getBoundChannel(src_img,channel,
				    j-filter_y+v,
				    i-filter_x+u)
	  *gsl_matrix_get(filter,v,u);
	}
      //buffer the final result
      //the new_px is just so we can see the old_px as an array
      old_px.ch[channel] = new_ch;
      result_buff.bufferResult(old_px,j,i);
    }
    //write a (if any) rows the convolution will never read again
    result_buff.writeClear(dst_img,j-filter_y);
  }
  //flush the buffer
  result_buff.flush(dst_img);
}

//convolution on RGB channels
void convolution_RGB(ARVP_Image* src_img, ARVP_Image* dst_img, 
		 gsl_matrix*filter, int filter_y, int filter_x)
{
  //error check that the images are aligned
  if(src_img->height != dst_img->height || src_img->width != dst_img->width)
    return;
  //i,j for image traverse, k for channels, u,v for filter traverse
  size_t i,j,k,u,v;
  //newPx is to buffer the convolution result
  pixel_RGB new_px;
  //create a buffer to store convolution results that cannot yet be written
  ConvResult result_buff(src_img->width);
  //j scans down the rows, i scans across the columns
  //printf("Starting convolution\n");
  for(j=0;j<src_img->width;j++)
  {
    //printf("  Row %i\n",(int)j);
    for(i=0;i<src_img->height;i++)
    {
      //printf("    Px (%i,%i)\n",(int)j,(int)i);
      for(k=0;k<3;k++)
	new_px.ch[k] = 0;
      //v scans down the rows, u scans across the columns
      for(v=0;v<filter->size1;v++)
	for(u=0;u<filter->size2;u++)
	{
	  //apply filter element to image element
	  for(k=0;k<3;k++)
	    new_px.ch[k] += getBoundChannel(src_img,k,
					 j-filter_y+v,
					 i-filter_x+u)
	      *gsl_matrix_get(filter,v,u);
	}
      //buffer the final result
      result_buff.bufferResult(new_px,j,i);
    }
    //write a (if any) rows the convolution will never read again
    result_buff.writeClear(dst_img,j-filter_y);
  }
  //flush the buffer
  result_buff.flush(dst_img);
}

/*
//convolution, overwites destination on the channel
//convolution in place, overwrites image
void convolution_buffered(ARVP_Image* src_img, ARVP_Image* dst_img, gsl_matrix*filter,
		 int filter_x, int filter_y,
		 unsigned long src_channel_width, int src_channel_offset,
		 unsigned long dst_channel_width, int dst_channel_offset)
{
  size_t i,j,u,v;
  //create a buffer to store convolution results
  //gsl_matrix* img_buff = gsl_matrix_calloc(src_img->size1,src_img->size2);
  ARVP_Image * img_buff = new ARVP_Image(src_img->height,src_img->width);
  //j scans down the rows, i scans across the columns
  for(j=0;j<src_img->height;j++)
    for(i=0;i<src_img->width;i++)
    {
      double newPx = 0;
      //v scans down the rows, u scans across the columns
      for(v=0;v<filter->size1;v++)
	for(u=0;u<filter->size2;u++)
	{
	  //apply filter element to image element
	  newPx += (((unsigned long)getBoundPixel(src_img,
					j-filter_y+v,
					i-filter_x+u)
		     >> src_channel_offset) & src_channel_width)
	    *gsl_matrix_get(filter,v,u);
	}
      //gsl_matrix_set(img_buff,j,i,(((int)newPx)&dst_channel_width)<<dst_channel_offset);
      img_buff->set(j,i,(((unsigned long)newPx)&dst_channel_width)<<dst_channel_offset);
    }
  //copy the buffer into dst_img
  for(j=0;j<img_buff->height;j++)
    for(i=0;i<img_buff->width;i++)
    {
      //gsl_matrix_set(dst_img,j,i,(unsigned long)img_buff->get(j,i));
      dst_img->set(j,i,img_buff->get(j,i));
    }
  delete img_buff;
  return;
}
*/
#endif
