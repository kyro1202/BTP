// C++ program to print all paths from a source to destination. 
#include<iostream> 
#include<bits/stdc++.h>
#include <list> 
using namespace std; 

map<pair<int,int>,pair<double,double> > E;
vector<double> energyMultiplier;
vector<double> timeMultiplier;

// A directed graph using adjacency list representation 
class Graph 
{ 
	int V; // No. of vertices in graph 
	list<int> *adj; // Pointer to an array containing adjacency lists 

	// A recursive function used by printAllPaths() 
	void printAllPathsUtil(int , int , bool [], int [], int &); 

public: 
	Graph(int V); // Constructor 
	void addEdge(int u, int v); 
	void printAllPaths(int s, int d); 
}; 

Graph::Graph(int V) 
{ 
	this->V = V; 
	adj = new list<int>[V]; 
} 

void Graph::addEdge(int u, int v) 
{ 
	adj[u].push_back(v); // Add v to u’s list. 
} 

// Prints all paths from 's' to 'd' 
void Graph::printAllPaths(int s, int d) 
{ 
	// Mark all the vertices as not visited 
	bool *visited = new bool[V]; 

	// Create an array to store paths 
	int *path = new int[V]; 
	int path_index = 0; // Initialize path[] as empty 

	// Initialize all vertices as not visited 
	for (int i = 0; i < V; i++) 
		visited[i] = false; 

	// Call the recursive helper function to print all paths 
	printAllPathsUtil(s, d, visited, path, path_index); 
} 

// A recursive function to print all paths from 'u' to 'd'. 
// visited[] keeps track of vertices in current path. 
// path[] stores actual vertices and path_index is current 
// index in path[] 
void Graph::printAllPathsUtil(int u, int d, bool visited[], 
							int path[], int &path_index) 
{ 
	// Mark the current node and store it in path[] 
	visited[u] = true; 
	path[path_index] = u; 
	path_index++; 

	// If current vertex is same as destination, then print 
	// current path[] 
	if (u == d) 
	{ 
		double energy = 0.0, time = 0.0;
		for (int i = 0; i<path_index; i++) 
		{
			if(i != path_index-1)
			{
				energy += E[make_pair(path[i],path[i+1])].first;
				time += E[make_pair(path[i],path[i+1])].second;
			}
			// cout << path[i] << " "; 
		}
		cout << energy << "," << time << endl; 
	} 
	else // If current vertex is not destination 
	{ 
		// Recur for all the vertices adjacent to current vertex 
		list<int>::iterator i; 
		for (i = adj[u].begin(); i != adj[u].end(); ++i) 
			if (!visited[*i]) 
				printAllPathsUtil(*i, d, visited, path, path_index); 
	} 

	// Remove current vertex from path[] and mark it as unvisited 
	path_index--; 
	visited[u] = false; 
} 

// Driver program 
int main() 
{
	energyMultiplier.push_back(7.0); energyMultiplier.push_back(4.0); energyMultiplier.push_back(6.0); energyMultiplier.push_back(8.0);
    timeMultiplier.push_back(1.0/20.0); timeMultiplier.push_back(1.0/40.0); timeMultiplier.push_back(1.0/60.0); timeMultiplier.push_back(1.0/80.0); 
	// Create a graph given in the above diagram 
	int n;
	int m;
	//cout << "Enter number of vertices " << endl;
	cin >> n;
	//cout << "Enter the number of edges " << endl;
	cin >> m;
	Graph g(n+4*m);
	E.clear();
	int u,v,temp = n;
    double dis;
	for(int i = 0 ; i < m ; i++)
	{
		cin >> u >> v >> dis;
		for(int l = 0 ; l < 4 ; l++)
		{
			double e = dis*energyMultiplier[l];
			double t = dis*timeMultiplier[l];
			g.addEdge(u,temp);
			g.addEdge(temp,u);
			g.addEdge(temp,v);
			g.addEdge(v,temp);
			E[make_pair(u,temp)] = make_pair(e,t);
    		E[make_pair(temp,u)] = make_pair(e,t);
    		E[make_pair(temp,v)] = make_pair(0.0,0.0);
    		E[make_pair(v,temp)] = make_pair(0.0,0.0);
    		temp++;
		}
	} 

	int s = 0, d = 4; 
	//cout << "Following are all different paths from " << s 
		//<< " to " << d << endl; 
	g.printAllPaths(s, d); 

	return 0; 
} 
