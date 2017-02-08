//TODO:
//Multithreading split-up
//Barrier synchronization


//To compile: g++ -O3 -w assignment1.cpp -lpthread -o assignment1
//To run: ./assignment1

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sys/time.h>

using namespace std;

//void findNeighbors(int z);
vector<int> dothething(int q);
int compare_vectors(vector<int> vect1, vector<int> vect2);


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


int main(int argc, char **argv)
{
	char *filepath;
	if(argc<2)
	{
	//	cout<<"using default file path"<<endl;
		filepath = "/home/mfevold/git-repos/projects/parallel/ass2/HcNetwork.txt";
	}
	else
	{
		filepath = argv[1];
	}
	cout<<"Filepath: "<<filepath<<endl;
    struct timeval start,end;
    gettimeofday(&start,NULL);
//    fstream myfile("/home/mfevold/git-repos/projects/parallel/ass2/toyGraph.txt",std::ios_base::in);
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


	double global_coef  = 0;
     //findNeighbors(0); //or whatever other number. Iterate as necessary.
    for(int r = 0; r < n; r++){
	std::vector<int> test = dothething(r);
	int actualEdges = 0;
	for(int i = 0;i<test.size();i++){
		int testvalue = test[i];
		std::vector<int> friends_Friends = dothething(testvalue);
		
		actualEdges +=  compare_vectors(test,friends_Friends);
		
	
	}
	//cout << "Node"<< r << ": " << actualEdges<<endl<<endl;
	if(actualEdges>0){
		double local_coef = actualEdges/(test.size()*(test.size()-1.0));
		global_coef += local_coef;
	}
//	cout << endl<< local_coef<<endl;
 	}

	cout <<endl<<endl<<(global_coef*(1.0/n));
	cout << endl << endl<<endl<<endl<<endl<<endl;
//	findNeighbors(1);
    //You can also make a list of neighbors for each node if you want.
   // printAdjMatrix(adjMatrix);
cout<< "Number of connections: " << count <<endl;

	
	gettimeofday(&end,NULL);
        //since the struct returns seconds and microseconds this converts the 
	//seconds into microseconds, and takes the difference between
	//end and start time to calculate total runtime.
	int second_to_microsecond = 1000000;
        int time = ((end.tv_sec * second_to_microsecond + end.tv_usec) - (start.tv_sec * second_to_microsecond + start.tv_usec));
        cout<< "Execution Time in Micro Seconds: " << time<<endl;

return 0;
}
//this is what we got to work, we now have vector that we need to implement our algorithm on.
std::vector<int> dothething(int originalNode)
{
	std::vector<int> friends; //This is the array that will hold the selected node's friends.   	

	//// Populating original node list for node originalNode (received as argument)

	for (int originalNodeNeighbor = 0; originalNodeNeighbor < nodes ; originalNodeNeighbor++)
	{
		if (adjMatrix[originalNode][originalNodeNeighbor] == 1)
		{
			friends.push_back(originalNodeNeighbor); //Put each of Node originalNode's friends into an array. We now have an array of friends.
		}
	}
	//cout << originalNode << "'s Friends: " ;	
	for (int neighborCheck = 0; neighborCheck < friends.size() ; neighborCheck++)
	{
		int friendName = friends[neighborCheck];
	//	cout << friendName << " ";  // Confirm contents of friends vector
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
	cout<<endl<<neighbor_count<<endl;
        return neighbor_count;
}

