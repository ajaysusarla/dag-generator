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

#ifndef _GRAPH_H_
#define _GRAPH_H_

typedef struct graph Graph;
typedef struct vertex Vertex;
typedef struct edge Edge;

/* Graph */
struct graph {
        int count;
        Vertex *first;
        int (* compare)(void *arg1, void *arg2);
};

/* Vertex */
struct vertex {
        Vertex *next;
        void *data;
        int indegree;
        int outdegree;
        Edge *edge;
};

/* Edge */
struct edge {
        Vertex *dest;
        int weight;
        Edge *next;
};

/* Functions */
Graph *graph_init(int (* compare)(void *arg1, void *arg2));
void graph_free(Graph *graph);
int graph_new_vertex(Graph *graph, void *data);
int graph_delete_vertex(Graph *graph, void *data);
int graph_add_edge(Graph *graph, void *from, void *to, int weight);
void graph_print(Graph *graph);

#endif  /* _GRAPH_H_ */
