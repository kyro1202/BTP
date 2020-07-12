// final algorithm but energy consumption and energy restored are not shown separately

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
map<int,pair<double,double> > stations;
map<int,set<pair<double,double> > > waitingTime;
vector<double> energyMultiplier;
vector<double> timeMultiplier;
vector<vector<pair<int,double> > > G;
vector<vector<pair<int,vector<pair<double,double> > > > > Cong; 
set<pair<double,double> > S;
vector<pair<pair<double,double>,vii> > ANS;
vector<pair<pair<double,double>,vii> > final_answer;
vector<pair<ii,pair<double,double> > > Req;
int source,destination;
double charge, maxCharge, energyAtCS = 0.0;
double globalEnergyMax,globalTimeMax;
double lambda;
double hyperParameterLambda,hyperParameterDistance,congestionpercar;

double getCongestion(int u,int v,double at)
{
	if(u > v)
	{
		int temp = u;
		u = v;
		v = temp;
	}
	double ret = 0.0;
	vector<pair<double,double> > congpairs;
	FOR0(i,SZ(Cong[u]))
    {
        if(Cong[u][i].ff == v)
        {
			congpairs = Cong[u][i].ss;
			break;
		}
    }
	FOR0(i,SZ(congpairs))
	{
		if(congpairs[i].ss >= at && congpairs[i].ff <= at)
			ret += congestionpercar;
	}
	return ret;
}

double getWaitingTime(int chargingStation, double chargingAmount, double arrivalTime)
{
	double ret = 0.0;
	double drowtime = (chargingAmount/stations[chargingStation].ff)*stations[chargingStation].ss;
	double end = arrivalTime + drowtime;
	set<pair<double,double> >::iterator it = waitingTime[chargingStation].begin(), prev;
	while(it != waitingTime[chargingStation].end())
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

double getdistance(int u,int v)
{
    FOR0(i,SZ(G[u]))
    {
        if(G[u][i].ff == v)
            return G[u][i].ss;
    }
    return 0.0;
}

void updateCongestion(int u,int v,double start,double end)
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
			cout << "Congestion updated on " << u << " " << v << " from " << start << " to " << end << endl;
			Cong[u][i].ss.pb(mp(start,end));
			return;
		}
    }
    return;
}

double getCharginTime(int chargingStation, int chargingAmount)
{
    return (chargingAmount/stations[chargingStation].ff)*stations[chargingStation].ss;
    // return 0.0;
}

void updateWaitingTime(int chargingStation, int chargingAmount, double arrivalTime, double chargingTime)
{
	cout << "Waiting Time updated at " << chargingStation << endl;
    waitingTime[chargingStation].insert(mp(arrivalTime,arrivalTime+chargingTime));
    return;
}

void updateGraph(vii path)
{
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

double getAllowed(double c)
{
	if(c < 20.0)
		return 85.0;
	if(c < 40.0)
		return 65.0;
	if(c < 60.0)
		return 45.0;
	if(c < 80.0)
		return 25.0;
	return 5.0;
}

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
	double timePerLevel = ((stations[index].ss/stations[index].ff)*chargePerLevel);
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
		double tempTime = min(l*timePerLevel,stations[index].ss);	
		ember.pb(path);
		if(tempEnergy > maxCharge - charge + energyAtCS)
		{
			voidenergy[l-1] -= maxCharge - charge + energyAtCS;
			earthtime[l-1] += ((stations[index].ss/stations[index].ff)*(maxCharge - charge + energyAtCS)) + getWaitingTime(index,maxCharge - charge + energyAtCS,time[index]);
			ember[l-1].pb(mp(index,-1*(maxCharge - charge + energyAtCS)));
			//break;
		}
		else
		{
			voidenergy[l-1] -= tempEnergy;
			earthtime[l-1] += tempTime + getWaitingTime(index,tempEnergy,time[index]);
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
			// check this condition
			if(dist[j] == DBL_MAX)
				continue;
			for(int k = 0 ; k < G[j].size() ; k++)
			{
				int u = j;
				int v = G[j][k].ff;
				double d = G[j][k].ss;
				double c = getCongestion(u,v,time[u]);
				double allowed = getAllowed(c);
				FOR0(l,4)
				{
					if((l+1)*20.0 > allowed)
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
			if(energy[i] <= charge)
			{
				// double temp = dist[i] + dist2[i] + getNewEdgeWeight(-1.0*min(stations[i].ff,maxCharge - charge + energy[i]),stations[i].ss.ff + max(stations[i].ss.ss - time[i],0.0));
				double drowenergy = energy[i] + energy2[i] - min(stations[i].ff,maxCharge - charge + energy[i]);
				if(drowenergy > charge)
					continue;
				double enigmatime = time[i] + time2[i] + stations[i].ss + getWaitingTime(i,min(stations[i].ff,maxCharge - charge + energy[i]),time[i]);
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

	//Non dominated points
    sort(all(ANS));
	double ymin = DBL_MAX;
    FOR0(i,SZ(ANS))
    {
    	if(ANS[i].ff.ff == DBL_MAX)
    		continue;
		if(ANS[i].ff.ss < ymin)
		{
			ymin = ANS[i].ff.ss;
            final_answer.pb(ANS[i]);
		}
    }
} 

void getGraphInput()
{
	globalTimeMax = 0.0;
    globalEnergyMax = 0.0;
	int n,m,cs;
    cin >> n;
    cin >> m;
	int u,v;
    double d,c,e,t;
    stations.clear();
	waitingTime.clear();
	G.resize(n,vector<pair<int,double> >());
	Cong.resize(n,vector<pair<int,vector<pair<double,double> > > >()); 
	vector<pair<double,double> > earthshaker;
    FOR0(i,m)
    {
    	cin >> u >> v >> d;
    	globalEnergyMax = max(globalEnergyMax,d*8.0);
    	globalTimeMax = max(globalTimeMax,d/20.0);
    	G[u].pb(mp(v,d));
    	G[v].pb(mp(u,d));
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
		final_answer.clear();	
		source = Req[i].ff.ff;
		destination = Req[i].ff.ss;
		charge = Req[i].ss.ff;
		maxCharge = Req[i].ss.ss;
		DichotomyAlgorithm();
		if(SZ(final_answer) == 0)
			cout << "NO PATH" << endl;
		else
		{	
			// int r = 0 + (rand() % static_cast<int>(SZ(final_answer) - 0 + 1));
			int r = SZ(final_answer)-1;
			cout << "Energy Consumption - " << final_answer[r].ff.ff << " Time Consumption - " << final_answer[r].ff.ss << endl;
			FOR0(j,SZ(final_answer[r].ss)-1)
			{	
				if(final_answer[r].ss[j].ss > 0)
				{
					cout << "From " << final_answer[r].ss[j].ff;
					cout << " go to " << final_answer[r].ss[j+1].ff;
					cout << " with speed " << final_answer[r].ss[j].ss << endl;
				}
				else
				{
					cout << "Charge your car at " << final_answer[r].ss[j].ff << " for " 
					<< (double)-1*final_answer[r].ss[j].ss << " units" << endl;
				}
			}
			if(final_answer[r].ss[SZ(final_answer[r].ss)-1].ss > 0)
			{
				cout << "From " << final_answer[r].ss[SZ(final_answer[r].ss)-1].ff; 
				cout << " go to " << destination << " with speed "; 
				cout << final_answer[r].ss[SZ(final_answer[r].ss)-1].ss << endl;
			}
			else
			{
				cout << "Charge your car at " << final_answer[r].ss[SZ(final_answer[r].ss)-1].ff << " for " 
				<< (double)-1*final_answer[r].ss[SZ(final_answer[r].ss)-1].ss << " units" << endl;
			}
			updateGraph(final_answer[r].ss);
		}
	}
}

int main()
{
    SYNC

	hyperParameterLambda = 10e-10;
    hyperParameterDistance = 0.0;
    energyMultiplier.pb(7.0); energyMultiplier.pb(4.0); energyMultiplier.pb(6.0); energyMultiplier.pb(8.0);
    timeMultiplier.pb(1.0/20.0); timeMultiplier.pb(1.0/40.0); timeMultiplier.pb(1.0/60.0); timeMultiplier.pb(1.0/80.0);
    congestionpercar = 20.0;
    
	getGraphInput();
	getRequest();
	solve();	

}	