// Congestion with 4 Level velocity 

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

vector<double> energyMultiplier;
vector<double> timeMultiplier;
vector<vector<pair<int,pair<double,double> > > > G;
set<pair<double,double> > S;
vector<pair<pair<double,double>,vii> > ANS;
int source,destination;
double globalEnergyMax,globalTimeMax;
long double lambda;
long double hyperParameterLambda,hyperParameterDistance;

double getNewEdgeWeight(double energy,double time)
{
	return ((lambda*lambda*energy)/globalEnergyMax  + (1.0 - lambda*lambda)*time/globalTimeMax);
}

pair<pair<double,double>,vii> BFalgorithm()
{
	bool relax = false;
	int n = G.size();
	vector<double> dist;
	vector<pair<ii,pair<double,double> > > par(n,mp(mp(-1,-1),mp(0.0,0.0)));
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
				double d = G[j][k].ss.ff;
				double c = G[j][k].ss.ss;
				FOR0(l,4)
				{
					if((l+1)*20.0 > c)
						break;
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

void solveRecursison(double z1l,double z2l,double z1r,double z2r,long double lambdaleft,long double lambdaright)
{
	lambda = (lambdaleft+lambdaright)/2.0;
	
	pair<pair<double,double>,vii> p = BFalgorithm();
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
	pair<pair<double,double>,vii> p1 = BFalgorithm();
	S.insert(p1.ff);
	ANS.pb(p1);
	lambda = 1.0;
	pair<pair<double,double>,vii> p2 = BFalgorithm();
	S.insert(p2.ff);
	ANS.pb(p2);

	//Recursively Compute other solutions
	solveRecursison(p2.ff.ff,p2.ff.ss,p1.ff.ff,p1.ff.ss,(long double)1.0,(long double)0.0);
} 

int main()
{
    SYNC
    energyMultiplier.pb(7.0); energyMultiplier.pb(4.0); energyMultiplier.pb(6.0); energyMultiplier.pb(8.0);
    timeMultiplier.pb(1.0/20.0); timeMultiplier.pb(1.0/40.0); timeMultiplier.pb(1.0/60.0); timeMultiplier.pb(1.0/80.0);
    int n,m;
    cout << "Enter the number of vertices " << endl;
    cin >> n;
    cout << "Enter the number of edges " << endl;
    cin >> m;
    int u,v;
    double d,c;
    G.resize(n,vector<pair<int,pair<double,double> > >());
    S.clear();
    ANS.clear();
    globalTimeMax = 0.0;
    globalEnergyMax = 0.0;
    FOR0(i,m)
    {
    	cin >> u >> v >> d >> c;
    	globalEnergyMax = max(globalEnergyMax,d*8.0);
    	globalTimeMax = max(globalTimeMax,d/20.0);
    	G[u].pb(mp(v,mp(d,c)));
    	G[v].pb(mp(u,mp(d,c)));
    }
    cout << "Enter source and destination " << endl;
    cin >> source >> destination;
    // hyperParameterLambda = 10e-20;
    // cout << hyperParameterLambda << endl;
    hyperParameterDistance = 0.0;
    DichotomyAlgorithm();
    FOR0(i,SZ(ANS))
    {
    	cout << "Energy Consumption - " << ANS[i].ff.ff << " Time Consumption - " << ANS[i].ff.ss << endl;
    	FOR0(j,SZ(ANS[i].ss)-1)
    		cout << "From " << ANS[i].ss[j].ff << " go to " << ANS[i].ss[j+1].ff << " with speed " << ANS[i].ss[j].ss << endl;
		cout << "From " << ANS[i].ss[SZ(ANS[i].ss)-1].ff << " go to " << destination << " with speed " << ANS[i].ss[SZ(ANS[i].ss)-1].ss << endl; 
    }
}	