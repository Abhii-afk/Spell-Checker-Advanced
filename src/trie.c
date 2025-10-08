#include "../include/trie.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * Create a new TrieNode with all children initialized to NULL
 * @return Pointer to newly created TrieNode, or NULL on memory allocation failure
 */
static TrieNode* trie_node_create(void) {
    TrieNode* node = (TrieNode*)malloc(sizeof(TrieNode));
    if (node == NULL) {
        return NULL;
    }
    
    // Initialize all children to NULL
    for (int i = 0; i < 26; i++) {
        node->children[i] = NULL;
    }
    
    node->is_end_of_word = false;
    node->word_count = 0;
    
    return node;
}

/**
 * Recursively destroy a TrieNode and all its children
 * @param node Pointer to the TrieNode to destroy
 */
static void trie_node_destroy(TrieNode* node) {
    if (node == NULL) {
        return;
    }
    
    // Recursively destroy all children
    for (int i = 0; i < 26; i++) {
        if (node->children[i] != NULL) {
            trie_node_destroy(node->children[i]);
        }
    }
    
    free(node);
}

/**
 * Recursively count memory usage of a TrieNode and its children
 * @param node Pointer to the TrieNode
 * @return Memory usage in bytes
 */
static size_t trie_node_memory_usage(TrieNode* node) {
    if (node == NULL) {
        return 0;
    }
    
    size_t usage = sizeof(TrieNode);
    
    // Add memory usage of all children
    for (int i = 0; i < 26; i++) {
        if (node->children[i] != NULL) {
            usage += trie_node_memory_usage(node->children[i]);
        }
    }
    
    return usage;
}

Trie* trie_create(void) {
    Trie* trie = (Trie*)malloc(sizeof(Trie));
    if (trie == NULL) {
        return NULL;
    }
    
    trie->root = trie_node_create();
    if (trie->root == NULL) {
        free(trie);
        return NULL;
    }
    
    trie->total_words = 0;
    trie->memory_usage = sizeof(Trie) + sizeof(TrieNode);
    
    return trie;
}

bool trie_insert(Trie* trie, const char* word) {
    if (trie == NULL || word == NULL || strlen(word) == 0) {
        return false;
    }
    
    TrieNode* current = trie->root;
    size_t word_len = strlen(word);
    
    // Traverse/create path for each character in the word
    for (size_t i = 0; i < word_len; i++) {
        char c = tolower(word[i]);
        
        // Only accept alphabetic characters
        if (c < 'a' || c > 'z') {
            return false;
        }
        
        int index = c - 'a';
        
        // Create child node if it doesn't exist
        if (current->children[index] == NULL) {
            current->children[index] = trie_node_create();
            if (current->children[index] == NULL) {
                return false; // Memory allocation failed
            }
            trie->memory_usage += sizeof(TrieNode);
        }
        
        current = current->children[index];
    }
    
    // Mark end of word and update statistics
    if (!current->is_end_of_word) {
        current->is_end_of_word = true;
        trie->total_words++;
    }
    current->word_count++;
    
    return true;
}

bool trie_search(Trie* trie, const char* word) {
    if (trie == NULL || word == NULL || strlen(word) == 0) {
        return false;
    }
    
    TrieNode* current = trie->root;
    size_t word_len = strlen(word);
    
    // Traverse path for each character in the word
    for (size_t i = 0; i < word_len; i++) {
        char c = tolower(word[i]);
        
        // Only accept alphabetic characters
        if (c < 'a' || c > 'z') {
            return false;
        }
        
        int index = c - 'a';
        
        // If path doesn't exist, word is not in trie
        if (current->children[index] == NULL) {
            return false;
        }
        
        current = current->children[index];
    }
    
    // Word exists only if we reached a node marked as end of word
    return current->is_end_of_word;
}

size_t trie_get_memory_usage(Trie* trie) {
    if (trie == NULL) {
        return 0;
    }
    
    return sizeof(Trie) + trie_node_memory_usage(trie->root);
}

/**
 * Recursively collect all words from a TrieNode
 * @param node Current TrieNode
 * @param prefix Current prefix string
 * @param words Array to store words
 * @param count Current count of words found
 * @param capacity Current capacity of words array
 */
static void trie_collect_words(TrieNode* node, char* prefix, char*** words, int* count, int* capacity) {
    if (node == NULL) {
        return;
    }
    
    // If this node marks end of word, add it to results
    if (node->is_end_of_word) {
        // Resize array if needed
        if (*count >= *capacity) {
            *capacity *= 2;
            *words = (char**)realloc(*words, *capacity * sizeof(char*));
            if (*words == NULL) {
                return; // Memory allocation failed
            }
        }
        
        // Copy the current prefix as a word
        (*words)[*count] = (char*)malloc((strlen(prefix) + 1) * sizeof(char));
        if ((*words)[*count] != NULL) {
            strcpy((*words)[*count], prefix);
            (*count)++;
        }
    }
    
    // Recursively collect words from all children
    size_t prefix_len = strlen(prefix);
    for (int i = 0; i < 26; i++) {
        if (node->children[i] != NULL) {
            // Extend prefix with current character
            char* new_prefix = (char*)malloc((prefix_len + 2) * sizeof(char));
            if (new_prefix != NULL) {
                strcpy(new_prefix, prefix);
                new_prefix[prefix_len] = 'a' + i;
                new_prefix[prefix_len + 1] = '\0';
                
                trie_collect_words(node->children[i], new_prefix, words, count, capacity);
                free(new_prefix);
            }
        }
    }
}

void trie_get_all_words(Trie* trie, char*** words, int* count) {
    if (trie == NULL || words == NULL || count == NULL) {
        if (words != NULL) *words = NULL;
        if (count != NULL) *count = 0;
        return;
    }
    
    *count = 0;
    int capacity = 100; // Initial capacity
    *words = (char**)malloc(capacity * sizeof(char*));
    if (*words == NULL) {
        return;
    }
    
    char* empty_prefix = (char*)malloc(1 * sizeof(char));
    if (empty_prefix != NULL) {
        empty_prefix[0] = '\0';
        trie_collect_words(trie->root, empty_prefix, words, count, &capacity);
        free(empty_prefix);
    }
    
    // If no words found, free the array
    if (*count == 0) {
        free(*words);
        *words = NULL;
    }
}

void trie_destroy(Trie* trie) {
    if (trie == NULL) {
        return;
    }
    
    trie_node_destroy(trie->root);
    free(trie);
}