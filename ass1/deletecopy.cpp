//Author: Matthew Fevold
//Assignment 1: done the right way this time.

//To compile: g++ -O1 -w assignment1.cpp -lpthread -o assignment1
//To run: ./assignment1

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <cmath>

using namespace std;


//function declarations
std::vector<int> get_friends(int q);
int compare_vectors(vector<int> vect1, vector<int> vect2);
void *thread_action(void *i);
int get_index(int a);

//various variable declarations
//the mutex
pthread_mutex_t lock;
//initialize to 0
double global_coef = 0;


typedef vector<vector<int> > AdjacencyMatrix;
AdjacencyMatrix adjMatrix;
int count = 0;
int localEdges = 0;
int nodes = 0;
int thread_number = 0;

void printAdjMatrix(AdjacencyMatrix adjMatrix)
{
    for (int i=0; i<adjMatrix.size(); i++)
    {
        for (int j=0; j<adjMatrix[i].size(); j++) cout<<adjMatrix[i][j]<<" ";
        cout<<endl;
    }

}


int main(int argc, char **argv)
{
	struct timeval start,end;
	gettimeofday(&start,NULL);
	//int thread_number;   
	char *filepath;
        if(argc<3)
        {
		cout<< "using 1 thread by default"<<endl;
                cout<<"using default file path: (reminder: you can use 1 argument to specify filepath)"<<endl;
                filepath = "/home/mfevold/git-repos/projects/parallel/ass2/toyGraph.txt";
		thread_number = 1;
        }
        else
        {
		thread_number = atoi(argv[2]);
                filepath = argv[1];
        }
	cout<<" ::: " <<thread_number<<endl;
	fstream myfile(filepath,std::ios_base::in);
    	int u,v;
    	int maxNode = 0;
    	vector<pair<int,int> > allEdges;
    	while(myfile >> u >> v)
    	{
        	allEdges.push_back(make_pair(u,v));
        	if(u > maxNode)
        		maxNode = u;

        	if(v > maxNode)
        		maxNode = v;                 
    	}

    	int n = maxNode +1;  //Since nodes starts with 0
    	nodes = maxNode +1;  //update the global variable 
			     
    
    	adjMatrix = AdjacencyMatrix(n,vector<int>(n,0));
    	//populate the matrix
    	for(int i =0; i<allEdges.size() ; i++){
       		u = allEdges[i].first;
       		v = allEdges[i].second;
       		adjMatrix[u][v] = 1;
       		adjMatrix[v][u] = 1;
       		count++;
    	}

	//I was having issues just passing the index from the thread as the int value but
	//I was getting race condition issues... this proves to be an easy fix to that 
	//without being computationally complex. 
	int temp_array[thread_number];
	for(int j = 0; j < thread_number; j++){
		temp_array[j] = j;
	} 

	//create thread id's for each thread to be created. in this case for each node
	pthread_t thread[thread_number];

	for(int r = 0; r < thread_number; r++){

		int i = r;
		//create the thread... passing he index of the thread
		int rc = pthread_create(&thread[i],NULL,thread_action,(void *)&temp_array[i]);
		
		if(rc){
			cout <<"thread: not created: "<<i<<endl;
		}
		else{
			cout <<"thread Created: "<<i<<endl;
		} 
	}
	
	//join all threads before final calculation
	for(int i = 0; i < thread_number; i++)
	{
		pthread_join(thread[i],NULL);
	}	
	
	//output.
	cout <<endl<<"Clustering Coeff for the graph: " <<(global_coef*(1.0/n)) <<endl<<endl;
	
	//for output only
    	//printAdjMatrix(adjMatrix);
	cout<< "Number of connections: " << count <<endl;
	gettimeofday(&end,NULL);
	int second_to_microsecond = 1000000;
	int time = ((end.tv_sec * second_to_microsecond + end.tv_usec) - (start.tv_sec * second_to_microsecond + start.tv_usec));
	cout<< "Execution Time in Micro Seconds: " << time<<endl;

	return 0;
}

//returns starting point for threads in array
int get_index(int a )
{
	//cout<<endl<<(1.0 * nodes)/( thread_number)<<endl;
	int index = a * ceil((1.0 * nodes)/thread_number);	
	// ceil(a);
	
	//cout<<index<<endl;


	return index;
}

//returns vector containing friends to originalNode
std::vector<int> get_friends(int originalNode)
{
	//This is the vector that will hold the selected node's friends. 
	std::vector<int> friends;   	
	
	for (int originalNodeNeighbor = 0; originalNodeNeighbor < nodes ; originalNodeNeighbor++)
	{
		//if a friend, add to vector
		if (adjMatrix[originalNode][originalNodeNeighbor] == 1)
		{
			friends.push_back(originalNodeNeighbor);
		}
	}	

	return friends;
}

//takes two vectors by value (which might want to be changed) and sees if they share
//any values. returns the number of shared edges. no other info like which friend it was...
int compare_vectors(vector<int> vect1, vector<int> vect2){

       int neighbor_count = 0;

        for(int i = 0; i < vect1.size(); i++){
                for(int j = 0; j < vect2.size();j++){
                        if(vect1[i]==vect2[j]){
                                neighbor_count++;
                       }
               }
        }

        return neighbor_count;
}

//this calls get_friends and compare_vectors to determine the local clustering coefficient 
//for a node. 
void *thread_action(void *i)
{
	
	
	//properly dereference i into an int
	int m = *(int *) i;
	
	//get starting point from index m
	int starting_point = get_index(m);
	int stopping_point = get_index(m+1);
	//cout<<endl<<"thread "<<m<<" starting point "<<starting_point<<" stopping point "<<stopping_point<<endl;
	for(int j = starting_point; j < stopping_point; j++)
	{	
		//allows me to accept any number of threads
		//using ceiling function.
		if(j >= nodes)
		{
			pthread_exit(NULL);
		}
		//cout<<"thread " <<m<< " Doing "<<j<<endl;	
		//this vector will hold all originals neighbors aka friends
		std::vector<int> originals_Friends = get_friends(j);

		//actual edges shared between friends	
		int actualEdges = 0;

		for(int k = 0; k < originals_Friends.size() ;k++){
		
			int current_node = originals_Friends.at(k);
			//this will hold the current nodes friends so we can see if it matches
			//and of the original nodes friends
			std::vector<int> friends_Friends = get_friends(current_node);

			//this will get 2*actual edges since we end up counting each edge twice
			//which is convinient in this case. though would have to /2 in a different one
			actualEdges +=  compare_vectors(originals_Friends,friends_Friends);
		}

		//normally would have to do 2 * actualEdges/(test.size()*(test.size()-1.0))
		//but since we count each edge twice this is unnecessary
		//also 1.0 since 1 was causing issues with int to doubles...
        	
		//if there are any edges to add to global.
		if(actualEdges>0){
			double local_coef = actualEdges/(originals_Friends.size()*(originals_Friends.size()-1.0)); 
	
			//lock mutex
			pthread_mutex_lock(&lock);
			//modify global variable
			global_coef += local_coef;
			//free mutex
			pthread_mutex_unlock(&lock);
		}
	}

	pthread_exit(NULL);

}
