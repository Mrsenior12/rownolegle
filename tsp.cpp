#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>
//#include <unistd.h>

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#define n_cities 300

using namespace std;

void sequentialTSP(int** cityMatrix,int size, int** forceToGo){
    int actual = 0;
    int bestDistance = -1;
    int mustgo = -1;

    for(int start=0;start<3;start++){//wybranie każdego miasta startowego
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

        #pragma opm for
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

int main(int argc, char* argv[]){
    float avgTime = 0.0;
    double avg_par2 =0.0;
    double avg_par4 =0.0;
    double avg_par6 =0.0;
    double avg_par8 =0.0;
    double stime;
    clock_t t;
    argc--;
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

        for(int a=0;a<10;a++){
            int** cityMatrix = (int**)malloc(n_cities*sizeof(int*));
            for(int i=0;i<n_cities;i++){
                cityMatrix[i] = (int*)malloc(n_cities*sizeof(int)); 
            }

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
            srand(time(0));
            t = clock();
            sequentialTSP(cityMatrix,size,forceToGo);
            t = clock() - t;
            avgTime = avgTime + ((float)t)/CLOCKS_PER_SEC; //czas podawany w sekundach xDDD
            
            stime = omp_get_wtime();
            paralellTSP(cityMatrix,2,size,forceToGo);
            avg_par2 = avg_par2 + (omp_get_wtime() - stime);

            stime = omp_get_wtime();
            paralellTSP(cityMatrix,4,size,forceToGo);
            avg_par4 = avg_par4 + (omp_get_wtime() - stime);

            stime = omp_get_wtime();
            paralellTSP(cityMatrix,6,size,forceToGo);
            avg_par6 = avg_par6 + (omp_get_wtime() - stime);

            avg_par8 = 0.0;
            stime = omp_get_wtime();
            paralellTSP(cityMatrix,8,size,forceToGo);
            avg_par8 = avg_par8 + (omp_get_wtime() - stime);
        
            
            //cout<<avgTime<< " ";
            for(int i=0;i<n_cities;i++){free(cityMatrix[i]);}
            free(cityMatrix);
        }
        cout<<avgTime/10<<" ";
        cout<<avg_par2/10<<" ";
        cout<<avg_par4/10<<" ";
        cout<<avg_par6/10<<" ";
        cout<<avg_par8/10<<" ";
    }
}