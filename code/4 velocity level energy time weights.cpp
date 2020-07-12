// 4 Velocity Levels Graph

#define _CRT_SECURE_NO_WARNINGS
#include <bits/stdc++.h>
using namespace std;
#define DEBUG(x) cout << '>' << #x << ':' << x << endl;
#define FOR0(i,n) for(int i=0, _##i=(n); i<_##i; ++i)
#define FOR(i,l,r) for(int i=(l), _##i=(r); i<_##i; ++i)
#define FORD(i,l,r) for(int i=(r), _##i=(l); --i>=_##i; )
#define repi(i,a) for(__typeof((a).begin()) i=(a).begin(), _##i=(a).end(); i!=_##i; ++i)
#define dwni(i,a) for(__typeof((a).rbegin()) i=(a).rbegin(), _##i=(a).rend(); i!=_##i; ++i)
#define SZ(a) ((int)((a).size()))
#define pb push_back
#define mp make_pair
#define INF ((int)1e9)
#define SYNC std::ios::sync_with_stdio(false);  cin.tie(NULL);
#define ff first
#define ss second
#define all(a) (a).begin(),(a).end()
typedef long long ll;
typedef unsigned long long ull;
typedef pair<int, int> ii;
typedef vector<ii> vii;
typedef vector<int> vi;
typedef vector<vi> vvi;

vector<double> energyMultiplier; //stores the energy multiplier for different velocity levels (to calculate energy consumption)
vector<double> timeMultiplier; //stores the time multiplier for different velocity levels (to calculate time consumption)
vector<vector<pair<int,double> > > G; //the Graph
set<pair<double,double> > S; //parameter used in the main algorithm
vector<pair<pair<double,double>,vii> > ANS; //stores the answers
int source,destination;
double globalEnergyMax,globalTimeMax; //to normalise the edge weights
double lambda; //parameter used in the main algorithm
double hyperParameterLambda,hyperParameterDistance; //paramtere used in the main algorithm

double getNewEdgeWeight(double energy,double time) //converts energy and time weights into a single weight
{
	return ((lambda*lambda*energy)/globalEnergyMax  + (1.0 - lambda*lambda)*time/globalTimeMax);
}

pair<pair<double,double>,vii> BFalgorithm() //Bellman Ford Algorithm
{
	bool relax = false; //for early termination step
	int n = G.size();
	vector<double> dist; //stores the distance
	vector<pair<ii,pair<double,double> > > par(n,mp(mp(-1,-1),mp(0.0,0.0))); //parent vector to reconstruct path
	for(int i = 0 ; i < n ; i++)
		dist.pb(DBL_MAX);
	dist[source] = 0.0;
	for(int i = 0 ; i < n-1 ; i++)
	{
		relax = false;
		for(int j = 0 ; j < n ; j++)
		{
			for(int k = 0 ; k < G[j].size() ; k++)
			{
				int u = j;
				int v = G[j][k].ff;
				double d = G[j][k].ss;
				FOR0(l,4) //for the 4 velocity levels
				{
					double e = d*energyMultiplier[l];
					double t = d*timeMultiplier[l];
					if(dist[u] != DBL_MAX && dist[v] > dist[u] + getNewEdgeWeight(e,t))
					{
						relax = true;
						dist[v] = dist[u] + getNewEdgeWeight(e,t);
						par[v] = mp(mp(u,(l+1)*20),mp(e,t));
					}
				}
			}
		}
		if(!relax)
			break;
	}
	//constructing the path
	double energy = 0.0,time = 0.0;
	vii path;
	int prev = destination;
	int temp = destination;
	path.pb(par[destination].ff);
	while(temp != -1)
	{
		//cout << temp << " " << prev << endl;
		energy += par[temp].ss.ff;
		time += par[temp].ss.ss;
		temp = par[temp].ff.ff;
		if(temp == source)
			break;
		path.pb(par[temp].ff);
	}
	reverse(all(path));
	return mp(mp(energy,time),path);
}

void solveRecursison(double z1l,double z2l,double z1r,double z2r,double lambdaleft,double lambdaright)
{
	//calculating mid lambda value
	lambda = (lambdaleft+lambdaright)/2.0;
	
	//solving for this new lambda
	pair<pair<double,double>,vii> p = BFalgorithm();
	double z1 = p.ff.ff;
	double z2 = p.ff.ss;

	//if the new solution is already not found
	if(S.find(p.ff) == S.end())
	{
		//insert new solution in S
		S.insert(p.ff);
		ANS.pb(p);

		//checking condition for termination
		if(abs(lambda - lambdaright) >= hyperParameterLambda && abs(getNewEdgeWeight(z1,z2) - getNewEdgeWeight(z1r,z2r)) >= hyperParameterDistance )
			solveRecursison(z1,z2,z1r,z2r,lambda,lambdaright);

		if(abs(lambda - lambdaleft) >= hyperParameterLambda && abs(getNewEdgeWeight(z1,z2) - getNewEdgeWeight(z1l,z2l)) >= hyperParameterDistance )
			solveRecursison(z1l,z2r,z1,z2,lambdaleft,lambda);
	}
	else //if this solution is already in the set
	{
		if(p.ff == mp(z1l,z2l)) //call the other half of recursion
		{
			if(abs(lambda-lambdaright) >= hyperParameterLambda)
				solveRecursison(z1,z2,z1r,z2r,lambda,lambdaright);
		}
		else
		{
			if(abs(lambda-lambdaleft) >= hyperParameterLambda)
				solveRecursison(z1l,z2l,z1,z2,lambdaleft,lambda);
		}
	}
}

void DichotomyAlgorithm() //Main algorithm
{
	//Initialise dichotomic search
	S.clear();
	
	//Compute Solution for extreme points
	lambda = 0.0;
	pair<pair<double,double>,vii> p1 = BFalgorithm();
	S.insert(p1.ff);
	ANS.pb(p1);
	lambda = 1.0;
	pair<pair<double,double>,vii> p2 = BFalgorithm();
	S.insert(p2.ff);
	ANS.pb(p2);

	//Recursively Compute other solutions
	solveRecursison(p2.ff.ff,p2.ff.ss,p1.ff.ff,p1.ff.ss,1.0,0.0);
} 

int main()
{
    SYNC
	// energy and time multipliers
    energyMultiplier.pb(7.0); energyMultiplier.pb(4.0); energyMultiplier.pb(6.0); energyMultiplier.pb(8.0);
    timeMultiplier.pb(1.0/20.0); timeMultiplier.pb(1.0/40.0); timeMultiplier.pb(1.0/60.0); timeMultiplier.pb(1.0/80.0);
    //graph input
	int n,m;
    cout << "Enter the number of vertices " << endl;
    cin >> n;
    cout << "Enter the number of edges " << endl;
    cin >> m;
    int u,v;
    double d;
    G.resize(n,vector<pair<int,double> >());
    S.clear();
    ANS.clear();
    globalTimeMax = 0.0;
    globalEnergyMax = 0.0;
    FOR0(i,m)
    {
    	cin >> u >> v >> d;
    	globalEnergyMax = max(globalEnergyMax,d*8.0);
    	globalTimeMax = max(globalTimeMax,d/20.0);
    	G[u].pb(mp(v,d));
    	G[v].pb(mp(u,d));
    }
    cout << "Enter source and destination " << endl;
    cin >> source >> destination;
    hyperParameterLambda = 0.001;
    hyperParameterDistance = 0.0;
    DichotomyAlgorithm();
    //printing answers
	FOR0(i,SZ(ANS))
    {
    	cout << "Energy Consumption - " << ANS[i].ff.ff << " Time Consumption - " << ANS[i].ff.ss << endl;
    	FOR0(j,SZ(ANS[i].ss)-1)
    		cout << "From " << ANS[i].ss[j].ff << " go to " << ANS[i].ss[j+1].ff << " with speed " << ANS[i].ss[j].ss << endl;
		cout << "From " << ANS[i].ss[SZ(ANS[i].ss)-1].ff << " go to " << destination << " with speed " << ANS[i].ss[SZ(ANS[i].ss)-1].ss << endl; 
    }
}	