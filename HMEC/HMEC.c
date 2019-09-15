/******************************************************************************
 * HMEC.c: Hexagonal Motion Estimation and Compensation
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include "../CHIP/Misc/Defines.h"
#include "../CHIP/Misc/Precalcs.h"
#include "../CHIP/Misc/Types.h"

#include "../CHIP/CHIP/Hexint.h"
#include "../CHIP/CHIP/Hexarray.h"

#include "HMEC.h"




// Hexagonal Motion Estimation HME


// Metrics

// Sum of absolute differences
unsigned int SAD(int* a_1, int* a_2, unsigned int size) {
	unsigned int SAD = 0;

	for(unsigned int i = 0; i < size; i++)
		SAD += (unsigned int)abs(a_1[i] - a_2[i]);

	return SAD;
}

// Median absolute deviation
float MAD(int* a_1, int* a_2, unsigned int size) {
	return SAD(a_1, a_2, size) / (float)size;
}

// Mean squared error
float MSE(int* a_1, int* a_2, unsigned int size) {
	unsigned int MSE = 0;

	for(unsigned int i = 0; i < size; i++) {
		const int diff = a_1[i] - a_2[i];

		MSE += (unsigned int)(diff * diff);
	}

	return MSE / (float)size;
}

// Root-mean-square error
float RMSE(int* a_1, int* a_2, unsigned int size) {
	return sqrtf(MSE(a_1, a_2, size));
}

// Peak signal-to-noise ratio
float PSNR(int* a_1, int* a_2, unsigned int size) {
	return 10 * log10f(65025 / MSE(a_1, a_2, size)); // 255^2 = 65025
}

// Structural similarity
float SSIM(int* a_1, int* a_2, unsigned int size) {
	int   sum_1      = 0;
	int   sum_2      = 0;
	float variance_1 = 0.0f;
	float variance_2 = 0.0f;
	float covariance = 0.0f;


	for(unsigned int i = 0; i < size; i++) {
		sum_1 += a_1[i];
		sum_2 += a_2[i];
	}

	const float avg_1 = sum_1 / (float)size;
	const float avg_2 = sum_2 / (float)size;


	for(unsigned int i = 0; i < size; i++) {
		const float diff_1 = a_1[i] - avg_1;
		const float diff_2 = a_2[i] - avg_2;

		variance_1 += diff_1 * diff_1;
		variance_2 += diff_2 * diff_2;
		covariance += diff_1 * diff_2;
	}

	variance_1 /= size;
	variance_2 /= size;
	covariance /= size;


	// 6.5025 und 58.5225 sind Stabilisatoren
	return ((2 * avg_1 * avg_2 + 6.5025f) * (2 * covariance + 58.5225f)) / \
	        ((avg_1 * avg_1 + avg_2 * avg_2 + 6.5025f) * \
	         (variance_1 * variance_1 + variance_2 * variance_2 + 58.5225f));
}

// Structural dissimilarity
float DSSIM(int* a_1, int* a_2, unsigned int size) {
	return (1 - SSIM(a_1, a_2, size)) / 2;
}


int HME(unsigned int** vectors,
 RGB_Hexarray ha_1, RGB_Hexarray ha_2, unsigned int size,
 unsigned int mode, unsigned int metric, float range, float factor) {
	const unsigned int step         = !mode ? size : 1;
	      unsigned int vectors_size = 0;

	*vectors = NULL;


	for(unsigned int p = 0; p < ha_1.size; p += step) {
		const fPoint2d pr = getReal(Hexint_init(p, 0));
		      int      sha_1[3][size];


		for(unsigned int i = 0; i < size; i++) {
			if(!mode) {
				const unsigned int index = i + p;

				sha_1[0][i] = ha_1.p[index][0];
				sha_1[1][i] = ha_1.p[index][1];
				sha_1[2][i] = ha_1.p[index][2];
			} else {
				fPoint2d ps = getSpatial(Hexint_init(p, 0));

				switch(i) {
					case  1 : ps.x += 1;            break;
					case  2 : ps.x += 1; ps.y += 1; break;
					case  3 :            ps.y += 1; break;
					case  4 : ps.x -= 1;            break;
					case  5 : ps.x -= 1; ps.y -= 1; break;
					case  6 :            ps.y -= 1; break;

					case  7 : ps.x += 3; ps.y += 2; break;
					case  8 : ps.x += 4; ps.y += 2; break;
					case  9 : ps.x += 4; ps.y += 3; break;
					case 10 : ps.x += 3; ps.y += 3; break;
					case 11 : ps.x += 2; ps.y += 2; break;
					case 12 : ps.x += 2; ps.y += 1; break;
					case 13 : ps.x += 3; ps.y += 1; break;
					case 14 : ps.x += 1; ps.y += 3; break;
					case 15 : ps.x += 2; ps.y += 3; break;
					case 16 : ps.x += 2; ps.y += 4; break;
					case 17 : ps.x += 1; ps.y += 4; break;
					case 18 :            ps.y += 3; break;
					case 19 :            ps.y += 2; break;
					case 20 : ps.x += 1; ps.y += 2; break;
					case 21 : ps.x -= 2; ps.y += 1; break;
					case 22 : ps.x -= 1; ps.y += 1; break;
					case 23 : ps.x -= 1; ps.y += 2; break;
					case 24 : ps.x -= 2; ps.y += 2; break;
					case 25 : ps.x -= 3; ps.y += 1; break;
					case 26 : ps.x -= 3;            break;
					case 27 : ps.x -= 2;            break;
					case 28 : ps.x -= 3; ps.y -= 2; break;
					case 29 : ps.x -= 2; ps.y -= 2; break;
					case 30 : ps.x -= 2; ps.y -= 1; break;
					case 31 : ps.x -= 3; ps.y -= 1; break;
					case 32 : ps.x -= 4; ps.y -= 2; break;
					case 33 : ps.x -= 4; ps.y -= 3; break;
					case 34 : ps.x -= 3; ps.y -= 3; break;
					case 35 : ps.x -= 1; ps.y -= 3; break;
					case 36 :            ps.y -= 3; break;
					case 37 :            ps.y -= 2; break;
					case 38 : ps.x -= 1; ps.y -= 2; break;
					case 39 : ps.x -= 2; ps.y -= 3; break;
					case 40 : ps.x -= 2; ps.y -= 4; break;
					case 41 : ps.x -= 1; ps.y -= 4; break;
					case 42 : ps.x += 2; ps.y -= 1; break;
					case 43 : ps.x += 3; ps.y -= 1; break;
					case 44 : ps.x += 3;            break;
					case 45 : ps.x += 2;            break;
					case 46 : ps.x += 1; ps.y -= 1; break;
					case 47 : ps.x += 1; ps.y -= 2; break;
					case 48 : ps.x += 2; ps.y -= 2; break;

					default :                       break;
				}


				const unsigned int index = pc_spatials[(int)(ps.x - pc_smn.x)][(int)(ps.y - pc_smn.y)];

				if(index < ha_1.size) {
					sha_1[0][i] = ha_1.p[index][0];
					sha_1[1][i] = ha_1.p[index][1];
					sha_1[2][i] = ha_1.p[index][2];
				} else {
					break;
				}
			}
		}


		for(unsigned int q = 0; q < ha_2.size; q += step) {
			const fPoint2d qr = getReal(Hexint_init(q, 0));

			if(p == q || sqrtf((qr.x - pr.x) * (qr.x - pr.x) + (qr.y - pr.y) * (qr.y - pr.y)) > range)
				continue;

			float this_factor;
			int   sha_2[3][size];


			for(unsigned int i = 0; i < size; i++) {
				if(!mode) {
					const unsigned int index = i + q;

					sha_2[0][i] = ha_2.p[index][0];
					sha_2[1][i] = ha_2.p[index][1];
					sha_2[2][i] = ha_2.p[index][2];
				} else {
					fPoint2d qs = getSpatial(Hexint_init(q, 0));

					switch(i) {
						case  1 : qs.x += 1;            break;
						case  2 : qs.x += 1; qs.y += 1; break;
						case  3 :            qs.y += 1; break;
						case  4 : qs.x -= 1;            break;
						case  5 : qs.x -= 1; qs.y -= 1; break;
						case  6 :            qs.y -= 1; break;

						case  7 : qs.x += 3; qs.y += 2; break;
						case  8 : qs.x += 4; qs.y += 2; break;
						case  9 : qs.x += 4; qs.y += 3; break;
						case 10 : qs.x += 3; qs.y += 3; break;
						case 11 : qs.x += 2; qs.y += 2; break;
						case 12 : qs.x += 2; qs.y += 1; break;
						case 13 : qs.x += 3; qs.y += 1; break;
						case 14 : qs.x += 1; qs.y += 3; break;
						case 15 : qs.x += 2; qs.y += 3; break;
						case 16 : qs.x += 2; qs.y += 4; break;
						case 17 : qs.x += 1; qs.y += 4; break;
						case 18 :            qs.y += 3; break;
						case 19 :            qs.y += 2; break;
						case 20 : qs.x += 1; qs.y += 2; break;
						case 21 : qs.x -= 2; qs.y += 1; break;
						case 22 : qs.x -= 1; qs.y += 1; break;
						case 23 : qs.x -= 1; qs.y += 2; break;
						case 24 : qs.x -= 2; qs.y += 2; break;
						case 25 : qs.x -= 3; qs.y += 1; break;
						case 26 : qs.x -= 3;            break;
						case 27 : qs.x -= 2;            break;
						case 28 : qs.x -= 3; qs.y -= 2; break;
						case 29 : qs.x -= 2; qs.y -= 2; break;
						case 30 : qs.x -= 2; qs.y -= 1; break;
						case 31 : qs.x -= 3; qs.y -= 1; break;
						case 32 : qs.x -= 4; qs.y -= 2; break;
						case 33 : qs.x -= 4; qs.y -= 3; break;
						case 34 : qs.x -= 3; qs.y -= 3; break;
						case 35 : qs.x -= 1; qs.y -= 3; break;
						case 36 :            qs.y -= 3; break;
						case 37 :            qs.y -= 2; break;
						case 38 : qs.x -= 1; qs.y -= 2; break;
						case 39 : qs.x -= 2; qs.y -= 3; break;
						case 40 : qs.x -= 2; qs.y -= 4; break;
						case 41 : qs.x -= 1; qs.y -= 4; break;
						case 42 : qs.x += 2; qs.y -= 1; break;
						case 43 : qs.x += 3; qs.y -= 1; break;
						case 44 : qs.x += 3;            break;
						case 45 : qs.x += 2;            break;
						case 46 : qs.x += 1; qs.y -= 1; break;
						case 47 : qs.x += 1; qs.y -= 2; break;
						case 48 : qs.x += 2; qs.y -= 2; break;

						default :                       break;
					}


					const unsigned int index = pc_spatials[(int)(qs.x - pc_smn.x)][(int)(qs.y - pc_smn.y)];

					if(index < ha_2.size) {
						sha_2[0][i] = ha_2.p[index][0];
						sha_2[1][i] = ha_2.p[index][1];
						sha_2[2][i] = ha_2.p[index][2];
					} else {
						break;
					}
				}
			}


			// Sum of absolute differences
			if(!metric) {
				this_factor = (   SAD(sha_1[0], sha_2[0], size) +   SAD(sha_1[1], sha_2[1], size) + \
					  SAD(sha_1[2], sha_2[2], size) ) / (float)3;
			// Median absolute deviation
			} else if(metric == 1) {
				this_factor = (   MAD(sha_1[0], sha_2[0], size) +   MAD(sha_1[1], sha_2[1], size) + \
					  MAD(sha_1[2], sha_2[2], size) ) / 3;
			// Mean squared error
			} else if(metric == 2) {
				this_factor = (   MSE(sha_1[0], sha_2[0], size) +   MSE(sha_1[1], sha_2[1], size) + \
					  MSE(sha_1[2], sha_2[2], size) ) / 3;
			// Root-mean-square error
			} else if(metric == 3) {
				this_factor = (  RMSE(sha_1[0], sha_2[0], size) +  RMSE(sha_1[1], sha_2[1], size) + \
					 RMSE(sha_1[2], sha_2[2], size) ) / 3;
			// Peak signal-to-noise ratio
			} else if(metric == 4) {
				this_factor = (  PSNR(sha_1[0], sha_2[0], size) +  PSNR(sha_1[1], sha_2[1], size) + \
					 PSNR(sha_1[2], sha_2[2], size) ) / 3;
			// Structural similarity
			} else if(metric == 5) {
				this_factor = (  SSIM(sha_1[0], sha_2[0], size) +  SSIM(sha_1[1], sha_2[1], size) + \
					 SSIM(sha_1[2], sha_2[2], size) ) / 3;
			// Structural dissimilarity
			} else {
				this_factor = ( DSSIM(sha_1[0], sha_2[0], size) + DSSIM(sha_1[1], sha_2[1], size) + \
					DSSIM(sha_1[2], sha_2[2], size) ) / 3;
			}

			if( ((metric <  4 || metric >  5) && this_factor <= factor) || \
			    ((metric == 4 || metric == 5) && this_factor >= factor) ) {

				vectors_size += 2;
				*vectors = (unsigned int*)realloc(*vectors, vectors_size * sizeof(unsigned int));

				(*vectors)[vectors_size - 2] = p; // Start-Hexint
				(*vectors)[vectors_size - 1] = q; // und Ziel-
			}
		}
	}


	return vectors_size;
}


void vectors2file(unsigned int* vectors, unsigned int vectors_size,
 char* filename) {
	FILE* file = fopen(filename, "w");

	fputs("FROM TO\n", file);

	for(unsigned int i = 0; i < vectors_size; i += 2)
		fprintf(file, "%u %u\n", vectors[i], vectors[i + 1]);

	fclose(file);
}




// Hexagonal Motion Compensation HMC

// P-Frame = rgb_hexarray + vectors
void PFrame_gen(P_Frame* pframe,
 RGB_Hexarray rgb_hexarray, unsigned int order,
 unsigned int* vectors, unsigned int vectors_size) {
	Hexarray_init(&pframe->rgb_hexarray, order, 0);


	for(unsigned int i = 0; i < rgb_hexarray.size; i++) {
		pframe->rgb_hexarray.p[i][0] = rgb_hexarray.p[i][0];
		pframe->rgb_hexarray.p[i][1] = rgb_hexarray.p[i][1];
		pframe->rgb_hexarray.p[i][2] = rgb_hexarray.p[i][2];
	}

	pframe->vectors = (unsigned int*)malloc(vectors_size * sizeof(unsigned int));

	memcpy(pframe->vectors, vectors, vectors_size * sizeof(unsigned int));


	for(unsigned int i = 0; pframe->vectors[i]; i += 2) {
		const unsigned int p = pframe->vectors[i];

		pframe->rgb_hexarray.p[p][0] = -1;
		pframe->rgb_hexarray.p[p][1] = -1;
		pframe->rgb_hexarray.p[p][2] = -1;
	}
}

// B-Frame = rgb_hexarray + vectors_backwards + vectors_forwards
void BFrame_gen(B_Frame* bframe,
 RGB_Hexarray rgb_hexarray, unsigned int order,
 unsigned int* vectors_backwards, unsigned int vectors_backwards_size,
 unsigned int* vectors_forwards,  unsigned int vectors_forwards_size) {
	Hexarray_init(&bframe->rgb_hexarray, order, 0);


	for(unsigned int i = 0; i < rgb_hexarray.size; i++) {
		bframe->rgb_hexarray.p[i][0] = rgb_hexarray.p[i][0];
		bframe->rgb_hexarray.p[i][1] = rgb_hexarray.p[i][1];
		bframe->rgb_hexarray.p[i][2] = rgb_hexarray.p[i][2];
	}

	bframe->vectors_backwards = (unsigned int*)malloc(vectors_backwards_size * sizeof(unsigned int));
	bframe->vectors_forwards  = (unsigned int*)malloc(vectors_forwards_size  * sizeof(unsigned int));

	memcpy(bframe->vectors_backwards, vectors_backwards, vectors_backwards_size * sizeof(unsigned int));
	memcpy(bframe->vectors_forwards,  vectors_forwards,  vectors_forwards_size  * sizeof(unsigned int));


	for(unsigned int i = 0; bframe->vectors_backwards[i]; i += 2) {
		const unsigned int p = bframe->vectors_backwards[i];

		bframe->rgb_hexarray.p[p][0] = -1;
		bframe->rgb_hexarray.p[p][1] = -1;
		bframe->rgb_hexarray.p[p][2] = -1;
	}
	for(unsigned int i = 1; bframe->vectors_forwards[i];  i += 2) {
		const unsigned int p = bframe->vectors_forwards[i];

		bframe->rgb_hexarray.p[p][0] = -1;
		bframe->rgb_hexarray.p[p][1] = -1;
		bframe->rgb_hexarray.p[p][2] = -1;
	}
}

