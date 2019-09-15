/******************************************************************************
 * HMEC-Test: Hexagonal Motion Estimation and Compensation
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

#include "CHIP/Misc/Precalcs.h"

#include "CHIP/CHIP/Hexarray.h"

#include "HMEC/HMEC.h"


int main(int argc, char** argv) {
	const unsigned int  order =  2;
	const unsigned int  size  = 49; // = 7^2
	      unsigned int* vectors;
	      RGB_Hexarray  ha_1;
	      RGB_Hexarray  ha_2;


	precalcs_init(order + 1, 1.0f, 1.0f);

	Hexarray_init(&ha_1, order, 0);
	Hexarray_init(&ha_2, order, 0);


	for(unsigned int i = 0; i < size; i++) {
		if(i < 7) {
			ha_1.p[i][0] = 64;
			ha_1.p[i][1] = 64;
			ha_1.p[i][2] = 64;
		}

		if(i > 6) {
			ha_2.p[i][0] = 64;
			ha_2.p[i][1] = 64;
			ha_2.p[i][2] = 64;
		}
	}

	const unsigned int vectors_size = \
		HME(&vectors, ha_1, ha_2, 7, 0, 0, 16.0f, 1.0f);

	printf("Vector 1 = [%u, %u]: ", vectors[0], vectors[1]); // = [0, 7]

	if(vectors[0] == 0 && vectors[1] == 7) {
		puts("test passed");
	} else {
		puts("test not passed");
	}

	vectors2file(vectors, vectors_size, "vectors.dat");


	precalcs_free();

	Hexarray_free(&ha_1, 0);
	Hexarray_free(&ha_2, 0);

	free(vectors);


	return 0;
}
