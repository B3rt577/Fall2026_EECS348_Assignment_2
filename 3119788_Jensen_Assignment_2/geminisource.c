/*
Author Name: Edbert Jensen
Program Name: EECS 348 Assignment 2
Brief description: Email Priority Program that has an hierarchical order from most to least important (Boss -> Subordinate -> Peer -> ImportantPerson -> OtherPerson)
Inputs: multiple lines of commands (EMAIL, NEXT, READ, COUNT) with their respective parameters
Outputs: Number of emails to read and the next email if NEXT command and COUNT command are called
Collaborators: Gemini
Other sources: Gemini
Creation date: 9/15/2026 5:00PM
Revision date: 9/16/2026 2:24AM
Revisions: add gemini generated code to github
*/

/*
 * Email Priority Program
 * As specified in instructions.md
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char category[50];
    char subject[256];
    char date[15];
    int priority;
    long date_val;
} Email;

typedef struct {
    Email *list;
    int size;
    int capacity;
} MaxHeap;

void trim_whitespace(char *str) {
    char *start = str;
    while (*start && isspace((unsigned char)*start)) start++;
    size_t len = strlen(start);
    memmove(str, start, len + 1);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
}

int get_category_priority(const char *category) {
    if (strcmp(category, "Boss") == 0) return 5;
    if (strcmp(category, "Subordinate") == 0) return 4;
    if (strcmp(category, "Peer") == 0) return 3;
    if (strcmp(category, "ImportantPerson") == 0) return 2;
    if (strcmp(category, "OtherPerson") == 0) return 1;
    return 0;
}

long parse_date(const char *date_str) {
    int m = 0, d = 0, y = 0;
    sscanf(date_str, "%d-%d-%d", &m, &d, &y);
    return (long)y * 10000 + (long)m * 100 + (long)d;
}

int compare_emails(Email a, Email b) {
    if (a.priority != b.priority) {
        return a.priority - b.priority;
    }
    if (a.date_val != b.date_val) {
        return (a.date_val > b.date_val) ? 1 : -1;
    }
    return 0; 
}

void init_heap(MaxHeap *heap, int capacity) {
    heap->capacity = capacity;
    heap->size = 0;
    heap->list = (Email *)malloc(capacity * sizeof(Email));
}

void swap(Email *a, Email *b) {
    Email temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_up(MaxHeap *heap, int index) {
    if (index && compare_emails(heap->list[index], heap->list[(index - 1) / 2]) > 0) {
        swap(&heap->list[index], &heap->list[(index - 1) / 2]);
        heapify_up(heap, (index - 1) / 2);
    }
}

void heapify_down(MaxHeap *heap, int index) {
    int largest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < heap->size && compare_emails(heap->list[left], heap->list[largest]) > 0)
        largest = left;

    if (right < heap->size && compare_emails(heap->list[right], heap->list[largest]) > 0)
        largest = right;

    if (largest != index) {
        swap(&heap->list[index], &heap->list[largest]);
        heapify_down(heap, largest);
    }
}

void insert(MaxHeap *heap, Email email) {
    if (heap->size == heap->capacity) {
        heap->capacity *= 2;
        heap->list = (Email *)realloc(heap->list, heap->capacity * sizeof(Email));
    }
    heap->list[heap->size] = email;
    heapify_up(heap, heap->size);
    heap->size++;
}

Email extract_max(MaxHeap *heap) {
    Email max_email = heap->list[0];
    heap->list[0] = heap->list[heap->size - 1];
    heap->size--;
    heapify_down(heap, 0);
    return max_email;
}

int main() {
    MaxHeap heap;
    init_heap(&heap, 10);
    char line[512];

    while (fgets(line, sizeof(line), stdin)) {
        trim_whitespace(line);
        if (strlen(line) == 0) continue;

        if (strncmp(line, "EMAIL ", 6) == 0) {
            Email new_email;
            char *content = line + 6;
            
            char *cat_ptr = strtok(content, ",");
            char *subj_ptr = strtok(NULL, ",");
            char *date_ptr = strtok(NULL, "");

            if (cat_ptr && subj_ptr && date_ptr) {
                strcpy(new_email.category, cat_ptr);
                strcpy(new_email.subject, subj_ptr);
                strcpy(new_email.date, date_ptr);

                trim_whitespace(new_email.category);
                trim_whitespace(new_email.subject);
                trim_whitespace(new_email.date);

                new_email.priority = get_category_priority(new_email.category);
                new_email.date_val = parse_date(new_email.date);

                insert(&heap, new_email);
            }
        } else if (strcmp(line, "NEXT") == 0) {
            if (heap.size > 0) {
                Email top = heap.list[0];
                printf("Next email:\n");
                printf("Sender: %s\n", top.category);
                printf("Subject: %s\n", top.subject);
                printf("Date: %s\n", top.date);
            } else {
                printf("Inbox is empty.\n");
            }
        } else if (strcmp(line, "READ") == 0) {
            if (heap.size > 0) {
                extract_max(&heap);
            }
        } else if (strcmp(line, "COUNT") == 0) {
            printf("There are %d emails to read.\n", heap.size);
        }
    }

    free(heap.list);
    return 0;
}