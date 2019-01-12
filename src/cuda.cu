#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <cuda.h>
#include <curand.h>
#include <helper_cuda.h>

#define TPB 512//threads per block
#define BLOCKS 4

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
  
  /* printf("%6i %6i %6i %6i %6i %6i\n", */
  /* 	 indo, nThreads, idxPerThread, begin, end, indo<underCompute); */

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
      
      
      for(x += begin, xu0 += begin; i < (end-begin); ++x, ++xu0, i++)
	res[x] += min(min(res[xu0-1], res[xu0]), res[xu0+1]);
    } 
    __syncthreads();
  }
}

__device__ int find_min(int32_t * arr, int n){
    int minIdx = 0;
    for(int i = 0; i < n; i++)
      if (arr[i] < arr[minIdx])
	minIdx = i;
    return minIdx;
  };


__global__ void findMinPath(int32_t * energy, int32_t * energySum, int32_t * removedPixels, int64_t  w, int64_t h){

  
  int32_t * res = energySum;
    
  int y = w * (h-1);
  int x = find_min(&res[y], w);
    
  for(int i=1; i < h; i++){
    int off = x;
    //removedPixels[off + (w*(h-i))] = true;
    removedPixels[h-i] = off;
    y -= w;
    
     if(off == 0)
       x = find_min(&res[y], 2);
     else if(off == w-1)
       x += find_min(&res[y+off-1], 2) -1;
     else
       x += find_min(&res[y+off-1], 3) -1;
  }
  int off = x;
  //removedPixels[off] = true;
  removedPixels[0] = off;
}


__global__ void removeSeam(int32_t * removedPixels, pixel_t * img, pixel_t * img_res, int64_t  w, int64_t h){
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

  
  int64_t i = begin;
  for (int y=i/(w-1); y<h; ++y)
    for (int x= i==begin ? i%(w-1) : 0; x<w-1; ++x, i++){
      if(i >= end) return;
      img_res[x+(w-1)*y] = img[x + w*y + ((x >= removedPixels[y])? 1:0)];
    }
}


__global__ void seamCarving(pixel_t * img, int32_t * energy, int32_t * energySum, int64_t  w, int64_t h){
  /* computeEnergy(img, energy, w, h); */
  /* computeEnergySum(energy, energySum, w, h); */
}


extern "C" void cudaProxy(uint8_t* h_img, uint8_t* h_img_res, int64_t w, int64_t h, int64_t N){
  /*
    memory start
  */  
  pixel_t *d_img;
  pixel_t *d_img_res;
  int32_t *d_energy;
  int32_t *d_energySum;
  int32_t *d_removedPixels;
  //int32_t h_energy[w*h];
  //int32_t h_energySum[w*h];
  //int32_t  h_removedPixels[h];

  checkCudaErrors( cudaMalloc((void **)&d_img,         sizeof(pixel_t)*w*h+2*sizeof(pixel_t) ) );
  checkCudaErrors( cudaMalloc((void **)&d_energy,        sizeof(int32_t)*w*h ) );
  checkCudaErrors( cudaMalloc((void **)&d_energySum,     sizeof(int32_t)*w*h ) );
  checkCudaErrors( cudaMalloc((void **)&d_removedPixels, sizeof(int32_t)*h ) );


  checkCudaErrors( cudaMemcpy(&d_img[1], h_img, sizeof(pixel_t)*w*h, cudaMemcpyHostToDevice) );
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

  
  for(int i = 0; i < N; i++){

    checkCudaErrors( cudaMalloc((void **)&d_img_res, sizeof(pixel_t)*(w-1)*h+2*sizeof(pixel_t)) );
    
    pixel_t *d_img_off     = &d_img[1];
    pixel_t *d_img_res_off = &d_img_res[1];
    
    computeEnergy<<<BLOCKS, TPB>>>(d_img_off, d_energy, w, h);
    checkCudaErrors( cudaMemcpy(d_energySum, d_energy, sizeof(int32_t)*w*h, cudaMemcpyDeviceToDevice) );
    computeEnergySum<<<1, TPB>>>(d_energy, d_energySum, w, h);
    findMinPath<<<1, 1>>>(d_energy, d_energySum, d_removedPixels, w, h);
    removeSeam<<<BLOCKS, TPB>>>(d_removedPixels, d_img_off, d_img_res_off, w, h);

    checkCudaErrors( cudaFree(d_img) );

    d_img = d_img_res;
    w--;

  }
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

  /* for(int i = 0; i < h; i++) */
  /*   checkCudaErrors( cudaMemcpy(&h_img_res[i*w], &d_img[(w+N)*i], sizeof(pixel_t)*w, cudaMemcpyDeviceToHost) ); */

  checkCudaErrors( cudaMemcpy(h_img_res, &d_img[1], sizeof(pixel_t)*w*h, cudaMemcpyDeviceToHost) ); 

  
  //checkCudaErrors( cudaMemcpy(&h_energy, d_energy, sizeof(int32_t)*w*h, cudaMemcpyDeviceToHost) );
  //checkCudaErrors( cudaMemcpy(&h_energySum, d_energySum, sizeof(int32_t)*w*h, cudaMemcpyDeviceToHost) );
  //checkCudaErrors( cudaMemcpy(&h_removedPixels, d_removedPixels, sizeof(int8_t)*w*h, cudaMemcpyDeviceToHost) );
  checkCudaErrors( cudaFree(d_img) );
  checkCudaErrors( cudaFree(d_energy) );
  checkCudaErrors( cudaFree(d_energySum) );
  checkCudaErrors( cudaFree(d_removedPixels) );

  
  /* for(int y = 0; y < h; y++){ */
  /*   for(int x = 0; x < w; x++) */
  /*     printf("%8i", (int)h_removedPixels[(x+y*w)]); */
  /*   printf("\n"); */
  /* } */

  
  /*
    memory free stop
   */
  
  cudaDeviceReset();
}
