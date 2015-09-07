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
typedef struct node Node;
typedef struct edge Edge;

/* Graph */
struct graph {
        int count;
        Node *first;
        int (* compare)(void *arg1, void *arg2);
};

/* Node */
struct node {
        Node *next;
        void *data;
        int indegree;
        int outdegree;
        Edge *edge;
};

/* Edge */
struct edge {
        Node *dest;
        int weight;
        Edge *next;
};

/* Functions */
Graph *graph_init(void);
void graph_free(Graph *graph);

#endif  /* _GRAPH_H_ */
