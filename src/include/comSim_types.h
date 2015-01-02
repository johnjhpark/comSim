/* File: comSim_types.h
 *
 * Description: Data type definitions for ComSim
 * Copyright (C) 2011-2014, Jonghun John Park
 * Last updated on Dec. 07, 2014
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

#ifndef __COMSIM_TYPES_H__
#define __COMSIM_TYPES_H__

/* Headers */

/* Defines */
#define MAX_SRC_LENGTH	1024

// Complex Variable
typedef struct{
	double re;
	double im;
} complex;

// Signal to Noise Ratio
typedef struct {
	double min;
	double max;
	double step;
	double snrdB;
	double snrLin;
} snr_str;

// Simulation Parameters
typedef struct {
	int	numIter;				// # of simulation iterations
	int modType;				// Modulation Type
	int lenSrc;					// length of source bit length
	int lenFrm;					// length of a frame
	snr_str	snr;				// SNR related parameters
	//
} simParam_str;

// Data Paths
typedef struct{
	int src[MAX_SRC_LENGTH];	// Source Data
	complex mapperOut[MAX_SRC_LENGTH];
	complex chanOut[MAX_SRC_LENGTH];
	int dec[MAX_SRC_LENGTH];	// Decision Output
	int numBitErr;
	int numFrmErr;
} dataPath_str;

/* Functions */

#endif /* __COMSIM_TYPES_H__ */
