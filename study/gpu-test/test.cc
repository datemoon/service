#include "cuda.h"
#include "cublas_v2.h"
#include "cuda_runtime.h"
#include "stdio.h"
#include "stdlib.h"

void WriteMat(float *tempA,const char *file,int rows,int cols)
{
	FILE *fp=fopen(file,"w");
	for(int i =0;i<rows;++i)
	{
		for(int j =0;j<cols;++j)
		{
			//fwrite(&tempA(i,j),sizeof(float),1,fp);
			fprintf(fp,"%e ",tempA[i*cols+j]);
		}
		fprintf(fp,"\n");		
	}	
	fclose(fp);	
}
																		//

float* ReadMatrix(const char *file,int row,int col)
{
	float *A = (float*)malloc(sizeof(float)*row*col);
	FILE *fp = fopen(file,"r");
	fread(A, sizeof(float),row*col,fp);
	fclose(fp);
	return A;
}
int main(int argc,char*argv[])
{
	int rows = atoi(argv[4]);
	unsigned int size_A = rows*3072*sizeof(float),
				 size_B = 3072 * 512 * sizeof(float),
				 size_C = rows * 512*sizeof(float);

	float *A = ReadMatrix(argv[1],rows,3072);
	float *B = ReadMatrix(argv[2],3072,512);
	float *g_A, *g_B, *g_C;
	cudaMalloc((void **) &g_A, size_A);
	cudaMalloc((void **) &g_B, size_B);
	cudaMalloc((void **) &g_C, size_C);
	float *C = (float*)malloc(size_C);

	cudaMemcpy(g_A, A, size_A, cudaMemcpyHostToDevice);
	cudaMemcpy(g_B, B, size_B, cudaMemcpyHostToDevice);
	float alpha_=1.0;
	float beta_=0.0;
	cublasHandle_t handle;
	cublasCreate(&handle);
	cublasSgemm(handle,
				CUBLAS_OP_N, CUBLAS_OP_N, 512, rows, 3072,
				&alpha_, g_B, 512, g_A, 3072,
				&beta_, g_C, 512);
	cudaMemcpy(C, g_C, size_C, cudaMemcpyDeviceToHost);
	WriteMat(C,argv[3], rows, 512);
	return 0;
}
