/*
Author Name: Edbert Jensen
Program Name: EECS 348 Assignment 2
Brief description: Email Priority Program that has an hierarchical order from most to least important (Boss -> Subordinate -> Peer -> ImportantPerson -> OtherPerson)
Inputs: multiple lines of commands (EMAIL, NEXT, READ, COUNT) with their respective parameters
Outputs: Number of emails to read and the next email if NEXT command and COUNT command are called
Collaborators: Chatgpt
Other sources: Chatgpt
Creation date: 9/15/2026 6:00PM
Revision date: 9/16/2026 2:26AM
Revisions: add chatgpt generated code to github
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16
#define MAX_SUBJECT 256
#define MAX_CATEGORY 32
#define MAX_DATE 11

typedef struct {
    char sender[MAX_CATEGORY];
    char subject[MAX_SUBJECT];
    char date[MAX_DATE];
    int priority;
} Email;

typedef struct {
    Email *items;
    int size;
    int capacity;
} MaxHeap;

/* Get the priority of the sender category. */
int getPriority(const char *sender) {
    if (strcmp(sender, "Boss") == 0)
        return 5;
    if (strcmp(sender, "Subordinate") == 0)
        return 4;
    if (strcmp(sender, "Peer") == 0)
        return 3;
    if (strcmp(sender, "ImportantPerson") == 0)
        return 2;
    return 1;  /* OtherPerson */
}

/* Convert MM-DD-YYYY into a number that can be compared. */
int dateValue(const char *date) {
    int month, day, year;

    sscanf(date, "%d-%d-%d", &month, &day, &year);

    return year * 10000 + month * 100 + day;
}

/* Determine whether email a has a higher priority than email b. */
int higherPriority(Email a, Email b) {
    if (a.priority != b.priority)
        return a.priority > b.priority;

    /* If sender priority is the same, newest date comes first. */
    return dateValue(a.date) > dateValue(b.date);
}

/* Swap two emails. */
void swapEmails(Email *a, Email *b) {
    Email temp = *a;
    *a = *b;
    *b = temp;
}

/* Initialize the MaxHeap. */
void initHeap(MaxHeap *heap) {
    heap->items = malloc(INITIAL_CAPACITY * sizeof(Email));

    if (heap->items == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    heap->size = 0;
    heap->capacity = INITIAL_CAPACITY;
}

/* Increase heap capacity when necessary. */
void resizeHeap(MaxHeap *heap) {
    heap->capacity *= 2;

    Email *newItems = realloc(
        heap->items,
        heap->capacity * sizeof(Email)
    );

    if (newItems == NULL) {
        free(heap->items);
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    heap->items = newItems;
}

/* Restore the MaxHeap property upward after insertion. */
void heapifyUp(MaxHeap *heap, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;

        if (!higherPriority(heap->items[index],
                             heap->items[parent])) {
            break;
        }

        swapEmails(&heap->items[index], &heap->items[parent]);
        index = parent;
    }
}

/* Insert an email into the MaxHeap. */
void insertHeap(MaxHeap *heap, Email email) {
    if (heap->size == heap->capacity)
        resizeHeap(heap);

    heap->items[heap->size] = email;
    heapifyUp(heap, heap->size);

    heap->size++;
}

/* Restore the MaxHeap property downward after removal. */
void heapifyDown(MaxHeap *heap, int index) {
    while (1) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;

        if (left < heap->size &&
            higherPriority(heap->items[left],
                           heap->items[largest])) {
            largest = left;
        }

        if (right < heap->size &&
            higherPriority(heap->items[right],
                           heap->items[largest])) {
            largest = right;
        }

        if (largest == index)
            break;

        swapEmails(&heap->items[index],
                    &heap->items[largest]);

        index = largest;
    }
}

/* Return the highest-priority email without removing it. */
Email peekHeap(MaxHeap *heap) {
    return heap->items[0];
}

/* Remove the highest-priority email from the MaxHeap. */
Email extractMax(MaxHeap *heap) {
    Email result = heap->items[0];

    heap->size--;

    if (heap->size > 0) {
        heap->items[0] = heap->items[heap->size];
        heapifyDown(heap, 0);
    }

    return result;
}

/* Free the memory used by the MaxHeap. */
void destroyHeap(MaxHeap *heap) {
    free(heap->items);
    heap->items = NULL;
    heap->size = 0;
    heap->capacity = 0;
}

/* Remove leading and trailing whitespace from a string. */
void trim(char *str) {
    int start = 0;
    int end = strlen(str) - 1;

    while (str[start] == ' ' || str[start] == '\t')
        start++;

    while (end >= start &&
           (str[end] == ' ' ||
            str[end] == '\t' ||
            str[end] == '\n' ||
            str[end] == '\r')) {
        end--;
    }

    if (start > 0)
        memmove(str, str + start, end - start + 1);

    str[end - start + 1] = '\0';
}

/* Process an EMAIL command. */
void processEmail(MaxHeap *heap, char *line) {
    char *data = line + 6;
    char *sender;
    char *subject;
    char *date;

    sender = strtok(data, ",");
    subject = strtok(NULL, ",");
    date = strtok(NULL, ",");

    if (sender == NULL || subject == NULL || date == NULL)
        return;

    trim(sender);
    trim(subject);
    trim(date);

    Email email;

    strcpy(email.sender, sender);
    strcpy(email.subject, subject);
    strcpy(email.date, date);

    email.priority = getPriority(email.sender);

    insertHeap(heap, email);
}

int main(void) {
    MaxHeap heap;
    char line[512];

    initHeap(&heap);

    while (fgets(line, sizeof(line), stdin) != NULL) {
        trim(line);

        if (strncmp(line, "EMAIL ", 6) == 0) {
            processEmail(&heap, line);
        }
        else if (strcmp(line, "NEXT") == 0) {
            if (heap.size > 0) {
                Email next = peekHeap(&heap);

                printf("Next email:\n");
                printf("Sender: %s\n", next.sender);
                printf("Subject: %s\n", next.subject);
                printf("Date: %s\n", next.date);
            }
        }
        else if (strcmp(line, "READ") == 0) {
            if (heap.size > 0) {
                extractMax(&heap);
            }
        }
        else if (strcmp(line, "COUNT") == 0) {
            printf("There are %d emails to read.\n", heap.size);
        }
    }

    destroyHeap(&heap);

    return 0;
}