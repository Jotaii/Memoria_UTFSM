#include <iostream>
#include <time.h>
#include <cuda_runtime.h>


__global__ void kernel(int *dev_array, int N){
    int tId = threadIdx.x + blockIdx.x * blockDim.x;
    if(tId < N){
        dev_array[tId] = 1;
    }
}




/*
 *  Procesamiento Imagen CPU
 */
void funcionCPU(){

}

/*
 *  Procesamiento Imagen GPU
 */
__global__ void kernelGPU(){

}

/*
 *  Codigo Principal
 */
int main(int argc, char **argv){

    // /*
    //  *  Inicializacion
    //  */
	// clock_t t1, t2;
	// cudaEvent_t ct1, ct2;
	// double ms;
	// float dt;
	// int M, N;
    // float *Rhost, *Ghost, *Bhost;
    // float *Rhostout, *Ghostout, *Bhostout;
    // float *Rdev, *Gdev, *Bdev;
    // float *Rdevout, *Gdevout, *Bdevout;

    // Read(&Rhost, &Ghost, &Bhost, &M, &N, "img.txt");

    // /*
    //  *  Parte CPU
    //  */
    // Rhostout = new float[M*N];
    // Ghostout = new float[M*N];
    // Bhostout = new float[M*N];

    // t1 = clock();
    // funcion(); // Agregar parametros!
    // t2 = clock();
    // ms = 1000.0 * (double)(t2 - t1) / CLOCKS_PER_SEC;
    // std::cout << "Tiempo CPU: " << ms << "[ms]" << std::endl;
    // Write(Rhostout, Ghostout, Bhostout, M, N, "imgCPU.txt");

    // delete[] Rhostout; delete[] Ghostout; delete[] Bhostout;
    
    // /*
    //  *  Parte GPU
    //  */

    // int grid_size, block_size = 256;
    // grid_size = (int)ceil((float) M * N / block_size);
        
    // cudaMalloc((void**)&Rdev, M * N * sizeof(float));
    // cudaMalloc((void**)&Gdev, M * N * sizeof(float));
    // cudaMalloc((void**)&Bdev, M * N * sizeof(float));
    // cudaMemcpy(Rdev, Rhost, M * N * sizeof(float), cudaMemcpyHostToDevice);
    // cudaMemcpy(Gdev, Ghost, M * N * sizeof(float), cudaMemcpyHostToDevice);
    // cudaMemcpy(Bdev, Bhost, M * N * sizeof(float), cudaMemcpyHostToDevice);
        
    // cudaMalloc((void**)&Rdevout, M * N * sizeof(float));
    // cudaMalloc((void**)&Gdevout, M * N * sizeof(float));
    // cudaMalloc((void**)&Bdevout, M * N * sizeof(float));
    
    // cudaEventCreate(&ct1);
    // cudaEventCreate(&ct2);
    // cudaEventRecord(ct1);
    // kernel<<<grid_size, block_size>>>(); // Agregar parametros!
    // cudaEventRecord(ct2);
    // cudaEventSynchronize(ct2);
    // cudaEventElapsedTime(&dt, ct1, ct2);
    // std::cout << "Tiempo GPU: " << dt << "[ms]" << std::endl;

    // Rhostout = new float[M*N];
    // Ghostout = new float[M*N];
    // Bhostout = new float[M*N];
    // cudaMemcpy(Rhostout, Rdevout, M * N * sizeof(float), cudaMemcpyDeviceToHost);
    // cudaMemcpy(Ghostout, Gdevout, M * N * sizeof(float), cudaMemcpyDeviceToHost);
    // cudaMemcpy(Bhostout, Bdevout, M * N * sizeof(float), cudaMemcpyDeviceToHost);
    // Write(Rhostout, Ghostout, Bhostout, M, N, "imgGPU.txt");

    // cudaFree(Rdev); cudaFree(Gdev); cudaFree(Bdev);
    // cudaFree(Rdevout); cudaFree(Gdevout); cudaFree(Bdevout);
    // delete[] Rhost; delete[] Ghost; delete[] Bhost;
    // delete[] Rhostout; delete[] Ghostout; delete[] Bhostout;
    
    int N = 100;
    int block_size = 256;
    int grid_size = (int)ceil((float)N / block_size);
    // std::cout << 0 << "\n";
    int *host_array = new int[N];
    int *dev_array;
    cudaMalloc(&dev_array, N* sizeof(int));
    kernel<<<grid_size, block_size>>>(dev_array, N);
    cudaMemcpy(host_array, dev_array, N* sizeof(int), cudaMemcpyDeviceToHost);
    cudaFree(dev_array);
    std::cout << "(";
    for (int i = 0; i < N; i++){
        std::cout << host_array[i] << " ";
    }
    std::cout << ")\n";
	return 0;
}