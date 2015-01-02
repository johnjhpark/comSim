/* File: fir.h
 *
 * Description: Functions for finite impulse response (FIR) filter
 * Copyright (C) 2011-2014, Jonghun John Park
 * Last updated on Jan. 18, 2014
 *
 * This file is part of ComSim.
 * ComSim is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with ComSim. If not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#ifndef __FIR_H__
#define __FIR_H__

/* Headers */

/* Defines */

//using shared memory
#define USE_SHMEM
#ifdef USE_SHMEM
#define LEN_SH_MEM		1024
static double sh_mem[LEN_SH_MEM];
#endif

/* Functions */

/* function intpl_fir()

	Description: get filter coefficient from the given file

	Output parameters:
		*tap				taps gathered from the file
	input parameters:
		*fName				the name of fileter tap file
		len_taps			length of taps
	Return indicator:
		0					Success
 */

int intpl_fir(double *y, int len_y,			/* output */
	const char *fltTapFile, int len_flt,	/* filter */
	double *x, int len_x,					/* input */
	int upRate)								/* interpolation rate */
{

	double *outUpSamp;
	int ret = 0;

	if((outUpSamp = (double *) malloc(sizeof(double)*len_x*upRate)) == NULL){
		ret = -1;
		printf("Fail to mem alloc");
		goto ERR_MEM;
	}

	upSamp(outUpSamp, len_x*upRate, x, upRate);

	if(fir(y, len_y, fltTapFile, len_flt, outUpSamp, len_x*upRate) < 0){
		printf("Fail to mem alloc");
		goto ERR_FIR;
	}

ERR_FIR:
	free(outUpSamp);

ERR_MEM:
	return ret;

}

/* function getTapsFrmFile()

	Description: get filter coefficient from the given file

	Output parameters:
		*tap				taps gathered from the file
	input parameters:
		*fName				the name of fileter tap file
		len_taps			length of taps
	Return indicator:
		0					Success
 */

int getTapsFrmFile(double *taps, const char *fName, int len_taps)
{
	double *ptrTap = taps;
	FILE *file;
	int tmp = 0, ret = 0;

	if((file = fopen(fName, "r")) == NULL){
		printf("Unable to open coefficient file(%s)\n",
				fName);
			ret = -1;
			goto ERR_RETURN;
	}

	while(fscanf(file, "%lf\n", &ptrTap[tmp]) > 0){
		/* prevent infinite loop*/
		if(tmp++ > len_taps){
			ret = -2;
			printf("Aborted from fileter tap parsing\n");
			break;
		}
	}

	fclose(file);

ERR_RETURN:
	return ret;
}

/* function fir()

	Description: do fir filtering with given FIR filter from a file

	Output parameters:
		*y					filtering output
		*len_y				output length
	input parameters:
		*fltTapFile			file containing FIR filter coefficients
		*x 					input vector
		len_x				length of input vector length
	Return indicator:
		0					Success
*/

int fir(double *y, int len_y,				/* output */
	const char *fltTapFile, int len_flt,	/* filter */
	double *x, int len_x)					/* input */
{
	double *fltTaps;
	int ret = 0;

	/* alloc mem for filter taps */
	if((fltTaps = (double *) malloc(sizeof(double)*len_flt)) == NULL){
		ret = -1;
		printf("Fail to mem alloc\n");
		goto ERR_MEM;
	}

	/* get filter tap */
	if(getTapsFrmFile(fltTaps, fltTapFile, len_flt) < 0){
		printf("Cannot get filter coefficients from file\n");
		ret = -2;
		goto ERR;
	}

	/* do convolution */
	if(conv(y, len_y, fltTaps, len_flt, x, len_x) < 0){
		printf("Error occur during convolution\n");
		ret = -3;
	}

ERR:
	free(fltTaps);
ERR_MEM:
	return ret;
}

/* function upSamp() */
int upSamp(double *y, int len_out, double*x, int rate)
{
	int len_in = len_out/rate;
	int i;

	for(i = 0; i <len_out; i++){
		if(i%rate == 0)
			y[i] = x[i/rate];
		else
			y[i] = 0;
	}

	return 0;
}

int downSamp(double *y, double*x, int len_in, int rate, int offset)
{
	int len_out = (len_in - offset) / rate;
	int i;

	for(i = 0; i < len_in; i++){
		if((i - offset) % rate == 0)
			y[(i - offset) / rate] = x[i];
	}

	return 0;
}

/* function conv()

	Description: convolution btw intput x and filter h

	Output parameters:
		*y					convolution result

	Return indicator:
		0					Success
		-1					Short output buffer error
		-2					Memory allocation error
	Caution:
		output vector should be initialized before entering this func.
		if not, some errors could be occurred
*/

int conv(double *y, int len_y,	//output
		double *h, int len_h,	//filter
		double *x, int len_x)	//input
{
	int idx, idx2, ret = 0;
	int len_buf = (len_x + (len_h-1)*2);
	double *inv_h, *buf;

	/* output length check(no truncation allowed) */
	if(len_y < len_x + len_h - 1){
		printf("[fir]Invalid output length\n");
		goto err;
	}

	/* allocate temporal buffers */
	if(((inv_h = (double *)malloc(sizeof(double)*len_h)) == NULL) ||
	   ((buf = (double *)malloc(sizeof(double)*len_buf)) == NULL))
		goto err_alloc;
	else {
		/* inv_h(tau) = h(-t-tau) */
		for(idx = 0; idx < len_h; idx++)
			inv_h[idx] = h[len_h-1-idx];
		/* init data buff */
		for(idx = 0; idx < len_buf; idx++)
			buf[idx] = 0.0;
		/* add input values to buffer */
		for(idx = len_h - 1; idx < len_x-1+len_h; idx++)
			buf[idx] = x[idx - len_h +1];
	}

	/* do convolution */
	for(idx = 0; idx < len_x + len_h - 1; idx++)
		for(idx2 = 0; idx2 < len_h ; idx2++)
			y[idx] += buf[idx + idx2] * inv_h[idx2];

	free(buf);
	free(inv_h);
	return 0;

err_alloc:
	free(buf);
	free(inv_h);
	ret -= 1;
err:
	return ret - 1;
}

#endif
