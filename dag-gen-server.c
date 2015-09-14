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
#include <signal.h>

#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <arpa/inet.h>
#include <unistd.h>

#include "graph.h"
#include "list.h"
#include "utils.h"

#define PORT 8865

#define MIN_EDGE_NUM 2
#define MAX_EDGE_NUM 9

#define MAX_VERTICES 20
#define MAX_STR_LEN 4

/* Server listening socket */
int server_sock;

static void usage(char *prog)
{
        fprintf(stderr, "USAGE: %s <num>\n\n", prog);
        fprintf(stderr, "        num: max number of edges for a node, ");
        fprintf(stderr, "can be between %d and %d.\n",
                MIN_EDGE_NUM, MAX_EDGE_NUM);
}

static void cleanup()
{
        close(server_sock);
        return;
}

static void complete_graph(Graph *g)
{
        Vertex *v;

        v = g->first;

        while (v) {
                int i;

                for (i = 0; i < g->max_edges; i++) {
                        if (v->edges[i] != NULL) {
                                if (v->edges[i]->dest->outdegree == 0) {
                                        graph_add_edge(g, v->edges[i]->dest->data,
                                                       "END", 0);
                                }
                        }
                }

                v = v->next;
        }
}

static void add_random_edges(List *l, Graph *g, int edge_count, int weight)
{
        int i;

        for (i = 0; i < edge_count; i++) {
                char *vert1;
                char *vert2;
                int tmp_idx;
                int ret;

                tmp_idx = irand(l->length);
                vert1 = list_get_index(l, tmp_idx);
                while (strcmp(vert1, "END") == 0)
                        vert1 = list_get_index(l, irand(l->length));

                vert2 = list_get_index(l, tmp_idx);
                while ((strcmp(vert2, "START") == 0) ||
                       (strcmp(vert2, vert1) == 0))
                        vert2 = list_get_index(l, irand(l->length));

                ret = graph_add_edge(g, vert1, vert2, irand(weight));
        }
}

static Graph *generate_graph(int edges)
{
        Graph *g, *t;
        List *list = NULL;
        int num_vertices, i, max_edges, edge_count = 0;
        char *prev_data;

        g = graph_init(NULL, edges);
        t = graph_init(NULL, edges);

        list = list_init();

        /* Select a random number of vertices */
        num_vertices = 0;
        while (num_vertices == 0)
                num_vertices = irand(MAX_VERTICES);

        /* Create data for vertices */
        for (i = 0; i < num_vertices; i++) {
                char arr[MAX_STR_LEN] = {0};
                int n = 0;

                while (n == 0)
                        n = irand(100);

                sprintf(arr, "%d", irand(100));

                while (list_has_element(list, arr))
                        sprintf(arr, "%d", irand(100));

                list_append(list, strdup(arr));
                graph_new_vertex(t, strdup(arr));
        }

        list_append(list, strdup("END"));
        graph_new_vertex(t, strdup("END"));

        /* Max edges */
        max_edges = num_vertices * edges;

        /* Create Graph */
        prev_data = "START";
        graph_new_vertex(g, strdup(prev_data));

        while (t->count) {
                int tmp_idx = irand(list->length);
                char *cur_data = NULL;
                Vertex *v;

                v = graph_get_vertex(g, prev_data);
                if (v->outdegree >= edges) {
                        Vertex *tmp = g->first;

                        while (tmp) {
                                if (tmp->outdegree < edges) {
                                        prev_data = tmp->data;
                                        break;
                                }
                                tmp = tmp->next;
                        }
                }

                cur_data = list_get_index(list, tmp_idx);

                if (graph_get_vertex(g, cur_data) == NULL) {
                        graph_new_vertex(g, strdup(cur_data));

                        if ((strcmp(prev_data, "END") == 0) ||
                            (strcmp(cur_data, "START") == 0)) {
                                graph_add_edge(g, cur_data, prev_data, irand(edges));
                                edge_count++;
                        } else {
                                graph_add_edge(g, prev_data, cur_data, irand(edges));
                                edge_count++;
                        }
                        graph_delete_vertex(t, cur_data);
                }

                prev_data = cur_data;
        }

        add_random_edges(list, g, irand(max_edges - edge_count), edges);
        complete_graph(g);
        graph_free(t);
        list_free(list);

        return g;
}


int main(int argc, char **argv)
{
        fd_set active_fd_set, read_fd_set;
        int max_edge;

        int i;
        struct sockaddr_in clientname;
        Graph *g;
#ifdef MACOSX
        unsigned int size;
#else
        socklen_t size;
#endif
        if (argc != 2) {
                usage(argv[0]);
                exit(EXIT_FAILURE);
        }

        max_edge = atoi(argv[1]);
        if ((max_edge < MIN_EDGE_NUM) || (max_edge > MAX_EDGE_NUM)) {
                usage(argv[0]);
                exit(EXIT_FAILURE);
        }

        signal(SIGINT, cleanup);
        signal(SIGTERM, cleanup);

        server_sock = create_server_socket(PORT);
        if (listen(server_sock, 1) < 0) {
                perror("listen");
                exit(EXIT_FAILURE);
        }

        FD_ZERO(&active_fd_set);
        FD_SET(server_sock, &active_fd_set);

        while (1) {
                read_fd_set = active_fd_set;

                if (select (FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
                        perror("select");
                        exit(EXIT_FAILURE);
                }

                for (i = 0; i < FD_SETSIZE; i++) {
                        if (FD_ISSET(i, &read_fd_set)) {
                                if (i == server_sock) { /* New connection */
                                        int new;

                                        size = sizeof(clientname);
                                        new = accept(server_sock,
                                                     (struct sockaddr *)&clientname,
                                                     &size);
                                        if (new < 0) {
                                                perror("accept");
                                                exit(EXIT_FAILURE);
                                        }
                                        fprintf(stderr, "[DAG Server] Connect from host %s:%d\n",
                                                inet_ntoa(clientname.sin_addr), ntohs(clientname.sin_port));
                                        g = generate_graph(max_edge);
                                        graph_print(g);
                                        graph_free(g);
                                        g = NULL;
                                        write_to_socket(new, START, strlen(START));
                                        FD_SET(new, &active_fd_set);
                                } else {         /* Existing connection */
                                        char buffer[MSG_SIZE] = {0};
                                        if (read_from_socket(i, buffer) < 0) {
                                                close(i);
                                                FD_CLR(i, &active_fd_set);
                                        } else {
                                                if (strncmp(buffer, RESET, strlen(RESET)) == 0) {
                                                        write_to_socket(i, START, strlen(START));
                                                }
                                        }
                                }
                        }
                }

        }

        exit(EXIT_SUCCESS);
}
