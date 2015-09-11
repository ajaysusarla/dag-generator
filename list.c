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

#include "list.h"

List *list_init(void)
{
        List *list = NULL;

        list = (List *)malloc(sizeof(List));
        if (list) {
                list->length = 0;
                list->first = NULL;
                list->last = NULL;
        }

        return list;
}

void list_free(List *list)
{
        if (list) {
                Node *cur, *prev;

                cur = list->first;

                while(cur) {
                        prev = cur;
                        cur = cur->next;

                        free(prev->data);
                        free(prev);
                        prev = NULL;
                        list->length--;
                }

                free(list);
                list = NULL;
        }
}

int list_lenght(List *list)
{
        if (!list) {
                return -1;
        }

        return list->length;
}

int list_append(List *list, char *data)
{
        Node *node;

        if (!list) {
                return -1;
        }

        node = (Node *)malloc(sizeof(Node));
        if(!node) {
                return -1;
        }

        node->data = data;
        node->next = NULL;

        if (list->first == NULL) {
                list->first = node;
                list->last = node;
        } else {
                list->last->next = node;
                list->last = node;
        }

        list->length++;

        return 1;
}

int list_has_element(List *list, char *data)
{
        Node *node;
        int exists = 0;

        if (!list) {
                return -1;
        }

        node = list->first;

        while (node) {
                if (strcmp(node->data, data) == 0) {
                        exists = 1;
                        break;
                }

                node = node->next;
        }

        return exists;
}

void node_free(Node *node)
{
        if (!node) {
                return;
        }

        free(node->data);
        node->next = NULL;
        free(node);
}

/*
  list_remove() removes an element from the list at position pointed to by
  the 'index'. If found, returns, the data in the node, NULL otherwise.

  The data, returned from this function should be freed.
 */
char * list_remove(List *list, int index)
{
        Node *node = NULL, *walker = NULL;
        int count;
        char *data = NULL;

        if (!list) {
                return NULL;
        }

        if ((index >= list->length) || (index < 0)) {
                return NULL;
        }

        walker = list->first;
        if (index == 0) {
                list->first = walker->next;
                data = strdup(walker->data);
                node_free(walker);
                list->length--;
                return data;
        }

        /* Move to the second node - index:1 */
        count = 1;
        node = walker;
        walker = walker->next;

        while (walker) {
                if (index == count) {
                        node->next = walker->next;
                        break;
                }
                node = walker;
                walker = walker->next;
                count++;
        }

        if (walker) {
                data = strdup(walker->data);
                node_free(walker);
                list->length--;
        }

        return data;
}

char *list_get_index(List *list, int index)
{
        Node *walker = NULL;
        int count;

        if (!list) {
                return NULL;
        }

        if ((index >= list->length) || (index < 0)) {
                return NULL;
        }

        walker = list->first;
        if (index == 0)
                return walker->data;

        count = 1;

        while (walker) {
                if (count == index)
                        break;

                count++;
                walker = walker->next;
        }

        if (walker)
                return walker->data;
        else
                return NULL;
}

void list_print(List *list)
{
        Node *walker;

        if (!list) {
                return;
        }

        walker = list->first;

        while (walker) {
                printf("[%s]\n", walker->data);
                walker = walker->next;
        }
}
