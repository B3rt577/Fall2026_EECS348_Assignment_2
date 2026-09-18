/*
Author Name: Edbert Jensen
Program Name: EECS 348 Assignment 2
Brief description: Email Priority Program that has an hierarchical order from most to least important (Boss -> Subordinate -> Peer -> ImportantPerson -> OtherPerson)
Inputs: multiple lines of commands (EMAIL, NEXT, READ, COUNT) with their respective parameters
Outputs: Number of emails to read and the next email if NEXT command and COUNT command are called
Collaborators: Chatgpt
Other sources: Chatgpt, Gemini
Creation date: 9/15/2026 6:00PM
Revision date: 9/16/2026 10:16AM
Revisions: edit some comments
*/

#include <stdio.h>                           // Includes standard input/output functions such as printf() and fgets().
#include <stdlib.h>                              // Includes memory-management functions such as malloc(), realloc(), and free().
#include <string.h>                              // Includes string functions such as strcmp(), strcpy(), strlen(), and strtok().

#define INITIAL_CAPACITY 16                          // Defines the starting number of emails that the heap can store.
#define MAX_SUBJECT 256                              // Defines the maximum number of characters allowed for an email subject.
#define MAX_CATEGORY 32                              // Defines the maximum number of characters allowed for a sender category.
#define MAX_DATE 11                                  // Defines the maximum number of characters needed for MM-DD-YYYY plus the null terminator.

typedef struct {                             // Begins the definition of the Email structure.
    char sender[MAX_CATEGORY];               // Stores the sender category of the email.
    char subject[MAX_SUBJECT];               // Stores the subject line of the email.
    char date[MAX_DATE];                     // Stores the date of the email in MM-DD-YYYY format.
    int priority;                            // Stores the numerical priority of the sender category.
} Email;                                     // Gives the structure the name Email.

typedef struct {                             // Begins the definition of the MaxHeap structure.
    Email *items;                            // Stores the emails in a dynamically allocated array.
    int size;                                // Stores the current number of emails in the heap.
    int capacity;                            // Stores the maximum number of emails that can currently fit in the heap.
} MaxHeap;                                   // Gives the structure the name MaxHeap.

/* Get the priority of the sender category. */
int getPriority(const char *sender) {                            // Defines a function that converts a sender category into a priority number.
    if (strcmp(sender, "Boss") == 0)                             // Checks whether the sender category is Boss.
        return 5;                           // Gives Boss the highest priority.
    if (strcmp(sender, "Subordinate") == 0)                              // Checks whether the sender category is Subordinate.
        return 4;                           // Gives Subordinate the second-highest priority.
    if (strcmp(sender, "Peer") == 0)                                     // Checks whether the sender category is Peer.
        return 3;                           // Gives Peer the third-highest priority.
    if (strcmp(sender, "ImportantPerson") == 0)                          // Checks whether the sender category is ImportantPerson.
        return 2;                           // Gives ImportantPerson the fourth-highest priority.
    return 1;                               // Gives any remaining valid category, OtherPerson, the lowest priority.
}                                           // Ends the getPriority() function.

/* Convert MM-DD-YYYY into a number that can be compared. */
int dateValue(const char *date) {                            // Defines a function that converts a date into a comparable integer.
    int month, day, year;                            // Declares variables to store the month, day, and year.
    sscanf(date, "%d-%d-%d", &month, &day, &year);                           // Reads the month, day, and year from the date string.
    return year * 10000 + month * 100 + day;                             // Combines the date parts into YYYYMMDD so newer dates have larger values.
}                            // Ends the dateValue() function.

/* Determine whether email a has a higher priority than email b. */
int higherPriority(Email a, Email b) {                       // Defines a function that determines which of two emails has higher priority.
    if (a.priority != b.priority)                            // Checks whether the sender priorities are different.
        return a.priority > b.priority;                      // Returns true when email a has a higher sender priority than email b.
    /* If sender priority is the same, newest date comes first. */
    return dateValue(a.date) > dateValue(b.date);                            // If priorities match, returns true when email a has the newer date.
}                          

/* Swap two emails. */
void swapEmails(Email *a, Email *b) {                            // Defines a function that swaps two Email structures.
    Email temp = *a;                             // Temporarily stores the first email.
    *a = *b;                             // Copies the second email into the first email's position.
    *b = temp;                           // Copies the original first email into the second email's position.
}                                    

/* Initialize the MaxHeap. */
void initHeap(MaxHeap *heap) {                           // Defines a function that initializes an empty MaxHeap.
    heap->items = malloc(INITIAL_CAPACITY * sizeof(Email));                              // Allocates memory for the initial number of emails.
    if (heap->items == NULL) {                           // Checks whether the memory allocation failed.
        fprintf(stderr, "Memory allocation failed.\n");                              // Prints an error message to the standard error stream.
        exit(EXIT_FAILURE);                              // Stops the program because the heap could not be created.
    }                                                    // Ends the memory allocation failure check.
    heap->size = 0;                                      // Sets the initial number of emails to zero.
    heap->capacity = INITIAL_CAPACITY;                           // Sets the initial heap capacity to 16 emails.
}                         

/* Increase heap capacity when necessary. */
void resizeHeap(MaxHeap *heap) {                             // Defines a function that increases the heap's storage capacity.
    heap->capacity *= 2;                             // Doubles the current capacity of the heap.
    Email *newItems = realloc(heap->items, heap->capacity * sizeof(Email));                              // Attempts to resize the existing memory allocation.
    if (newItems == NULL) {                              // Checks whether the reallocation failed.
        free(heap->items);                           // Frees the original heap memory before terminating.
        fprintf(stderr, "Memory allocation failed.\n");                              // Prints an error message to the standard error stream.
        exit(EXIT_FAILURE);                              // Stops the program because the heap could not be resized.
    }                            // Ends the reallocation failure check.
    heap->items = newItems;                              // Updates the heap to use the newly allocated memory.
}                            // Ends the resizeHeap() function.

/* Restore the MaxHeap property upward after insertion. */
void heapifyUp(MaxHeap *heap, int index) {                           // Defines a function that restores the MaxHeap property upward.
    while (index > 0) {                              // Continues while the current element is not the root.
        int parent = (index - 1) / 2;                            // Calculates the array index of the current element's parent.
        if (!higherPriority(heap->items[index], heap->items[parent]))                            // Checks whether the current element already belongs below its parent.
            break;                                   // Stops the loop if the parent already has equal or higher priority.
        swapEmails(&heap->items[index], &heap->items[parent]);                           // Swaps the current email with its parent.
        index = parent;                              // Moves the current index upward to the parent's position.
    }                                                // Ends the loop that moves the email upward.
}                         

/* Insert an email into the MaxHeap. */
void insertHeap(MaxHeap *heap, Email email) {                    // Defines a function that inserts a new email into the MaxHeap.
    if (heap->size == heap->capacity)                            // Checks whether the heap is completely full.
        resizeHeap(heap);                                        // Doubles the heap capacity when it is full.
    heap->items[heap->size] = email;                             // Places the new email at the next available array position.
    heapifyUp(heap, heap->size);                                 // Moves the new email upward until the MaxHeap property is restored.
    heap->size++;                                                // Increases the number of emails stored in the heap.
}          

/* Restore the MaxHeap property downward after removal. */
void heapifyDown(MaxHeap *heap, int index) {                             // Defines a function that restores the MaxHeap property downward.
    while (1) {                                              // Continues checking children until the heap property is restored.
        int left = 2 * index + 1;                            // Calculates the array index of the current node's left child.
        int right = 2 * index + 2;                           // Calculates the array index of the current node's right child.
        int largest = index;                             // Initially assumes the current node has the highest priority.
        if (left < heap->size && higherPriority(heap->items[left], heap->items[largest]))                            // Checks whether the left child exists and has higher priority.
            largest = left;                              // Changes largest to the left child's index.
        if (right < heap->size && higherPriority(heap->items[right], heap->items[largest]))                              // Checks whether the right child exists and has higher priority than the current largest.
            largest = right;                             // Changes largest to the right child's index.
        if (largest == index)                            // Checks whether the current element is already the highest-priority element among its children.
            break;                                       // Stops the loop because the MaxHeap property has been restored.
        swapEmails(&heap->items[index], &heap->items[largest]);                              // Swaps the current element with the highest-priority child.
        index = largest;                                 // Moves the current index downward to the child's position.
    }                        
}   

/* Return the highest-priority email without removing it. */
Email peekHeap(MaxHeap *heap) {                              // Defines a function that returns the highest-priority email without removing it.
    return heap->items[0];                           // Returns the root element of the MaxHeap.
}                            // Ends the peekHeap() function.

/* Remove the highest-priority email from the MaxHeap. */
Email extractMax(MaxHeap *heap) {                        // Defines a function that removes and returns the highest-priority email.
    Email result = heap->items[0];                       // Saves the root email so it can be returned later.
    heap->size--;                                        // Decreases the heap size because the highest-priority email is being removed.
    if (heap->size > 0) {                                // Checks whether there are still emails remaining in the heap.
        heap->items[0] = heap->items[heap->size];        // Moves the last email into the root position.
        heapifyDown(heap, 0);                            // Restores the MaxHeap property starting from the root.
    }                                           // Ends the check for remaining emails.
    return result;                              // Returns the email that was removed from the heap.
}            

/* Free the memory used by the MaxHeap. */
void destroyHeap(MaxHeap *heap) {                            // Defines a function that releases all memory used by the heap.
    free(heap->items);                           // Frees the dynamically allocated array of emails.
    heap->items = NULL;                              // Sets the pointer to NULL so it no longer points to freed memory.
    heap->size = 0;                              // Resets the number of emails to zero.
    heap->capacity = 0;                              // Resets the heap capacity to zero.
}                            // Ends the destroyHeap() function.

/* Remove leading and trailing whitespace from a string. */
void trim(char *str) {                           // Defines a function that removes unnecessary whitespace from the beginning and end of a string.
    int start = 0;                           // Starts the search for leading whitespace at the first character.
    int end = strlen(str) - 1;                           // Sets end to the final character in the string.
    while (str[start] == ' ' || str[start] == '\t')                              // Continues while the beginning contains spaces or tabs.
        start++;                             // Moves the starting position forward to skip whitespace.
    while (end >= start && (str[end] == ' ' || str[end] == '\t' || str[end] == '\n' || str[end] == '\r')) {                              // Continues while the end contains whitespace characters.
        end--;                           // Moves the ending position backward to skip whitespace.
    }                            // Ends the loop that removes trailing whitespace.
    if (start > 0)                           // Checks whether leading whitespace was found.
        memmove(str, str + start, end - start + 1);                              // Moves the meaningful part of the string to the beginning.
    str[end - start + 1] = '\0';                             // Adds the null terminator to properly end the shortened string.
}                       

/* Process an EMAIL command. */
void processEmail(MaxHeap *heap, char *line) {                           // Defines a function that processes an EMAIL command.
    char *data = line + 6;                           // Skips the six characters in "EMAIL " so data points to the sender category.
    char *sender;                            // Declares a pointer that will store the sender category.
    char *subject;                           // Declares a pointer that will store the subject line.
    char *date;                              // Declares a pointer that will store the email date.
    sender = strtok(data, ",");                              // Extracts the sender category before the first comma.
    subject = strtok(NULL, ",");                             // Extracts the subject line between the first and second commas.
    date = strtok(NULL, ",");                            // Extracts the date after the second comma.
    if (sender == NULL || subject == NULL || date == NULL)                           // Checks whether any required email field is missing.
        return;                              // Stops processing the email when the input is incomplete.
    trim(sender);                            // Removes unnecessary whitespace from the sender category.
    trim(subject);                           // Removes unnecessary whitespace from the subject line.
    trim(date);                              // Removes unnecessary whitespace from the date.
    Email email;                             // Creates a new Email structure.
    strcpy(email.sender, sender);                            // Copies the sender category into the Email structure.
    strcpy(email.subject, subject);                              // Copies the subject line into the Email structure.
    strcpy(email.date, date);                            // Copies the date into the Email structure.
    email.priority = getPriority(email.sender);                              // Calculates and stores the priority of the sender.
    insertHeap(heap, email);                             // Adds the completed email to the MaxHeap.
}


int main(void) {                             // Defines the main function where program execution begins.
    MaxHeap heap;                            // Creates a MaxHeap variable for storing the CEO's unread emails.
    char line[512];                              // Creates a buffer large enough to hold an input command.
    initHeap(&heap);                             // Initializes the MaxHeap before any commands are processed.
    while (fgets(line, sizeof(line), stdin) != NULL) {                           // Reads commands from standard input one line at a time.
        trim(line);                              // Removes the newline and unnecessary whitespace from the input line.
        if (strncmp(line, "EMAIL ", 6) == 0) {                           // Checks whether the current command begins with "EMAIL ".
            processEmail(&heap, line);                           // Processes the EMAIL command and inserts the email into the heap.
        }                         
        else if (strcmp(line, "NEXT") == 0) {                // Checks whether the current command is NEXT.
            if (heap.size > 0) {                             // Checks whether there is at least one unread email.
                Email next = peekHeap(&heap);                // Gets the highest-priority email without removing it.
                printf("Next email:\n");                             // Prints the required first line of NEXT output.
                printf("Sender: %s\n", next.sender);                 // Prints the sender category of the next email.
                printf("Subject: %s\n", next.subject);               // Prints the subject line of the next email.
                printf("Date: %s\n", next.date);                     // Prints the date of the next email.
            }                           
        }                       
        else if (strcmp(line, "READ") == 0) {                            // Checks whether the current command is READ.
            if (heap.size > 0) {                             // Checks whether there is at least one unread email.
                extractMax(&heap);                           // Removes the highest-priority email from the heap.
            }                          
        }                        
        else if (strcmp(line, "COUNT") == 0) {                           // Checks whether the current command is COUNT.
            printf("There are %d emails to read.\n", heap.size);                             // Prints the current number of unread emails.
        }                                          
    }                                                // Ends the loop that reads commands from standard input.
    destroyHeap(&heap);                              // Frees all dynamically allocated memory before the program ends.
    return 0;                            // Indicates that the program completed successfully.
}