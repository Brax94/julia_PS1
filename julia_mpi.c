#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "julia_mpi.h"
#include "mpi.h"

double x_start=-2.01;
double x_end=1;
double yupper;
double ylower;

double ycenter=1e-6;
double step;

int pixel[XSIZE*YSIZE];
int bufferpixel[XSIZE*YSIZE];


// I suggest you implement these, however you can do fine without them if you'd rather operate
// on your complex number directly.
complex_t square_complex(complex_t c){
  complex_t temp;
    temp.real = (c.real*c.real) - (c.imag*c.imag);
    temp.imag = (2*c.real*c.imag);
    return temp;
}

complex_t add_complex(complex_t a, complex_t b){
  complex_t temp;
    temp.real = a.real + b.real;
    temp.imag = a.imag + b.imag;
    return temp;
}

complex_t add_real(complex_t a, int b){
  complex_t temp;
    temp.real = a.real + b;
    temp.imag = a.imag;
    return temp;
}

complex_t add_imag(complex_t a, int b){
    complex_t temp;
    temp.real = a.real;
    temp.imag = a.imag + b;
    return temp;
}



// add julia_c input arg here?
void calculate(complex_t julia_C, int rank, int numberprcs) {

    //Decide domain for each process
    int domain = XSIZE/(numberprcs);
    int loopend;
    if(rank != (numberprcs - 1)){
        loopend = rank*domain;
    }
    else{
        loopend = XSIZE;
    }


    //Rank 0 will be responsible for collecting and output, last rank will have to catch rests of the xsize
    //TODO: Redo this - rank 0 can partake in the calculation
        for(int i=(rank*domain);i<loopend;i++) {
            for(int j=0;j<YSIZE;j++) {


                /* Calculate the number of iterations until divergence for each pixel.
                   If divergence never happens, return MAXITER */
                complex_t c;
                complex_t z;
                complex_t temp;
                int iter = 0;

                // find our starting complex number c
                c.real = (x_start + step * i);
                c.imag = (ylower + step * j);

                // our starting z is c
                z = c;
                // iterate until we escape
                while (z.real * z.real + z.imag * z.imag < 4) {
                    // Each pixel in a julia set is calculated using z_n = (z_n-1)² + C
                    // C is provided as user input, so we need to square z and add C until we
                    // escape, or until we've reached MAXITER

                    // z = z squared + C

                    //temp = square_complex(z);
                    //z = add_complex(temp,julia_C);
                    z = square_complex(z);
                    z = add_complex(z, julia_C);

                    if (++iter == MAXITER) break;
                }
                //printf("The int is: %d\n", iter);
                pixel[PIXEL(i, j)] = iter;
            }
        }

}


int main(int argc,char **argv) {
	if(argc==1) {
		puts("Usage: JULIA\n");
		puts("Input real and imaginary part. ex: ./PS1_files 0.0 -0.8");
		return 0;
	}

    int rank;
    int numberprcs;

    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &numberprcs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    double my_runtime;
    my_runtime =- MPI_Wtime();
    printf("My rank is %d\n", rank);

	/* Calculate the range in the y-axis such that we preserve the
	   aspect ratio */
	step=(x_end-x_start)/XSIZE;
	yupper=ycenter+(step*YSIZE)/2;
	ylower=ycenter-(step*YSIZE)/2;

  // Unlike the mandelbrot set where C is the coordinate being iterated, the
  // julia C is the same for all points and can be chosed arbitrarily
  complex_t julia_C;

  // Get the command line args
    puts(argv[1]);
  julia_C.real = strtod(argv[1], NULL);
  julia_C.imag = strtod(argv[2], NULL);

	calculate(julia_C, rank, numberprcs);

    MPI_Reduce(&pixel, &bufferpixel, XSIZE*YSIZE, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Only rank 0 will output this
    if(rank == 0) {

        /* create nice image from iteration counts. take care to create it upside
           down (bmp format) */
        unsigned char *buffer = calloc(XSIZE * YSIZE * 3, 1);
        for (int i = 0; i < XSIZE; i++) {
            for (int j = 0; j < YSIZE; j++) {
                int p = ((YSIZE - j - 1) * XSIZE + i) * 3;
                fancycolour(buffer + p, bufferpixel[PIXEL(i, j)]);
            }
        }
        my_runtime += MPI_Wtime();

        /* write image to disk */
        savebmp("julia_test.bmp", buffer, XSIZE, YSIZE);

        printf("My runtime is: %f\n", my_runtime);
    }
    MPI_Finalize();
	return 0;
}
