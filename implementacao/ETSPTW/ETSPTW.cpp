#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;

#define ll long long

const ll INF = 1e18;

struct Node
{
    ll layer, city;
    ll visited;
    set<ll> Smenos, Smais;
    double batteryLevel;
    ll recharge;

    Node() {};
    Node(ll layer, ll city, ll visited, const set<ll> &Smenos, const set<ll> &Smais, double batteryLevel, ll recharge) : layer(layer), city(city), visited(visited), Smenos(Smenos), Smais(Smais), batteryLevel(batteryLevel), recharge(recharge) {};

    bool operator==(const Node &other) const {
        return (layer == other.layer && city == other.city && visited == other.visited && 
                Smenos == other.Smenos && Smais == other.Smais && batteryLevel == other.batteryLevel, recharge == other.recharge);
    }

    bool operator<(const Node &other) const {
        return tie(layer, city, visited, Smenos, Smais, batteryLevel, recharge) < tie(other.layer, other.city, other.visited, other.Smenos, other.Smais, other.batteryLevel, other.recharge);
    }
};

struct Costumer
{
    ll id, x, y, timeWindowStart, timeWindowEnd;

    Costumer() {};
    Costumer(ll id, ll x, ll y, ll timeWindowStart, ll timeWindowEnd) : id(id), x(x), y(y), timeWindowStart(timeWindowStart), timeWindowEnd(timeWindowEnd) {};
};

struct RechargingStation
{
    ll id, x, y, timeWindowStart, timeWindowEnd;

    RechargingStation() {};
    RechargingStation(ll id, ll x, ll y, ll timeWindowStart, ll timeWindowEnd) : id(id), x(x), y(y), timeWindowStart(timeWindowStart), timeWindowEnd(timeWindowEnd) {};
};

map<ll, Node> nodes;
map<Node, ll> nodeIndex;

ll allVisited;
ll numberOfCustomers, numberOfRechargingStations;
ll nextIndex, sourceIndex, sinkIndex;
double consumptionRate, rechargingRate, batteryCapacity;

vector<vector<double>> distancia;
vector<ll> k, p, jL, jR;
vector<Costumer> costumers;
vector<RechargingStation> rechargingStations;

ll layerAux = 0, sumAux = 0;

ll roundBattery(double x) {
    return round(x)+1;
}

bool hasBattery(Node node) {
    return node.batteryLevel >= 0.0;
}

bool viable(const Node& node){
    if(!hasBattery(node)) return false;
    if(node.layer == numberOfCustomers) return true;

    ll i = node.layer + 1;

    for(ll v = 2; v <= i; v++) {
        if(!(node.visited & (1ll << (v - 1))) && node.city >= v+k[v]) return false;
    }
    
    return true;
}

void withoutRechargeStation(Node &node, vector<pair<ll,double>> &adjCities, queue<ll> &q, Node &previous) {
    node.batteryLevel = previous.batteryLevel - consumptionRate*distancia[previous.city][node.city];

    if(!viable(node)) return;

    if(nodeIndex.find(node) != nodeIndex.end()) {
        adjCities.push_back({nodeIndex[node], distancia[previous.city][node.city]});
    } else {
        ll id = nextIndex++;
        nodeIndex[node] = id;
        nodes[id] = node;
        adjCities.push_back({id, distancia[previous.city][node.city]});
        q.push(id);
    }
}

void withRechargeStation(Node &node, vector<pair<ll,double>> &adjCities, queue<ll> &q, Node &previous) {
    if(!viable(node)) return;

    vector<pair<Node, double>> comparation;
    for(auto rs : rechargingStations) {
        Node nodeAux = node;
        double rechargingTime = (batteryCapacity-nodeAux.batteryLevel)*rechargingRate;
        nodeAux.batteryLevel -= consumptionRate*distancia[previous.city][rs.id];
        if(!hasBattery(nodeAux)) continue;
        
        nodeAux.batteryLevel = batteryCapacity;
        nodeAux.batteryLevel -= consumptionRate*distancia[rs.id][nodeAux.city];
        if(!hasBattery(nodeAux)) continue;
        
        comparation.push_back({nodeAux, distancia[previous.city][rs.id]+distancia[rs.id][nodeAux.city]+rechargingTime});
    }

    for(ll i = 0; i < (ll)comparation.size(); i++) {
        bool flag = true;
        for(ll j = 0; j < (ll)comparation.size(); j++) {
            if(i == j) continue;
            if(comparation[j].first.batteryLevel < comparation[i].first.batteryLevel && comparation[j].second < comparation[i].second) {
                flag = false;
                break;
            }
        }

        if(flag) {
            if(nodeIndex.find(comparation[i].first) != nodeIndex.end()) {
                adjCities.push_back({nodeIndex[comparation[i].first], comparation[i].second});
            } else {
                ll id = nextIndex++;
                nodeIndex[comparation[i].first] = id;
                nodes[id] = comparation[i].first;
                adjCities.push_back({id, comparation[i].second});
                q.push(id);
            }
        }
    }
    
}

void returnToOrigin(Node node) {
    bool canReturn = false;
    double minDist = INF;
    
    if(node.batteryLevel - consumptionRate*distancia[node.city][1] >= 0.0) {
        canReturn = true;
        minDist = distancia[node.city][1];
    }

    for(auto rs : rechargingStations) {
        Node nodeAux = node;
        double rechargingTime = (batteryCapacity-nodeAux.batteryLevel)*rechargingRate;
        nodeAux.batteryLevel -= consumptionRate*distancia[node.city][rs.id];
        if(!hasBattery(nodeAux)) continue;
        
        nodeAux.batteryLevel = batteryCapacity;
        nodeAux.batteryLevel -= consumptionRate*distancia[rs.id][1];
        if(!hasBattery(nodeAux)) continue;
        
        canReturn = true;
        minDist = min(minDist, distancia[nodeAux.city][rs.id]+distancia[rs.id][1]+rechargingTime);
    }

    
    if(canReturn) cout << "1 " << sinkIndex << " " << minDist << endl;
    else cout << 0 << endl;

}

// indice / numero de adj / adj1 / adj2 / ...
void tsp()
{
    queue<ll> nextCitiesQueue;
    nextCitiesQueue.push(sourceIndex);

    while (!nextCitiesQueue.empty()) {
        ll id = nextCitiesQueue.front(); nextCitiesQueue.pop();
        cout << id << " ";

        Node l = nodes[id];

        if (l.visited == allVisited) {
            returnToOrigin(l);
            continue;
        }

        ll i = l.layer + 1;
        vector<pair<ll,double>> adjCities;

        for (ll j = jL[i]; j <= jR[i]; j++)
        {
            ll bit = 1LL << (j-1);
            if (j != l.city && !(l.visited & bit))
            {
                set<ll> SmenosAux = l.Smenos;
                set<ll> SmaisAux = l.Smais;

                if (l.city < (i - 1) && l.Smenos.find(i - 1) != l.Smenos.end()) {
                    SmaisAux.erase(l.city);
                    SmenosAux.erase(i - 1);
                }
                else if (l.city < (i - 1) && l.Smenos.find(i - 1) == l.Smenos.end()) {
                    SmaisAux.erase(l.city);
                    SmaisAux.insert(i - 1);
                }
                else if (l.city > (i - 1) && l.Smenos.find(i - 1) != l.Smenos.end()) {
                    SmenosAux.erase(i - 1);
                    SmenosAux.insert(l.city);
                }
                else if (l.city > (i - 1) && l.Smenos.find(i - 1) == l.Smenos.end())
                {
                    SmenosAux.insert(l.city);
                    SmaisAux.insert(i - 1);
                }

                Node nodeAux(i, j, l.visited | bit, SmenosAux, SmaisAux, l.batteryLevel, l.recharge);
                withoutRechargeStation(nodeAux, adjCities, nextCitiesQueue, l);
                withRechargeStation(nodeAux, adjCities, nextCitiesQueue, l);
            }
        }
        
        cout << adjCities.size() << " ";
        for(auto [to, dist] : adjCities) {
            cout << to << " " << dist << " ";
        }
        cout << endl;
        
    }
}

void getCostumers() {
    costumers.assign(numberOfCustomers+1, Costumer());
    costumers[0].timeWindowStart = costumers[0].timeWindowEnd = costumers[0].id = -1;
    for (ll i = 1; i <= numberOfCustomers; i++) {
        cin >> costumers[i].id >> costumers[i].x >> costumers[i].y >> costumers[i].timeWindowStart >> costumers[i].timeWindowEnd;
        costumers[i].id++;
    }
}

void getRechargingStations() {
    rechargingStations.resize(numberOfRechargingStations+1);
    rechargingStations[0] = RechargingStation(1, costumers[1].x, costumers[1].y, costumers[1].timeWindowStart, costumers[1].timeWindowEnd);

    for(ll i = 1; i <= numberOfRechargingStations; i++) {
        cin >> rechargingStations[i].id >> rechargingStations[i].x >> rechargingStations[i].y >> rechargingStations[i].timeWindowStart >> rechargingStations[i].timeWindowEnd;
        rechargingStations[i].id++;
    }
}

void getDistances() {
    distancia.assign(numberOfCustomers + numberOfRechargingStations + 1, vector<double>(numberOfCustomers + numberOfRechargingStations + 1, 0));
    for (ll i = 1; i <= numberOfCustomers + numberOfRechargingStations; i++) {
        for (ll j = 1; j <= numberOfCustomers + numberOfRechargingStations; j++) 
            cin >> distancia[i][j];
    }
}

bool sortMidpoll(const Costumer &a, const Costumer &b) {
    return a.timeWindowStart + a.timeWindowEnd < b.timeWindowStart + b.timeWindowEnd;
}

void getVariableK() {
    k.assign(numberOfCustomers + 1, 0);
    p.assign(numberOfCustomers + 1, 0);
    jL.assign(numberOfCustomers + 2, numberOfCustomers + 1);
    jR.assign(numberOfCustomers + 2, 0);

    sort(costumers.begin() + 2, costumers.end(), sortMidpoll);

    vector<vector<double>> distanciaAux = distancia;
    for(ll i = 1; i <= numberOfCustomers; i++) {
        for(ll j = 1; j <= numberOfCustomers; j++) {
            distanciaAux[i][j] = distancia[costumers[i].id][costumers[j].id];
        }
    }
    distancia = distanciaAux;

    for(ll i = 1; i <= numberOfCustomers; i++) {
        bool flag = false;
        ll j0 = numberOfCustomers + 1;
        for(ll j = i+1; j <= numberOfCustomers; j++) {
            if(!flag && costumers[j].timeWindowStart + distancia[j][i] > costumers[i].timeWindowEnd) {
                j0 = j;
                flag = true;
            } else if(costumers[j].timeWindowStart <= costumers[i].timeWindowEnd) {
                flag = false;
                j0 = numberOfCustomers + 1;
            }
        }
        k[i] = j0 - i;
    }
    k[1] = 1;

    for (ll j = 2; j <= numberOfCustomers; j++) {
        ll cnt = 0;
        for (ll l = 1; l < j; l++) {
            if (l + k[l] >= j + 1) cnt++;
        }
        p[j] = cnt;
    }

    for (ll i = 1; i <= numberOfCustomers+1; i++) {
        for (ll j = 1; j <= numberOfCustomers; j++) {
            if (j + k[j] >= i + 1) { jL[i] = j; break; }
        }
    }

    for (ll i = 1; i <= numberOfCustomers+1; i++) {
        for (ll j = numberOfCustomers; j >= 1; j--) {
            if (p[j] >= j - i) { jR[i] = j; break; }
        }
    }

    jL[1] = jR[1] = 1;
    jL[numberOfCustomers+1] = jR[numberOfCustomers+1] = 1;
}

void initializeNodes() {
    sourceIndex = 1;
    sinkIndex = 2;
    nextIndex = 3;

    nodes[sourceIndex] = Node(1,1,1LL, {}, {}, batteryCapacity, 0);
    nodeIndex[nodes[sourceIndex]] = sourceIndex;

    nodes[sinkIndex] = Node(numberOfCustomers+1,1,allVisited, {}, {}, batteryCapacity, 0);
    nodeIndex[nodes[sinkIndex]] = sinkIndex;
}

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    auto start = high_resolution_clock::now();

    cin >> numberOfCustomers;
    cin >> numberOfRechargingStations;
    cin >> batteryCapacity;
    cin >> consumptionRate;
    cin >> rechargingRate;

    numberOfCustomers++;
    allVisited = (1ll << numberOfCustomers) - 1;

    getCostumers();
    getRechargingStations();
    getDistances();
    getVariableK();
    initializeNodes();
    tsp();

    cout << 0 << endl;

    for (auto [i, node] : nodes) {
        cout << i << " " << node.layer << " " << node.city << " " << node.visited << " " << costumers[node.city].id << " " << node.batteryLevel << " ";
        
        cout << node.Smenos.size() << " ";
        for (ll s : node.Smenos)
            cout << s << " ";

        cout << node.Smais.size() << " ";
        for (ll s : node.Smais)
            cout << s << " ";

        cout << endl;
    }
    cout << 0 << endl;

    cout << numberOfCustomers << endl;
    for (ll i = 1; i <= numberOfCustomers; i++) {
        cout << costumers[i].timeWindowStart << " " << costumers[i].timeWindowEnd << endl;
    }

    auto end = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(end - start); 
    cout << "Tempo: " << duration.count() << " ms\n";

    return 0;
}