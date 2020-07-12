// genrates a random graph to make a sample test case

#define _CRT_SECURE_NO_WARNINGS
#include <bits/stdc++.h>
#include<time.h> 
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

int main()
{
	int n = 7;
	int m = 10;
	int cs = 2;
	srand(time(0));
	cout << n << endl;
	vii edges;
	for(int i = 0 ; i < n ; i++)
		for(int j = i+1 ; j < n ; j++)
			edges.pb(mp(i,j));
	random_shuffle(all(edges));
	cout << m << endl;
	set<int> node;
	FOR0(i,m)
	{
		int u = edges[i].ff;
		int v = edges[i].ss;
		node.insert(u);
		node.insert(v);
		cout << u << " " << v << " " << rand()%10 + 1 << " " << rand()%80 + 21 << endl;
	}
	int u = rand()%n;
	int v = rand()%n;
	while(u == v)
		v = rand()%n;
	cout << u << " " << v << endl;
	cout << cs << endl;
	set<int> charging_stations;
	FOR0(i,cs)
	{
		int j = rand()%SZ(node);
		set<int>::iterator it = node.begin();
		advance(it,j);
		j = *it;
		while(charging_stations.find(j) != charging_stations.end())
		{
			j = rand()%SZ(node);
			it = node.begin();
			advance(it,j);
			j = *it;
		}
		charging_stations.insert(j);
		// charging station, energy, time and time at which station becomes available
		cout << j << " " << rand()%80 + 20 << " " << (rand()%20 + 1)*0.1 << " " << (rand()%10 + 1)*0.1 << endl;
	}
	cout << 50 << endl << 100 << endl;
}