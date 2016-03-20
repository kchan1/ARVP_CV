#include <iostream>
#include "imgformat.hpp"
#include "imgops.hpp"
#include <gsl/gsl_blas.h>
#include <jpeglib.h>
int main(int argc,char*argv[])
{
  std::cout<<"--Testing basic image functions--\n";

  const int img_rows = 1000;
  const int img_cols = 1000;
  gsl_matrix*img = gsl_matrix_calloc(img_rows,img_cols);
  for(unsigned int j=0;j<img->size1;j++)
    for(unsigned int i=0;i<img->size2;i++)
      gsl_matrix_set(img,j,i,i%2==0?255:0);
  std::cout<<"Image Diagonal:\n";
  for(unsigned int i=0;i<img->size1&&i<img->size2;i++)
    printf("\timg[%i,%i]=%i\n",i,i,int(gsl_matrix_get(img,i,i)));
  
  std::cout<<"--Gaussian Test--\n";
  gsl_matrix*gauss = gsl_matrix_alloc(7,7);
  gaussian(gauss,0.84089642);
  
  std::cout<<"Gaussian Diagonal:\n";
  for(unsigned int i=0;i<gauss->size1&&i<gauss->size2;i++)
    printf("\tgauss[%i,%i]=%0.10f\n",i,i,gsl_matrix_get(gauss,i,i));
  
  std::cout<<"--Convolution Test--\n";
  convolution(img,img,gauss,
	      gauss->size1/2,gauss->size2/2,
	      COLOR_WHITE,0,
	      COLOR_WHITE,0);
  /*
  std::cout<<"Image Diagonal:\n";
  for(unsigned int i=0;i<img->size1&&i<img->size2;i++)
    printf("\timg[%i,%i]=%i\n",i,i,int(gsl_matrix_get(img,i,i)));
  */
  //img buffer for later use
  unsigned char img_int[img_rows*img_cols];
  for(int j=0;j<img_rows;j++)
    for(int i=0;i<img_cols;i++)
      img_int[i+j*img_cols] = gsl_matrix_get(img,j,i);
  //weird jpeglib commands here for compressing to a file
  printf("Writing JPEG image!\n");
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  jpeg_create_compress(&cinfo);
  
  FILE *outfile;
  if ((outfile = fopen("img.jpg", "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", "saveimg.jpg");
    exit(1);
  }
  jpeg_stdio_dest(&cinfo, outfile);
  printf("Destination Set\n");
  cinfo.err = jpeg_std_error(&jerr);
  printf("Error Set\n");
  cinfo.image_width = img_cols;      /* image width and height, in pixels */
  cinfo.image_height = img_rows;
  printf("Width & Height Set\n");
  cinfo.input_components = 1;     /* # of color components per pixel */
  cinfo.in_color_space = JCS_GRAYSCALE; /* colorspace of input image */
  printf("Colors Set\n");
  jpeg_set_defaults(&cinfo);
  /* Make optional parameter settings here */
  printf("Compression Start\n");
  jpeg_start_compress(&cinfo, TRUE);
  JSAMPROW row_pointer[1];	/* pointer to a single row */
  int row_stride;			/* physical row width in buffer */
  row_stride = img_cols;	/* JSAMPLEs per row in image_buffer */
  printf("Scanning... ");
  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = &(img_int[cinfo.next_scanline * row_stride]);
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  printf("Done!\n");
  jpeg_finish_compress(&cinfo);
  printf("Destroy Everything!\n");
  jpeg_destroy_compress(&cinfo);
  gsl_matrix_free(img);
  gsl_matrix_free(gauss);
  return 0;
}
