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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "utils.h"

int read_from_socket(int sockfd, char *buffer)
{
        int nbytes;
        int len;

        nbytes = read(sockfd, buffer, MSG_SIZE);
        if (nbytes < 0) {
                perror("read");
                return nbytes;
        }

        if (nbytes == 0) {
                return -1;      /* EOF */
        }

        /* Remove trailing '\r\n' or '\n' */
        len = strcspn(buffer, "\r\n");
        buffer[len] = 0;
        return len;
}

int write_to_socket(int sockfd, char *buffer, int len)
{
        int nbytes;

        nbytes = write(sockfd, buffer, len);
        if (nbytes < 0) {
                perror("write");
                exit(EXIT_FAILURE);
        }

        /* Write "\n" to the socket. */
        write(sockfd, "\n", 2);

        return nbytes;
}

int create_server_socket(uint16_t port)
{
        int sock;
        struct sockaddr_in name;

        sock = socket(PF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
                perror("socket");
                exit(EXIT_FAILURE);
        }

        name.sin_family = AF_INET;
        name.sin_port = htons(port);
        name.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(sock, (struct sockaddr *)&name, sizeof(name)) < 0) {
                perror("bind");
                exit(EXIT_FAILURE);
        }
        return sock;
}

void init_sockaddr(struct sockaddr_in *name, const char *hostname, uint16_t port)
{
        struct hostent *hostinfo;

        name->sin_family = AF_INET;
        name->sin_port = htons(port);
        hostinfo = gethostbyname(hostname);
        if (hostinfo == NULL) {
                fprintf(stderr, "Unknown host %s.\n", hostname);
                exit(EXIT_FAILURE);
        }

        name->sin_addr = *(struct in_addr *)hostinfo->h_addr;
}


/* From: http://www.azillionmonkeys.com/qed/random.html */
#define RS_SCALE (1.0 / (1.0 + RAND_MAX))

double drand(void)
{
    double d;

    do {
            d = (((rand () * RS_SCALE) + rand ()) * RS_SCALE + rand ()) * RS_SCALE;
    } while (d >= 1); /* Round off */

    return d;
}
