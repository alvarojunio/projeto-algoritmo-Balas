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
    int rechargingStationId;
    set<int> Smenos, Smais;

    Node() {};
    Node(int layer, int city, int realId, long long visited, double batteryLevel, int rechargingStationId, set<int> Smenos, set<int> Smais) : 
        layer(layer), 
        city(city), 
        realId(realId),
        visited(visited), 
        batteryLevel(batteryLevel),
        rechargingStationId(rechargingStationId),
        Smenos(Smenos), 
        Smais(Smais) {};

    bool operator==(const Node &n1) const {
        return (layer == n1.layer && city == n1.city && visited == n1.visited &&
                Smenos == n1.Smenos && Smais == n1.Smais && batteryLevel == n1.batteryLevel && rechargingStationId == n1.rechargingStationId);
    }

    bool operator<(const Node &n1) const {
        return tie(layer, city, visited, Smenos, Smais, batteryLevel, rechargingStationId) < tie(n1.layer, n1.city, n1.visited, n1.Smenos, n1.Smais, n1.batteryLevel, n1.rechargingStationId);
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
map<int,int> returnId;
vector<vector<double>> distances;

vector<vector<pair<double,int>>> adj;
vector<TimeWindow> timeWindows;
vector<int> k;

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
            /* 
                Nas próximas linhas ocorre a verificação da janela de tempo.
                A variável "newDist" recebe o máximo entre o valor da distância 
                até agora no tour + distância da aresta u,v e o valor do começo 
                da janela de tempo para aquela cidade, ou seja, se o caixeiro chegar
                antes do começo da janela de tempo, ele irá esperar. Depois é conferido
                se a distância ultrapassa o fim da janela de tempo, e, se ultrapassar,
                é descartado o resultado.
            */
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

vector<int> getPathWithRechargingStations(int target, const vector<int>& parent) {
    vector<int> path;
    for (int v = target; v != -1; v = parent[v]) {
        if(parent[v] == 1 && returnId.find(v) != returnId.end()) path.push_back(returnId[v]);
        path.push_back(nodes[v].city);
        if(nodes[v].rechargingStationId != -1) path.push_back(nodes[v].rechargingStationId);
    }
    reverse(path.begin(), path.end());
    return path;
}

vector<int> getOriginalPathWithRechargingStations(int target, const vector<int>& parent) {
    vector<int> path;
    for (int v = target; v != -1; v = parent[v]) {
        if(parent[v] == 1 && returnId.find(v) != returnId.end()) path.push_back(returnId[v]);
        path.push_back(nodes[v].realId);
        if(nodes[v].rechargingStationId != -1) path.push_back(nodes[v].rechargingStationId);
    }
    reverse(path.begin(), path.end());
    return path;
}

vector<int> getPath(int target, const vector<int>& parent) {
    vector<int> path;
    for (int v = target; v != -1; v = parent[v]) {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());
    return path;
}

signed main() {
    auto start = high_resolution_clock::now();
    int ind, numAdj, adjNode, numberOfCities, rechargingIdReturn;
    double weight;

    adj.assign(1, vector<pair<double,int>>());

    while(true) {
        cin >> ind;
        if(ind == 0) break;
        adj.resize(ind+1);
        cin >> numAdj;  
        for(int i = 0; i < numAdj; i++) {
            cin >> adjNode;
            if(adjNode == 2) {
                cin >> weight >> rechargingIdReturn;
                if(rechargingIdReturn != -1) returnId[ind] = rechargingIdReturn;
            } else cin >> weight;
            adj[ind].push_back({weight, adjNode});
        }
    }

    int layer, city, SmenosSize, SmaisSize, SmenosI, SmaisI, realId, rechargingId;
    long long visited;
    double batteryLevel;

    while(true) {
        cin >> ind;
        if(ind == 0) break;
        cin >> layer >> city >> visited >> realId >> batteryLevel >> rechargingId;
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

        nodes[ind] = Node(layer, city, realId, visited, batteryLevel,  rechargingId, Smenos, Smais);
    }

    cin >> numberOfCities;
    timeWindows.assign(numberOfCities+1, TimeWindow());
    k.assign(numberOfCities+1, 0);
    for(int i = 1; i <= numberOfCities; i++) {
        cin >> k[i] >> timeWindows[i].a >> timeWindows[i].b;
    }

    int nMatriz;
    cin >> nMatriz;
    distances.assign(nMatriz, vector<double>(nMatriz, 0));
    for(int i = 0; i < nMatriz; i++) {
        for(int j = 0; j < nMatriz; j++) {
            cin >> distances[i][j];
        }
    }

    vector<int> parent;
    vector<double> dist = dijkstra(1, parent);

    cout << "Distancia minima até o nó 2 (nó 2 representa o nó final): " << dist[2] << endl << endl;

    vector<int> path = getPath(2, parent);
    vector<int> pathRecharging = getPathWithRechargingStations(2, parent);
    vector<int> pathOriginalRecharging = getOriginalPathWithRechargingStations(2, parent);

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
    for (int v : path) cout << dist[v] << " ";
    cout << endl;

    cout << "Bateria: " << endl;
    for (int v : path) cout << nodes[v].batteryLevel << " ";
    cout << endl;

    cout << "Path com postos (ordenado): " << endl;
    for (int v : pathRecharging) cout << v << " ";
    cout << endl;

    cout << "Path com postos (original): " << endl;
    for (int v : pathOriginalRecharging) cout << v << " ";
    cout << endl;

    double caminhoTotal = 0;
    for(int i = 1; i < pathRecharging.size(); i++) {
        caminhoTotal += distances[pathRecharging[i-1]][pathRecharging[i]];
    }
    cout << "Caminho total com postos: " << caminhoTotal << endl;

    double caminhoTotalsemPostos = 0;
    for(int i = 1; i < path.size(); i++) {
        caminhoTotalsemPostos += distances[nodes[path[i-1]].city][nodes[path[i]].city];
    }
    cout << "Caminho total sem postos: " << caminhoTotalsemPostos << endl;

    cout << "k(i)" << endl;
    for(int i = 1; i <=numberOfCities; i++) cout << k[i] << " ";
    cout << endl;

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start); 
    cout << "Tempo: " << duration.count() << " ms\n";

    return 0;
}