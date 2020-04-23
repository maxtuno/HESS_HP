/* 
 * validates hamiltionian cycle solutions
 */

#include <stdio.h>

#include "hclib.h"

static void print_usage();

int main(int argc, char *argv[])
{

    /* process command line arguments */
    char *graph_file;
    char *tour_file;
    if (argc > 1)
    {
        graph_file = argv[1];
        tour_file = argv[2];
    } else
    {
        print_usage();
        return 1;
    }

    /* read graph file */
    int **graph;
    int node_count;
    int edge_count;
    graph = read_graph(graph_file, &node_count, &edge_count);

    printf("Graph has %d nodes and %d edges\n", node_count, edge_count);


    /* read tour file */
    int *tour;
    int tour_node_count;
    tour = read_tour(tour_file, &tour_node_count);

    printf("Tour has %d nodes\n", tour_node_count);

    /* validate */
    hc_validate(tour, tour_node_count, graph, node_count, true); 

    /* clean up */
    free_graph(graph, &node_count);
    free_tour(tour);

    return 0;
}

/**
 * Prints help message on how to use the program
 */
static void print_usage()
{
    printf("Checks if tour is a valid Hamiltonian Path/Cycle for a gprah\n\n");

    printf("Usage:\n");
    printf("hc_check graphfile.hcp tourfile\n\n");
    
}
