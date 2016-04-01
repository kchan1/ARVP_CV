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
  if(src_img->channels == 3)
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
  if(src_img->channels == 3)
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
  printf("OPENING IMAGE\n");
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  
  JSAMPARRAY buffer;		/* Output row buffer */
  int row_stride;		/* physical row width in output buffer */
  unsigned int i,j;
  //printf("OPENING INPUT FILE\n");
  cinfo.err = jpeg_std_error(&jerr);
  FILE * infile;
  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    exit(1);
  }
  //printf("INITIALIZING PARAMETERS\n");
  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);
  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components; 
  /* Make a one-row-high sample array that will go away when done with image */
  //printf("CREATING BUFFER\n");
  
  //buffer = (*cinfo.mem->alloc_sarray)
  //  ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
  unsigned char * rowbuff = new unsigned char[row_stride];
  buffer = &rowbuff;
  //printf("CREATING DATA\n");
  unsigned char*data = new unsigned char[cinfo.output_width*cinfo.output_height*3];
  //printf("LOOPING TO READ...\n");  
  while(cinfo.output_scanline < cinfo.output_height)
  {
    int scanned_line = cinfo.output_scanline;
    jpeg_read_scanlines(&cinfo, buffer, 1);
    //printf("%i\n",cinfo.output_scanline);
    //printf("DECIDING WHERE TO PUT IT\n");
    //put_scanline_someplace(buffer,rowstride);
    /*
    printf("BUFFER: (");
    for(i=0;(int)i<row_stride;i++)
    {
      printf("%i ",buffer[0][i]);
    }
    printf(") \n");
    */
    if(cinfo.out_color_components==3)
    {
      for(i=0;(int)i<row_stride;i++)
      {
	/*
	printf("d[%i,%i,%i]=d[%i]=b[%i]=%i\n",
	       i%3,
	       scanned_line,
	       i/3,
	       (scanned_line*cinfo.output_width*3)+i,
	       i,buffer[0][i]);
	*/
	data[(scanned_line*cinfo.output_width*3)+i] = buffer[0][i];
      }
    }
    else
    {
      for(i=0;i<cinfo.output_width;i++)
      {
	for(j=0;j<3;j++)
	{
	  //printf("d[%i,%i]=b[%i]=%i ",
      	  //	 (scanned_line*cinfo.output_width+i)*3+j,j,i,buffer[0][i]);
	  data[(scanned_line*cinfo.output_width+i)*3+j] = buffer[0][i];
	}
	printf("\n");
      }
    }
  }
  //printf("\n");
  //printf("DESTROYING SETUP\n");  
  ARVP_Image*dst_img = new ARVP_Image(cinfo.output_height,
				      cinfo.output_width,
				      data);  
  //printf("FINISHING\n");
  jpeg_finish_decompress(&cinfo);
  //printf("DESTROYING\n"); 
  jpeg_destroy_decompress(&cinfo);
  //printf("DELETEBUFF\n");
  delete[] rowbuff;
  //printf("RETURNING\n");
  return dst_img;
}
#endif
