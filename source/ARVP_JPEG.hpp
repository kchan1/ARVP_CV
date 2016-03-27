#include "imgformat.hpp"
#include <jpeglib.h>
#ifndef _ARVP_JPEG_HPP_
#define _ARVP_JPEG_HPP_
bool saveARVP_Image(ARVP_Image * src_img,const char*filename)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  jpeg_create_compress(&cinfo);
  
  FILE *outfile;
  if ((outfile = fopen(filename, "wb")) == NULL) 
  {
    fprintf(stderr, "can't open %s\n",filename);
    return false;
  }
  jpeg_stdio_dest(&cinfo, outfile);
  //printf("Destination Set\n");
  cinfo.err = jpeg_std_error(&jerr);
  //printf("Error Set\n");
  cinfo.image_width = src_img->width;      /* image width and height, in pixels */
  cinfo.image_height = src_img->height;
  //printf("Width & Height Set\n");
  if(src_img->CHANNELS == 3)
  {
    cinfo.input_components = 3;     /* # of color components per pixel */
    cinfo.in_color_space = JCS_RGB; /* colorspace of input image */
  }
  else
  {
    cinfo.input_components = 1;     /* # of color components per pixel */
    cinfo.in_color_space = JCS_GRAYSCALE; /* colorspace of input image */
  }
  //printf("Colors Set\n");
  jpeg_set_defaults(&cinfo);
  /* Make optional parameter settings here */
  //printf("Compression Start\n");
  jpeg_start_compress(&cinfo, TRUE);
  JSAMPROW row_pointer[1];	/* pointer to a single row */
  int row_stride;	        /* physical row width in buffer */
  if(src_img->CHANNELS == 3)
    row_stride = src_img->width*3;	/* JSAMPLEs per row in image_buffer */
  else
    row_stride = src_img->width;
  //printf("Scanning... ");
  while (cinfo.next_scanline < cinfo.image_height) 
  {
    row_pointer[0] = &(src_img->data[cinfo.next_scanline * row_stride]);
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  //printf("Done!\n");
  jpeg_finish_compress(&cinfo);
  //printf("Destroy Everything!\n");
  jpeg_destroy_compress(&cinfo);
  return true;
}

ARVP_Image* openJPEG(const char* filename)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  
  JSAMPARRAY buffer;		/* Output row buffer */
  int row_stride;		/* physical row width in output buffer */
  unsigned int i,j;
  
  cinfo.err = jpeg_std_error(&jerr);
  FILE * infile;
  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    exit(1);
  }
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);
  //unsigned char*buffer = new unsigned char[cinfo.output_width];
  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components; 
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
  unsigned char*data = new unsigned char[cinfo.output_width*cinfo.output_height*3];
  while(cinfo.output_scanline < cinfo.output_height)
  {
    jpeg_read_scanlines(&cinfo, buffer, 1);
    //put_scanline_someplace(rowstride);
    if(cinfo.out_color_components==3)
    {
      for(i=0;i<cinfo.output_width;i++)
	for(j=0;j<3;i++)
	  data[cinfo.output_scanline*cinfo.output_width+i+j] = buffer[0][i];
    }
    else
    {
      for(i=0;i<cinfo.output_width;i++)
	data[cinfo.output_scanline*cinfo.output_width+i]=buffer[0][i];
    }
  }
  ARVP_Image*dst_img = new ARVP_Image(cinfo.output_height,cinfo.output_width,data);
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  return dst_img;
}
#endif
