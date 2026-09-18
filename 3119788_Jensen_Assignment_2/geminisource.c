/*
Author Name: Edbert Jensen
Program Name: EECS 348 Assignment 2
Brief description: Email Priority Program that has an hierarchical order from most to least important (Boss -> Subordinate -> Peer -> ImportantPerson -> OtherPerson)
Inputs: multiple lines of commands (EMAIL, NEXT, READ, COUNT) with their respective parameter(s)
Outputs: Number of emails to read and the next email if NEXT command and COUNT command are called
Collaborators: Gemini
Other sources: Gemini
Creation date: 9/15/2026 5:00PM
Revision date: 9/17/2026 8:59PM
Revisions: debug EMAIL parsing method
*/

/*
 * Email Priority Program
 * As specified in instructions.md
 */

#include <stdio.h>    // Include standard I/O for reading and printing
#include <stdlib.h>   // Include standard library for memory management
#include <string.h>   // Include string functions for parsing logic
#include <ctype.h>    // Include ctype for whitespace checking

// Define the structure for an individual Email
typedef struct {
    char category[50]; // Buffer to store the sender category
    char subject[256]; // Buffer to store the email subject
    char date[15];     // Buffer to store the date string
    int priority;      // Integer to store calculated category priority
    long date_val;     // Long integer to store sortable date value
} Email;

// Define the structure for a MaxHeap priority queue
typedef struct {
    Email *list;       // Pointer to dynamic array of Emails
    int size;          // Track the current number of emails in the heap
    int capacity;      // Track the maximum capacity of the allocated array
} MaxHeap;

// Function to trim leading and trailing whitespace from strings
void trim_whitespace(char *str) {
    char *start = str;                                        // Set start pointer to beginning of string
    while (*start && isspace((unsigned char)*start)) start++; // Increment pointer past leading whitespace
    size_t len = strlen(start);                               // Get length of the remaining string
    memmove(str, start, len + 1);                             // Shift the trimmed string to the start buffer
    while (len > 0 && isspace((unsigned char)str[len - 1])) { // Check for trailing whitespace
        str[len - 1] = '\0';                                  // Null-terminate to remove trailing spaces
        len--;                                                // Decrement length accordingly
    }
}

// Function to convert category strings into priority values
int get_category_priority(const char *category) {
    if (strcmp(category, "Boss") == 0) return 5;            // Highest priority for Boss
    if (strcmp(category, "Subordinate") == 0) return 4;     // Second priority for Subordinate
    if (strcmp(category, "Peer") == 0) return 3;            // Third priority for Peer
    if (strcmp(category, "ImportantPerson") == 0) return 2; // Fourth priority for ImportantPerson
    if (strcmp(category, "OtherPerson") == 0) return 1;     // Lowest priority for OtherPerson
    return 0;                                               // Default fallback
}

// Function to convert MM-DD-YYYY date into a sortable YYYYMMDD integer
long parse_date(const char *date_str) {
    int m = 0, d = 0, y = 0;                          // Initialize month, day, and year variables
    sscanf(date_str, "%d-%d-%d", &m, &d, &y);         // Extract values from formatted string
    return (long)y * 10000 + (long)m * 100 + (long)d; // Calculate and return sortable long value
}

// Function to compare two emails to establish max-heap order
int compare_emails(Email a, Email b) {
    if (a.priority != b.priority) {                    // Check if sender priorities differ
        return a.priority - b.priority;                // Higher priority sender wins
    }
    if (a.date_val != b.date_val) {                    // Check if dates differ for identical senders
        return (a.date_val > b.date_val) ? 1 : -1;     // Newer date (higher value) wins
    }
    return 0;                                          // Emails are perfectly equal in priority
}

// Function to initialize the heap with a starting capacity
void init_heap(MaxHeap *heap, int capacity) {
    heap->capacity = capacity;                              // Set the heap capacity limit
    heap->size = 0;                                         // Initialize current size to zero
    heap->list = (Email *)malloc(capacity * sizeof(Email)); // Allocate memory for the email array
}

// Utility function to swap two emails
void swap(Email *a, Email *b) {
    Email temp = *a; // Store element 'a' in a temporary variable
    *a = *b;         // Overwrite 'a' with the contents of 'b'
    *b = temp;       // Overwrite 'b' with the temporary variable
}

// Function to "bubble up" an email to maintain max-heap properties
void heapify_up(MaxHeap *heap, int index) {
    while (index > 0) {                         // loop when index is larger than 0
        int parent = (index - 1) / 2;           // parent expression
        if (compare_emails(heap->list[index], heap->list[parent]) <= 0) {   // when index is smaller than parent
            break;                                                          // break out from loop
        }
        swap(&heap->list[index], &heap->list[parent]);                      // swap current index when it is larger than parent 
        index = parent;                                                     // the current index is now assigned to parent index
    }
}

// Function to "trickle down" an email to maintain max-heap properties
void heapify_down(MaxHeap *heap, int index) {
    while (1) {                                                             // Continues checking children until the heap property is restored.
        int largest = index;                                                // current index assigned to largest            
        int left = 2 * index + 1;                                           // left child expression
        int right = 2 * index + 2;                                          // right child expression

        if (left < heap->size && compare_emails(heap->list[left], heap->list[largest]) > 0) {       // if left child value has high priority than current index value
            largest = left;         // Moves largest to left child
        }                                                                 

        if (right < heap->size && compare_emails(heap->list[right], heap->list[largest]) > 0) {     // if left child value has high priority than current index value
            largest = right;        // Moves largest to right child
        }                                                                  

        if (largest == index) {     // Checks whether the current element is already the highest-priority element among its children.
            break;                  // Stops the loop because the MaxHeap property has been restored.
        }                                                    
        swap(&heap->list[index], &heap->list[largest]);      // Swaps the current element with the highest-priority child.
        index = largest;                                     // Moves the current index downward to the child's position.
    }
}

// Function to insert a newly arrived email into the heap
void insert(MaxHeap *heap, Email email) {
    if (heap->size == heap->capacity) {                                            // Check if heap array is full
        heap->capacity *= 2;                                                       // Double the capacity limit
        heap->list = (Email *)realloc(heap->list, heap->capacity * sizeof(Email)); // Reallocate memory to new limit
    }
    heap->list[heap->size] = email; // Place the new email at the end of the heap
    heapify_up(heap, heap->size);   // Bubble up the new email to its proper position
    heap->size++;                   // Increment total size counter
}

// Function to remove and return the most important email
Email extract_max(MaxHeap *heap) {
    Email max_email = heap->list[0];            // Store the root (highest priority) email
    heap->list[0] = heap->list[heap->size - 1]; // Move the last leaf node to the root position
    heap->size--;                               // Decrement total size counter
    heapify_down(heap, 0);                      // Trickle down the new root to its proper position
    return max_email;                           // Return the extracted email
}

// Seperated function to handle the entire EMAIL command processing
void process_email_command(MaxHeap *heap, char *line) {
    // If the string is exactly 5 characters, all parameters (and trailing spaces) are missing
    if (strlen(line) == 5) {
        fprintf(stderr, "Error: EMAIL command supplied with zero parameters.\n");
        return;
    }
    
    // Ensure there is actually a space after EMAIL (prevents matching invalid commands like "EMAILBOX")
    if (line[5] != ' ') {
        return;
    }

    char *content = line + 6;          // Pointer to content skipping "EMAIL "
    trim_whitespace(content);          // remove whitespaces in contents of email
    
    if (strlen(content) == 0) {        // Secondary safety check for empty content
        fprintf(stderr, "Error: EMAIL command supplied with zero parameters.\n");
        return;
    }
    
    char *cat_ptr = strtok(content, ","); // Extract category up to first comma
    char *subj_ptr = strtok(NULL, ",");   // Extract subject up to second comma
    char *date_ptr = strtok(NULL, "");    // Extract date (the remainder of the line)

    if (cat_ptr && subj_ptr && date_ptr) { // Verify all three parts exist
        Email new_email;                   // Declare a new email struct
        strcpy(new_email.category, cat_ptr); // Copy parsed category
        strcpy(new_email.subject, subj_ptr); // Copy parsed subject
        strcpy(new_email.date, date_ptr);    // Copy parsed date

        trim_whitespace(new_email.category); // Clean parsed category whitespace
        trim_whitespace(new_email.subject);  // Clean parsed subject whitespace
        trim_whitespace(new_email.date);     // Clean parsed date whitespace

        // --- THE FIX ---                                             
        // Check if any fields resulted in an empty string after trimming (e.g. missing text between commas)
        if (strlen(new_email.category) == 0 || 
            strlen(new_email.subject) == 0 || 
            strlen(new_email.date) == 0) {
            fprintf(stderr, "Error: EMAIL command has empty parameters. Expected format: EMAIL Category, Subject, Date\n");
            return;
        }

        new_email.priority = get_category_priority(new_email.category); // Derive priority value
        new_email.date_val = parse_date(new_email.date);                // Derive sortable date

        insert(heap, new_email); // Insert fully processed email into heap
    } else {
        fprintf(stderr, "Error: EMAIL command is missing required parameters. Expected format: EMAIL Category, Subject, Date\n");
    }
}

// Main execution loop
int main() {
    MaxHeap heap;             // Declare the heap variable
    init_heap(&heap, 10);     // Initialize the heap with a capacity of 10
    char line[512];           // Buffer to store standard input strings

    while (fgets(line, sizeof(line), stdin)) { // Continuously read lines from standard input

        // Strip the trailing newline (\n or \r\n) from fgets to ensure exact string matching works smoothly
        line[strcspn(line, "\r\n")] = '\0';

        if (strncmp(line, "EMAIL", 5) == 0) {  // Command check: starts with "EMAIL"
            process_email_command(&heap, line);
        } else if (strcmp(line, "NEXT") == 0) { // Command check: NEXT
            if (heap.size > 0) {                // Ensure the heap is not empty
                Email top = heap.list[0];       // Peek at the root element
                printf("Next email:\n");              // Output format label
                printf("Sender: %s\n", top.category); // Output sender category
                printf("Subject: %s\n", top.subject); // Output subject line
                printf("Date: %s\n", top.date);       // Output date
            } else {
                printf("Inbox is empty.\n");    // Fallback if no emails exist
            }
        } else if (strcmp(line, "READ") == 0) { // Command check: READ
            if (heap.size > 0) {                // Ensure the heap is not empty
                extract_max(&heap);             // Delete top priority email from heap
            }
        } else if (strcmp(line, "COUNT") == 0) { // Command check: COUNT
            printf("There are %d emails to read.\n", heap.size); // Output current email count
        }
    }

    free(heap.list); // Free the heap's dynamically allocated memory
    return 0;        // Terminate program successfully
}