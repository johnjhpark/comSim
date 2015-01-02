/* File symMapper.h
 *
 * Description: Linear symbol mapper
 * Copyright (C) 2011-2014, Jonghun John Park
 * Last updated on Dec. 24, 2014
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

#ifndef __SYMMAPPER_H__
#define __SYMMAPPER_H__

/* Headers */
#include <math.h>
#include "comSim_types.h"

/* Defines */

#define	SQRT2	(1.4142135623730950488016887242097)
#define SQRT10	(3.1622776601683793319988935444327)
#define SQRT42	(6.480740698407860230965967436088)
#define COS_PI8  0.923828125
#define SIN_PI8  0.382568359375
#define COS_PI4  0.707092285156250

static complex psk8[8] = {{SIN_PI8,COS_PI8},
	{COS_PI8,SIN_PI8},
	{SIN_PI8,-COS_PI8},
	{COS_PI8,-SIN_PI8},
	{-SIN_PI8,COS_PI8},
	{-COS_PI8,SIN_PI8},
	{-SIN_PI8,-COS_PI8},
	{-COS_PI8,-SIN_PI8}};

complex psk4[4] = {{COS_PI4,COS_PI4},
	{-COS_PI4,COS_PI4},
	{COS_PI4,-COS_PI4},
	{-COS_PI4,-COS_PI4}};

/* Modulation types, or order */
enum{
	BPSK = 2,
	QPSK = 4,
	PSK8 = 8,
	QAM16 = 16,
	QAM64 = 64,
	QAM256 = 256,
};

/* Functions */

/* function mapPsk()

Description: PSK based bit to symbol mapper

Output parameters:
 *lenSym			The final length of mapped symbol vector
 symVec[]			The output symbol vector, whose length is *lenSym

Input parameters:
 lenBit				The number of input bits
 bitStream[]		The input bitstream
 type				Modulation type, or order
 avePow				The average power of output symbol, default is unity.

Return indicator:
 0					Success

 */

int mapPsk(int *lenSym,
		complex symVec[],
		int lenBit,
		int bitStream[],
		int type,
		double avePow)
{
	double boost = 1.0;
	int symLen = *lenSym;
	int symIndex = -1;
	int i;

	if(symLen != lenBit/log2(type)){
		symLen = lenBit/log2(type);
		*lenSym = symLen;
	}

	switch(type){
		case BPSK:
			boost = avePow * boost;
			for (i = 0; i < symLen ; i++){
				symVec[i].re = boost * (1 - 2 * (bitStream[i]));
				symVec[i].im = 0;
			}
			break;
		case QPSK:
			boost = avePow * boost / SQRT2;
			symIndex = bitStream[2*i]*2 + bitStream[2*i+1];
			for (i = 0; i < symLen ; i++){
				symVec[i].re = boost * psk4[symIndex].re;
				symVec[i].im = boost * psk8[symIndex].im;
			}
			break;
		case PSK8:
			for (i=0; i<symLen; i++){
				/* [1 0 4 5 7 6 2 3], pi/8 counter-clockwise rotated

				             4   |   0
				           5     |     1
				        ------------------
				           7     |     3
				             6   |   2
				*/
				symIndex = bitStream[3*i]*4 + bitStream[3*i+1]*2 + bitStream[3*i+2];
				symVec[i].re = psk8[symIndex].re;
				symVec[i].im = psk8[symIndex].im;
			}
			break;
		default:
			/* Error */
			break;
	}
	return 0;

}

/* function mapQam()

Description: QAM based bit to symbol mapper

Output parameters:
 *lenSym			The final length of mapped symbol vector
 symVec[]			The output symbol vector, whose length is *lenSym

 Input parameters:
 lenBit				The number of input bits
 bitStream[]		The input bitstream
 type				Modulation type, or order
 avePow				The average power of output symbol, default is unity.

 Return indicator:
 0					Success

*/

int mapQam(int *lenSym,
		complex symVec[],
		int lenBit,
		int bitStream[],
		int type,
		double avePow)
{
	double boost = 1.0;
	int symLen = *lenSym;
	int symIndex = -1;
	int i;

	if(symLen != lenBit/log2(type)){
		symLen = lenBit/log2(type);
		*lenSym = symLen;
	}

	switch(type){
		case QAM16:
			for (i = 0; i < symLen; i++)
			{
				symVec[i].re = ( (2*bitStream[4*i]-1) * (2*bitStream[4*i+1]+1) )/sqrt(10);
				symVec[i].im = ( (2*bitStream[4*i+2]-1) * (2*bitStream[4*i+3]+1) )/sqrt(10);
			}
			break;
		case QAM64:
			for (i = 0; i < symLen; i++)
			{
				symVec[i].re = ( (2*bitStream[6*i  ]-1) * ( 4 + (2*bitStream[6*i+1]-1)
							* (2*bitStream[6*i+2]+1) ) )/sqrt(42);
				symVec[i].im = ( (2*bitStream[6*i+3]-1) * ( 4 + (2*bitStream[6*i+4]-1)
							* (2*bitStream[6*i+5]+1) ) )/sqrt(42);
			}
			break;
		case QAM256:
			for (i = 0; i < symLen; i++)
			{
				symVec[i].re = ((2*bitStream[8*i  ]-1) * (8 + 4*(2*bitStream[8*i+1]-1)
							+ (2*bitStream[8*i+2]-1)*(2*bitStream[8*i+3]+1)))/sqrt(170);
				symVec[i].im = ((2*bitStream[8*i+4]-1)*(8 + 4*(2*bitStream[8*i+5]-1)
							+ (2*bitStream[8*i+6]-1)*(2*bitStream[8*i+7]+1)))/sqrt(170);
			}
			break;
		default:
			/* Error */
			break;
	}
	return 0;

}

/* function PskHd()

Description: PSK based symbol to bit demapper

Output parameters:
 *lenBit				The number of output bits
 bitStream[]			The output bitstream

 Input parameters:
 lenSym				The length of received symbol vector
 symVec[]			The input symbol vector, whose length is *lenSym
 type				Modulation type, or order

 Return indicator:
 0					Success

*/

int PskHd(
		int *lenBit,
		int bitStream[],
		int lenSym,
		complex symVec[],
		int type)
{
	int idx;
	int lengthSym = lenSym;

	switch(type){
		case BPSK:
			for(idx = 0; idx < lengthSym; idx++)
				bitStream[idx] = (symVec[idx].re < 0)? 1:0;
			break;
		case QPSK:
			for(idx = 0; idx < lengthSym; idx++){
				bitStream[2*idx  ] = (symVec[idx].re < 0)? 1:0;
				bitStream[2*idx+1] = (symVec[idx].im < 0)? 1:0;
			}
			break;
		case PSK8:
			for(idx = 0; idx < lengthSym; idx++){
				bitStream[3*idx] = symVec[idx].re > 0 ? 0 : 1;
				bitStream[3*idx+1] = symVec[idx].im  > 0 ? 0 : 1;
				bitStream[3*idx+2] = (symVec[idx].re+symVec[idx].im)*
					(-symVec[idx].re+symVec[idx].im) > 0 ? 0 : 1;
			}
			break;
		case QAM16:
			for(idx = 0; idx < lengthSym; idx++){
				bitStream[4*idx  ] = (symVec[idx].re > 0)? 1:0;
				bitStream[4*idx+1] = (fabs(symVec[idx].re) > 2/sqrt(10))? 1:0;
				bitStream[4*idx+2] = (symVec[idx].im > 0)? 1:0;
				bitStream[4*idx+3] = (fabs(symVec[idx].im) > 2/sqrt(10))? 1:0;
			}
			break;
		case QAM64:
			for (idx=0; idx < lengthSym; idx++)
			{
				bitStream[6*idx  ] = (       symVec[idx].re  > 0)? 1:0;
				bitStream[6*idx+1] = (  fabs(symVec[idx].re) > 4/sqrt(42))? 1:0;
				bitStream[6*idx+2] = (((fabs(symVec[idx].re) > 2/sqrt(42)) &&
							((fabs(symVec[idx].re) < 6/sqrt(42)))))? 0:1;
				bitStream[6*idx+3] = (       symVec[idx].im  > 0)? 1:0;
				bitStream[6*idx+4] = (  fabs(symVec[idx].im) > 4/sqrt(42))? 1:0;
				bitStream[6*idx+5] = (((fabs(symVec[idx].im) > 2/sqrt(42)) &&
							((fabs(symVec[idx].im) < 6/sqrt(42)))))? 0:1;
			}
			break;
		case QAM256:
			for (idx=0; idx < lengthSym; idx++)
			{
				bitStream[8*idx  ] = (        symVec[idx].re  > 0)? 1:0;
				bitStream[8*idx+1] = (   fabs(symVec[idx].re) > 8/sqrt(170))? 1:0;
				bitStream[8*idx+2] = ((((fabs(symVec[idx].re) > 4/sqrt(170)) &&
								(fabs(symVec[idx].re) < 8/sqrt(170))) ||
							((fabs(symVec[idx].re) >12/sqrt(170)))))? 1:0;
				bitStream[8*idx+3] = ((((fabs(symVec[idx].re) > 2/sqrt(170)) &&
								(fabs(symVec[idx].re) < 6/sqrt(170))) ||
							((fabs(symVec[idx].re) >10/sqrt(170)) &&
							 (fabs(symVec[idx].re) <14/sqrt(170)))))? 0:1;
				bitStream[8*idx+4] = (        symVec[idx].im  > 0)? 1:0;
				bitStream[8*idx+5] = (   fabs(symVec[idx].im) > 8/sqrt(170))? 1:0;
				bitStream[8*idx+6] = ((((fabs(symVec[idx].im) > 4/sqrt(170)) &&
								(fabs(symVec[idx].im) < 8/sqrt(170))) ||
							((fabs(symVec[idx].im) >12/sqrt(170)))))? 1:0;
				bitStream[8*idx+7] = ((((fabs(symVec[idx].im) > 2/sqrt(170)) &&
								(fabs(symVec[idx].im) < 6/sqrt(170))) ||
							((fabs(symVec[idx].im) >10/sqrt(170)) &&
							 (fabs(symVec[idx].im) <14/sqrt(170)))))? 0:1;
			}
			break;
		default:
			/* Error */
			break;
	}
	return 0;
}

/* function QamHd()

Description: QAM based symbol to bit demapper

Output parameters:
 *lenBit				The number of output bits
 bitStream[]			The output bitstream

 Input parameters:
 lenSym				The length of received symbol vector
 symVec[]			The input symbol vector, whose length is *lenSym
 type				Modulation type, or order

 Return indicator:
 0					Success

*/

int QamHd(
		int *lenBit,
		int bitStream[],
		int lenSym,
		complex symVec[],
		int type)
{
	int idx;
	int lengthSym = lenSym;

	switch(type){
		case QAM16:
			for(idx = 0; idx < lengthSym; idx++){
				bitStream[4*idx  ] = (symVec[idx].re > 0)? 1:0;
				bitStream[4*idx+1] = (fabs(symVec[idx].re) > 2/sqrt(10))? 1:0;
				bitStream[4*idx+2] = (symVec[idx].im > 0)? 1:0;
				bitStream[4*idx+3] = (fabs(symVec[idx].im) > 2/sqrt(10))? 1:0;
			}
			break;
		case QAM64:
			for (idx=0; idx < lengthSym; idx++)
			{
				bitStream[6*idx  ] = (       symVec[idx].re  > 0)? 1:0;
				bitStream[6*idx+1] = (  fabs(symVec[idx].re) > 4/sqrt(42))? 1:0;
				bitStream[6*idx+2] = (((fabs(symVec[idx].re) > 2/sqrt(42)) &&
							((fabs(symVec[idx].re) < 6/sqrt(42)))))? 0:1;
				bitStream[6*idx+3] = (       symVec[idx].im  > 0)? 1:0;
				bitStream[6*idx+4] = (  fabs(symVec[idx].im) > 4/sqrt(42))? 1:0;
				bitStream[6*idx+5] = (((fabs(symVec[idx].im) > 2/sqrt(42)) &&
							((fabs(symVec[idx].im) < 6/sqrt(42)))))? 0:1;
			}
			break;
		case QAM256:
			for (idx=0; idx < lengthSym; idx++)
			{
				bitStream[8*idx  ] = (        symVec[idx].re  > 0)? 1:0;
				bitStream[8*idx+1] = (   fabs(symVec[idx].re) > 8/sqrt(170))? 1:0;
				bitStream[8*idx+2] = ((((fabs(symVec[idx].re) > 4/sqrt(170)) &&
								(fabs(symVec[idx].re) < 8/sqrt(170))) ||
							((fabs(symVec[idx].re) >12/sqrt(170)))))? 1:0;
				bitStream[8*idx+3] = ((((fabs(symVec[idx].re) > 2/sqrt(170)) &&
								(fabs(symVec[idx].re) < 6/sqrt(170))) ||
							((fabs(symVec[idx].re) >10/sqrt(170)) &&
							 (fabs(symVec[idx].re) <14/sqrt(170)))))? 0:1;
				bitStream[8*idx+4] = (        symVec[idx].im  > 0)? 1:0;
				bitStream[8*idx+5] = (   fabs(symVec[idx].im) > 8/sqrt(170))? 1:0;
				bitStream[8*idx+6] = ((((fabs(symVec[idx].im) > 4/sqrt(170)) &&
								(fabs(symVec[idx].im) < 8/sqrt(170))) ||
							((fabs(symVec[idx].im) >12/sqrt(170)))))? 1:0;
				bitStream[8*idx+7] = ((((fabs(symVec[idx].im) > 2/sqrt(170)) &&
								(fabs(symVec[idx].im) < 6/sqrt(170))) ||
							((fabs(symVec[idx].im) >10/sqrt(170)) &&
							 (fabs(symVec[idx].im) <14/sqrt(170)))))? 0:1;
			}
			break;
		default:
			/* Error */
			break;
	}
	return 0;
}

#endif /* __SYMMAPPER_H__ */
