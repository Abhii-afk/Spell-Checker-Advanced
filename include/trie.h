#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @file trie.h
 * @brief Trie (Prefix Tree) data structure for efficient dictionary storage and lookup
 * 
 * This module implements a Trie data structure optimized for spell checking applications.
 * The Trie provides O(m) time complexity for insert and search operations, where m is
 * the length of the word. Memory usage is optimized through careful node allocation
 * and the structure supports efficient prefix-based operations.
 * 
 * Algorithm: Standard Trie with 26-way branching for lowercase English letters (a-z).
 * Each node contains an array of 26 pointers to child nodes, a boolean flag indicating
 * if the node represents the end of a valid word, and statistics for performance tracking.
 * 
 * Time Complexity:
 * - Insert: O(m) where m is word length
 * - Search: O(m) where m is word length  
 * - Memory: O(ALPHABET_SIZE * N * L) where N is number of words, L is average length
 * 
 * Space Complexity: O(N * L) in the best case (no shared prefixes), 
 * O(ALPHABET_SIZE * N * L) in the worst case (no shared prefixes)
 * 
 * Usage Example:
 * @code
 * Trie* dict = trie_create();
 * trie_insert(dict, "hello");
 * trie_insert(dict, "world");
 * 
 * if (trie_search(dict, "hello")) {
 *     printf("Word found!\n");
 * }
 * 
 * printf("Memory usage: %zu bytes\n", trie_get_memory_usage(dict));
 * trie_destroy(dict);
 * @endcode
 */

/**
 * @brief TrieNode structure representing a single node in the Trie
 * 
 * Each node can have up to 26 children representing lowercase letters a-z.
 * The is_end_of_word flag marks valid word endings, and word_count provides
 * statistics for performance analysis and debugging.
 */
typedef struct TrieNode {
    struct TrieNode* children[26];  ///< Child nodes for letters a-z (index = letter - 'a')
    bool is_end_of_word;           ///< True if this node represents the end of a valid word
    int word_count;                ///< Statistics: number of words ending at this node
} TrieNode;

/**
 * @brief Trie structure containing the root node and metadata
 * 
 * The main Trie structure maintains a pointer to the root node and tracks
 * statistics including total word count and approximate memory usage for
 * performance monitoring and optimization.
 */
typedef struct Trie {
    TrieNode* root;         ///< Root node of the trie (represents empty string)
    int total_words;        ///< Total number of unique words stored in the trie
    size_t memory_usage;    ///< Approximate memory usage in bytes (updated on insert/delete)
} Trie;

/**
 * @brief Create a new empty Trie
 * 
 * Allocates and initializes a new Trie structure with an empty root node.
 * The root node represents the empty string and serves as the starting point
 * for all insert and search operations.
 * 
 * @return Pointer to newly created Trie, or NULL on memory allocation failure
 * 
 * @note Caller is responsible for calling trie_destroy() to free memory
 * 
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 */
Trie* trie_create(void);

/**
 * @brief Insert a word into the Trie
 * 
 * Inserts a word into the Trie by traversing from the root and creating
 * nodes as necessary. The word must contain only lowercase alphabetic
 * characters (a-z). Updates the total word count and memory usage statistics.
 * 
 * Algorithm: Traverse the trie character by character, creating new nodes
 * when necessary. Mark the final node as end-of-word and update statistics.
 * 
 * @param trie Pointer to the Trie (must not be NULL)
 * @param word Word to insert (must be lowercase, alphabetic characters only)
 * @return true if insertion successful, false on error (NULL parameters, invalid characters, memory allocation failure)
 * 
 * @pre trie != NULL && word != NULL
 * @pre word contains only lowercase letters a-z
 * 
 * Time Complexity: O(m) where m is the length of the word
 * Space Complexity: O(m) in worst case (all new nodes), O(1) in best case (word already exists)
 * 
 * Example:
 * @code
 * Trie* trie = trie_create();
 * if (trie_insert(trie, "hello")) {
 *     printf("Word inserted successfully\n");
 * }
 * @endcode
 */
bool trie_insert(Trie* trie, const char* word);

/**
 * @brief Search for a word in the Trie
 * 
 * Searches for a complete word in the Trie by traversing from the root
 * following the character path. Returns true only if the path exists
 * and the final node is marked as end-of-word.
 * 
 * @param trie Pointer to the Trie (must not be NULL)
 * @param word Word to search for
 * @return true if word exists in trie, false otherwise (including NULL parameters)
 * 
 * @pre trie != NULL && word != NULL
 * 
 * Time Complexity: O(m) where m is the length of the word
 * Space Complexity: O(1)
 * 
 * Example:
 * @code
 * if (trie_search(trie, "hello")) {
 *     printf("Word found in dictionary\n");
 * } else {
 *     printf("Word not found - possible misspelling\n");
 * }
 * @endcode
 */
bool trie_search(Trie* trie, const char* word);

/**
 * @brief Get approximate memory usage of the Trie
 * 
 * Returns the approximate memory usage of the Trie in bytes, including
 * all nodes and the main structure. This is useful for performance
 * monitoring and memory optimization.
 * 
 * @param trie Pointer to the Trie
 * @return Memory usage in bytes, or 0 if trie is NULL
 * 
 * Time Complexity: O(1) - uses cached value updated during insert/delete
 * Space Complexity: O(1)
 * 
 * @note The returned value is an approximation and may not include
 * all memory overhead from the system allocator
 */
size_t trie_get_memory_usage(Trie* trie);

/**
 * @brief Get all words stored in the Trie
 * 
 * Performs a depth-first traversal of the Trie to collect all stored words.
 * Allocates an array of strings containing all words in the trie.
 * 
 * @param trie Pointer to the Trie (must not be NULL)
 * @param words Pointer to array of strings (will be allocated by this function)
 * @param count Pointer to store number of words found
 * 
 * @pre trie != NULL && words != NULL && count != NULL
 * @post *words points to allocated array of strings, *count contains array size
 * 
 * @note Caller is responsible for freeing the allocated array and all strings
 * 
 * Time Complexity: O(N * L) where N is number of words, L is average word length
 * Space Complexity: O(N * L) for the output array
 * 
 * Example:
 * @code
 * char** all_words;
 * int word_count;
 * trie_get_all_words(trie, &all_words, &word_count);
 * 
 * for (int i = 0; i < word_count; i++) {
 *     printf("Word %d: %s\n", i, all_words[i]);
 *     free(all_words[i]);
 * }
 * free(all_words);
 * @endcode
 */
void trie_get_all_words(Trie* trie, char*** words, int* count);

/**
 * @brief Destroy the Trie and free all allocated memory
 * 
 * Recursively frees all nodes in the Trie and the main Trie structure.
 * After calling this function, the trie pointer becomes invalid and
 * should not be used.
 * 
 * @param trie Pointer to the Trie to destroy (can be NULL)
 * 
 * @post All memory associated with the trie is freed
 * @post trie pointer becomes invalid
 * 
 * Time Complexity: O(N) where N is the total number of nodes
 * Space Complexity: O(h) where h is the height of the trie (for recursion stack)
 * 
 * @note Safe to call with NULL pointer (no-op)
 * 
 * Example:
 * @code
 * Trie* trie = trie_create();
 * // ... use trie ...
 * trie_destroy(trie);
 * trie = NULL; // Good practice to avoid dangling pointer
 * @endcode
 */
void trie_destroy(Trie* trie);

#endif // TRIE_H