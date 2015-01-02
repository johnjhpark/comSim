/* File: dataGen.h
 *
 * Description: Random source data generation
 * Copyright (C) 2011-2014, Jonghun John Park
 * Last updated on Dec. 16, 2014
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

#ifndef __DATAGEN_H__
#define __DATAGEN_H__

#include <math.h>
#include <time.h>

int genBitSource(int *Out, int lenSrc){

	int index;
	srand((unsigned)time(NULL));	//randomize
	for (index = 0; index < lenSrc; index++)
		Out[index] = rand()%2;

	return 0;
}

/* Tranfrom binary array into decimal array with M bits groupping
   Func. Name : biA2decA
   Parameters : decOut -> a pointer to int array for output
                bitIn -> a pointer to int array directing to input bit stream
                lenIn -> length of bitIn array
                M -> number of bits per decimal

   Return: 0 -> Success
           1 -> Dummy with last decimal

   Caution : I suggest that lenIn should be a multiple of M, but there's
             a dummy at the end of decOut array. Caller can ignore it.
*/

int biA2decA(int *decOut, int *bitIn, int lenIn, int M)
{
	int *input = bitIn;
	int *output = decOut;
	int i, cnt = 0, cursor;

	for(i = 0; i<lenIn; i=i+M){
		cursor = M;
		while(--cursor >= 0)
			output[cnt] += (1<<cursor)*input[i+M-1-cursor];
		cnt++;
	}

	/* Return with Last Dec Error Indication*/
	if(lenIn%M != 0){
		output[cnt-1] = 0;
		return 1;
	}

	/* Return without error */
	return 0;
}

#endif
