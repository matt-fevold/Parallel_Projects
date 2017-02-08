//TODO:
//Multithreading split-up
//Barrier synchronization


//To compile: g++ -O3 -w assignment1.cpp -lpthread -o assignment1
//To run: ./assignment1

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <stdlib.h>


using namespace std;
pthread_mutex_t lock;
pthread_mutex_t seg_fault;
int counter;
double global_coef  = 0;

//void findNeighbors(int z);
vector<int> dothething(int q);
int compare_vectors(vector<int> vect1, vector<int> vect2);
void *thread_action(void *i);

typedef vector<vector<int> > AdjacencyMatrix;
AdjacencyMatrix adjMatrix;
int count = 0;
int localEdges = 0;
int nodes = 0;

void printAdjMatrix(AdjacencyMatrix adjMatrix)
{
    for (int i=0; i<adjMatrix.size(); i++)
    {
        for (int j=0; j<adjMatrix[i].size(); j++) cout<<adjMatrix[i][j]<<" ";
        cout<<endl;
    }

}


int main()
{
    
    fstream myfile("/home/mfevold/git-repos/projects/parallel/ass2/toyGraph1.txt",std::ios_base::in);
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
    nodes = maxNode +1;
    //cout<<"Graph has "<< n <<" nodes"<<endl;

    adjMatrix = AdjacencyMatrix(n,vector<int>(n,0));
    //populate the matrix
    for(int i =0; i<allEdges.size() ; i++){
       u = allEdges[i].first;
       v = allEdges[i].second;
       adjMatrix[u][v] = 1;
       adjMatrix[v][u] = 1;
       count++;
    } 


	//double global_coef  = 0;
     //findNeighbors(0); //or whatever other number. Iterate as necessary.
	pthread_t thread[0];
	for(int r = 0; r < n; r++){
		//std::vector<int> test = dothething(r);
		//int actualEdges = 0;
		int i = r;
		//cout<<endl<<"^^^"<<i<<endl;
//this is where I should make multiple threads
		int rc = pthread_create(&thread[i],NULL,thread_action,(void *)&i);
		if(rc){
			cout <<"thread: not created: "<<i<<endl;
		}
		else{
			//cout <<"thread Created: "<<i<<endl;
		}
		/*
		for(int i = 0;i<test.size();i++){
			int testvalue = test[i];
			std::vector<int> friends_Friends = dothething(testvalue);
		
			actualEdges +=  compare_vectors(test,friends_Friends);
		
	
		}
		*/
	//	cout << "Node"<< r << ": " << actualEdges<<endl<<endl;
		//double local_coef = actualEdges/(test.size()*(test.size()-1.0));
		//global_coef += local_coef;
	//	cout << endl<< local_coef<<endl;
 	}
	for(int i = 0; i < n; i++)
	{
	//	cout << "&&&&&&&&&";
		pthread_join(thread[i],NULL);
	}
	//cout <<endl<<endl<<"counter: "<<counter<<endl;	

	cout <<endl<<endl<<"Clustering Coefficient for the graph: " <<(global_coef*(1.0/n)) <<endl<<endl;
//	cout << endl << endl<<endl<<endl<<endl<<endl;
//	findNeighbors(1);
    //You can also make a list of neighbors for each node if you want.
    printAdjMatrix(adjMatrix);
cout<< "Number of connections: " << count <<endl;

return 0;
}
//this is what we got to work, we now have vector that we need to implement our algorithm on.
std::vector<int> dothething(int originalNode)
{
	//This is the vector that will hold the selec    ted node's friends. 
	std::vector<int> friends;   	
	cout<<"*****";
	//int temp = originalNode;
	for (int originalNodeNeighbor = 0; originalNodeNeighbor < nodes ; originalNodeNeighbor++)
	{
		//cout<<adjMatrix[originalNode][originalNodeNeighbor];
		if (adjMatrix[originalNode][originalNodeNeighbor] == 1)
		{
		//	cout << originalNodeNeighbor<<"))))))))"<<endl;
			friends.push_back(originalNodeNeighbor);
		}
	}	
	return friends;
}

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

void *thread_action(void *i)
{
	int m = *(int *) i;
	//printAdjMatrix(adjMatrix);
	cout<<"M:  "<<m<<endl;
	std::vector<int> test = dothething(m);

	int actualEdges = 0;
	for(int k = 0; test.size() ;k++){
		int testvalue = test[k];
		std::vector<int> friends_Friends = dothething(testvalue);
		//actualEdges +=1;
		actualEdges +=  compare_vectors(test,friends_Friends);
	}

	//double local_coef = actualEdges/(test.size()*(test.size()-1.0));
        
	pthread_mutex_lock(&lock);
	//global_coef += local_coef;
	global_coef += actualEdges;
	pthread_mutex_unlock(&lock);
	//int m = *((int *) i);
	//pthread_mutex_lock(&lock);
	//counter +=1;
	//cout <<endl<< "thread: "<<m<<" reporting for duty"<<endl;
	//pthread_mutex_unlock(&lock);
	pthread_exit(NULL);

}
