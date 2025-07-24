#include <bits/stdc++.h>
using namespace std;

struct Node
{
    int layer, city;
    long long visited;
    set<int> Smenos, Smais;

    Node() {};
    Node(int layer, int city, long long visited, const set<int> &Smenos, const set<int> &Smais) : layer(layer), city(city), visited(visited), Smenos(Smenos), Smais(Smais) {};

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
    int a, b, ind;

    TimeWindow() {};
    TimeWindow(int a, int b) : a(a), b(b) {};
};

map<long long, Node> nodes;
map<Node, long long> nodeIndex;

long long allVisited;
int numberOfCities;

vector<vector<double>> distances;
vector<TimeWindow> timeWindows;
vector<int> k, p, jL, jR;

long long nextIndex, sourceIndex, sinkIndex;

bool viable(const Node& node){
    if(node.layer == numberOfCities) return true;

    int i = node.layer + 1;

    for(int v = 2; v <= i; v++) {
        if(!(node.visited & (1ll << (v - 1))) && node.city >= v+k[v]) return false;
    }
    
    return true;
}

void addAdj(Node &node, vector<int> &adjCities, queue<long long> &q) {
    if(!viable(node)) return;

    if(nodeIndex.find(node) != nodeIndex.end()) {
        adjCities.push_back(nodeIndex[node]);
    } else {
        long long id = nextIndex++;
        nodeIndex[node] = id;
        nodes[id] = node;
        adjCities.push_back(id);
        q.push(id);
    }
}

// indice / numero de adj / adj1 / adj2 / ...
void tsp()
{
    queue<long long> q;
    q.push(sourceIndex);

    while (!q.empty()) {
        long long id = q.front(); q.pop();
        cout << id << " ";

        Node l = nodes[id];

        if (l.visited == allVisited) {
            cout << "1 " << sinkIndex << " " << distances[l.city][1] << endl;
            continue;
        }

        int i = l.layer + 1;
        vector<int> adjCities;

        for (int j = jL[i]; j <= jR[i]; j++)
        {
            long long bit = 1ll << (j-1);
            if (j != l.city && !(l.visited & bit))
            {
                set<int> SmenosAux = l.Smenos;
                set<int> SmaisAux = l.Smais;

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
        for(int v : adjCities) {
            cout << v << " " << distances[l.city][nodes[v].city] << " ";
        }
        cout << endl;
        
    }
}

void getDistances() {
    distances.assign(numberOfCities + 1, vector<double>(numberOfCities + 1, 0));
    for (int i = 1; i <= numberOfCities; i++) {
        for (int j = 1; j <= numberOfCities; j++) cin >> distances[i][j];
    }
}

void getTimeWindows() {
    timeWindows.assign(numberOfCities+1, TimeWindow());
    timeWindows[0].a = timeWindows[0].b = timeWindows[0].ind = -1;
    for (int i = 1; i <= numberOfCities; i++) {
        cin >> timeWindows[i].a >> timeWindows[i].b;
        timeWindows[i].ind = i;
    }
    timeWindows[1].a = timeWindows[1].b = 0;
}

bool sortMidpoint(const TimeWindow &a, const TimeWindow &b) {
    return a.a + a.b < b.a + b.b;
}

void getVariableK() {
    k.assign(numberOfCities + 1, 0);
    p.assign(numberOfCities + 1, 0);
    jL.assign(numberOfCities + 2, numberOfCities + 1);
    jR.assign(numberOfCities + 2, 0);

    sort(timeWindows.begin() + 2, timeWindows.end(), sortMidpoint);

    // Mudança da matriz de distâncis pós mudança nos índices na ordenação das janelas de tempo
    vector<vector<double>> distancesAux(numberOfCities + 1, vector<double>(numberOfCities + 1, 0));
    for(int i = 1; i <= numberOfCities; i++) {
        for(int j = 1; j <= numberOfCities; j++) {
            distancesAux[i][j] = distances[timeWindows[i].ind][timeWindows[j].ind];
        }
    }

    distances = distancesAux;

    for(int i = 1; i <= numberOfCities; i++) {
        bool flag = false;
        int j0 = numberOfCities + 1;
        for(int j = i+1; j <= numberOfCities; j++) {
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

    for (int j = 2; j <= numberOfCities; j++) {
        int cnt = 0;
        for (int l = 1; l < j; l++) {
            if (l + k[l] >= j + 1) cnt++;
        }
        p[j] = cnt;
    }
    
    for (int i = 1; i <= numberOfCities+1; i++) {
        for (int j = 1; j <= numberOfCities; j++) {
            if (j + k[j] >= i + 1) { jL[i] = j; break; }
        }
    }

    for (int i = 1; i <= numberOfCities+1; ++i) {
        for (int j = numberOfCities; j >= 1; --j) {
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

    cin >> numberOfCities;
    allVisited = (1ll << numberOfCities) - 1;

    getDistances();
    getTimeWindows();
    getVariableK();
    initializeNodes();
    tsp();

    cout << 0 << endl;

    // numero / layer / cidade / visitados / s-.size() / s- / s+.size() / s+ / start Timewindow / end TimeWindow
    for (auto [i, node] : nodes) {
        cout << i << " " << node.layer << " " << node.city << " " << node.visited << " ";

        cout << node.Smenos.size() << " ";
        for (int s : node.Smenos)
            cout << s << " ";
        
        cout << node.Smais.size() << " ";
        for (int s : node.Smais)
            cout << s << " ";

        cout << timeWindows[node.city].a << " " << timeWindows[node.city].b;
        cout << endl;
    }

    return 0;
}