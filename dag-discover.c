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
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/select.h>
#include <sys/socket.h>

#include "graph.h"
#include "utils.h"

#define SERVER "127.0.0.1"
#define PORT 8865

int main(int argc, char **argv)
{
        int sock;
        struct sockaddr_in servername;
        char buffer[MSG_SIZE] = {0};
        int bytes;

        sock = socket(PF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
                perror("socket");
                exit(EXIT_FAILURE);
        }

        init_sockaddr(&servername, SERVER, PORT);

        if (connect(sock, (struct sockaddr *)&servername, sizeof(servername)) < 0) {
                perror("connect");
                exit(EXIT_FAILURE);
        }

        bytes = read_from_socket(sock, buffer);
        if (bytes != strlen(START)) {
                fprintf(stderr, "ERROR: Invalid START token from Server.\n");
                exit(EXIT_FAILURE);
        }

        while(1) {
                write_to_socket(sock, RESET, strlen(RESET));
                break;
        }

        close(sock);

        exit(EXIT_SUCCESS);
}
