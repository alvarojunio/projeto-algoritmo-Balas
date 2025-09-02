#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;

const long long INF = 1e9;

/* 
    O algoritmo de dijkstra foi modificado para obedecer às janelas de tempo de cada nó.
    Caso alguma rota chegar a um nó com uma distância menor do que o início da janela de tempo daquele nó, 
    a distância passa a ser o valor do início da janela, uma vez que o veículo precisa "esperar" no local para ser atendido.
*/

struct Node
{
    int layer, city, realId;
    long long visited;
    double batteryLevel;
    set<int> Smenos, Smais;

    Node() {};
    Node(int layer, int city, int realId, long long visited, double batteryLevel, set<int> Smenos, set<int> Smais) : 
        layer(layer), 
        city(city), 
        realId(realId),
        visited(visited), 
        batteryLevel(batteryLevel),
        Smenos(Smenos), 
        Smais(Smais) {};

    bool operator==(const Node &n1) const {
        return (layer == n1.layer && city == n1.city && visited == n1.visited &&
                Smenos == n1.Smenos && Smais == n1.Smais && batteryLevel == n1.batteryLevel);
    }

    bool operator<(const Node &n1) const {
        return tie(layer, city, visited, Smenos, Smais, batteryLevel) < tie(n1.layer, n1.city, n1.visited, n1.Smenos, n1.Smais, batteryLevel);
    }
};

struct TimeWindow
{
    int a, b;

    TimeWindow() {};
    TimeWindow(int a, int b) : a(a), b(b) {};
};

map<int, Node> nodes;
map<Node, int> nodeIndex;

vector<vector<pair<double,int>>> adj;
vector<TimeWindow> timeWindows;

vector<double> dijkstra(int s, vector<int>& parent) {
    int n = adj.size();
    vector<double> d(n+1, INF);
    parent.assign(n+1, -1);
    d[s] = 0.0;

    priority_queue<pair<double,int>, vector<pair<double,int>>, greater<pair<double,int>>> pq;
    pq.push({0, s});

    while (!pq.empty()) {
        auto [dist, v] = pq.top(); pq.pop();

        if (dist != d[v])
            continue;

        for (auto [len, next] : adj[v]) {
            Node nodeAux = nodes[next];
            double newDist = max((double)timeWindows[nodeAux.city].a, dist + len);
            if(newDist > timeWindows[nodeAux.city].b*1.0) continue;
            if (newDist < d[next]) {
                d[next] = newDist;
                parent[next] = v;
                pq.push({newDist, next});
            }
        }
    }
    return d;
}

vector<int> getPath(int target, const vector<int>& parent) {
    vector<int> path;
    for (int v = target; v != -1; v = parent[v])
        path.push_back(v);
    reverse(path.begin(), path.end());
    return path;
}

signed main() {
    auto start = high_resolution_clock::now();
    int ind, numAdj, adjNode, numberOfCities;
    double weight;

    adj.assign(1, vector<pair<double,int>>());

    while(true) {
        cin >> ind;
        if(ind == 0) break;
        adj.resize(ind+1);
        cin >> numAdj;  
        for(int i = 0; i < numAdj; i++) {
            cin >> adjNode >> weight;
            adj[ind].push_back({weight, adjNode});
        }
    }

    int layer, city, SmenosSize, SmaisSize, SmenosI, SmaisI, realId;
    long long visited;
    double batteryLevel;

    while(true) {
        cin >> ind;
        if(ind == 0) break;
        cin >> layer >> city >> visited >> realId >> batteryLevel;
        set<int> Smenos, Smais;
        cin >> SmenosSize;

        for(int i = 0; i < SmenosSize; i++) {
            cin >> SmenosI;
            Smenos.insert(SmenosI);
        }

        cin >> SmaisSize;

        for(int i = 0; i < SmaisSize; i++) {
            cin >> SmaisI;
            Smais.insert(SmaisI);
        }

        nodes[ind] = Node(layer, city, realId, visited, batteryLevel, Smenos, Smais);
    }

    cin >> numberOfCities;
    timeWindows.assign(numberOfCities+1, TimeWindow());
    for(int i = 1; i <= numberOfCities; i++) {
        cin >> timeWindows[i].a >> timeWindows[i].b;
    }

    vector<int> parent;
    vector<double> dist = dijkstra(1, parent);

    cout << "Distancia minima até o nó 2 (nó 2 representa o nó final): " << dist[2] << endl << endl;

    vector<int> path = getPath(2, parent);

    cout << "Caminho minimo até o nó 2 (Numero dos nós depois das arestas ordenadas): " << endl;
    for (int v : path) cout << nodes[v].city << " ";
    cout << endl << endl;

    cout << "Caminho minimo até o nó 2 (Número dos nós reais da instância): " << endl;
    for (int v : path) cout << nodes[v].realId << " ";
    cout << endl << endl;

    cout << "Caminho minimo até o nó 2 (Número dos nós reais da instância): " << endl;
    for (int v : path) cout << v << " ";
    cout << endl << endl;

    cout << "Distâncias: " << endl;
    for (int v : path) cout << dist[v] << '/' << nodes[v].batteryLevel << " ";
    cout << endl;

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start); 
    cout << "Tempo: " << duration.count() << " ms\n";

    return 0;
}