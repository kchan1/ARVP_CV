#ifndef _IMAGEOPS_HPP_
#define _IMAGEOPS_HPP_
#include "imgformat.hpp"
#include "LinkedList.hpp"
#include <math.h>
#include <gsl/gsl_blas.h>

const int bin_thres[] = {100,200,150};
const int dbl_thres[] = {20,50};

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
      for(i=0;i<this->width;i++)
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
      result_data.unshift(new pixel_RGB[this->width]);
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
      if(*dst_row > 0 && *dst_row < (int)dst_img->height())
      {
	for(i=0;i<(int)this->width;i++)
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
  double sum;
  for(j=0;j<filter->size1;j++)
    for(i=0;i<filter->size2;i++)
    {
      value = 1.0/(2.0*M_PI*pow(stdev,2))
	*exp(-(pow(i-org_x,2)+
	       pow(j-org_y,2))/(2.0*pow(stdev,2)));
      //printf("GAUSS VALUE (%i,%i): %f",i,j,value);
      gsl_matrix_set(filter,j,i,value);
      sum += value;
    }
  printf("SUM: %f",sum);
  gsl_matrix_scale(filter,1.0/sum);
}

void sobel_x(gsl_matrix*filter)
{
  if(filter->size1!=3 || filter->size2!=3)
    return;
  int i,j;
  double sobel_arr[] = {-1,0,1,
			-2,0,2,
			-1,0,1};
  for(j=0;j<3;j++)
    for(i=0;i<3;i++)
      gsl_matrix_set(filter,j,i,sobel_arr[j*3+i]);
  //scale so we don't overflow
  gsl_matrix_scale(filter,0.25);
}
void sobel_y(gsl_matrix*filter)
{
  if(filter->size1!=3 || filter->size2!=3)
    return;
  sobel_x(filter);
  gsl_matrix_transpose(filter);
}

//convolution on a single channel
void convolution_single(ARVP_Image* src_img,int src_channel,
			ARVP_Image* dst_img,int dst_channel,
			gsl_matrix*filter, int filter_y, int filter_x)
{
  //error check that the images are aligned
  if(src_img->height() != dst_img->height() || src_img->width() != dst_img->width())
    return;
  //i,j for image traverse, u,v for filter traverse
  size_t i,j,u,v;
  //newPx is to buffer the convolution result
  double new_ch;
  //create a buffer to store convolution results that cannot yet be written
  ConvResult result_buff(src_img->width());
  //j scans down the rows, i scans across the columns
  //printf("Starting convolution\n");
  for(j=0;j<src_img->width();j++)
  {
    //printf("  Row %i\n",(int)j);
    for(i=0;i<src_img->height();i++)
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
	  new_ch += getBoundChannel(src_img,src_channel,
				    j-filter_y+v,
				    i-filter_x+u)
	  *gsl_matrix_get(filter,v,u);
	}
      //buffer the final result
      //the new_px is just so we can see the old_px as an array
      old_px.ch[dst_channel] = new_ch;
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
  if(src_img->height() != dst_img->height() || src_img->width() != dst_img->width())
    return;
  //i,j for image traverse, k for channels, u,v for filter traverse
  size_t i,j,k,u,v;
  //newPx is to buffer the convolution result
  double new_px[3];
  //create a buffer to store convolution results that cannot yet be written
  ConvResult result_buff(src_img->width());
  //j scans down the rows, i scans across the columns
  //printf("Starting convolution\n");
  for(j=0;j<src_img->width();j++)
  {
    //printf("  Row %i\n",(int)j);
    for(i=0;i<src_img->height();i++)
    {
      //printf("Pixel (%i,%i) \n",(int)j,(int)i);
      for(k=0;k<3;k++)
	new_px[k] = 0;
      //v scans down the rows, u scans across the columns
      for(v=0;v<filter->size1;v++)
	for(u=0;u<filter->size2;u++)
	{
	  //printf("\ti(%i,%i) * f(%i,%i)\n",(int)j,(int)i,(int)v,(int)u);
	  //apply filter element to image element
	  for(k=0;k<3;k++)
	    new_px[k] += getBoundChannel(src_img,k,
					 j-filter_y+v,
					 i-filter_x+u)
	      *gsl_matrix_get(filter,v,u);
	}
      //buffer the final result
      pixel_RGB temp;
      for(k=0;k<3;k++)
	temp.ch[k] = (unsigned char)new_px[k];
      result_buff.bufferResult(temp,j,i);
    }
    //write a (if any) rows the convolution will never read again
    result_buff.writeClear(dst_img,j-filter_y);
  }
  //flush the buffer
  result_buff.flush(dst_img);
}

void cannyEdgeDetection(ARVP_Image* src_img, ARVP_Image*dst_img)
{
  size_t i,j,u,v;
  ARVP_Image* img_buff = dst_img;
  //pixel_RGB px_buff;
  double ch_buff;
  //do a threshold
  //j scans down the rows, i scans down the columns
  /*
  printf("THRESHOLDING\n");
  for(j=0;j<src_img->height();j++)
    for(i=0;i<src_img->width();i++)
    {
      for(k=0;k<3;k++)
      {
	//thresholding
	if(src_img->get_ch(k,j,i)>=bin_thres[k])
	  px_buff.ch[k]=255;
	else
	  px_buff.ch[k]=0;
      }
      img_buff->set(j,i,px_buff);
    }
  */
  printf("BLURRING\n");  
  //use a 5x5 to blur, stdev = 1 
  gsl_matrix*gauss = gsl_matrix_alloc(5,5);
  gaussian(gauss,1);
  //blur the image
  convolution_RGB(img_buff,img_buff,gauss,5/2,5/2);
  //deallocate the gaussian filter because it never gets used again
  gsl_matrix_free(gauss);
  printf("FLATTENING\n");  
  //flatten back to grayscale into the red channel
  for(j=0;j<img_buff->height();j++)
    for(i=0;i<img_buff->width();i++)
    {
      ch_buff = 0;
      ch_buff += 0.2989*img_buff->get_ch(0,j,i);
      ch_buff += 0.5870*img_buff->get_ch(1,j,i);
      ch_buff += 0.1140*img_buff->get_ch(2,j,i);
      img_buff->set_ch(0,j,i,(unsigned char)ch_buff);
    }
  printf("GRADIENT\n");  
  //find gradient
  
  //a pointer for finding the gradient
  gsl_matrix * deriv = gsl_matrix_calloc(3,3);
  //store Gx in Green, Gy in Blue
  sobel_x(deriv);
  convolution_single(img_buff,0,img_buff,1,deriv,1,1);
  //transpose of sobel x is sobel y
  //gsl_matrix_transpose(deriv);
  sobel_y(deriv);
  convolution_single(img_buff,0,img_buff,2,deriv,1,1);
  //free the deriv matrix
  gsl_matrix_free(deriv);
  img_buff->debugPrint(false,true,true);
  printf("GRADIENT CRUNCHING\n");  
  //store G in Green, reduced theta in Blue
  pixel_RGB px;
  double Gx,Gy,G,theta;
  for(j=0;j<img_buff->height();j++)
    for(i=0;i<img_buff->width();i++)
    {
      px = img_buff->get(j,i);
      Gx = pow((signed char)px.ch[1],2);
      Gy = pow((signed char)px.ch[2],2);
      G = sqrt(pow(Gx,2) + pow(Gy,2));
      theta = atan2(Gy,Gx);
      //reduce theta, 0->0d, 1->45d, 2->90d, 3->135d
      //if(theta > M_PI)
      //theta = M_2_PI - theta;
      if(abs(theta)<=M_PI_4/2)
	theta = 0;
      else if(abs(theta)<=M_PI_4+M_PI_4/2)
	theta = 1;
      else if(abs(theta)<M_PI_2+M_PI_4/2)
	theta = 2;
      else if(abs(theta)<M_PI_2+3*M_PI_4/2)
	theta = 3;
      else
	theta = 0;
      img_buff->set_ch(1,j,i,(unsigned char)G);
      img_buff->set_ch(2,j,i,(unsigned char)theta);
    }
  img_buff->debugPrint(false,false,false);
  printf("NONMAX SUPPRESSION\n");  
  //nonmax suppression
  bool * is_max = new bool[img_buff->height()*img_buff->width()];
  //unsigned char theta;
  unsigned char G_curr,G_pos,G_neg;
  //figure out which pixels are non-max
  printf("MAX FINDING LOOP\n");
  img_buff->debugPrint(false,true,false);
  for(j=0;j<img_buff->height();j++)
    for(i=0;i<img_buff->width();i++)
    {
      //printf("img[%i,%i]\n",(int)j,(int)i);  
      theta = img_buff->get_ch(2,j,i);
      //theta = 0;
      //printf("\tTHETA SWITCH %i\n",theta);
      if(theta == 0)
      {
	G_pos = (signed char)getBoundChannel(img_buff,1,j,i+1);
	G_neg = (signed char)getBoundChannel(img_buff,1,j,i-2);
      }
      else if(theta == 1)
      {
	//printf("\tCASE 1\n");
	G_pos = (signed char)getBoundChannel(img_buff,1,j+1,i-1);
	G_neg = (signed char)getBoundChannel(img_buff,1,j-1,i+1);
      }
      else if(theta == 2)
      {
	//printf("\tCASE 2\n");
	G_pos = (signed char)getBoundChannel(img_buff,1,j-1,i);
	G_neg = (signed char)getBoundChannel(img_buff,1,j+1,i);
      }
      else
      {
	//printf("\tCASE 3\n");
	G_pos = (signed char)getBoundChannel(img_buff,1,j-1,i-1);
	G_neg = (signed char)getBoundChannel(img_buff,1,j+1,i+1);
      }
      //printf("\tCURR SET\n");
      G_curr = (signed char)getBoundChannel(img_buff,1,j,i);
      //G_curr = 0;
      //printf("\tISMAX DETERMINE\n");
      if(G_curr > G_pos && G_curr > G_neg)
	is_max[j*img_buff->width()+i] = true;
      else
	is_max[j*img_buff->width()+i] = false;
      /*
      printf("%i/%i * %i/%i = %i/%i\n",
	     (int)j,
	     (int)img_buff->height(),
	     (int)i,
	     (unsigned int)img_buff->width(),
	     int(j*img_buff->width()+i),
	     int(img_buff->height()*img_buff->width()));
      */
      //printf("\tend\n");
    }//end for
  printf("ACTUAL SUPRESSION\n");  
  //suppress all the non-max
  for(j=0;j<img_buff->height();j++)
    for(i=0;i<img_buff->width();i++)
    {
      if(!is_max[j*img_buff->width()+i])
      {
	img_buff->set_ch(0,j,i,0);
	img_buff->set_ch(1,j,i,0);
      }
      else
	printf("%i,%i MAX!\n",(int)j,(int)i);
    }
  img_buff->debugPrint();
  printf("DOUBLE THRESHOLD\n");   
  //double threshold, results in blue
  //0 is supress, 1 is weak, 2 is strong
  for(j=0;j<img_buff->height();j++)
    for(i=0;i<img_buff->width();i++)
    {
      //if strong
      if(abs((signed char)img_buff->get_ch(1,j,i))>=dbl_thres[1])
	img_buff->set_ch(2,j,i,2);
      //if weak
      else if(abs((signed char)img_buff->get_ch(1,j,i))>=dbl_thres[0])
	img_buff->set_ch(2,j,i,1);
      //weaker than weak
      else
      {
	img_buff->set_ch(0,j,i,0);
	img_buff->set_ch(1,j,i,0);
	img_buff->set_ch(2,j,i,0);
      }
    }
  printf("EDGE TRACKING BY HYSTERESIS\n");   
  
  //edge tracking by hysteresis
  //all weak values evaluated for strong neighbors
  for(j=0;j<img_buff->height();j++)
    for(i=0;i<img_buff->width();i++)
    {
      if(img_buff->get_ch(2,j,i)==2)
	is_max[j*img_buff->width()+i] = true;
      else if(img_buff->get_ch(2,j,i)==1)
      {
	is_max[j*img_buff->width()+i] = false;
	for(v=0;v<3;v++)
	  for(u=0;u<3;u++)
	    if(getBoundChannel(img_buff,2,j+v,i+u)==2)
	      is_max[j*img_buff->width()+i] = true;
      }
    }
  //finish the canny edge detection
  pixel_RGB edge;
  for(i=0;i<3;i++)
    edge.ch[i]=255;
  pixel_RGB not_edge;
  for(i=0;i<3;i++)
    not_edge.ch[i]=0;
  for(j=0;j<img_buff->height();j++)
    for(i=0;i<img_buff->width();i++)
      if(is_max[j*img_buff->width()+i])
	img_buff->set(j,i,edge);
      else
	img_buff->set(j,i,not_edge);
  printf("DELETING LEFTOVER BUFFER\n");  
  delete[] is_max;
  printf("RETURN\n");    
}


#endif
