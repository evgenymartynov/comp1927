#include <stdlib.h>
#include <stdio.h>
#include "GRAPH.h"

#define MAX_VERT 100

int main(int argc, char *argv[]) { 
    Edge e, *edges;
    Graph g;
    int graphSize, i, noOfEdges;

    if (argc < 2) {
        printf("Setting max. no of vertices to %d\n", MAX_VERT);
        graphSize = MAX_VERT;
    } else  { 
        graphSize = atoi(argv[1]);
    }

    g = GRAPHinit(graphSize);    

    while (GRAPHedgeScan(&e)) {
        GRAPHinsertE(g, e);
    }

    edges = malloc(sizeof (*edges) * MAX_VERT * MAX_VERT);
    noOfEdges = GRAPHedges(edges, g);
    printf ("Edges of the graph:\n");
    for (i = 0; i < noOfEdges; i++) {
        GRAPHEdgePrint(edges[i]);
        printf("\n");
    }

#define test(a, b) printf("%2.1d %2.1d: %d\n", a, b, GRAPHpath(g, a, b));
    test(1, 2);
    test(9, 10);
    test(9, 5);
    test(4, 0);

    return EXIT_SUCCESS;
}
