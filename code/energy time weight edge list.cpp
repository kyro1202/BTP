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

vvi G;
map<ii,pair<double,double> > E;
set<pair<double,double> > S;
vector<pair<pair<double,double>,vi> > ANS;
int source,destination;
double globalEnergyMax,globalTimeMax;
double lambda;
double hyperParameterLambda,hyperParameterDistance;

double getNewEdgeWeight(double energy,double time)
{
	return ((lambda*lambda*energy)/globalEnergyMax  + (1.0 - lambda*lambda)*time/globalTimeMax);
}

pair<pair<double,double>,vi> BFalgorithm()
{
	bool relax = false;
	int n = G.size();
	vector<double> dist;
	vector<int> par(n,-1);
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
				int v = G[j][k];
				double t = E[mp(u,v)].ss;
				double e = E[mp(u,v)].ff;
				if(dist[u] != DBL_MAX && dist[v] > dist[u] + getNewEdgeWeight(e,t))
				{
					relax = true;
					dist[v] = dist[u] + getNewEdgeWeight(e,t);
					par[v] = u;
				}
			}
		}
		if(!relax)
			break;
	}
	double energy = 0.0,time = 0.0;
	vector<int> path;
	int prev = destination;
	int temp = destination;
	path.pb(destination);
	temp = par[temp];
	while(temp != -1)
	{
		//cout << temp << " " << prev << endl;
		energy += E[mp(temp,prev)].ff;
		time += E[mp(temp,prev)].ss;
		path.pb(temp);
		prev = temp;
		temp = par[temp];
	}
	reverse(all(path));
	return mp(mp(energy,time),path);
}

void solveRecursison(double z1l,double z2l,double z1r,double z2r,double lambdaleft,double lambdaright)
{
	lambda = (lambdaleft+lambdaright)/2.0;
	
	pair<pair<double,double>,vi> p = BFalgorithm();
	double z1 = p.ff.ff;
	double z2 = p.ff.ss;

	if(S.find(p.ff) == S.end())
	{
		S.insert(p.ff);
		ANS.pb(p);

		if(abs(lambda - lambdaright) >= hyperParameterLambda && abs(getNewEdgeWeight(z1,z2) - getNewEdgeWeight(z1r,z2r)) >= hyperParameterDistance )
			solveRecursison(z1,z2,z1r,z2r,lambda,lambdaright);

		if(abs(lambda - lambdaleft) >= hyperParameterLambda && abs(getNewEdgeWeight(z1,z2) - getNewEdgeWeight(z1l,z2l)) >= hyperParameterDistance )
			solveRecursison(z1l,z2r,z1,z2,lambdaleft,lambda);
	}
	else
	{
		if(p.ff == mp(z1l,z2l))
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

void DichotomyAlgorithm()
{
	//Initialise dichotomic search
	S.clear();
	
	//Compute Solution for extreme points
	lambda = 0.0;
	pair<pair<double,double>,vi> p1 = BFalgorithm();
	S.insert(p1.ff);
	ANS.pb(p1);
	lambda = 1.0;
	pair<pair<double,double>,vi> p2 = BFalgorithm();
	S.insert(p2.ff);
	ANS.pb(p2);

	//Recursively Compute other solutions
	solveRecursison(p2.ff.ff,p2.ff.ss,p1.ff.ff,p1.ff.ss,1.0,0.0);
} 

int main()
{
    SYNC
    int n,m;
    cout << "Enter the number of vertices " << endl;
    cin >> n;
    cout << "Enter the number of edges " << endl;
    cin >> m;
    int u,v;
    double e,t;
    G.resize(n,vi());
    S.clear();
    ANS.clear();
    E.clear();
    globalTimeMax = 0.0;
    globalEnergyMax = 0.0;
    FOR0(i,m)
    {
    	cin >> u >> v >> e >> t;
    	globalEnergyMax = max(globalEnergyMax,e);
    	globalTimeMax = max(globalTimeMax,t);
    	G[u].pb(v);
    	G[v].pb(u);
    	E[mp(u,v)] = mp(e,t);
    	E[mp(v,u)] = mp(e,t);
    }
    cout << "Enter source and destination " << endl;
    cin >> source >> destination;
    hyperParameterLambda = 0.01;
    hyperParameterDistance = 0.0;
    DichotomyAlgorithm();
    FOR0(i,SZ(ANS))
    {
    	cout << "--------" << ANS[i].ff.ff << "---------" << ANS[i].ff.ss << endl;
    	FOR0(j,SZ(ANS[i].ss))
    		cout << ANS[i].ss[j] << " ";
		cout << endl;
    }
}	