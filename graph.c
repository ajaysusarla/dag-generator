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
#include <string.h>

#include "graph.h"

/* Internal Functions */
static void _destroy_graph(Vertex *vert)
{
        if (vert) {
                /* TODO: Complete this */
        }

        return;
}

/*
 * graph_init:
 *
 * Arguments :
 * Returns   :
 */
Graph *graph_init(int (* compare)(void *arg1, void *arg2))
{
        Graph *graph = NULL;

        graph = (Graph *)malloc(sizeof(Graph));
        if (graph) {
                graph->count = 0;
                graph->first = NULL;
                /* If no compare() function is given, default to strcmp */
                if (compare != NULL)
                        graph->compare = compare;
                else
                        graph->compare = (int (*)(void *, void *))strcmp;
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

/*
 * Function: graph_new_vertex()
 * Arguments :
 * Return    :
 *             1  - success
 */
int graph_new_vertex(Graph *graph, void *data)
{
        Vertex *v_new = NULL;
        Vertex *v_cur = NULL;
        Vertex *v_prev;

        /* Create new node */
        v_new = (Vertex *)malloc(sizeof(Vertex));
        if (!v_new) {
                fprintf(stderr, "[ERROR] Memory issue.\n");
                exit(EXIT_FAILURE);
        }

        v_new->next = NULL;
        v_new->data = data;
        v_new->indegree = 0;
        v_new->outdegree = 0;
        v_new->edge = NULL;

        ++graph->count;

        /* Insert node into graph */
        v_cur = graph->first;
        if (!v_cur) {
                graph->first = v_new;
        } else {
                v_prev = NULL;

                while (v_cur && (graph->compare(data, v_cur->data) > 0)) {
                        v_prev = v_cur;
                        v_cur = v_cur->next;
                }

                if (!v_prev)
                        graph->first = v_new;
                else
                        v_prev->next = v_new;

                v_new->next = v_cur;
        }

        return 1;
}

/*
 * Function: graph_delete_vertex()
 * Arguments :
 * Return    :
 *             1  - success
 *             -1 - degree of vertex not 0
 *             -2 - vertex not found
 */
int graph_delete_vertex(Graph *graph, void *data)
{
        Vertex *v_prev = NULL;
        Vertex *v_walker = NULL;

        if (!graph->first)
                return -2;

        v_walker = graph->first;
        while (v_walker && (graph->compare(data, v_walker->data) > 0)) {
                v_prev = v_walker;
                v_walker = v_walker->next;
        }

        if (!v_walker || (graph->compare(data, v_walker->data) != 0))
                return -2;

        /* Found vertex. */
        if ((v_walker->indegree > 0) || (v_walker->outdegree > 0))
                return -1;

        if (!v_prev)
                graph->first = v_walker->next;
        else
                v_prev->next = v_walker->next;

        --graph->count;

        free(v_walker);


        return 1;
}

/*
 * Function: graph_add_edge()
 * Arguments :
 * Return    :
 *             1  - success
 *             -1 - degree of vertex not 0
 *             -2 - from vertex not found
 *             -3 - to vertex not found
 */
int graph_add_edge(Graph *graph, void *from, void *to, int weight)
{
        Edge *e_new;
        Edge *e_prev;
        Edge *e_walker;

        Vertex *v_from;
        Vertex *v_to;

        e_new = (Edge *)malloc(sizeof(Edge));
        if (!e_new) {
                fprintf(stderr, "[ERROR] Memory issue.\n");
                exit(EXIT_FAILURE);
        }

        e_new->weight = weight;

        /* Find source vertex */
        v_from = graph->first;
        while (v_from && (graph->compare(from, v_from->data) > 0))
                v_from = v_from->next;

        if (!v_from || (graph->compare(from, v_from->data) != 0)) {
                free(e_new);
                return -2;
        }

        /* Find destination vertex */
        v_to = graph->first;
        while (v_to && (graph->compare(to, v_to->data) > 0)) {
                free(e_new);
                v_to = v_to->next;
        }

        if (!v_to || (graph->compare(to, v_to->data) != 0)) {
                free(e_new);
                return -3;
        }

        ++v_from->outdegree;
        ++v_to->indegree;

        e_new->dest = v_to;

        /* New edge from this vertex */
        if (!v_from->edge) {
                v_from->edge = e_new;
                e_new->next = NULL;
                return 1;
        }

        /* Vertex has edge(s), Insert in edge list */
        e_prev = NULL;
        e_walker = v_from->edge;

        while(e_walker && (graph->compare(to, e_walker->dest->data) >= 0)) {
                e_prev = e_walker;
                e_walker = e_walker->next;
        }

        if (!e_prev)
                v_from->edge = e_new;
        else
                e_prev->next = e_new;

        e_new->next = e_walker;

        return 1;
}
