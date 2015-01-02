/* File nco.h
 *
 * Description: macros and routines for NCO
 * Copyright (C) 2011-2012, Jonghun John Park
 * Last updated on Jan. 25, 2012
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

#ifndef __NCO_H__
#define __NCO_H__

/* Headers */
#include <math.h>

/* Defines */
#define PHASE_ACC_BITS	32
#define	QT_PHASE_ACC_BITS	18

/* Static Values */
//phase accumulator
static unsigned int phaseAcc;

/* Functions */
/* function load_lut()

	Description: load look-up table from file

*/

int
load_lut(double *lut, char *fn_lut, int len_lut){

	double *ptrLut = lut;
	FILE *file;
	int tmp = 0, ret = 0;

	if((file = fopen(fn_lut, "r")) == NULL){
		printf("Unable to open lookup table file(%s)\n", fn_lut);
			ret = -1;
			goto ERR_RETURN;
	}

	while(fscanf(file,"%lf\n", &ptrLut[tmp]) > 0){
		/* prevent infinite loop */
		if(tmp++ > len_lut){
			ret = -2;
			printf("Aborted from lut file  parsing\n");
			break;
		}
	}

	fclose(file);

ERR_RETURN:
	return ret;
}

/* function nco_complex()

	Description: generate both sine and cosine signal

	Output parameters:
		*y_sine				output sine signal
		*y_cosine			output cosine signal
	input parameters:
		len_out				output signal length
		*LUT				look-up table with sinusoid value
		LUT_SIZE			length of lut
		phaseInc			phase increment for nco
		offset				phase offset

	Return indicator:
		0					success
		< 0					return with an error
 */

int nco_complex(
		double *y_sine,
		double *y_cosine,
		int len_out,
		double *LUT,
		int LUT_SIZE,
		unsigned int phaseInc,
		unsigned int offset)
{
		unsigned int qt_phaseAcc, Stage, Addr;
		int i;

	for(i = 0;i < len_out; i++){
		qt_phaseAcc	= (phaseAcc >> (PHASE_ACC_BITS - QT_PHASE_ACC_BITS))
						& (unsigned int)(pow(2,QT_PHASE_ACC_BITS)-1);
		Stage 		= qt_phaseAcc >> (QT_PHASE_ACC_BITS - 2);
		Addr 		= qt_phaseAcc & (unsigned int)(pow(2,QT_PHASE_ACC_BITS-2)-1);

		/* find correspond sine value */
		if(Stage == 0){
			y_sine[i] = LUT[Addr];
		} else if(Stage == 1) {
			if(Addr == 0)
				y_sine[i] = 1;
			else
				y_sine[i] = LUT[LUT_SIZE - Addr];
		} else if(Stage == 2) {
			y_sine[i] = -LUT[Addr];
		} else if(Stage == 3) {
			if(Addr == 0)
				y_sine[i] = -1;
			else
				y_sine[i] = -LUT[LUT_SIZE - Addr];
		}

		/* generate correspond cosine value */
		if(Stage == 3){
			y_cosine[i] = LUT[Addr];
		} else if(Stage == 0) {
			if(Addr == 0)
				y_cosine[i] = 1;
			else
				y_cosine[i] = LUT[LUT_SIZE - Addr];
		} else if(Stage == 1) {
			y_cosine[i] = -LUT[Addr];
		} else if(Stage == 2) {
			if(Addr == 0)
				y_cosine[i] = -1;
			else
				y_cosine[i] = -LUT[LUT_SIZE - Addr];
		}

		/* accumulate phase */
		phaseAcc += (unsigned int)phaseInc;
		phaseAcc &= (unsigned int)(pow(2, PHASE_ACC_BITS) - 1);
		phaseAcc += (unsigned int)offset;
	}
}

#endif
