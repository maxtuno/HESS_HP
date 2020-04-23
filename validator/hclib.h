#include <stdbool.h>

/**
 * Reads graph file into an adjacency matrix
 * note that index 0 for rows and columns is not used
 * this is because nodes start numbering at 1
 */
int** read_graph(char* file, int *node_count, int *edge_count);

/**
 * free's up graph allocated space
 */
void free_graph(int** graph, int *node_count);

/**
 * Reads tour file into an list of nodes in the order
 * they should be visited by the tour
 */
int* read_tour(char* file, int *node_count);

/**
 * free's up tour allocated space
 */
void free_tour(int* tour);

/**
 * validate that the provided tour (node list) 
 * is a hamiltonian cycle for provided graph (adjacency matrix)
 */
bool hc_validate(int* tour, int tour_node_count, int** graph, int node_count, bool verbose);

/**
 * prints path seperated by commas
 */
void print_path(int* path, int* node_count);
