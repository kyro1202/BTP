// extra nodes which represent speed levels
// exponential solution to compare it against our algorithm

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
set<pair<pair<double,double>,vi> > S; 
int source,destination;
double charge, maxCharge, energyAtCS = 0.0;
double currentCharge,currentEnergy,currentTime;
double lambda;
int n,m,cs;
bool visited[10000];
int path[10000];
 
void printAllPathsUtil(int u, int d, int path[], int &path_index) 
{ 
	// Mark the current node and store it in path[] 
	visited[u] = true; 
	path[path_index] = u; 
	path_index++; 
    double tempe = currentEnergy;
    double tempt = currentTime;
    double tempc = currentCharge;

    if( u >= n && u < 2*n)
    {
        double t = stations[u-n].ss.ff;
        double tt = stations[u-n].ss.ss;
        double e = stations[u-n].ff;
        if(currentTime >= tt)
            currentTime += t;
        else
            currentTime = t + tt;
        double r = maxCharge - currentCharge;
        if(e >= r)
        {
            currentCharge = maxCharge;
            currentEnergy -= r;
        }
        else
        {
            currentCharge += e;
            currentEnergy -= r;
        }
    }
	// If current vertex is same as destination, then print 
	// current path[] 
	if (u == d) 
	{ 
        vi stormspirit;
		for (int i = 0; i<path_index; i++) 
		{
			stormspirit.pb(path[i]);
		}
		// cout << endl << currentEnergy << " " << currentTime << endl;
        S.insert(mp(mp(currentEnergy,currentTime),stormspirit));
    } 
	else // If current vertex is not destination 
	{ 
		// Recur for all the vertices adjacent to current vertex  
		for (int i = 0; i < G[u].size(); i++) 
			if (!visited[G[u][i].ff])
            {
                if(currentCharge < G[u][i].ss.ff)
                    continue;
                currentCharge -= G[u][i].ss.ff;
				currentEnergy += G[u][i].ss.ff;
                currentTime += G[u][i].ss.ss;
                printAllPathsUtil(G[u][i].ff, d, path, path_index);
                currentEnergy -= G[u][i].ss.ff;
                currentTime -= G[u][i].ss.ss;
                currentCharge += G[u][i].ss.ff;
            } 
	} 

	// Remove current vertex from path[] and mark it as unvisited 
	path_index--; 
	visited[u] = false;

    if( u >= n && u < 2*n)
    {
        currentCharge = tempc;
        currentEnergy = tempe;
        currentTime = tempt;
    }

}
// Prints all paths from 's' to 'd' 
void printAllPaths(int s, int d) 
{   
	int path_index = 0; // Initialize path[] as empty 

    currentCharge = charge;
	// Initialize all vertices as not visited 
	for (int i = 0; i < 10000; i++) 
		visited[i] = false; 

	// Call the recursive helper function to print all paths 
	printAllPathsUtil(s, d, path, path_index); 
}  

int main()
{
    SYNC

    energyMultiplier.pb(7.0); energyMultiplier.pb(4.0); energyMultiplier.pb(6.0); energyMultiplier.pb(8.0);
    timeMultiplier.pb(1.0/20.0); timeMultiplier.pb(1.0/40.0); timeMultiplier.pb(1.0/60.0); timeMultiplier.pb(1.0/80.0);
    
    //cout << "Enter the number of vertices " << endl;
    cin >> n;
    // cout << "Enter the number of edges " << endl;
    cin >> m; 
    int u,v,temp=2*n;
    double d,c;
    G.resize(2*n+4*m,vector<pair<int,pair<double,double> > >());
    stations.clear();
    S.clear();
    currentTime = 0.0;
    currentEnergy = 0.0;
    FOR0(i,m)
    {
    	cin >> u >> v >> d >> c;
    	for(int l = 0 ; l < 4 ; l++)
        {
            if((l+1)*20.0 > c)
                break;
            double e = d*energyMultiplier[l];
            double t = d*timeMultiplier[l];
            G[u].pb(mp(temp,mp(e,t)));
            G[temp].pb(mp(u,mp(0.0,0.0)));
            G[temp].pb(mp(v,mp(0.0,0.0)));
            G[v].pb(mp(temp,mp(e,t)));
            temp++;
        }
    }
    // cout << "Enter source and destination " << endl;
    cin >> source >> destination;
    // cout << "Enter the number of charging stations " << endl;
    cin >> cs;
    // cout << "Enter the Charging Stations " << endl;
    FOR0(i,cs)
    {
    	int u;
    	double e,t,at;
    	cin >> u >> e >> t >> at;
        if(u == source || u == destination)
            continue;
    	int add = n + u;
    	for(int j = 0 ; j < G[u].size() ; j++)
    	{
            double temp1 = G[u][j].ss.ff;
            double temp2 = G[u][j].ss.ss;
    		G[G[u][j].ff].pb(mp(add,mp(0.0,0.0)));
    		G[add].pb(mp(G[u][j].ff,mp(temp1,temp2)));
    	}
    	stations[u] = mp(e,mp(t,at));
    }
    // cout << "Enter the initial amount of charge " << endl;
    cin >> charge;
    // cout << "Enter the capacity of the battery " << endl;
    cin >> maxCharge;
   

    //auto start = chrono::steady_clock::now();

    printAllPaths(source,destination);
    int ans = 0;
    set<pair<pair<double,double>,vi> > :: iterator it;
    double ymin = DBL_MAX;
    for(it = S.begin() ; it != S.end() ; it++ )
    {
        if((*it).ff.ss < ymin)
        {
            ans++;
            cout << (*it).ff.ff << " " << (*it).ff.ss << endl;
            ymin = (*it).ff.ss;
            vi meepo = (*it).ss;
            FOR0(i,SZ(meepo))
                cout << meepo[i] << " ";
            cout << endl;
        }
    }
    cout << ans << endl;
    
}