#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <cuda.h>
#include <curand.h>
#include <helper_cuda.h>

#define TPB 21//threads per block
#define BLOCKS 1

typedef struct{
  uint8_t r, g, b;
}pixel_t;


__device__ void computeEnergyShared(pixel_t * img, int64_t  w, int64_t h){
  extern __shared__  int32_t energy[];

  int indo = threadIdx.x + blockIdx.x*blockDim.x;
  int nThreads = blockDim.x * gridDim.x;

  int idxPerThread = max(1, (int)(w*h/nThreads));
  int underCompute = (w*h) - (idxPerThread*nThreads);


  int begin = max((int)(idxPerThread * indo), 0);
  begin += indo < underCompute ? indo: underCompute;
  idxPerThread += !!(indo < underCompute);
  
  int end = min((int)(begin + idxPerThread), (int)(w*h));

  //printf("%6i %6i %6i %6i %6i %6i\n", indo, nThreads, idxPerThread, begin, end);
  
  for(int i = begin; i < end; i++){
    energy[i] =
      (img[i-1].r - img[i+1].r) * (img[i-1].r - img[i+1].r) +
      (img[i-1].g - img[i+1].g) * (img[i-1].g - img[i+1].g) +
      (img[i-1].b - img[i+1].b) * (img[i-1].b - img[i+1].b);
  }
  
  /* for(int y = 0; y < h; y++){ */
  /*   for(int x = 0; x < w; x++) */
  /*     printf("%8i", energy[(x+y*w)]); */
  /*   printf("\n"); */
  /* } */

  
}
__global__ void computeEnergy(pixel_t * img, int32_t * energy, int64_t  w, int64_t h){
  int indo = threadIdx.x + blockIdx.x*blockDim.x;
  int nThreads = blockDim.x * gridDim.x;

  int idxPerThread = max(1, (int)(w*h/nThreads));
  int underCompute = max((int)((w*h) - (idxPerThread*nThreads)), 0);


  int begin = max((int)(idxPerThread * indo), 0);
  begin += indo < underCompute ? indo: underCompute;
  idxPerThread += !!(indo < underCompute);
  
  int end = min((int)(begin + idxPerThread), (int)(w*h));

  /* printf("%6i %6i %6i %6i %6i %6i\n", */
  /* 	 indo, nThreads, idxPerThread, begin, end, indo<underCompute); */
  
  for(int i = begin; i < end; i++){
    energy[i] =
      (img[i-1].r - img[i+1].r) * (img[i-1].r - img[i+1].r) +
      (img[i-1].g - img[i+1].g) * (img[i-1].g - img[i+1].g) +
      (img[i-1].b - img[i+1].b) * (img[i-1].b - img[i+1].b);
  }
}

__global__ void computeEnergySum(int32_t * energy, int32_t * energySum, int64_t  w, int64_t h){
  int indo = threadIdx.x + blockIdx.x*blockDim.x;
  int nThreads = blockDim.x * gridDim.x;

  int idxPerThread = max(1, (int)(w/nThreads));
  int underCompute = max((int)((w) - (idxPerThread*nThreads)), 0);


  int begin = max((int)(idxPerThread * indo), 0);
  begin += indo < underCompute ? indo: underCompute;
  idxPerThread += !!(indo < underCompute);
  
  int end = min((int)(begin + idxPerThread), (int)(w));
  
  printf("%6i %6i %6i %6i %6i %6i\n",
  	 indo, nThreads, idxPerThread, begin, end, indo<underCompute);

  int32_t * res = energySum;
  
  for(int y=1; y < h; ++y){
    if(begin < end){
      int x   = w * y;
      int xu0 = w * (y-1);
      
      int i = 0;
      if(end == w){
	res[x+w-1] += min(res[xu0+w-1-1], res[xu0+w-1]);
	i++;
      }
      if(begin == 0){
	res[x] += min(res[xu0], res[xu0+1]);
	i++; x++; xu0++;
      }
      
      x   += begin;
      xu0 += begin; 
      
      for(x, xu0; i < (end-begin); ++x, ++xu0, i++)
	res[x] += min(min(res[xu0-1], res[xu0]), res[xu0+1]);
    } 
    __syncthreads();
  }
}
__global__ void seamCarving(pixel_t * img, int32_t * energy, int32_t * energySum, int64_t  w, int64_t h){
  /* computeEnergy(img, energy, w, h); */
  /* computeEnergySum(energy, energySum, w, h); */
}


extern "C" void cudaProxy(uint8_t* h_img, int64_t w, int64_t h){
  /*
    memory start
  */  
  pixel_t *d_img;
  int32_t *d_energy;
  int32_t *d_energySum;
  int32_t h_energy[w*h];
  int32_t h_energySum[w*h];
  
  checkCudaErrors( cudaMalloc((void **)&d_img,       sizeof(pixel_t)*w*h+1 ) );
  checkCudaErrors( cudaMalloc((void **)&d_energy,    sizeof(int32_t)*w*h ) );
  checkCudaErrors( cudaMalloc((void **)&d_energySum, sizeof(int32_t)*w*h ) );
  checkCudaErrors( cudaMemcpy(d_img, h_img, sizeof(pixel_t)*w*h, cudaMemcpyHostToDevice) );
  
  /*
    memory stop
  */
  
  
  float elapsed=0;
  cudaEvent_t start, stop;

  checkCudaErrors(cudaEventCreate(&start));
  checkCudaErrors(cudaEventCreate(&stop));

  checkCudaErrors( cudaEventRecord(start, 0));
  /*
    Run kernel
  */
  //strumienie moga przyspieszyc
  //child kerlen - lepsza organizacja
  //czy uzywajac shared mozna przyspieczyc 
  
  //int shared_mem_size = w*h *sizeof(int32_t);
  //seamCarving<<<BLOCKS, TPB, shared_mem_size>>>(d_img, w, h);
  //seamCarving<<<BLOCKS, TPB>>>(d_img, d_energy, d_energySum, w, h);
  computeEnergy<<<BLOCKS, TPB>>>(d_img, d_energy, w, h);
  checkCudaErrors( cudaMemcpy(d_energySum, d_energy,
			      sizeof(int32_t)*w*h, cudaMemcpyDeviceToDevice) );
  computeEnergySum<<<BLOCKS, 1>>>(d_energy, d_energySum, w, h);
  cudaDeviceSynchronize();
  /*
    Stop kernel 
  */
  checkCudaErrors(cudaEventRecord(stop, 0));
  checkCudaErrors(cudaEventSynchronize (stop) );

  checkCudaErrors(cudaEventElapsedTime(&elapsed, start, stop) );

  checkCudaErrors(cudaEventDestroy(start));
  checkCudaErrors(cudaEventDestroy(stop));

  printf("The elapsed time in gpu was %.2f ms\n", elapsed);

  /*
    memory free start
  */
  
  checkCudaErrors( cudaMemcpy(&h_energy, d_energy, sizeof(int32_t)*w*h, cudaMemcpyDeviceToHost) );
  checkCudaErrors( cudaMemcpy(&h_energySum, d_energySum, sizeof(int32_t)*w*h, cudaMemcpyDeviceToHost) );
  checkCudaErrors( cudaFree(d_img) );

  for(int y = 0; y < h; y++){
    for(int x = 0; x < w; x++)
      printf("%8i", h_energySum[(x+y*w)]);
    printf("\n");
  }

  
  /*
    memory free stop
   */
  
  cudaDeviceReset();
}
