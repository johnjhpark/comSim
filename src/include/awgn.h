/* File: awgn.h
 *
 * Description: random number & noise generation
 * Copyright (C) 2011-2014, John Jonghun Park
 * Last updated on Dec. 21, 2014
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

#include <math.h>

#ifndef __AWGN_H__
#define __AWGN_H__

/* Random Number Generation [0,1] */
double rnd()
{
	double rv;
	rv = (double)rand() / (double)RAND_MAX;
	return rv;
}

/* Gaussian Random Number Generator */
double GaussRand()
{
	static double t = 0.;
	double x, v1, v2, r;

	if (t == 0){
		do{
			v1 = 2. * rnd() - 1.;
			v2 = 2. * rnd() - 1.;
			r = v1 * v1 + v2 * v2;
		}while (r >= 1.);

		r = sqrt(-2. * log(r) / r);
		t = v2 * r;

		return v1 * r;
	} else {
		x = t;
		t = 0.;
		return x;
	}
}

/* real AWGN generation  */
int awgn_real(double noise[], int len, double var)
{
	double *vec_noise = noise;
	int i;

	for(i = 0; i < len; i++)
		vec_noise[i] = sqrt(var) * GaussRand();

	return 0;
}

/* complex AWGN generation  */
int awgn_complex(complex noise[], int len, double var)
{
	complex *vec_noise = noise;
	int i;

	for(i = 0; i < len; i++){
		vec_noise[i].re = sqrt(0.5) * sqrt(var) * GaussRand();
		vec_noise[i].im = sqrt(0.5) * sqrt(var) * GaussRand();
	}

	return 0;
}

/* Add real-valued AWGN channel to input signal */
int ch_awgn_real(double input[], int len, double var)
{
	double *in = input;
	int i;

	for(i = 0; i < len; i++)
		in[i] = in[i] + sqrt(var) * GaussRand();

	return 0;
}

/* Add complex-valued AWGN channel to input signal */
int ch_awgn_complex(complex input[], int len, double var)
{
	complex *inc = input;
	int i = 0;

	for(i = 0; i < len; i++){
		inc[i].re = inc[i].re + sqrt(0.5) * sqrt(var) * GaussRand();
		inc[i].im = inc[i].im + sqrt(0.5) * sqrt(var) * GaussRand();
	}

	return 0;
}

#endif
