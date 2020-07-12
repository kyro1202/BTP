
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

map<int,pair<double,pair<double,double> > > stations;
vector<double> energyMultiplier;
vector<double> timeMultiplier;
vector<vector<pair<int,pair<double,double> > > > G;
vi ANS;
vector<bool> vis;
vector<int> order;
int source;


void Reach(int index, double power)
{
	vis[index] = true;
	int found = 0;
	FOR0(i,SZ(G[index]))
	{
		int v = G[index][i].ff;
		if(vis[v])
			continue;
		double d = G[index][i].ss.ff;
		double c = G[index][i].ss.ss;
		FOR0(l,4)
		{
			if((order[l]+1)*20.0 < c)
			{
				double e = d*energyMultiplier[order[l]];
				double t = d*timeMultiplier[order[l]];
				if(power >= e)
				{
					found++;
					Reach(v,power - e);
				}
				break;
			}
		}
	}
	if(stations.find(index) != stations.end())
		cout << "Charging Station at " << index << endl;
	else if(!found)
		cout << index << endl;
} 


int main()
{
    SYNC
    order.pb(1);
    order.pb(2);
    order.pb(0);
    order.pb(3);
    energyMultiplier.pb(7.0); energyMultiplier.pb(4.0); energyMultiplier.pb(6.0); energyMultiplier.pb(8.0);
    timeMultiplier.pb(1.0/20.0); timeMultiplier.pb(1.0/40.0); timeMultiplier.pb(1.0/60.0); timeMultiplier.pb(1.0/80.0);
    int n,m,cs;
    cout << "Enter the number of vertices " << endl;
    cin >> n;
    vis.resize(n,false);
    cout << "Enter the number of edges " << endl;
    cin >> m; 
    int u,v;
    double d,c;
    G.resize(n,vector<pair<int,pair<double,double> > >());
    stations.clear();
    ANS.clear();
    FOR0(i,m)
    {
    	cin >> u >> v >> d >> c;
    	G[u].pb(mp(v,mp(d,c)));
    	G[v].pb(mp(u,mp(d,c)));
    }
    cout << "Enter the number of charging stations " << endl;
    cin >> cs;
    cout << "Enter the Charging Stations " << endl;
    FOR0(i,cs)
    {
    	int u;
    	double e,t,at;
    	cin >> u >> e >> t >> at;
    	stations[u] = mp(e,mp(t,at));
    }
    cout << "Enter source " << endl;
    cin >> source;
    double power;
    cout << "Enter current level of charge " << endl;
    cin >> power;
    cout << "Current Reach - " << endl;
    Reach(source, power);
}	