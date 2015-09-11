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

#ifndef _LIST_H_
#define _LIST_H_

typedef struct list List;
typedef struct node Node;

struct list {
        int length;
        Node *first;
        Node *last;
};

struct node {
        char *data;
        struct node *next;
};

List *list_init(void);
void list_free(List *list);
int list_lenght(List *list);
int list_append(List *list, char *data);
int list_has_element(List *list, char *data);
void node_free(Node *node);
char * list_remove(List *list, int index);
void list_print(List *list);
char *list_get_index(List *list, int index);

#endif  /* _LIST_H_ */

