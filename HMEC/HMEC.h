/******************************************************************************
 * HMEC.h: Hexagonal Motion Estimation and Compensation
 ******************************************************************************
 * v1.0 - 01.04.2016
 *
 * Copyright (c) 2016 Tobias Schlosser
 *  (tobias.schlosser@informatik.tu-chemnitz.de)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************/


#ifndef HMEC_H
#define HMEC_H


#include "../CHIP/Misc/Types.h"

#include "../CHIP/CHIP/Hexarray.h"


#define ARGS \
	int* a_1, int* a_2, unsigned int size


// Hexagonal Motion Estimation HME

// Metrics
unsigned int   SAD(ARGS); // sum of absolute differences
float          MAD(ARGS); // median absolute deviation
float          MSE(ARGS); // mean squared error
float         RMSE(ARGS); // root-mean-square error
float         PSNR(ARGS); // peak signal-to-noise ratio
float         SSIM(ARGS); // structural similarity
float        DSSIM(ARGS); // structural dissimilarity

int HME(unsigned int** vectors,
 RGB_Hexarray ha_1, RGB_Hexarray ha_2, unsigned int size,
 unsigned int mode, unsigned int metric, float range, float factor);

void vectors2file(unsigned int* vectors, unsigned int vectors_size,
 char* filename);


// Hexagonal Motion Compensation HMC

// P-Frame = rgb_hexarray + vectors
void PFrame_gen(P_Frame* pframe,
 RGB_Hexarray rgb_hexarray, unsigned int order,
 unsigned int* vectors, unsigned int vectors_size);

// B-Frame = rgb_hexarray + vectors_backwards + vectors_forwards
void BFrame_gen(B_Frame* bframe,
 RGB_Hexarray rgb_hexarray, unsigned int order,
 unsigned int* vectors_backwards, unsigned int vectors_backwards_size,
 unsigned int* vectors_forwards,  unsigned int vectors_forwards_size);


#endif
