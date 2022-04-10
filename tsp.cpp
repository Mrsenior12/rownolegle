#include <iostream>
#include <cstdlib>
#include <ctime>
#include <omp.h>
//#include <unistd.h>

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#define n_cities 120000

using namespace std;

void sequentialTSP(int** cityMatrix, int actual){

    int visitedCities[n_cities] = {0};
    int path[n_cities+1];

    
    visitedCities[0] = 1;

    path[0] = actual;
    path[n_cities] = actual;

    int sum = 0;

    for(int city1=0;city1<n_cities-1;city1++){
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
    sum = sum + cityMatrix[actual][0];
   // cout<<"Best Naive Distance: "<< sum<<"\n";
}

void parallellTSP(int** cityMatrix,int thread_num,int actual){

    omp_set_dynamic(0);
    omp_set_num_threads(thread_num);
    #pragma parallel for
    {
    int visitedCities[n_cities] = {0};
    int path[n_cities+1];

    
    visitedCities[0] = 1;

    int actual = rand()%n_cities;
    path[0] = actual;
    path[n_cities] = actual;

    int sum = 0;

     
    for(int city1=0;city1<n_cities-1;city1++){
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
    sum = sum + cityMatrix[actual][0];
    }
   // cout<<"Best Naive Distance: "<< sum<<"\n";
}


int main(){
    float avgTime = 0.0;
    double avg_par2 =0.0;
    double avg_par4 =0.0;
    double avg_par6 =0.0;
    double avg_par8 =0.0;
    double stime;
    clock_t t;

   // for(int a=0;a<1;a++){
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
        int actual = rand()%n_cities;
        srand(time(0));
        t = clock();
        sequentialTSP(cityMatrix,actual);
        t = clock() - t;
        avgTime = avgTime + ((float)t)/CLOCKS_PER_SEC; //czas podawany w sekundach xDDD
        
        stime = omp_get_wtime();
        parallellTSP(cityMatrix,2,actual);
        avg_par2 = avg_par2 + (omp_get_wtime() - stime);

        stime = omp_get_wtime();
        parallellTSP(cityMatrix,4,actual);
        avg_par4 = avg_par4 + (omp_get_wtime() - stime);

        stime = omp_get_wtime();
        parallellTSP(cityMatrix,6,actual);
        avg_par6 = avg_par6 + (omp_get_wtime() - stime);

        avg_par8 = 0.0;
        stime = omp_get_wtime();
        parallellTSP(cityMatrix,8,actual);
        avg_par8 = avg_par8 + (omp_get_wtime() - stime);
    
        
        //cout<<avgTime<< " ";
        for(int i=0;i<n_cities;i++){free(cityMatrix[i]);}
        free(cityMatrix);
   // }
    cout<<avgTime/1<<" ";
    cout<<avg_par2/1<<" ";
    cout<<avg_par4/1<<" ";
    cout<<avg_par6/1<<" ";
    cout<<avg_par8/1<<" ";

}