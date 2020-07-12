// Added Partial Charging 

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

double chargePerLevel = 20.0;
map<int,pair<double,pair<double,double> > > stations;
vector<double> energyMultiplier;
vector<double> timeMultiplier;
vector<vector<pair<int,pair<double,double> > > > G;
set<pair<double,double> > S;
vector<pair<pair<double,double>,vii> > ANS;
int source,destination;
double charge, maxCharge, energyAtCS = 0.0;
double globalEnergyMax,globalTimeMax;
double lambda;
double hyperParameterLambda,hyperParameterDistance;

double getNewEdgeWeight(double energy,double time)
{
	return ((lambda*lambda*energy)/globalEnergyMax  + (1.0 - lambda*lambda)*time/globalTimeMax);
}

vector<pair<pair<double,double>,vii> > buildPathWithoutCharging(int s, int d, vector<pair<ii,pair<double,double> > > par)
{
	vector<pair<pair<double,double>,vii> > ret;
	double energy = 0.0,time = 0.0;
	vii path;
	int prev = d;
	int temp = d;
	if(s != d)
	{
		path.pb(par[d].ff);
		while(temp != -1)
		{
			energy += par[temp].ss.ff;
			time += par[temp].ss.ss;
			temp = par[temp].ff.ff;
			if(temp == s)
				break;
			path.pb(par[temp].ff);
		}
		reverse(all(path));
	}
	if(energy > charge)
		energy = DBL_MAX;
	ret.pb(mp(mp(energy,time),path));
	return ret;
}

vector<pair<pair<double,double>,vii> > buildPathWithCharging(int s, int index, int d, vector<double> time, vector<pair<ii,pair<double,double> > > par, vector<pair<ii,pair<double,double> > > par2)
{
	//levels
	int levels = ceil(stations[index].ff/chargePerLevel);
	double timePerLevel = ((stations[index].ss.ff/stations[index].ff)*chargePerLevel);
	vector<pair<pair<double,double>,vii> > ret;
	double energy = 0.0,timetaken = 0.0;
	vector<vii> ember;
	vector<double> voidenergy,earthtime;
	vii path;
	int prev = index;
	int temp = index;
	if(s != index)
	{
		path.pb(par[index].ff);
		while(temp != -1)
		{
			//cout << temp << " " << prev << endl;
			energy += par[temp].ss.ff;
			timetaken += par[temp].ss.ss;
			temp = par[temp].ff.ff;
			if(temp == s)
				break;
			path.pb(par[temp].ff);
		}
		reverse(all(path));
	}
	for(int l = 1 ; l <= levels ; l++)
	{
		voidenergy.pb(energy);
		earthtime.pb(timetaken);
		double tempEnergy = min(l*chargePerLevel,stations[index].ff);
		double tempTime = min(l*timePerLevel,stations[index].ss.ff);	
		ember.pb(path);
		if(tempEnergy > maxCharge - charge + energyAtCS)
		{
			voidenergy[l-1] -= maxCharge - charge + energyAtCS;
			earthtime[l-1] += ((stations[index].ss.ff/stations[index].ff)*(maxCharge - charge + energyAtCS)) + max(stations[index].ss.ss - time[index],0.0);
			ember[l-1].pb(mp(index,-1*(maxCharge - charge + energyAtCS)));
			//break;
		}
		else
		{
			voidenergy[l-1] -= tempEnergy;
			earthtime[l-1] += tempTime + max(stations[index].ss.ss - time[index],0.0);
			ember[l-1].pb(mp(index,-1*(tempEnergy)));
		}
	}		
	prev = index;
	temp = index;
	energy = 0.0;
	timetaken = 0.0;
	while(temp != d)
	{
		energy += par2[temp].ss.ff;
		timetaken += par2[temp].ss.ss;
		temp = par2[temp].ff.ff;
		path.pb(mp(prev,par2[prev].ff.ss));
		for(int l = 0 ; l < SZ(ember) ; l++)
			ember[l].pb(mp(prev,par2[prev].ff.ss));
		prev = temp;
	}
	for(int l = 1 ; l <= levels ; l++)
	{
		voidenergy[l-1] += energy;
		earthtime[l-1] += timetaken;
		ret.pb(mp(mp(voidenergy[l-1],earthtime[l-1]),ember[l-1]));
	}
	return ret;
}

void getDistance(int s, vector<double> &dist, vector<double> &time, vector<double> &energy, vector<pair<ii,pair<double,double> > > &par)
{
	bool relax = false;
	int n = G.size();
	for(int i = 0 ; i < n ; i++)
	{
		dist.pb(DBL_MAX);
		time.pb(DBL_MAX);
		energy.pb(DBL_MAX);
	}
	dist[s] = 0.0;
	time[s] = 0.0;
	energy[s] = 0.0;
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
						time[v] = time[u] + t;
						energy[v] = energy[u] + e;
						par[v] = mp(mp(u,(l+1)*20),mp(e,t));
					}
				}
			}
		}
		if(!relax)
			break;
	}
}

int getCS(vector<double> time, vector<double> energy, vector<double> time2, vector<double> energy2)
{
	// double cur_min = DBL_MAX;
	double cur_min_time = DBL_MAX;
	double cur_min_energy = DBL_MAX;
	int index = -1;
	int n = G.size();
	for(int i = 0 ; i < n ; i++)
	{
		// dist comparision to avoid loops
		if(stations.find(i) != stations.end() && i != source && i != destination)
		// if(stations.find(i) != stations.end() && dist[i] < dist[destination] && dist2[i] < dist2[source])
		{
			// //levels
			// if(energy[i] <= charge)
			// {
			// 	int levels = ceil(stations[i].ff/50);
			// 	double timePerLevel = ((stations[i].ss.ff/stations[i].ff)*50.0);
			// 	for(int l = 1 ; l <= levels ; l++)
			// 	{
			// 		double tempEnergy = min(l*50.0,stations[i].ff);
			// 		double time = min(l*timePerLevel,stations[i].ss.ff);

			// 	}
			// }
			// energy
			if(energy[i] <= charge)
			{
				// double temp = dist[i] + dist2[i] + getNewEdgeWeight(-1.0*min(stations[i].ff,maxCharge - charge + energy[i]),stations[i].ss.ff + max(stations[i].ss.ss - time[i],0.0));
				double drowenergy = energy[i] + energy2[i] - min(stations[i].ff,maxCharge - charge + energy[i]);
				double enigmatime = time[i] + time2[i] + stations[i].ss.ff + max(stations[i].ss.ss - time[i],0.0);
				if((cur_min_energy > drowenergy && cur_min_time > enigmatime) ||
				(cur_min_energy == drowenergy && cur_min_time > enigmatime) ||
				(cur_min_energy > drowenergy && cur_min_time == enigmatime) ||
				(cur_min_energy < drowenergy && cur_min_time > enigmatime))
				{
					cur_min_energy = drowenergy;
					cur_min_time = enigmatime;
					index = i;
					energyAtCS = energy[i];
				}
			}
		}
	}
	return index;
}

vector<pair<pair<double,double>,vii> > BFalgorithm()
{
	bool relax = false;
	int n = G.size();

	vector<double> dist;
	vector<double> time;
	vector<double> energy;
	vector<pair<ii,pair<double,double> > > par(n,mp(mp(-1,-1),mp(0.0,0.0)));
	getDistance(source, dist, time, energy, par);

	vector<double> dist2;
	vector<double> time2;
	vector<double> energy2;
	vector<pair<ii,pair<double,double> > > par2(n,mp(mp(-1,-1),mp(0.0,0.0)));
	getDistance(destination, dist2, time2, energy2, par2);

	vii temp;
	int optimalCS = getCS(time, energy, time2, energy2);
	vector<pair<pair<double,double>,vii> > second;
	vector<pair<pair<double,double>,vii> > first = buildPathWithoutCharging(source, destination, par);
	if(optimalCS != -1) 
		second = buildPathWithCharging(source, optimalCS, destination, time, par, par2);	

	second.pb(first[0]);
	return second;
}

void solveRecursison(double z1l,double z2l,double z1r,double z2r,double lambdaleft,double lambdaright)
{
	lambda = (lambdaleft+lambdaright)/2.0;
	
	vector<pair<pair<double,double>,vii> > bf = BFalgorithm();
	if(bf.size() > 1)
	{
		for(int i = 0 ; i < SZ(bf) ; i++)
		{
			if(bf[i].ff.ff <= charge)
			{
				S.insert(bf[i].ff);
				ANS.pb(bf[i]);
			}
		}
	}
	else if(bf.size() > 0 && bf[0].ff.ff != DBL_MAX)
	{
		pair<pair<double,double>,vii> p = bf[0];
		double z1 = p.ff.ff;
		double z2 = p.ff.ss;
		if(S.find(p.ff) == S.end())
		{
			if(p.ff.ff <= charge)
			{
				S.insert(p.ff);
				ANS.pb(p);
			}
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
}

void DichotomyAlgorithm()
{
	//Initialise dichotomic search
	S.clear();
	
	//Compute Solution for extreme points
	lambda = 0.0;
	vector<pair<pair<double,double>,vii> > p1 = BFalgorithm();
	for(int i = 0 ; i < SZ(p1) ; i++)
	{
		if(p1[i].ff.ff <= charge)
		{
			S.insert(p1[i].ff);
			ANS.pb(p1[i]);
		}
	}
	lambda = 1.0;
	vector<pair<pair<double,double>,vii> > p2 = BFalgorithm();
	for(int i = 0 ; i < SZ(p2) ; i++)
	{
		if(p2[i].ff.ff <= charge)
		{
			S.insert(p2[i].ff);
			ANS.pb(p2[i]);
		}
	}
	//Recursively Compute other solutions
	if(SZ(p1) > 0 && SZ(p2) > 0)
		solveRecursison(p2[0].ff.ff,p2[0].ff.ss,p1[0].ff.ff,p1[0].ff.ss,1.0,0.0);
} 

int main()
{
    SYNC

    // auto start = chrono::steady_clock::now();

    energyMultiplier.pb(7.0); energyMultiplier.pb(4.0); energyMultiplier.pb(6.0); energyMultiplier.pb(8.0);
    timeMultiplier.pb(1.0/20.0); timeMultiplier.pb(1.0/40.0); timeMultiplier.pb(1.0/60.0); timeMultiplier.pb(1.0/80.0);
    int n,m,cs;
    //cout << "Enter the number of vertices " << endl;
    cin >> n;
    // cout << "Enter the number of edges " << endl;
    cin >> m; 
    int u,v;
    double d,c;
    G.resize(n,vector<pair<int,pair<double,double> > >());
    S.clear();
    stations.clear();
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
    	stations[u] = mp(e,mp(t,at));
    }
    // cout << "Enter the initial amount of charge " << endl;
    cin >> charge;
    // cout << "Enter the capacity of the battery " << endl;
    cin >> maxCharge;
    hyperParameterLambda = 10e-10;
    // cout << hyperParameterLambda << endl;
    hyperParameterDistance = 0.0;
    DichotomyAlgorithm();
    int number = 0;
	sort(all(ANS));
	double ymin = DBL_MAX;
    FOR0(i,SZ(ANS))
    {
    	if(ANS[i].ff.ff == DBL_MAX)
    		continue;
		if(ANS[i].ff.ss < ymin)
		{
			number++;
			ymin = ANS[i].ff.ss;
			cout << "Energy Consumption - " << ANS[i].ff.ff << " Time Consumption - " << ANS[i].ff.ss << endl;
			FOR0(j,SZ(ANS[i].ss)-1)
			{	
				if(ANS[i].ss[j].ss > 0)
				{
					cout << "From " << ANS[i].ss[j].ff;
					cout << " go to " << ANS[i].ss[j+1].ff;
					cout << " with speed " << ANS[i].ss[j].ss << endl;
				}
				else
				{
					cout << "Charge your car at " << ANS[i].ss[j].ff << " for " 
					<< (double)-1*ANS[i].ss[j].ss << " units" << endl;
				}
			}
			if(ANS[i].ss[SZ(ANS[i].ss)-1].ss > 0)
			{
				cout << "From " << ANS[i].ss[SZ(ANS[i].ss)-1].ff; 
				cout << " go to " << destination << " with speed "; 
				cout << ANS[i].ss[SZ(ANS[i].ss)-1].ss << endl;
			}
			else
			{
				cout << "Charge your car at " << ANS[i].ss[SZ(ANS[i].ss)-1].ff << " for " 
				<< (double)-1*ANS[i].ss[SZ(ANS[i].ss)-1].ss << " units" << endl;
			}
		}
    }
    //cout << number << endl;
    // auto end = chrono::steady_clock::now();

	// Store the time difference between start and end
	// auto diff = end - start;

	// cout << chrono::duration <double, milli> (diff).count() << " ms" << endl;
}	