/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "utils.h"

#ifndef _GRAPH_H_
#define _GRAPH_H_

typedef struct graph Graph;
typedef struct vertex Vertex;
typedef struct edge Edge;

/* Graph */
struct graph {
        int count;
        int max_edges;          /* Max edges per Vertex */
        Vertex *first;
        int (* compare)(void *arg1, void *arg2);
};

/* Vertex */
struct vertex {
        Vertex *next;
        void *data;
        int indegree;
        int outdegree;
        bool processed;
        bool visited;
        Edge **edges;
};

/* Edge */
struct edge {
        Vertex *dest;
        int weight;
};

/* Functions */
Graph *graph_init(int (* compare)(void *arg1, void *arg2), int max_edges);
void graph_free(Graph *graph);
int graph_new_vertex(Graph *graph, void *data);
int graph_delete_vertex(Graph *graph, void *data);
Vertex * graph_get_vertex(Graph *graph, void *data);
int graph_add_edge(Graph *graph, void *from, void *to, int weight);
int graph_delete_edge(Vertex *from, Vertex *to, int weight);
void graph_print(Graph *graph);
void graph_print_dot(Graph *graph, FILE *fp);


#define print_graph(g) do {                     \
                graph_print_dot(g, NULL);       \
                fflush(stdout);                 \
        } while(0)                              \

#endif  /* _GRAPH_H_ */
