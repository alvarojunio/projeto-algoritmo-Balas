#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;

#define ll long long

struct Node
{
    ll layer, city;
    ll visited;
    set<ll> Smenos, Smais;

    Node() {};
    Node(ll layer, ll city, ll visited, const set<ll> &Smenos, const set<ll> &Smais) : layer(layer), city(city), visited(visited), Smenos(Smenos), Smais(Smais) {};

    bool operator==(const Node &n1) const {
        return (layer == n1.layer && city == n1.city && visited == n1.visited && 
                Smenos == n1.Smenos && Smais == n1.Smais);
    }

    bool operator<(const Node &n1) const {
        return tie(layer, city, visited, Smenos, Smais) < tie(n1.layer, n1.city, n1.visited, n1.Smenos, n1.Smais);
    }
};

struct TimeWindow
{
    ll a, b, ind;

    TimeWindow() {};
    TimeWindow(ll a, ll b) : a(a), b(b) {};
};

map<ll, Node> nodes;
map<Node, ll> nodeIndex;

ll allVisited;
ll numberOfCities;

vector<vector<double>> distances;
vector<TimeWindow> timeWindows, originalTimeWindows;
vector<ll> k, p, jL, jR;

ll nextIndex, sourceIndex, sinkIndex;

bool viable(const Node& node){
    if(node.layer == numberOfCities) return true;

    ll i = node.layer + 1;

    for(ll v = 2; v <= i; v++) {
        if(!(node.visited & (1ll << (v - 1))) && node.city >= v+k[v]) return false;
    }
    
    return true;
}

void addAdj(Node &node, vector<ll> &adjCities, queue<ll> &nextCitiesQueue) {
    if(!viable(node)) return;

    if(nodeIndex.find(node) != nodeIndex.end()) {
        adjCities.push_back(nodeIndex[node]);
    } else {
        ll id = nextIndex++;
        nodeIndex[node] = id;
        nodes[id] = node;
        adjCities.push_back(id);
        nextCitiesQueue.push(id);
    }
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
            cout << "1 " << sinkIndex << " " << distances[l.city][1] << endl;
            continue;
        }

        ll i = l.layer + 1;
        vector<ll> adjCities;

        for (ll j = jL[i]; j <= jR[i]; j++)
        {
            ll bit = 1ll << (j-1);
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

                Node nodeAux(i, j, l.visited | bit, SmenosAux, SmaisAux);
                addAdj(nodeAux, adjCities, nextCitiesQueue);
            }
        }
        
        cout << adjCities.size() << " ";
        for(ll v : adjCities) {
            cout << v << " " << distances[l.city][nodes[v].city] << " ";
        }
        cout << endl;
        
    }
}

void getDistances() {
    distances.assign(numberOfCities + 1, vector<double>(numberOfCities + 1, 0));
    for (ll i = 1; i <= numberOfCities; i++) {
        for (ll j = 1; j <= numberOfCities; j++) cin >> distances[i][j];
    }
}

void getTimeWindows() {
    timeWindows.assign(numberOfCities+1, TimeWindow());
    timeWindows[0].a = timeWindows[0].b = timeWindows[0].ind = -1;
    for (ll i = 1; i <= numberOfCities; i++) {
        cin >> timeWindows[i].a >> timeWindows[i].b;
        timeWindows[i].ind = i;
    }
    originalTimeWindows = timeWindows;
    timeWindows[1].a = timeWindows[1].b = 0;
}

bool sortMidpoll(const TimeWindow &a, const TimeWindow &b) {
    return a.a + a.b < b.a + b.b;
}

void getVariableK() {
    k.assign(numberOfCities + 1, 0);
    p.assign(numberOfCities + 1, 0);
    jL.assign(numberOfCities + 2, numberOfCities + 1);
    jR.assign(numberOfCities + 2, 0);

    sort(timeWindows.begin() + 2, timeWindows.end(), sortMidpoll);
    sort(originalTimeWindows.begin() + 2, originalTimeWindows.end(), sortMidpoll);

    // Mudança da matriz de distâncias pós mudança nos índices na ordenação das janelas de tempo
    vector<vector<double>> distancesAux(numberOfCities + 1, vector<double>(numberOfCities + 1, 0));
    for(ll i = 1; i <= numberOfCities; i++) {
        for(ll j = 1; j <= numberOfCities; j++) {
            distancesAux[i][j] = distances[timeWindows[i].ind][timeWindows[j].ind];
        }
    }

    distances = distancesAux;

    for(ll i = 1; i <= numberOfCities; i++) {
        bool flag = false;
        ll j0 = numberOfCities + 1;
        for(ll j = i+1; j <= numberOfCities; j++) {
            if(!flag && timeWindows[j].a + distances[j][i] > timeWindows[i].b) {
                j0 = j;
                flag = true;
            } else if(timeWindows[j].a <= timeWindows[i].b) {
                flag = false;
                j0 = numberOfCities + 1;
            }
        }
        k[i] = j0 - i;
    }

    k[1] = 1;

    for (ll j = 2; j <= numberOfCities; j++) {
        ll cnt = 0;
        for (ll l = 1; l < j; l++) {
            if (l + k[l] >= j + 1) cnt++;
        }
        p[j] = cnt;
    }
    
    for (ll i = 1; i <= numberOfCities+1; i++) {
        for (ll j = 1; j <= numberOfCities; j++) {
            if (j + k[j] >= i + 1) { jL[i] = j; break; }
        }
    }

    for (ll i = 1; i <= numberOfCities+1; i++) {
        for (ll j = numberOfCities; j >= 1; j--) {
            if (p[j] >= j - i) { jR[i] = j; break; }
        }
    }

    jL[1] = jR[1] = 1;
    jL[numberOfCities+1] = jR[numberOfCities+1] = 1;
}

void initializeNodes() {
    sourceIndex = 1;
    sinkIndex = 2;
    nextIndex = 3;

    nodes[sourceIndex] = Node(1,1,1ll, {}, {});
    nodeIndex[nodes[sourceIndex]] = sourceIndex;

    nodes[sinkIndex] = Node(numberOfCities+1,1,allVisited, {}, {});
    nodeIndex[nodes[sinkIndex]] = sinkIndex;
}

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    auto start = high_resolution_clock::now();

    cin >> numberOfCities;
    allVisited = (1ll << numberOfCities) - 1;

    getDistances();
    getTimeWindows();
    getVariableK();
    initializeNodes();
    tsp();

    cout << 0 << endl;

    for (auto [i, node] : nodes) {
        cout << i << " " << node.layer << " " << node.city << " " << node.visited << " ";

        cout << node.Smenos.size() << " ";
        for (ll s : node.Smenos)
            cout << s << " ";
        
        cout << node.Smais.size() << " ";
        for (ll s : node.Smais)
            cout << s << " ";

        cout << endl;
    }
    cout << 0 << endl;

    cout << numberOfCities << endl;
    for (ll i = 1; i <= numberOfCities; i++) {
        cout << originalTimeWindows[i].a << " " << originalTimeWindows[i].b << " " << originalTimeWindows[i].ind << endl;
    }

    for (ll i = 1; i <= numberOfCities; i++) {
        cout << k[i] << " " << jL[i] << " " << jR[i] << endl;
    }

    auto end = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(end - start); 
    cout << "Tempo: " << duration.count() << " ms\n";

    
    return 0;
}