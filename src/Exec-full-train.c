/*
 ============================================================================
 Author      : Roberto Diaz Morales
 ============================================================================
 
 Copyright (c) 2016 Roberto Díaz Morales

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
 (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
 publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
 ============================================================================
 */

/**
 * @brief Main command to train a full SVM using the parallel IRWLS procedure.
 * 
 * For a detailed description of the algorithm and its parameters read the following paper: \n Pérez-Cruz, F., Alarcón-Diana, P. L., Navia-Vázquez, A., & Artés-Rodríguez, A. (2001). Fast Training of Support Vector Classifiers. In Advances in Neural Information Processing Systems (pp. 734-740)
 *
 * For a detailed description about the parallelization read the following paper: \n
 Díaz-Morales, R., & Navia-Vázquez, Á. (2016). Efficient parallel implementation of kernel methods. Neurocomputing, 191, 175-186.
 *
 * @file Exec-full-train.c
 * @author Roberto Diaz Morales
 * @date 23 Aug 2016
 * @see full-train.h
 * 
 */

#include <omp.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#if !defined(_WIN32)
#include <sys/time.h>
#endif

#include "ParallelAlgorithms.h"
#include "full-train.h"
#include "kernels.h"



/**
 * @brief Is the main function to build the executable file to train a full SVM using the parallel IRWLS procedure.
 */
  
  
int main(int argc, char** argv)
{

    srand(0);	
    //srand48(0);

    properties props = parseTrainFULLParameters(&argc, &argv);
  
    if (argc != 3) {
        printFULLInstructions();
        return 4;
    }

    
    char * data_file = argv[1];
    char * data_model = argv[2];

    if(props.verbose==1){ 
        printf("\nRunning with parameters:\n");
        printf("------------------------\n");
        printf("Training set: %s\n",data_file);
        printf("The model will be saved in: %s\n",data_model);
        printf("Cost c = %f\n",props.C);
        printf("Working set size = %d\n",props.MaxSize);
        printf("Stop criteria = %f\n",props.Eta);

        if(props.kernelType == 0){
            printf("Using linear kernel\n");
        }else{
            printf("Using gaussian kernel with gamma = %f\n",props.Kgamma);
        }
        printf("------------------------\n");
        printf("\n");
    }
    // Loading dataset
    if(props.verbose==1) printf("\nReading dataset from file:%s\n",data_file);
    FILE *In = fopen(data_file, "r+");
    if (In == NULL) {
        fprintf(stderr, "Input file with the training set not found: %s\n",data_file);
        exit(2);
    }
    fclose(In);

    svm_dataset dataset;

    if(props.file==1){
        dataset = readTrainFile(data_file);
    }else{
        dataset = readTrainFileCSV(data_file,props.separator);
    }
    if(props.verbose==1) printf("Dataset Loaded\n\nTraining samples: %d\nNumber of features: %d\n\n",dataset.l,dataset.maxdim);

    #ifdef OSX    
    setenv("VECLIB_MAXIMUM_THREADS", "1", 1);
    #endif

    struct timeval tiempo1, tiempo2;
    omp_set_num_threads(props.Threads);

    if(props.verbose==1) printf("Running IRWLS\n");	
    gettimeofday(&tiempo1, NULL);

    initMemory(props.Threads,(props.MaxSize+1));
    double * W = trainFULL(dataset,props);
    freeMemory(props.Threads);

    gettimeofday(&tiempo2, NULL);
    if(props.verbose==1) printf("\nWeights calculated in %ld miliseconds\n\n",((tiempo2.tv_sec-tiempo1.tv_sec)*1000+(tiempo2.tv_usec-tiempo1.tv_usec)/1000));

    model modelo = calculateFULLModel(props, dataset, W);

    if(props.verbose==1) printf("Saving model in file: %s\n\n",data_model);	 
    FILE *Out = fopen(data_model, "wb");
    storeModel(&modelo, Out);
    fclose(Out);
    
    freeModel(modelo);
    freeDataset(dataset);
    free(W);
    return 0;
}

/**
 * @endcond
 */


