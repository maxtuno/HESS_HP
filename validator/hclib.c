#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "hclib.h"

#define LINESIZE 100000 /* maximum line size */

/* local var to turn debug messages on or off */
static bool debug = true;
static void dprint(char *msg) {
    if (debug) 
        printf("%s\n", msg);
}

/**
 * reads TSPLIF format graph from file name
 */
int** read_graph(char* file, int *node_count, int *edge_count)
{
    FILE * fp = fopen(file, "r");
    int** graph;
    *node_count = 0;
    *edge_count = 0;
   
    if ( fp != NULL )
    {
        char line [ LINESIZE ]; /* or other suitable maximum line size */
        const char header_delim[4] = " : ";
        const char edge_delim[2] = " ";
        bool reading_edges = false;
        char *token;

        /* read the header and find number of nodes */
        while ( !reading_edges && fgets ( line, LINESIZE, fp ) != NULL ) /* read a line */
        {

            /* Look for edge data marker */
            if (strncmp("EDGE_DATA_SECTION", line, 17) == 0)
            {
                reading_edges = true;
                break;
            }

            /* look for number of nodes */
            token = strtok(line, header_delim);    
            if (token != NULL && strcmp("DIMENSION", token) == 0)
            {
                token = strtok(NULL, header_delim);    
                *node_count = atoi(token);
            }
        }

        /* initialize graph matrix to 0s 
         * note that column and rows with index 0 are not used
         * since nodes have numbers starting at 1
         **/
        graph = malloc( ((*node_count)+1) * sizeof(int*));
        for (int i = 0; i <= *node_count; i++) 
        {
            graph[i] = malloc( ((*node_count)+1) * sizeof(int*));

            for (int j = 0; j <= *node_count; j++)
            {
                graph[i][j] = 0;
            }
        }

        /* read the edges */
        while ( fgets ( line, LINESIZE, fp ) != NULL ) /* read a line */
        {
            int u = 0;
            int v = 0;
            token = strtok(line, edge_delim);    
            if (token != NULL)
            {
                u = atoi(token);
            }

            if (u == -1) 
            {
                break;
            }

            token = strtok(NULL, edge_delim);    
            if (token != NULL)
            {
                v = atoi(token);
            }

            if (u > 0 && v > 0) 
            {
                graph[u][v] = 1;
                graph[v][u] = 1;
                (*edge_count)++;
                //printf("Edge from %d to %d\n", u,v);
            }

        }
        
        fclose(fp);
    }

    return graph;
}


/**
 * clean up graph's heap memory
 */
void free_graph(int **graph, int *node_count)
{

    if (graph == NULL || ! (*node_count > 0) )
    {
        return;
    } 

    for (int i = 0; i <= *node_count; i++) 
    {
        free(graph[i]);
    }
    free(graph);

}

/**
 * read TSPLIB tour file format (node list)
 */
int* read_tour(char* file, int *node_count)
{
    FILE * fp = fopen(file, "r");
    int* tour;
    *node_count = 0;
   
    if ( fp != NULL )
    {
        char line [ LINESIZE ]; /* or other suitable maximum line size */
        const char header_delim[4] = " : ";
        const char node_delim[2] = " ";
        bool reading_nodes = false;
        char *token;

        /* read the header and find number of nodes */
        while ( !reading_nodes && fgets ( line, LINESIZE, fp ) != NULL ) /* read a line */
        {

            /* Look for edge data marker */
            if (strncmp("TOUR_SECTION", line, 12) == 0)
            {
                reading_nodes = true;
                break;
            }

            /* look for number of nodes */
            token = strtok(line, header_delim);    
            if (token != NULL && strcmp("DIMENSION", token) == 0)
            {
                token = strtok(NULL, header_delim);    
                *node_count = atoi(token);
            }
        }

        /* initialize tour nodes to 0s */
        tour = malloc( *node_count * sizeof(int*));
        for (int i = 0; i < *node_count; i++) 
        {
            tour[i] = 0;
        }

        /* read the nodes */
        int current_pos = 0; 
        bool detected_end_node = false;
        while ( !detected_end_node && fgets ( line, LINESIZE, fp ) != NULL ) /* read a line */
        {
            token = strtok(line, node_delim);    
            while (token != NULL)
            {
                int node = 0;
                node = atoi(token);
                if (node == -1) 
                {
                    detected_end_node = true;
                    break;
                }

                if (node > 0 && node <= *node_count) 
                {
                    tour[current_pos] = node;
                    current_pos++;
                } else 
                {
                    fprintf(stderr, "Node %d is out of range\n", node);
                }

                // get next node on same line
                token = strtok(NULL, node_delim);
            }

        }

        if (current_pos != (*node_count)) {
            fprintf(stderr, "Not all nodes specified in tour solution expected %d found %d \n", *node_count, current_pos);
        } //if
        
        fclose(fp);
    }

    return tour;
}


/**
 * clean up tour heap memory
 */
void free_tour(int *tour)
{
    free(tour);
}

/**
 * checks that provided tour is a valid Hamiltonian Cycle for graph
 */
bool hc_validate(int* tour, int tour_node_count, int** graph, int node_count, bool verbose)
{

    if (tour_node_count != node_count)
    {
        if (verbose)
            printf("Tour and graph do not contain same number of nodes\n");

        return false;
    }

    if (node_count == 0)
    {
        if (verbose)
            printf("graph has no nodes\n");

        return false;
    }

    bool valid = true;

    /* track which nodes have been visited */
    int* visited_nodes;
    visited_nodes = malloc( (node_count+1) * sizeof(int*));
    for (int i = 0; i <= node_count; i++) 
    {
        visited_nodes[i] = 0;
    }

    int current_node; 
    int previous_node = -1; 
    int visited_node_count = 0; 
    for (int current_pos = 0; current_pos < node_count; current_pos++)
    {
        current_node = tour[current_pos]; 

        /* make sure previous node is
         * connected to current node
         * unless there is no prev node
         */
        if (previous_node != -1)
        {
            if (graph[previous_node][current_node] == 0) 
            {
                if (verbose)
                    printf("No edge between %d and %d\n", previous_node, current_node);

                valid = false;
                break;
            }
        }

        /* make sure we did not visit
         * this node already
         */
        if (visited_nodes[current_node] == 0) 
        {
            visited_nodes[current_node] = 1;
            visited_node_count++;
        }

        previous_node = current_node;
    }
    free(visited_nodes);

    /* if still valid check that we visited all nodes */
    if (valid && visited_node_count != node_count)
    {
        if (verbose)
            printf("Not all nodes visited (visited %i, expected %i)\n", visited_node_count, node_count);

        valid = false;
    }

    /* if still valid we have a valid Hamiltonian cycle*/
    if (valid && verbose)
    {
        if (verbose)
            printf("Valid Hamiltoninan Path\n");
    }

    /* check there is a edge to complete the tour*/
    if (valid)
    {
        int firstNode = tour[0];
        int lastNode = tour[ (node_count - 1) ];

        if (graph[firstNode][lastNode] == 0) 
        {
            if (verbose)
                printf("First node does not connect with last node of tour\n");

            valid = false;
        }
    }

    /* if still valid we have a valid Hamiltonian cycle*/
    if (valid && verbose)
    {
        if (verbose)
            printf("Valid Hamiltoninan Cycle\n");
    }
    
    return valid;
}

/**
 * Prints a comma seperated path
 */
void print_path(int* path, int* node_count)
{
    printf("%i", path[0]);
    for (int i = 1; i < *node_count; i++) {
        printf(", %i", path[i]);
    } //for
    printf("\n");
}
