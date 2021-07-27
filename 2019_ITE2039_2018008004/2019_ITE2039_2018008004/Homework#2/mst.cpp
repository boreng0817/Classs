#include <vector>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <string>
#include <sstream>

using namespace std;

typedef struct edge {
    int weight;
    int src;
    int dest;

    bool operator<(edge b) {
        return weight < b.weight;
    }
} edge;


void Union(int a, int b);
int Find(int a);
int kruskal(int N, int M);
int *disjoint_set;

int main() {
    string str;
    int N, M;

    while (getline(cin, str) && str != "") {
        istringstream iss(str);
        iss >> N >> M;
        kruskal(N, M);
    }

    return 0;

}

int kruskal(int N, int M) {

    int i, sum = 0;
    char enter;

    if (N == 0 || M == 0) {
        printf("invalid input\n");
        return 0;
    }

    vector<edge> edges(M);
    
    disjoint_set = new int[N + 1];

    for (i = 1; i <= N; ++i)
        disjoint_set[i] = 0;

    for (i = 0; i < M; ++i)
        scanf("%d %d %d", &edges[i].src, &edges[i].dest, &edges[i].weight);
    scanf("%c", &enter);

    sort(edges.begin(), edges.end());

    for (i = 0; i < M; ++i) {
        edge E = edges[i];

        if (Find(E.src) != Find(E.dest)) {
            sum += E.weight;
            Union(Find(E.src), Find(E.dest));
        }
    }

    printf("%d\n", sum);
	
	delete[] disjoint_set;	
    return 0;
}

void Union(int a, int b) {
    disjoint_set[b] = a;
}

int Find(int a) {
    if (disjoint_set[a] <= 0)
        return a;
    else
        return (disjoint_set[a] = Find(disjoint_set[a]));
}
