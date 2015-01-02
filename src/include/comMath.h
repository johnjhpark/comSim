/* File: comMath.h
 *
 * Description: Math library used by ComSim
 * Copyright (C) 2011-2013, Jonghun John Park
 * Last updated on June. 28, 2013
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

#ifndef __COMMATH_H__
#define __COMMATH_H__

/* Headers */
#include <math.h>
#include <stdlib.h>
#include "comSim_types.h"

/* Defines */
#define		MAX_MAGIC	-65534
#define		MIN_MAGIC	65533

/* Variables */
int GRAY_MAPPING[16] = {0, 1, 3, 2, 6, 7, 5, 4, 12, 13, 15, 14, 10, 11, 9, 8};

/* Functions */

/***********************************
 * Integer Operations              *
 ***********************************/
int xorInt(int a[], int b[], int len)
{
	int i;
	int count = 0;
	for(i = 0; i < len; i++)
		if(a[i] != b[i])
			count++;

	return count;
}

/***********************************
 * Complex Operations              *
 ***********************************/
complex conjComp(complex in)
{
	complex ccomp;
	ccomp.re = in.re;
	ccomp.im = -in.im;

	return ccomp;
}

complex genComp(double re, double im)
{
	complex newComp;
	newComp.re = re;
	newComp.im = im;

	return newComp;
}

double absComp(complex in)
{
	return sqrt(pow(in.re, 2)+pow(in.im, 2));
}

double absSqrComp(complex in)
{
	return pow(in.re, 2)+pow(in.im, 2);
}

double normComp(complex in)
{
	return absComp(in);
}

complex mulComp(complex a, complex b)
{
	complex newComp;

	newComp.re = a.re*b.re - a.im*b.im;
	newComp.im = a.re*b.im + a.im*b.re;

	return newComp;
}

complex addComp(complex a, complex b)
{
	return genComp(a.re+b.re, a.im+b.im);
}

/***********************************
 * Correlation Funcations          *
 ***********************************/

/* function xcorr_range()

	Description: calculate cross correlation vector btw two double typed inputs
	             within a range of correlation threshold

	Output parameters:
		*output				taps gathered from the file
	input parameters:
		int	len_output		length of output
		*rev				correlated signal
		len_rev				length of rev
		*local				correlating signal
		len_local			length of local
		max_threshold		max threshold to stop(-65534 is magicnumber)
		min_threshold		min threshold to stop

	Return indicator:
		0					(Success) Correlation complete within all range
		> 0					(Success) Correlation has stopped ans array index is returned
		< 0					(Fail) There's an error during process

	Comment:
		1. only len_output values will be calculated
		2. if len_output is bigger than its original length,
		   zero will be padded at the end of *output
 */

int xcorr_range(
		double *output,
		int	len_output,
		double *rev,
		int len_rev,
		double *local,
		int len_local,
		double max_threshold,
		double min_threshold)
{

	int len_out = len_rev + len_local - 1;	/* length of output */
	int oBuff;								/* offset start from zero index of rev vector */
	int len_win; 							/* length of correlation window */
	int i;
	int len_dummy;

	/* output length verification */
	if(len_output < len_out)
		printf("[xcorr] output vector is shorter than which it was expected\n");
	else if((len_dummy = len_output - len_out) > 0)
		printf("[xcorr] output vector has %d dummy values at the end of it\n", len_dummy);

	/* correlation loop */
	for(oBuff = 0; oBuff < len_output ; oBuff++)
	{
		output[oBuff] = 0.;

		/* skip calculation if there's no need to do it */
		if(oBuff > len_out)
			continue;

		if(len_rev >= len_local){
			if(oBuff < len_local){
				len_win = oBuff;
				for(i = 0; i <= oBuff; i++)
					output[oBuff] += rev[len_win - i] * local[len_local-1-i];
			} else if(oBuff >= len_rev){
				len_win = len_out - oBuff;
				for(i = 0; i < len_win; i++)
					output[oBuff] += rev[len_rev-len_win+i] * local[len_local
						- len_win - 1 + i];
			} else {
				len_win = len_local;
				for(i = 0; i < len_win; i++)
					output[oBuff] += rev[oBuff-i] * local[len_win-1-i];
			}
		} else {
			if(oBuff < len_rev){
				len_win = oBuff;
				for(i = 0; i <= len_win; i++)
					output[oBuff] += rev[len_win - i] * local[len_local-1-i];
			} else if(oBuff >= len_local){
				len_win = len_out - oBuff;
				for(i = 0; i < len_win; i++)
					output[oBuff] += rev[len_rev-len_win+i] * local[i];
			} else {
				len_win = len_rev;
				for(i = 0; i < len_win; i++)
					output[oBuff] += rev[len_win-1-i]
						* local[len_local-1-(oBuff+1-len_rev)-i];
			}
		}

		/* Stop calculation if correlation result is out of range */
		if((max_threshold != MAX_MAGIC && output[oBuff] > max_threshold) ||
			(min_threshold != MIN_MAGIC && output[oBuff] < min_threshold))
			return oBuff;
	}

	return 0;
}

/* function xcorr()

	Description: calculate cross correlation vector btw two double typed inputs

	Output parameters:
		*output				taps gathered from the file
	input parameters:
		int	len_output		length of output
		*rev				correlated signal
		len_rev				length of rev
		*local				correlating signal
		len_local			length of local

	Return indicator:
		0					(Success) Correlation complete within all range
		< 0					(Fail) There's an error during process

	Comment:
		1. it will call xcorr_range with MAX_THRESHOLD, MIN_THRESHOLD
 */

int xcorr(double *output,
		int	len_output,
		double *rev,
		int len_rev,
		double *local,
		int len_local)
{
	return xcorr_range(output, len_output, rev, len_rev,
			local, len_local, MAX_MAGIC, MIN_MAGIC);
}

/* function autocorr()

	Description: calculate auto-correlation of double inputs

	Output parameters:
		*output				taps gathered from the file
	input parameters:
		*rev				correlated signal
		len_rev				length of rev
	Return indicator:
		0					Success
 */

int autocorr_range(double *output,
		int	len_output,
		double *rev,
		int len_rev,
		double max_threshold,
		double min_threshold)
{
	return xcorr_range(output, len_output, rev, len_rev,
			rev, len_rev, max_threshold, min_threshold);
}

/* function autocorr()

	Description: calculate auto-correlation of double inputs

	Output parameters:
		*output				taps gathered from the file
	input parameters:
		*rev				correlated signal
		len_rev				length of rev
	Return indicator:
		0					Success
 */

int autocorr(double *output,
		int	len_output,
		double *rev,
		int len_rev)
{
	return xcorr(output, len_output, rev, len_rev, rev, len_rev);
}

/* function xcorr_rangeComp()

	Description: another version of xcorr_range for complex inputs

	Caution: output = correlation with rev and conjugation of local
*/

int xcorr_rangeComp(
		complex *output,
		int	len_output,
		complex *rev,
		int len_rev,
		complex *local,
		int len_local,
		double max_threshold,
		double min_threshold)
{

	int len_out = len_rev + len_local - 1;	/* length of output */
	int oBuff;								/* offset start from zero index of rev vector */
	int len_win; 							/* length of correlation window */
	int i;
	int len_dummy;
	complex *compLocal;

	/* output length verification */
	if(len_output < len_out)
		printf("[xcorr] output vector is shorter than which it was expected\n");
	else if((len_dummy = len_output - len_out) > 0)
		printf("[xcorr] output vector has %d dummy values at the end of it\n", len_dummy);

	compLocal = (complex *)malloc(sizeof(complex)*len_local);

	for(i = 0; i < len_local; i++)
		compLocal[i] = conjComp(local[i]);

	/* correlation loop */
	for(oBuff = 0; oBuff < len_output ; oBuff++)
	{
		output[oBuff] = genComp(0.0, 0.0);

		if(oBuff > len_out)
			continue;

		if(len_rev >= len_local){
			if(oBuff < len_local){
				len_win = oBuff;
				for(i = 0; i <= oBuff; i++)
					output[oBuff] = addComp(output[oBuff],
							mulComp(rev[len_win - i], compLocal[len_local-1-i]));

			} else if(oBuff >= len_rev){
				len_win = len_out - oBuff;
				for(i = 0; i < len_win; i++)
					output[oBuff] = addComp(output[oBuff],
							mulComp(rev[len_rev-len_win+i],
								compLocal[len_local - len_win - 1 + i]));
			} else {
				len_win = len_local;
				for(i = 0; i < len_win; i++)
					output[oBuff] = addComp(output[oBuff],
							mulComp(rev[oBuff-i], compLocal[len_win-1-i]));
			}
		} else {
			if(oBuff < len_rev){
				len_win = oBuff;
				for(i = 0; i <= len_win; i++)
					output[oBuff] = addComp(output[oBuff],
							mulComp(rev[len_win - i], compLocal[len_local-1-i]));
			} else if(oBuff >= len_local){
				len_win = len_out - oBuff;
				for(i = 0; i < len_win; i++)
					output[oBuff] = addComp(output[oBuff],
							mulComp(rev[len_rev-len_win+i], compLocal[i]));
			} else {
				len_win = len_rev;
				for(i = 0; i < len_win; i++)
					output[oBuff] = addComp(output[oBuff],
							mulComp(rev[len_win-1-i],
								compLocal[len_local-1-(oBuff+1-len_rev)-i]));
			}
		}

		/* Stop calculation if correlation result is out of range */
		if((max_threshold != MAX_MAGIC && absComp(output[oBuff]) > max_threshold))
			goto STOP;
	}

	free(compLocal);
	return 0;

STOP:
	free(compLocal);
	return oBuff;
}

#endif /* __COMMATH_H__ */
