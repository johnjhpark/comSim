/* File fixedpoint.h
 *
 * Description: Functions and macros for fixedpoint operation
 * Copyright (C) 2011-2012, Jonghun John Park
 * Last updated on Jan. 06, 2012
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

#ifndef __FIXEDPOINT_H__
#define __FIXEDPOINT_H__

/* Headers */

/* Defines */
typedef long long sfxp64_t;
typedef int sfxp_t;
typedef unsigned int ufxp_t;
typedef struct _strSfxp_t{
	sfxp_t	sign;
	sfxp_t	integer;
	sfxp_t	frac;
}strSfxp_t;

/* Maximum and minimum signed stored integer value */
#define ONE(FL)			(sfxp_t)(1<<FL)
#define MAX_SFXP(WL)	(sfxp_t)((1<<(WL-1))-1)
#define MIN_SFXP(WL)   	(sfxp_t)(-(1<<(WL-1)))

/* Bit masks */
#define MASK_SIGN(WL)	(ufxp_t)(1<<(WL-1))
#define MASK_FRAC(FL)	(ufxp_t)((1<<FL)-1)

#define GET_SIGN(I, WL)	(((I & MASK_SIGN(WL))>>(WL-1))? -1: 1)

/* Unsafe conversion macros */
#define	REAL2FXPFLOOR(R, bp) (sfxp_t)(R * (1<<bp))
#define	REAL2FXPROUND(R, bp) (sfxp_t)(R * (1<<bp) + (R>=0 ? 0.5 : -0.5))
#define	FXP2FLOAT(I, bp) (float)I/ (1<<bp)
#define	FXP2DOUBLE(I, bp) (double)I / (1<<bp)

/* Safe conversion macros */
#define SREAL2FXPFLOOR(I, R, WL, FL)\
	do{\
		I = REAL2FXPFLOOR(R, FL);\
		RANGECHECK(I, WL);\
	}while(0)

#define SREAL2FXPROUND(I, R, WL, FL) \
	do{\
		I = REAL2FXPROUND(R, FL);\
		RANGECHECK(I, WL);\
	}while(0)

/* Check the range of fixed point values */
#define RANGECHECK(I, WL)\
	do{\
		if(I > MAX_SFXP(WL)){\
			Dprintf(FXP,"Overfloor occurred when r2f conversion");\
			I = MAX_SFXP(WL);break;}\
		else if(I < MIN_SFXP(WL)){\
			Dprintf(FXP,"Underfloor occurred when r2f conversion");\
			I = MIN_SFXP(WL);break;}\
	}while(0)

/* Functions */

/* Arithmatic Operators for fixed point value */
static inline sfxp_t fxpAdd(sfxp_t A, sfxp_t B, int WL)
{
	sfxp_t C = A + B;
	RANGECHECK(C, WL);
	return C;
}

/* XXX: Recommend to use fxpSub(A,-B,WL) */
static inline sfxp_t fxpSub(sfxp_t A, sfxp_t B, int WL)
{
	sfxp_t C = A - B;
	RANGECHECK(C, WL);
	return C;
}

static inline sfxp_t fxpMul(sfxp_t A, sfxp_t B, int WL, int FL)
{
	sfxp_t ret;
	sfxp64_t tmp;

	tmp = ((sfxp64_t)A * (sfxp64_t)B) >> FL;
	ret = (sfxp_t)tmp;

	RANGECHECK(ret, WL);
	return ret;
}

static inline sfxp_t fxpDiv(sfxp_t A, sfxp_t B, int WL, int FL)
{
	sfxp_t ret;
	sfxp64_t tmp;

	tmp = ((sfxp64_t)A << FL) / (sfxp64_t)B ;
	ret = (sfxp_t)tmp;

	RANGECHECK(ret, WL);
	return ret;
}

/* Reciprocal of fixed point value */
static inline sfxp_t fxpRec(sfxp_t A, int WL, int FL){
	return fxpDiv(ONE(FL), A, WL, FL);
}

/* DEPRECATED: converts sfxp_t to strSfxp_T */
void fxp2strFxp(strSfxp_t *out, sfxp_t in, int WL, int FL){
	strSfxp_t *struc = out;
	struc->sign = GET_SIGN(in, WL);
	struc->integer = ((in >> FL) & (WL-FL+1)-1);
	struc->frac = (in & ((1 << FL) -1));
}

#endif /*__FIXEDPOINT_H__*/
