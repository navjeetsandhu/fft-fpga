// Author: Arjun Ramaswami

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "helper.h"

/**
 * \brief  create random single precision complex floating point values  
 * \param  inp : pointer to float2 data of size N 
 * \param  N   : number of points in the array
 * \return 0 if successful 1 if not
 */
int fftf_create_data(float2 *inp, int N){

  if(inp == NULL || N <= 0){
    return 1;
  }

  for(int i = 0; i < N; i++){
    inp[i].x = (float)((float)rand() / (float)RAND_MAX);
    inp[i].y = (float)((float)rand() / (float)RAND_MAX);
  }

#ifdef DEBUG          
    FILE *fptr = fopen("input_data.txt", "w"); 
    for(int i = 0; i < N; i++){
      if (fptr != NULL){
        fprintf(fptr, "%d : fft[%d] = (%f, %f) \n", i, i, inp[i].x, inp[i].y);
      }
    }
    fclose(fptr); 
#endif

  return 0;
}

/**
 * \brief  create random double precision complex floating point values  
 * \param  inp : pointer to double2 data of size N 
 * \param  N   : number of points in the array
 * \return 0 if successful 1 if not
 */
int fft_create_data(double2 *inp, int N){

  if(inp == NULL || N <= 0 || N > 1024){
    return 1;
  }

  for(int i = 0; i < N; i++){
    inp[i].x = (double)((double)rand() / (double)RAND_MAX);
    inp[i].y = (double)((double)rand() / (double)RAND_MAX);
  }

#ifdef DEBUG          
    FILE *fptr = fopen("input_data.txt", "w"); 
    for(int i = 0; i < N; i++){
      if (fptr != NULL){
        fprintf(fptr, "%d : fft[%d] = (%lf, %lf) \n", i, i, inp[i].x, inp[i].y);
      }
    }
    fclose(fptr); 
#endif

  return 0;
}

/**
 * \brief  print configuration chosen to execute on FPGA
 * \param  N: fft size
 * \param  dim: number of dimensions of size
 * \param  iter: number of iterations of each transformation (if BATCH mode)
 * \param  inv: 1, backward transform
 * \param  sp: 1, single precision floating point transformation
 * \param  use_bram: 1 if transpose uses BRAM, not DDR (valid for 2d and 3d FFT)
 */
void print_config(int N, int dim, int iter, int inv, int sp, int use_bram){
  printf("\n------------------------------------------\n");
  printf("FFT Configuration: \n");
  printf("--------------------------------------------\n");
  printf("Type               = Complex to Complex\n");
  printf("Points             = %d%s \n", N, dim == 1 ? "" : dim == 2 ? "^2" : "^3");
  printf("Precision          = %s \n",  sp==1 ? "Single": "Double");
  printf("Direction          = %s \n", inv ? "Backward":"Forward");
  printf("Placement          = In Place    \n");
  printf("Iterations         = %d \n", iter);
  printf("Transpose          = %s \n", use_bram ? "BRAM":"DDR");
  printf("--------------------------------------------\n\n");
}

/**
 * \brief  print time taken for fpga and fftw runs to a file
 * \param  timing: kernel execution and pcie transfer timing 
 * \param  N: fft size
 * \param  dim: number of dimensions of size
 * \param  iter: number of iterations of each transformation (if BATCH mode)
 * \param  inv: 1 if backward transform
 * \param  single precision floating point transformation
 */
void display_measures(fpga_t timing, int N, int dim, int iter, int inv, int sp){

  double exec = timing.exec_t / iter;
  double gpoints_per_sec = (pow(N, dim)  / (exec * 1e-3)) * 1e-9;
  double gBytes_per_sec = 0.0;

  if(sp == 1){
    gBytes_per_sec =  gpoints_per_sec * 8; // bytes
  }
  else{
    gBytes_per_sec *=  gpoints_per_sec * 16;
  }

  double gflops = dim * 5 * pow(N, dim) * (log((double)N)/log((double)2))/(exec * 1e-3 * 1E9); 

  printf("\n------------------------------------------\n");
  printf("Measurements \n");
  printf("--------------------------------------------\n");
  printf("Points             = %d%s \n", N, dim == 1 ? "" : dim == 2 ? "^2" : "^3");
  printf("Precision          = %s\n",  sp==1 ? "Single": "Double");
  printf("Direction          = %s\n", inv ? "Backward":"Forward");
  printf("PCIe Write         = %.2lfms\n", timing.pcie_write_t);
  printf("Kernel Execution   = %.2lfms\n", exec);
  printf("PCIe Write         = %.2lfms\n", timing.pcie_read_t);
  printf("Throughput         = %.2lfGFLOPS/s | %.2lf GB/s\n", gflops, gBytes_per_sec);
}