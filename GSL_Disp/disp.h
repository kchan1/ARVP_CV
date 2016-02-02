#include <gsl/gsl_matrix.h>
#ifndef _DISP_H_
#define _DISP_H_

void calcDisp(gsl_matrix* imgL,
	      gsl_matrix* imgR, 
	      gsl_matrix* imgTgt,
	      int maxDisp,
	      int windowSize,
	      int patchStartX,
	      int patchStartY,
	      int patchWidth, 
	      int patchLength);

void calcSingleDisp(gsl_matrix* imgL,
		    gsl_matrix* imgR, 
		    gsl_matrix* imgTgt,
		    int maxDisp,
		    int windowSize,
		    int locationX,
		    int locationY);
