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

#include <stdio.h>
#include <stdlib.h>

#include "graph.h"

/* Internal Functions */
void _destroy_graph(Node *node)
{
        if (node) {
                /* TODO: Complete this */
        }

        return;
}

Graph *graph_new(void)
{
        Graph *graph = NULL;

        graph = (Graph *)malloc(sizeof(Graph));
        if (graph) {
                graph->count = 0;
                graph->first = NULL;
        }

        return graph;
}


void graph_free(Graph *graph)
{
        if (graph) {
                _destroy_graph(graph->first);

                free(graph);
        }

        return;
}

int graph_new_node(Graph *graph, void *data)
{
        Node *new = NULL;
        Node *tptr = NULL;

        /* Create new node */
        new = (Node *)malloc(sizeof(Node));
        if (!new) {
                fprintf(stderr, "[ERROR] Memory issue.\n");
                exit(EXIT_FAILURE);
        }

        new->next = NULL;
        new->data = data;
        new->indegree = 0;
        new->outdegree = 0;
        new->edge = NULL;

        /* Insert node into graph */
        tptr = graph->first;
        if (graph->first == NULL) {
                graph->first = new;
        }

        return 1;
}
