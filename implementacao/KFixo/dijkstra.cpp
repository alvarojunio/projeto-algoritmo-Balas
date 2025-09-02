#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;

const long long INF = 1e9;
const int maxE = 100000;

struct Node
{
    int layer, city;
    long long visited;
    set<int> Smenos, Smais;

    Node() {};
    Node(int layer, int city, long long visited, set<int> Smenos, set<int> Smais) : 
        layer(layer), 
        city(city), 
        visited(visited), 
        Smenos(Smenos), 
        Smais(Smais) {};

    bool operator==(const Node &n1) const {
        return (layer == n1.layer && city == n1.city && visited == n1.visited &&
                Smenos == n1.Smenos && Smais == n1.Smais);
    }

    bool operator<(const Node &n1) const {
        return tie(layer, city, visited, Smenos, Smais) < tie(n1.layer, n1.city, n1.visited, n1.Smenos, n1.Smais);
    }
};

map<int, Node> nodes;
map<Node, int> nodeIndex;

vector<vector<pair<double,int>>> adj;

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
            double newDist = dist + len;
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
    adj.resize(1);
    int ind, numAdj, adjNode;
    double weight;

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

    int layer, city, SmenosSize, SmaisSize, SmenosI, SmaisI;
    long long visited;

    while(cin >> ind) {
        cin >> layer >> city >> visited;

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

        nodes[ind] = Node(layer, city, visited, Smenos, Smais);
    }

    vector<int> parent;
    vector<double> dist = dijkstra(1, parent);

    cout << "Distancia minima ate o nó 2 (nó 2 representa o nó final): " << dist[2] << endl;

    vector<int> path = getPath(2, parent);

    cout << "Caminho minimo ate o nó 2: ";
    for (int v : path) cout << nodes[v].city << " ";
    cout << endl;

    cout << "Distâncias: " << endl;
    for (int v : path) cout << dist[v] << " ";
    cout << endl;

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start); 
    cout << "Tempo: " << duration.count() << " ms\n";

    return 0;
}