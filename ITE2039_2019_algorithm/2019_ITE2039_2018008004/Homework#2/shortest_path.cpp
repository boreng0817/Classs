#include <iostream>
#include <algorithm>
#include <queue>
#include <vector>
#include <utility>
#include <cstdio>
#include <sstream>
#include <string>

#define pll pair<long long int,long long int>
#define INF (long long int)1e17
using namespace std;

void dijkstra(int V, int E);

int main() {
    int V, E;
    string str;

    while (getline(cin, str) && str != "") {

        istringstream iss(str);
        iss >> V >> E;
        dijkstra(V, E);
    }

    return 0;
}

void dijkstra(int V, int E) {

    int i, src, dest, weight;
    vector<long long int> d(V + 1, INF);
    vector<pll > G[V + 1];
    priority_queue<pll, vector<pll >,greater<pll > > heap;

    char enter;

    if (V == 0 || E == 0) {
        printf("%s\n", "invalid input\n");
        return;
    }

    for (i = 1; i <= E; ++i) {
        scanf("%d %d %d",&src, &dest, &weight);
        G[src].push_back(make_pair(dest,weight));
        G[dest].push_back(make_pair(src,weight));
    }
    scanf("%c", &enter);

    d[1] = 0;
    heap.push(make_pair(d[1], 1));

    while (!heap.empty()) {
        pll node = heap.top();
        heap.pop();

        for (i = 0; i < G[node.second].size(); ++i) {
            pll temp = G[node.second][i];
            if (d[node.second] + temp.second < d[temp.first]) {
                d[temp.first] = d[node.second] + temp.second;
                heap.push(make_pair(d[temp.first], temp.first));
            }
        }
    }

    printf("%lld\n", d[V]);

    return;
}
