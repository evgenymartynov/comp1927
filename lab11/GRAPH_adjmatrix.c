#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "GRAPH.h"

Edge EDGE(int v, int w) {
    Edge e = {v, w};
    return e;
}

struct graph {
    int V;
    int E;
    int **adj;
};

Graph GRAPHinit(int V) {
    int i, j;
    Graph g = malloc(sizeof(*g));
    g->V = V;
    g->E = 0;

    // allocate space for V pointer
    g->adj = malloc(V * sizeof(*g->adj));
    for (i=0; i < V; i++) {
        // allocate space for V edges
        g->adj[i] = malloc(V * sizeof(*g->adj[i]));
        for (j = 0; j < V; j++) {
            g->adj[i][j] = 0;
        }
    }
    return g;
}

void GRAPHinsertE(Graph g, Edge e){
    if (g->adj[e.v][e.w] == 0) {
        g->E++;
        g->adj[e.v][e.w] = 1;
        g->adj[e.w][e.v] = 1;
    }
}


void GRAPHremoveE(Graph g , Edge e){
    if (g->adj[e.v][e.w] == 1) {
        g->E--;
        g->adj[e.v][e.w] = 0;
        g->adj[e.w][e.v] = 0;
    }
}


int GRAPHedges(Edge edges[], Graph g) {
    int i,j, noOfEdges;
    noOfEdges = 0;
    for (i = 1; i < g->V; i++) {
        for (j = 0; j < i; j++) {
            if (g->adj[i][j] == 1) {
                edges[noOfEdges].v = i;
                edges[noOfEdges].w = j;
                noOfEdges++;
            }
        }
    }
    return noOfEdges;
}


Graph GRAPHcopy(Graph g) {
// not yet implemented
    return NULL;
}

void GRAPHdestroy(Graph g) {
// not yet implemented
}


int GRAPHedgeScan(Edge *edge) {
    if (edge == NULL) {
        printf("GRAPHedgeScan: called with NULL \n");
        abort();
    }

    if (scanf("%d", &edge->v) == 1 && scanf("%d", &edge->w) == 1) {
        return 1;
    } else {
        return 0;
    }
}


void GRAPHEdgePrint(Edge edge) {
    printf("%d -- %d", edge.v, edge.w);
}

#if false
#undef true
#undef false
#define true 1
#define false 0
#endif

static bool dfs(Graph g, bool *seen, int v, int w) {
    if (v == w)  return true;
    if (seen[v]) return false;
    seen[v] = true;

    int to;
    for (to = 0; to < g->V; to++) {
        if (g->adj[v][to] && dfs(g, seen, to, w)) {
            return true;
        }
    }

    return false;
}

bool GRAPHpath(Graph g, int v, int w) {
    assert(0 <= v && v < g->V);
    assert(0 <= w && w < g->V);

    bool *seen = calloc(g->V, sizeof(bool));
    bool connected = dfs(g, seen, v, w);
    free(seen);

    return connected;
}
