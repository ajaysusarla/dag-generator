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
static void _destroy_graph(Vertex *vert, int max_edges)
{
        if (vert) {
                int i;

                free(vert->data);
                vert->data = NULL;

                for (i = 0; i < max_edges; i++) {
                        if (vert->edges[i] != NULL)
                                free(vert->edges[i]);
                }

                _destroy_graph(vert->next, max_edges);
        }

        return;
}

/*
 * graph_init:
 *
 * Arguments :
 * Returns   :
 */
Graph *graph_init(int (* compare)(void *arg1, void *arg2), int max_edges)
{
        Graph *graph = NULL;

        graph = (Graph *)malloc(sizeof(Graph));
        if (graph) {
                graph->count = 0;
                graph->first = NULL;
                graph->max_edges = max_edges;
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
                _destroy_graph(graph->first, graph->max_edges);

                free(graph);

                graph = NULL;
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
        int i;

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
        v_new->edges = NULL;
        v_new->processed = FALSE;
        v_new->visited = FALSE;

        v_new->edges = malloc(graph->max_edges * sizeof(Edge));
        if (!v_new->edges) {
                fprintf(stderr, "[ERROR] Memory issue.\n");
                exit(EXIT_FAILURE);
        }


        for (i = 0; i < graph->max_edges; i++)
                v_new->edges[i] = NULL;


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

        free(v_walker->edges);
        free(v_walker);


        return 1;
}

/*
 * Function: graph_get_vertex()
 * Arguments :
 * Return    : Finds and returns a vertex containing 'data',
 *             NULL otherwise
 */
Vertex * graph_get_vertex(Graph *graph, void *data)
{
        Vertex *vert = NULL;

        if (!graph->first)
                return NULL;

        vert = graph->first;
        while(vert && (graph->compare(data, vert->data) > 0)) {
                vert = vert->next;
        }

        if (!vert || (graph->compare(data, vert->data) != 0))
                return NULL;

        /* Found the vertex */
        return vert;
}

/*
 * Function: graph_delete_edge()
 * Arguments :
 * Return    :
 *             1  - success
 *             -1 - degree of vertex not 0
 *             -2 - from vertex not found
 *             -3 - to vertex not found
 */
int graph_delete_edge(Vertex *from, Vertex *to, int weight)
{

        if (from->edges[weight] != NULL) {
                Edge *e;
                e = from->edges[weight];
                e->dest = NULL;
                free(e);
                e = NULL;
                from->edges[weight] = NULL;
        }

        --from->outdegree;
        --to->indegree;

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

        Vertex *v_from;
        Vertex *v_to;


        if (weight >= graph->max_edges)
                return -1;


        e_new = (Edge *)malloc(sizeof(Edge));
        if (!e_new) {
                fprintf(stderr, "[ERROR] Memory issue.\n");
                exit(EXIT_FAILURE);
        }

        /* Find destination vertex */
        v_to = graph->first;
        while (v_to && (graph->compare(to, v_to->data) > 0))
                v_to = v_to->next;

        if (!v_to || (graph->compare(to, v_to->data) != 0)) {
                if (e_new) {
                        free(e_new);
                        e_new = NULL;
                }
                return -3;
        }

        /* Find source vertex */
        v_from = graph->first;
        while (v_from && (graph->compare(from, v_from->data) > 0))
                v_from = v_from->next;

        if (!v_from || (graph->compare(from, v_from->data) != 0)) {
                if (e_new) {
                        free(e_new);
                        e_new = NULL;
                }
                return -2;
        }

        if (v_from->outdegree >= graph->max_edges) {
                free(e_new);
                e_new = NULL;
                return -1;
        }

        if (v_from->edges[weight] != NULL) {
                int i;
                for (i = 0; i < graph->max_edges; i++) {
                        if (v_from->edges[i] == NULL) {
                                v_from->edges[i] = e_new;
                                e_new->weight = i;
                                goto end;
                        }
                }

                free(e_new);
                e_new = NULL;
                return -1;
        } else {
                v_from->edges[weight] = e_new;
                e_new->weight = weight;
        }

end:
        e_new->dest = v_to;

        ++v_from->outdegree;
        ++v_to->indegree;


        return 1;
}

/*
 * Function: graph_print()
 * Arguments :
 * Return    :
 */
void graph_print(Graph *graph)
{
        Vertex *v;

        v = graph->first;

        while (v) {
                int i;
                printf("[%s]:\n", (char *)v->data);

                for (i = 0; i < graph->max_edges; i++) {
                        if (v->edges[i] != NULL) {
                                printf("\t --(%d)--> [%s]\n",
                                       v->edges[i]->weight,
                                       (char*)v->edges[i]->dest->data);
                        }

                }


                v = v->next;
        }
}

void graph_print_dot(Graph *graph)
{
        Vertex *v;

        v = graph->first;

        printf("digraph {\n");
        while (v) {
                int i;

                for (i = 0; i < graph->max_edges; i++) {
                        if (v->edges[i] != NULL) {
                                printf("%s -> %s[label=%d];\n", (char *)v->data,
                                       (char*)v->edges[i]->dest->data,
                                        v->edges[i]->weight);
                        }

                }


                v = v->next;
        }

        printf("}\n");
}
