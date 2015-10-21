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

#ifndef _UTILS_H_
#define _UTILS_H_

#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>

double drand(void);
#define irand(x) ((unsigned int) ((x) * drand()))

#define MSG_SIZE 512
#define START "START"
#define END "END"
#define RESET "RESET"


typedef enum Bool {
        FALSE = 0,
        TRUE
} bool;

int read_from_socket(int sockfd, char *buffer);
int write_to_socket(int sockfd, char *buffer, int len);
int create_server_socket(uint16_t port);
void init_sockaddr(struct sockaddr_in *name, const char *hostname, uint16_t port);

#endif  /* _UTILS_H_ */
