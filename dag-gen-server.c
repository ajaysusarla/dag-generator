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
#include "utils.h"

#define PORT 8865

#define MIN_EDGE_NUM 2
#define MAX_EDGE_NUM 9

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

Graph *generate_graph(int edges)
{
        #define MAX_VERTICES 20
        #define MAX_STR_LEN 10
        Graph *g;
        int num_vertices, i, max_edges;
        char arr[MAX_VERTICES][MAX_STR_LEN];
        int start_idx, end_idx;

        g = graph_init(NULL);
        graph_new_vertex(g, "START");
        graph_new_vertex(g, "END");

        /* Create a random number of vertices */
        num_vertices = irand(MAX_VERTICES);
        for (i = 0; i < num_vertices; i++) {
                sprintf(arr[i], "%d", i*10);
                graph_new_vertex(g, strdup(arr[i]));
        }

        sprintf(arr[num_vertices++], "%s", strdup("START"));
        sprintf(arr[num_vertices++], "%s", strdup("END"));
        start_idx = num_vertices - 2;
        end_idx = num_vertices - 1;

        /* Max edges */
        max_edges = num_vertices * edges;
        /* Create edges */
        for (i = 0; i < max_edges; i++) {
                int from_idx, to_idx;
                int weight = irand(edges);

                from_idx = irand(num_vertices);
                to_idx = irand(num_vertices);

                /* "START" node should not have an indegree */
                while (to_idx == start_idx)
                        to_idx = irand(num_vertices);

                /* "END" node should not have an outdegree */
                while (from_idx == end_idx)
                        from_idx = irand(num_vertices);

                /* No loops */
                while (from_idx == to_idx)
                        to_idx = irand(num_vertices);

                graph_add_edge(g, arr[from_idx], arr[to_idx], weight);
        }

        return g;
}

int main(int argc, char **argv)
{
        fd_set active_fd_set, read_fd_set;
        int max_edge;

        int i;
        struct sockaddr_in clientname;
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
                                        Graph *g;

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
