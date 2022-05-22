#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <unistd.h>

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#define n_cities 2000

using namespace std;

/*
Kompilowanie: g++ -fopenmp TSP_par.cpp
*/

void sequentialTSP(int** cityMatrix,int size, int** forceToGo){
    int actual = 0;
    int bestDistance = -1;
    int mustgo = -1;

    for(int start=0;start<n_cities;start++){//wybranie każdego miasta startowego
        int visitedCities[n_cities] = {0};
        int path[n_cities+1];

        visitedCities[start] = 1;
        actual = start;
        path[0] = actual;
        path[n_cities] = actual;

        int sum = 0;

        for(int city1=0;city1<n_cities-1;city1++){//wybranie 
            int min = cityMatrix[city1][city1];
            int best = actual;

            for(int i=0;i<size;i++){
                if(actual==forceToGo[i][0]){mustgo=forceToGo[i][1];}
            }
            for(int city2=0;city2<n_cities;city2++){
                if(mustgo>-1){
                    best=mustgo;
                    min=cityMatrix[actual][mustgo];
                    break;
                }
                if(visitedCities[city2] == 0 && min > cityMatrix[actual][city2]){
                    best = city2;
                    min = cityMatrix[actual][city2];
                }
            }
            sum = sum + min;
            visitedCities[best] = 1;
            actual = best;
            path[city1+1] = actual;
            mustgo = -1;
            }
       // for(int i=0;i<n_cities;i++){std::cout<<path[i]<<" ";}
       // std::cout<<"\n";
        sum = sum + cityMatrix[actual][start];
        if(bestDistance == -1 || bestDistance > sum){
            bestDistance = sum;
        }
    }
}

void paralellTSP(int** cityMatrix,int thread_num,int size, int** forceToGo){

    int bestDistance;
    int globalBestDistance;
    omp_set_dynamic(0);
    omp_set_num_threads(thread_num);
    #pragma omp parallel private(bestDistance) shared(globalBestDistance) 
    {
        bestDistance = -1;
        globalBestDistance = -1;
        int actual = 0;
        int mustgo = -1;

        #pragma omp for
        for(int start=0;start<n_cities;start++){//wybranie każdego miasta startowego
            int visitedCities[n_cities] = {0};
            int path[n_cities+1];

            visitedCities[start] = 1;
            actual = start;
            path[0] = actual;
            path[n_cities] = actual;

            int sum = 0;

            for(int city1=0;city1<n_cities-1;city1++){//wybranie 
                int min = cityMatrix[city1][city1];
                int best = actual;

                for(int i=0;i<size;i++){
                    if(actual==forceToGo[i][0]){mustgo=forceToGo[i][1];}
                }
                for(int city2=0;city2<n_cities;city2++){
                    if(mustgo>-1){
                        best=mustgo;
                        min=cityMatrix[actual][mustgo];
                        break;
                    }
                    if(visitedCities[city2] == 0 && min > cityMatrix[actual][city2]){
                        best = city2;
                        min = cityMatrix[actual][city2];
                    }
                }
                sum = sum + min;
                visitedCities[best] = 1;
                actual = best;
                path[city1+1] = actual;
                mustgo = -1;
            }
        
            sum = sum + cityMatrix[actual][start];
            if(bestDistance == -1 || bestDistance > sum){
                bestDistance = sum;
            }

        }
        #pragma omp critical
        if(globalBestDistance == -1 || globalBestDistance > bestDistance){
            globalBestDistance = bestDistance;
        }
    }
}

void sequentialTSPWithout(int** cityMatrix){
    int actual = 0;
    int bestDistance = -1;

    for(int start=0;start<n_cities;start++){//wybranie każdego miasta startowego
        int visitedCities[n_cities] = {0};
        int path[n_cities+1];

        visitedCities[start] = 1;
        actual = start;
        path[0] = actual;
        path[n_cities] = actual;

        int sum = 0;

        for(int city1=0;city1<n_cities-1;city1++){//wybranie 
            int min = cityMatrix[city1][city1];
            int best = actual;

            for(int city2=0;city2<n_cities;city2++){
                if(visitedCities[city2] == 0 && min > cityMatrix[actual][city2]){
                    best = city2;
                    min = cityMatrix[actual][city2];
                }
            }
            sum = sum + min;
            visitedCities[best] = 1;
            actual = best;
            path[city1+1] = actual;
            }
        sum = sum + cityMatrix[actual][start];
        if(bestDistance == -1 || bestDistance > sum){
            bestDistance = sum;
        }
    }
}

void paralellTSPWithout(int** cityMatrix,int numThread){

    int bestDistance;
    int globalBestDistance;
    omp_set_dynamic(0);
    omp_set_num_threads(numThread);
    #pragma omp parallel private(bestDistance) shared(globalBestDistance) 
    {
        bestDistance = -1;
        globalBestDistance = -1;

        #pragma omp for
        for(int start=0;start<n_cities;start++){//wybranie każdego miasta startowego
            
            int actual = start;
            int sum = 0;

            int visitedCities[n_cities] = {0};
            visitedCities[start] = 1;
            int path[n_cities+1];
            path[0] = actual;

            for(int city1=0;city1<n_cities-1;city1++){//wybranie 
                int mini = cityMatrix[actual][actual];
                int best = actual;

                for(int city2=0;city2<n_cities;city2++){
                    if(visitedCities[city2] == 0 && mini > cityMatrix[actual][city2]){
                        best = city2;
                        mini = cityMatrix[actual][city2];
                    }
                }
                sum = sum + mini;
                visitedCities[best] = 1;
                actual = best;
                path[city1+1] = actual;
            }
        
            sum = sum + cityMatrix[actual][start];
            if(bestDistance == -1 || bestDistance > sum){
                bestDistance = sum;
            }

        }
        #pragma omp critical
        if(globalBestDistance == -1 || globalBestDistance > bestDistance){
            globalBestDistance = bestDistance;
        }
    }
}

int main(int argc, char* argv[]){
    double startTime = 0.0;
    double endTime = 0.0;
    argc--;

    for(int a=0;a<1;a++){
        int** cityMatrix = (int**)malloc(n_cities*sizeof(int*));
        for(int i=0;i<n_cities;i++){
            cityMatrix[i] = (int*)malloc(n_cities*sizeof(int)); 
        }
        srand(time(0));

        for(int row=0;row<n_cities;row++){
            for(int city=0;city<n_cities;city++){
                if(row==city){
                    cityMatrix[row][city]=9999;
                }else if(city<row){
                    cityMatrix[row][city] = cityMatrix[city][row];                
                }else{
                    cityMatrix[row][city] = rand()%100+10;
                }
            }
        }

        if(argc>=2){
            if(argc%2!=0){argc--;}
            int size = argc/2;
            int** forceToGo = (int**)malloc(size*sizeof(int*));
                
            for(int i=0;i<size;i++){forceToGo[i] = (int*)malloc(2*sizeof(int));}
                
            int el = 1;
            for(int i=0;i<size;i++){
                forceToGo[i][0]=atoi(argv[el]);
                forceToGo[i][1]=atoi(argv[el+1]);
                el = el +2;  
            }
            startTime = clock();
            sequentialTSP(cityMatrix,size,forceToGo);
            endTime = clock();
           cout<<"Sequential: "<<((endTime-startTime)/CLOCKS_PER_SEC)<<" seconds\n"; //czas podawany w sekundach xDDD
                
            startTime = clock();
            paralellTSP(cityMatrix,2,size,forceToGo);
            endTime = clock();
            cout<<"2 Threads: "<<((endTime-startTime)/CLOCKS_PER_SEC)<<" seconds\n"; //czas podawany w sekundach xDDD

            startTime = clock();
            paralellTSP(cityMatrix,4,size,forceToGo);
            endTime = clock();
            cout<<"4 Threads: "<<((endTime-startTime)/CLOCKS_PER_SEC)<<" seconds\n"; //czas podawany w sekundach xDDD

            startTime = clock();
            paralellTSP(cityMatrix,6,size,forceToGo);
            endTime = clock();
            cout<<"6 Threads: "<<((endTime-startTime)/CLOCKS_PER_SEC)<<" seconds\n"; //czas podawany w sekundach xDDD

            startTime = clock();
            paralellTSP(cityMatrix,8,size,forceToGo);
            endTime = clock();
            cout<<"8 Threads: "<<((endTime-startTime)/CLOCKS_PER_SEC)<<" seconds\n"; //czas podawany w sekundach xDDD
        }else
        {                
            startTime = clock();
            sequentialTSPWithout(cityMatrix);
            endTime = clock();
            cout<<"Sequential: "<<((endTime-startTime)/CLOCKS_PER_SEC)<<" seconds\n"; //czas podawany w sekundach xDDD
            
            startTime = clock();
            paralellTSPWithout(cityMatrix,2);
            endTime = clock();
            cout<<"2 Threads: "<<((endTime-startTime)/CLOCKS_PER_SEC)<<" seconds\n"; //czas podawany w sekundach xDDD

            startTime = clock();
            paralellTSPWithout(cityMatrix,4);
            endTime = clock();
            cout<<"4 Threads: "<<((endTime-startTime)/CLOCKS_PER_SEC)<<" seconds\n"; //czas podawany w sekundach xDDD

            startTime = clock();
            paralellTSPWithout(cityMatrix,6);
            endTime = clock();
            cout<<"6 Threads: "<<((endTime-startTime)/CLOCKS_PER_SEC)<<" seconds\n"; //czas podawany w sekundach xDDD*/

            startTime = clock();
            paralellTSPWithout(cityMatrix,8);
            endTime = clock();
            cout<<"8 Threads: "<<((endTime-startTime)/CLOCKS_PER_SEC)<<" seconds\n"; //czas podawany w sekundach xDDD
        }
        for (int j = 0; j < n_cities; j++) {
            free(cityMatrix[j]);
        }
        free(cityMatrix); 
    }
}
