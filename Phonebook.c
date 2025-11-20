#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the size of the hash table
#define TABLE_SIZE 100

// Define max lengths for contact details
#define MAX_NAME_LEN 50
#define MAX_PHONE_LEN 15

// Structure for a contact (a node in the linked list)
typedef struct ContactNode {
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    struct ContactNode *next;
} ContactNode;

// Structure for the hash table
typedef struct HashTable {
    int size;
    ContactNode **table; // Array of pointers to ContactNode
} HashTable;

/**
 * @brief Creates a new hash table.
 * @param size The number of buckets in the hash table.
 * @return A pointer to the newly created hash table.
 */
HashTable* createHashTable(int size) {
    HashTable *ht = (HashTable*)malloc(sizeof(HashTable));
    if (!ht) {
        perror("Failed to allocate HashTable");
        exit(EXIT_FAILURE);
    }

    ht->size = size;
    // Allocate memory for the array of pointers
    ht->table = (ContactNode**)calloc(size, sizeof(ContactNode*));
    if (!ht->table) {
        perror("Failed to allocate table array");
        free(ht);
        exit(EXIT_FAILURE);
    }
    
    // All pointers are automatically initialized to NULL by calloc()
    return ht;
}

/**
 * @brief The hash function.
 * Uses a simple polynomial rolling hash (djb2 variant).
 * @param name The key (contact name) to hash.
 * @param tableSize The size of the hash table.
 * @return The calculated hash index.
 */
unsigned int hashFunction(const char *name, int tableSize) {
    unsigned long hash = 5381;
    int c;

    while ((c = *name++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash % tableSize;
}

/**
 * @brief Inserts a new contact into the hash table.
 * @param ht A pointer to the hash table.
 * @param name The contact's name.
 * @param phone The contact's phone number.
 */
void insertContact(HashTable *ht, const char *name, const char *phone) {
    // 1. Get the hash index
    unsigned int index = hashFunction(name, ht->size);

    // 2. Create the new contact node
    ContactNode *newNode = (ContactNode*)malloc(sizeof(ContactNode));
    if (!newNode) {
        perror("Failed to allocate ContactNode");
        return;
    }
    strncpy(newNode->name, name, MAX_NAME_LEN - 1);
    newNode->name[MAX_NAME_LEN - 1] = '\0'; // Ensure null-termination
    strncpy(newNode->phone, phone, MAX_PHONE_LEN - 1);
    newNode->phone[MAX_PHONE_LEN - 1] = '\0'; // Ensure null-termination

    // 3. Insert at the head of the linked list (separate chaining)
    newNode->next = ht->table[index];
    ht->table[index] = newNode;

    printf("SUCCESS: Added '%s' with phone '%s'.\n", name, phone);
}

/**
 * @brief Searches for a contact by name.
 * @param ht A pointer to the hash table.
 * @param name The name to search for.
 * @return A pointer to the found ContactNode, or NULL if not found.
 */
ContactNode* searchContact(HashTable *ht, const char *name) {
    // 1. Get the hash index
    unsigned int index = hashFunction(name, ht->size);

    // 2. Traverse the linked list at that index
    ContactNode *temp = ht->table[index];
    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) {
            // Found it!
            return temp;
        }
        temp = temp->next;
    }

    // 3. Not found
    return NULL;
}

/**
 * @brief Deletes a contact by name.
 * @param ht A pointer to the hash table.
 * @param name The name of the contact to delete.
 */
void deleteContact(HashTable *ht, const char *name) {
    // 1. Get the hash index
    unsigned int index = hashFunction(name, ht->size);

    // 2. Traverse the list to find the node
    ContactNode *current = ht->table[index];
    ContactNode *prev = NULL;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            // Found the node to delete
            
            // Case 1: It's the head of the list
            if (prev == NULL) {
                ht->table[index] = current->next;
            } 
            // Case 2: It's in the middle or end
            else {
                prev->next = current->next;
            }

            free(current); // Free the memory
            printf("SUCCESS: Deleted '%s'.\n", name);
            return;
        }
        // Move to the next node
        prev = current;
        current = current->next;
    }

    // 3. If loop finishes, the contact was not found
    printf("ERROR: Contact '%s' not found.\n", name);
}

/**
 * @brief Displays all contacts in the phonebook.
 * @param ht A pointer to the hash table.
 */
void displayContacts(HashTable *ht) {
    printf("\n--- 📖 Phonebook Contacts 📖 ---\n");
    int empty = 1;
    for (int i = 0; i < ht->size; i++) {
        ContactNode *temp = ht->table[i];
        if (temp != NULL) {
            empty = 0;
            printf("Bucket[%d]:\n", i);
            while (temp != NULL) {
                printf("  -> Name: %-20s | Phone: %s\n", temp->name, temp->phone);
                temp = temp->next;
            }
        }
    }
    if (empty) {
        printf("Phonebook is empty.\n");
    }
    printf("----------------------------------\n");
}

/**
 * @brief Frees all allocated memory for the hash table.
 * @param ht A pointer to the hash table.
 */
void freeHashTable(HashTable *ht) {
    if (!ht) return;

    for (int i = 0; i < ht->size; i++) {
        ContactNode *current = ht->table[i];
        while (current != NULL) {
            ContactNode *temp = current;
            current = current->next;
            free(temp); // Free each node
        }
    }
    free(ht->table); // Free the array of pointers
    free(ht);        // Free the hash table structure
    printf("Phonebook memory freed.\n");
}

// Helper function to clear the input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Main driver function
int main() {
    HashTable *phonebook = createHashTable(TABLE_SIZE);
    int choice;
    char name[MAX_NAME_LEN];
    char phone[MAX_PHONE_LEN];
    ContactNode *found;

    while (1) {
        printf("\n--- Contact/Phonebook Menu ---\n");
        printf("1. Add Contact\n");
        printf("2. Search Contact\n");
        printf("3. Delete Contact\n");
        printf("4. Display All Contacts\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }
        clearInputBuffer(); // Consume the newline character

        switch (choice) {
            case 1: // Add
                printf("Enter Name: ");
                fgets(name, MAX_NAME_LEN, stdin);
                name[strcspn(name, "\n")] = 0; // Remove newline

                printf("Enter Phone: ");
                fgets(phone, MAX_PHONE_LEN, stdin);
                phone[strcspn(phone, "\n")] = 0; // Remove newline

                insertContact(phonebook, name, phone);
                break;

            case 2: // Search
                printf("Enter Name to Search: ");
                fgets(name, MAX_NAME_LEN, stdin);
                name[strcspn(name, "\n")] = 0; // Remove newline

                found = searchContact(phonebook, name);
                if (found) {
                    printf("FOUND: Name: %s, Phone: %s\n", found->name, found->phone);
                } else {
                    printf("ERROR: Contact '%s' not found.\n", name);
                }
                break;

            case 3: // Delete
                printf("Enter Name to Delete: ");
                fgets(name, MAX_NAME_LEN, stdin);
                name[strcspn(name, "\n")] = 0; // Remove newline
                deleteContact(phonebook, name);
                break;

            case 4: // Display
                displayContacts(phonebook);
                break;

            case 5: // Exit
                printf("Exiting...\n");
                freeHashTable(phonebook); // Clean up memory
                return 0;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}