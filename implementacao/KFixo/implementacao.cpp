#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;

#define ll long long

struct Node
{
    ll layer, city, visited;
    set<ll> Smenos, Smais;

    Node() {};
    Node(ll layer, ll city, ll visited, set<ll> Smenos, set<ll> Smais) : layer(layer), city(city), visited(visited), Smenos(Smenos), Smais(Smais) {};

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
    ll a, b;

    TimeWindow() {};
    TimeWindow(ll a, ll b) : a(a), b(b) {};
};

map<ll, Node> nodes;
map<Node, ll> nodeIndex;

ll todosVisitados, numeroCidades, k;
ll nextIndex, sourceIndex, sinkIndex;

vector<vector<double>> distancia;

bool viable(Node &node) {
    if(node.layer == numeroCidades) return true;
    ll i = node.layer + 1;

    for(ll v = 2; v <= i; v++) {
        if(!(node.visited & (1ll << (v - 1))) && (node.city >= v+k)) return false;
    }

    return true;
}

void addAdj(Node &node, vector<ll> &adjCities, queue<ll> &q) {
    if(!viable(node)) return;

    if(nodeIndex.find(node) != nodeIndex.end()) {
        adjCities.push_back(nodeIndex[node]);
    } else {
        ll id = nextIndex++;
        nodeIndex[node] = id;
        nodes[id] = node;
        adjCities.push_back(id);
        q.push(id);
    }
}

// indice / numero de adj / adj1 / adj2 / ...
void tsp()
{
    queue<ll> q;
    q.push(1);

    while (!q.empty())
    {
        ll n = q.front(); q.pop();
        cout << n << " ";

        Node l = nodes[n];

        if (l.visited == todosVisitados) {
            cout << "1 " << sinkIndex << " " << distancia[l.city][1] << endl;
            continue;
        }

        ll i = l.layer + 1;
        vector<ll> adjCities;

        for (ll j = max(2LL, i - k + 1); j <= min(numeroCidades, i + k - 1); j++)
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

                Node nodeAux(i, j, l.visited | bit, SmenosAux, SmaisAux);
                addAdj(nodeAux, adjCities, q);
            }
        }
        
        cout << adjCities.size() << " ";
        for(ll v : adjCities) {
            cout << v << " " << distancia[l.city][nodes[v].city] << " ";
        }
        cout << endl;
    }
}

void getDistances(){
    for (ll i = 1; i <= numeroCidades; i++) {
        for (ll j = 1; j <= numeroCidades; j++) cin >> distancia[i][j];
    }
}

void getTimeWindows(vector<TimeWindow> &timeWindows) {
    for (ll i = 1; i <= numeroCidades; i++) cin >> timeWindows[i].a >> timeWindows[i].b;
}

void initializeNodes() {
    sourceIndex = 1;
    sinkIndex = 2;
    nextIndex = 3;

    nodes[sourceIndex] = Node(1,1,1LL, {}, {});
    nodeIndex[nodes[sourceIndex]] = sourceIndex;

    nodes[sinkIndex] = Node(numeroCidades+1,1,todosVisitados, {}, {});
    nodeIndex[nodes[sinkIndex]] = sinkIndex;
}

signed main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    auto start = high_resolution_clock::now();

    cin >> numeroCidades;
    todosVisitados = (1ll << numeroCidades) - 1;
    k = 12;

    distancia.assign(numeroCidades + 1, vector<double>(numeroCidades + 1, 0));
    vector<TimeWindow> timeWindows(numeroCidades+1);

    getDistances();
    getTimeWindows(timeWindows);
    initializeNodes();

    tsp();

    cout << 0 << endl;

    for (auto [i, node] : nodes)
    {
        cout << i << " " << node.layer << " " << node.city << " " << node.visited << " ";

        cout << node.Smenos.size() << " ";
        for (ll s : node.Smenos) cout << s << " ";

        cout << node.Smais.size() << " ";
        for (ll s : node.Smais) cout << s << " ";

        cout << endl;
    }

    cout << 0 << endl;
    cout << numeroCidades << endl;
    for(ll i = 0; i < numeroCidades; i++) {
        cout << timeWindows[i+1].a << " " << timeWindows[i+1].b << endl;
    }

    auto end = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(end - start); 
    cout << "Tempo: " << duration.count() << " ms\n";

    return 0;
}