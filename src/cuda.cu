#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <cuda.h>
#include <curand.h>
#include <helper_cuda.h>

#define TPB 1 //threads per block
#define BLOCKS 1

typedef struct{
  uint8_t r, g, b;
}pixel_t;


__global__ void seamCarving(pixel_t * img, int64_t  w, int64_t h){
  extern __shared__  int32_t energy[];

  int indo = threadIdx.x + blockIdx.x*blockDim.x;
  int nThreads = blockDim.x * gridDim.x;
  int idxPreThread = w*h/nThreads;
  int begin = max((int)(idxPreThread * indo), 0);
  int end = min((int)(begin + idxPreThread), (int)(w*h));
  
  for(int i = begin; i < end; i++){
    energy[i] =
      (img[i-1].r - img[i+1].r) * (img[i-1].r - img[i+1].r) +
      (img[i-1].g - img[i+1].g) * (img[i-1].g - img[i+1].g) +
      (img[i-1].b - img[i+1].b) * (img[i-1].b - img[i+1].b);
  }
  
  printf("%i %i %i\n", (int)img[0].r, (int)img[0].g, (int)img[0].b);
  printf("size of img=%i  w=%li h=%li \n", sizeof(*img), w, h);
  //printf("size of img=%i  w=%lu h=%lu \n", sizeof(*img), width, height);

  //int32_t energy[w*h];

  for(int y = 0; y < h; y++){
    for(int x = 0; x < w; x++)
      printf("%4i%4i%4i     ", (int)img[(x+y*w)].r , (int)img[(x+y*w)].g, (int)img[(x+y*w)].b);
    //std::cout << std::setw(8) << energyTable(x,y);
    printf("\n");
  }

  for(int y = 0; y < h; y++){
    for(int x = 0; x < w; x++)
      printf("%8i", energy[(x+y*w)]);
    printf("\n");
  }

  
}

void cudaProxy(uint8_t* h_img, int64_t w, int64_t h){
  /*
    memory start
  */  
  pixel_t *d_img;
  
  checkCudaErrors( cudaMalloc((void **)&d_img, sizeof(pixel_t)*w*h ) );
  checkCudaErrors( cudaMemcpy(d_img, h_img, sizeof(pixel_t)*w*h, cudaMemcpyHostToDevice) );
  
  for(int y = 0; y < h; y++){
    for(int x = 0; x < w; x++)
      printf("%4i%4i%4i     ", (int)h_img[3*(x+y*w)] , (int)h_img[3*(x+y*w)+1], (int)h_img[3*(x+y*w)+2]);
    //std::cout << std::setw(8) << energyTable(x,y);
    printf("\n");
  }


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
  int shared_mem_size = w*h *sizeof(int32_t);
  seamCarving<<<BLOCKS, TPB, shared_mem_size>>>(d_img, w, h);
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
  
  //checkCudaErrors( cudaMemcpy(h_img, d_img, sizeof(h_img), cudaMemcpyDeviceToHost) );
  checkCudaErrors( cudaFree(d_img) );
  
  /*
    memory free stop
   */
  
  cudaDeviceReset();
}
