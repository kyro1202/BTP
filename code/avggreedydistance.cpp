// Brute Force Solution to compare our algorithm to

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

double chargePerLevel = 20.0; //Charge provided by Charging Stations per level
map<int,pair<double,double> > stations; //chargin stations
map<int,set<pair<double,double> > > waitingTime; //waiting time for each charging station
vector<double> energyMultiplier; //energy multiplier per velocity levels
vector<double> timeMultiplier; //time multiplier per velocity levels
vector<vector<pair<int,double> > > G; //Graph
vector<vector<pair<int,double> > > NewGraph; //new graph for brute force method
vector<vector<pair<int,vector<pair<double,double> > > > > Cong; //Congestion on every edge 
set<pair<pair<double,double>,pair<double,vii> > > S; //solution set
vector<pair<pair<pair<double,double>,pair<double,double> >,vii> > ANS; // stores answer
vector<pair<pair<pair<double,double>,pair<double,double> >,vii> > final_answer; //refined final answer
vector<pair<ii,pair<double,double> > > Req; //Requests
int source,destination;
double charge, maxCharge, energyAtCS = 0.0; //initial charge, maximum capacity and energy of battery when vehicle arrives at the charging station
double globalEnergyMax,globalTimeMax; //for normalization
double lambda;
double hyperParameterLambda,hyperParameterDistance,congestionpercar; 
//congestion per car denotes the amount we need to increase congestion by at an edge if a vehicle is travelling on it
double currentDistance,currentEnergy,currentTime;
bool visited[10000];
int path[10000];
int n,m,cs;
double distmin,enmin,tmin;
int amountcharged;
int stationindex;
vii bruh;
double AVGTIME,AVGENERGY; //final output - average of time cost and energy cost of all requests

double getCongestion(int u,int v,double at) //returns the congestion on edge u to v at time unit 'at'
{
	if(u > v)
	{
		int temp = u;
		u = v;
		v = temp;
	}
	double ret = 0.0;
	vector<pair<double,double> > congpairs;
	FOR0(i,SZ(Cong[u])) //finding the appropriate vector
    {
        if(Cong[u][i].ff == v)
        {
			congpairs = Cong[u][i].ss;
			break;
		}
    }
	FOR0(i,SZ(congpairs))
	{
		if(congpairs[i].ss >= at && congpairs[i].ff <= at) //increasing the congestion for every vehicle on the edge
			ret += congestionpercar;
	}
	return ret;
}

double getWaitingTime(int chargingStation, double chargingAmount, double arrivalTime) //returns the waiting time at the charging station
{
	// chargingStation - the charging station
	// chargingAmount - the amount to be charged
	// arrivalTime - the time at which the vehicle arrives at the charging station
	double ret = 0.0;
	double drowtime = (chargingAmount/stations[chargingStation].ff)*stations[chargingStation].ss; //time required to charged that amount
	double end = arrivalTime + drowtime; //time at which the charging will end
	set<pair<double,double> >::iterator it = waitingTime[chargingStation].begin(), prev;
	while(it != waitingTime[chargingStation].end()) //calculating the waiting time
	{
		pair<double,double> PL = *it;
		ret = max(ret,PL.ss);
		if( it == waitingTime[chargingStation].begin() )
		{
			if(PL.ff >= end)
				return 0.0;
		}
		else
		{
			if((*prev).ss <= arrivalTime && end <= PL.ff)
				return 0.0;
		}
		prev = it;
		it++;
	}
	if(ret > arrivalTime)
		return ret - arrivalTime;
	else
		return 0.0;
}

double getdistance(int u,int v) //distance from u to v
{
    FOR0(i,SZ(NewGraph[u]))
    {
        if(NewGraph[u][i].ff == v)
            return NewGraph[u][i].ss;
    }
    return 0.0;
}

void updateCongestion(int u,int v,double start,double end) //update congestion on edge u to v for the time start to end
{
	if(u > v)
	{
		int temp = u;
		u = v;
		v = temp;
	}
    FOR0(i,SZ(Cong[u]))
    {
        if(Cong[u][i].ff == v)
        {
			// cout << "Congestion updated on " << u << " " << v << " from " << start << " to " << end << endl;
			Cong[u][i].ss.pb(mp(start,end));
			return;
		}
    }
    return;
}

double getCharginTime(int chargingStation, int chargingAmount) //returns the charging time for the specified amount on that charging station
{
    return (chargingAmount/stations[chargingStation].ff)*stations[chargingStation].ss;
    // return 0.0;
}

void updateWaitingTime(int chargingStation, int chargingAmount, double arrivalTime, double chargingTime) //updates waiting time at the particular charging stations
{
	// cout << "Waiting Time updated at " << chargingStation << endl;
    waitingTime[chargingStation].insert(mp(arrivalTime,arrivalTime+chargingTime));
    return;
}

void updateGraph(vii path) //updates the graph according to the selected path
{
	if(path.size() == 0)
		return;
    int length = SZ(path);
    int u,v,s,cs,ce; //edge node 1, edge node 2, speed, charging station, charging energy
    double timetaken = 0.0,d,time1 = 0.0,time2 = 0.0;
    FOR0(i,length - 1)
    {
        if(path[i].ss > 0)
        {
            u = path[i].ff;
            v = path[i+1].ff;
            s = path[i].ss;
            time1 = timetaken;
            d = getdistance(u,v);
            timetaken += (double)d/s;
            time2 = timetaken;
            updateCongestion(u,v,time1,time2);
        }
        else
        {
            time1 = timetaken;
            cs = path[i].ff;
            ce = -1*path[i].ss;
            time2 = getCharginTime(cs,ce);
            updateWaitingTime(cs,ce,time1,time2);
            timetaken += time2;
        }
    }
	int i = length - 1;
	if(path[i].ss > 0)
	{
		u = path[i].ff;
		v = destination;
		s = path[i].ss;
		time1 = timetaken;
		d = getdistance(u,v);
		timetaken += (double)d/s;
		time2 = timetaken;
		updateCongestion(u,v,time1,time2);
	}
	else
	{
		time1 = timetaken;
		cs = path[i].ff;
		ce = -1*path[i].ss;
		time2 = getCharginTime(cs,ce);
		updateWaitingTime(cs,ce,time1,time2);
		timetaken += time2;
	}
}

int getAllowed(double c) //allowed level of velocity according to the congestion
{
	if(c < 20.0)
		return 3;
	if(c < 40.0)
		return 2;
	if(c < 60.0)
		return 1;
	else
		return 0;
}
 
void printAllPathsUtil(int u, int d, int path[], int &path_index)
{ 
	// brute force computes all paths which can be traveresed
	// using the initial charge of the battery
	// Mark the current node and store it in path[] 
	visited[u] = true;
	path[path_index] = u; 
	path_index++; 
	if (u == d) 
	{ 
		int numcharginstations = 0;
		for(int i = 0 ; i < path_index ; i++)
		{
			// cout << path[i] << " ";
			if(stations.find(path[i]) != stations.end() && path[i] != source && path[i] != destination)
			{
				numcharginstations++;
			}
		}
		// cout << endl;
		vector<pair<pair<double,double>,pair<double,bool> > > dazzle;
		vector<pair<int,int> > charginginformation;
		int index = 0;
		vii stormspirit;
		for(int w = 0 ; w <= numcharginstations ; w++)
		{
			bool check = true;
			int countchargingstations = 0;
			currentEnergy = 0.0;
			currentTime = 0.0;
			currentDistance = 0.0;
			dazzle.pb(mp(mp(0.0,0.0),mp(0.0,true)));
			charginginformation.pb(mp(0,-1));
			int s,ver = -1,ver2 = -1;
			bool found = false;
			for (int i = 0; i<path_index; i++) 
			{
				if(stations.find(path[i]) != stations.end() && path[i] != source && path[i] != destination)
					countchargingstations++;
				if(path[i] < n && !found)
				{
					found = true;
					ver = path[i];
				}
				else if(path[i] < n && found)
				{
					ver2 = ver;
					ver = path[i];
					double wowoow = getdistance(ver2,ver);
					currentDistance += wowoow;
					currentEnergy += energyMultiplier[s]*wowoow;
					currentTime += timeMultiplier[s]*wowoow;
					double c = getCongestion(ver2,ver,currentTime);
					int a = getAllowed(c);
					if(s > a || charge < currentEnergy)
					{
						check = false;
					}
					for(int j = index ; j < SZ(dazzle) ; j++)
					{
						double c = getCongestion(ver2,ver,dazzle[j].ff.ss);
						int a = getAllowed(c);
						dazzle[j].ff.ff += energyMultiplier[s]*wowoow;
						dazzle[j].ff.ss += timeMultiplier[s]*wowoow;
						dazzle[j].ss.ff += wowoow;
						if(s > a || charge < dazzle[j].ff.ff)
						{
							dazzle[j].ss.ss = false;
						}
						// dazzle[j].ss.ss = check;
					}
					if(w != 0 && stations.find(path[i]) != stations.end() && path[i] != source && path[i] != destination && w == countchargingstations)
					{
						double abbadoncharge = 20;
						while(true)
						{
							if(abbadoncharge > stations[ver].ff)
								abbadoncharge = stations[ver].ff;
							double newcharge,newtime;
							// index++;
							newcharge = currentEnergy - abbadoncharge;
							newtime = currentTime + (stations[ver].ss*abbadoncharge/stations[ver].ff) + getWaitingTime(path[i],abbadoncharge,currentTime);
							if(abbadoncharge <= 20)
							{
								dazzle[index].ff.ff = newcharge;
								dazzle[index].ff.ss = newtime;
								charginginformation[index].ff = abbadoncharge;
								charginginformation[index].ss = path[i];
							}	
							else
							{
								dazzle.pb(mp(mp(newcharge,newtime),mp(currentDistance,check)));
								charginginformation.pb(mp(abbadoncharge,path[i]));
							}
							if(abbadoncharge == stations[ver].ff)
								break;
							abbadoncharge += 20;
						}
					}
				}
				else if(path[i] >= 2*n)
				{
					s = (path[i]-2*n)%4;
					if(w == 0)
						stormspirit.pb(mp(ver,(s+1)*20));
				}
			}
			index = dazzle.size();
		}
		for(int j = 0 ; j < SZ(dazzle) ; j++)
		{
			// cout << dazzle[j].ff.ff << " " << dazzle[j].ff.ss << " " << dazzle[j].ss.ff << " " << dazzle[j].ss.ss << " " << charginginformation[j].ff << " " << charginginformation[j].ss << endl;
			if(dazzle[j].ss.ss) //if path is feasible
			{
				if(dazzle[j].ss.ff < distmin) //shortest distance first
				{
					distmin = dazzle[j].ss.ff;
					enmin = dazzle[j].ff.ff;
					tmin = dazzle[j].ff.ss;
					bruh = stormspirit;
					amountcharged = charginginformation[j].ff;
					stationindex = charginginformation[j].ss;
				}
				else if(dazzle[j].ss.ff == distmin)
				{
					if(dazzle[j].ff.ss < tmin)
					{
						distmin = dazzle[j].ss.ff;
						enmin = dazzle[j].ff.ff;
						tmin = dazzle[j].ff.ss;
						bruh = stormspirit;
						amountcharged = charginginformation[j].ff;
						stationindex = charginginformation[j].ss;
					}
					else if(dazzle[j].ff.ff == tmin)
					{
						if(dazzle[j].ff.ff < enmin)
						{
							distmin = dazzle[j].ss.ff;
							enmin = dazzle[j].ff.ff;
							tmin = dazzle[j].ff.ss;
							bruh = stormspirit;
							amountcharged = charginginformation[j].ff;
							stationindex = charginginformation[j].ss;
						}		
					}
				}
			}
		}
	} 
	else // If current vertex is not destination 
	{ 
		// Recur for all the vertices adjacent to current vertex  
		for (int i = 0; i < G[u].size(); i++)
		{ 
			if (!visited[G[u][i].ff])
            {	
                printAllPathsUtil(G[u][i].ff, d, path, path_index);
            }
		}
	} 

	// Remove current vertex from path[] and mark it as unvisited 
	path_index--; 
	visited[u] = false;
}

void printAllPaths(int s, int d) 
{   
	int path_index = 0; // Initialize path[] as empty 

	// Initialize all vertices as not visited 
	for (int i = 0; i < 10000; i++) 
		visited[i] = false; 

	// Call the recursive helper function to print all paths 
	printAllPathsUtil(s, d, path, path_index); 
}  

void GreedyAlgorithm()
{
    printAllPaths(source,destination);
	if(tmin != DBL_MAX) //add the time cost and energy cost
	{
		AVGTIME += tmin; AVGENERGY += enmin;
	}
	// cout << "Energy " << enmin << " Time " << tmin << " Distance " << distmin << endl;
	vii shadowfiend;
	for(int i = 0 ; i < bruh.size() ; i++)
	{
		// cout << bruh[i].ff << " " << bruh[i].ss << endl;
		shadowfiend.pb(bruh[i]);
		if(i+1 < bruh.size() && bruh[i+1].ff == stationindex)
		{
			shadowfiend.pb(mp(stationindex,-1*amountcharged));
		}
	}
	// FOR0(j,SZ(shadowfiend)-1)
	// {	
	// 	if(shadowfiend[j].ss > 0)
	// 	{
	// 		cout << "From " << shadowfiend[j].ff;
	// 		cout << " go to " << shadowfiend[j+1].ff;
	// 		cout << " with speed " << shadowfiend[j].ss << endl;
	// 	}
	// 	else
	// 	{
	// 		cout << "Charge your car at " << shadowfiend[j].ff << " for " 
	// 		<< (double)-1*shadowfiend[j].ss << " units" << endl;
	// 	}
	// }
	// if(shadowfiend[SZ(shadowfiend)-1].ss > 0)
	// {
	// 	cout << "From " << shadowfiend[SZ(shadowfiend)-1].ff; 
	// 	cout << " go to " << destination << " with speed "; 
	// 	cout << shadowfiend[SZ(shadowfiend)-1].ss << endl;
	// }
	// else
	// {
	// 	cout << "Charge your car at " << shadowfiend[SZ(shadowfiend)-1].ff << " for " 
	// 	<< (double)-1*shadowfiend[SZ(shadowfiend)-1].ss << " units" << endl;
	// }
	updateGraph(shadowfiend); //updates the graph
} 

void getGraphInput()
{
    cin >> n;
    cin >> m;
	int u,v,temp = 2*n;
    double d,c,e,t;
    currentTime = 0.0;
    currentEnergy = 0.0;
    stations.clear();
	waitingTime.clear();
	G.resize(2*n+4*m,vector<pair<int,double> >());
    NewGraph.resize(n,vector<pair<int,double> >());
	Cong.resize(n,vector<pair<int,vector<pair<double,double> > > >()); 
	vector<pair<double,double> > earthshaker;
    FOR0(i,m)
    {
    	cin >> u >> v >> d;
        NewGraph[u].pb(mp(v,d));
    	NewGraph[v].pb(mp(u,d));
    	for(int l = 0 ; l < 4 ; l++) //adding dummy nodes for the 4 velocity levels
        {
            G[u].pb(mp(temp,d));
            G[temp].pb(mp(u,0));
            G[temp].pb(mp(v,0));
            G[v].pb(mp(temp,d));
            temp++;
        }
		Cong[u].pb(mp(v,earthshaker));
		Cong[v].pb(mp(u,earthshaker));
    }
    cin >> cs;
    FOR0(i,cs)
    {
    	cin >> u >> e >> t;
    	stations[u] = mp(e,t);
        waitingTime[u] = set<pair<double,double> > ();
    }
}

void getRequest()
{
	int n,u,v;
	cin >> n;
	double charge,maxCharge;
	FOR0(i,n)
	{
		cin >> u >> v >> charge >> maxCharge;
		Req.pb(mp(mp(u,v),mp(charge,maxCharge)));
	}
}

void solve()
{
	FOR0(i,SZ(Req))
	{
		ANS.clear();
		bruh.clear();
		distmin = DBL_MAX;
		enmin = DBL_MAX;
		tmin = DBL_MAX;
		amountcharged = 0;
		stationindex = -1;
		final_answer.clear();	
		source = Req[i].ff.ff;
		destination = Req[i].ff.ss;
		charge = Req[i].ss.ff;
		maxCharge = Req[i].ss.ss;
		GreedyAlgorithm();
	}
}

int main()
{
    SYNC

    energyMultiplier.pb(7.0); energyMultiplier.pb(4.0); energyMultiplier.pb(6.0); energyMultiplier.pb(8.0);
    timeMultiplier.pb(1.0/20.0); timeMultiplier.pb(1.0/40.0); timeMultiplier.pb(1.0/60.0); timeMultiplier.pb(1.0/80.0);
    congestionpercar = 20.0;
    AVGTIME = 0.0; AVGENERGY = 0.0;
	getGraphInput();
	getRequest();
	solve();	
	
	cout << AVGENERGY/Req.size() << " " << AVGTIME/Req.size() << endl;		
}	