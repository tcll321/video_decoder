#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <stdio.h>

typedef unsigned int    uint32;
typedef int             int32;
typedef unsigned char   uint8;
typedef struct __uint32
{
	uint8 r;
	uint8 g;
	uint8 b;
	uint8 a;
};


extern "C"
__global__ void cutPicture(uint32 * srcImage, int srcWidth, int srcHeight, uint32 * dstImage, int cx, int cy, int dstWidth, int dstHeight)
{
	int32 x, y;
	int srcPitch = srcWidth * 4;
	int dstPitch = dstWidth * 4;
	__uint32 *srcImageU8 = (__uint32 *)srcImage;
	__uint32 *dstImageU8 = (__uint32 *)dstImage;

	x = blockIdx.x * (blockDim.x << 1) + (threadIdx.x << 1);
	y = blockIdx.y *  blockDim.y + threadIdx.y;

	if ((x-1) >= dstWidth)
		return; //x = width - 1;

	if ((y-1) >= dstHeight)
		return; // y = height - 1;

	dstImageU8[y * dstWidth + x].b = srcImageU8[(y + cy) * srcWidth + x + cx].b;
	dstImageU8[y * dstWidth + x].g = srcImageU8[(y + cy) * srcWidth + x + cx].g;
	dstImageU8[y * dstWidth + x].r = srcImageU8[(y + cy) * srcWidth + x + cx].r;
	dstImageU8[y * dstWidth + x].a = srcImageU8[(y + cy) * srcWidth + x + cx].a;
	dstImageU8[y * dstWidth + x + 1].b = srcImageU8[(y + cy) * srcWidth + x + cx + 1].b;
	dstImageU8[y * dstWidth + x + 1].g = srcImageU8[(y + cy) * srcWidth + x + cx + 1].g;
	dstImageU8[y * dstWidth + x + 1].r = srcImageU8[(y + cy) * srcWidth + x + cx + 1].r;
	dstImageU8[y * dstWidth + x + 1].a = srcImageU8[(y + cy) * srcWidth + x + cx + 1].a;
}

int MattingPicture(void * srcImage, int srcWidth, int srcHeight, void * dstImage, int x, int y, int dstWidth, int dstHeight)
{
	cudaError_t cudaStatus;
	dim3 block(32, 16, 1);
	dim3 grid((dstWidth + (2 * block.x - 1)) / (2 * block.x), (dstHeight + (block.y - 1)) / block.y, 1);
	cutPicture << <grid, block, 0 >> > ((uint32*)srcImage, srcWidth, srcHeight, (uint32*)dstImage, x, y, dstWidth, dstHeight);
	// Check for any errors launching the kernel
	cudaStatus = cudaGetLastError();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
	}

	// cudaDeviceSynchronize waits for the kernel to finish, and returns
	// any errors encountered during the launch.
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
	}
	return (int)cudaStatus;
}