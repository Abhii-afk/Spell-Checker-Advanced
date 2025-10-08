#ifndef EDIT_DISTANCE_H
#define EDIT_DISTANCE_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @file edit_distance.h
 * @brief Levenshtein Edit Distance computation using dynamic programming
 * 
 * This module implements the Levenshtein edit distance algorithm to compute
 * the minimum number of single-character edits (insertions, deletions, or
 * substitutions) required to transform one string into another. The implementation
 * uses space-optimized dynamic programming for efficiency.
 * 
 * Algorithm: Dynamic Programming approach using a 2D matrix where dp[i][j]
 * represents the minimum edit distance between the first i characters of word1
 * and the first j characters of word2. Space optimization uses only two rows
 * instead of the full matrix.
 * 
 * Recurrence Relation:
 * - If characters match: dp[i][j] = dp[i-1][j-1]
 * - If characters differ: dp[i][j] = 1 + min(dp[i-1][j], dp[i][j-1], dp[i-1][j-1])
 *   where dp[i-1][j] represents deletion, dp[i][j-1] represents insertion,
 *   and dp[i-1][j-1] represents substitution
 * 
 * Time Complexity: O(m * n) where m and n are the lengths of the input strings
 * Space Complexity: O(min(m, n)) with space optimization, O(m * n) for operation tracking
 * 
 * Usage Example:
 * @code
 * int distance = compute_edit_distance("kitten", "sitting");
 * printf("Edit distance: %d\n", distance); // Output: 3
 * 
 * EditResult* result = compute_edit_operations("cat", "bat");
 * printf("Distance: %d, Operations: %d\n", result->distance, result->operation_count);
 * free_edit_result(result);
 * @endcode
 */

/**
 * @brief Edit operation types for tracking transformations
 * 
 * Enumeration of possible edit operations used in the transformation
 * sequence from one string to another.
 */
typedef enum {
    EDIT_INSERT,     ///< Insert a character
    EDIT_DELETE,     ///< Delete a character  
    EDIT_SUBSTITUTE, ///< Substitute one character for another
    EDIT_MATCH       ///< Characters match (no operation needed)
} EditOperationType;

/**
 * @brief Represents a single edit operation in the transformation sequence
 * 
 * Contains information about a specific edit operation including the type
 * of operation, the characters involved, and the position where the
 * operation should be applied.
 */
typedef struct EditOperation {
    EditOperationType type; ///< Type of edit operation
    char from_char;        ///< Source character (for delete/substitute operations)
    char to_char;          ///< Target character (for insert/substitute operations)
    int position;          ///< Position in the source string where operation applies
} EditOperation;

/**
 * @brief Result structure containing edit distance and operation sequence
 * 
 * Contains the computed minimum edit distance and the complete sequence
 * of operations needed to transform the source string into the target string.
 */
typedef struct EditResult {
    int distance;                ///< Minimum edit distance between the strings
    EditOperation* operations;   ///< Array of edit operations in sequence
    int operation_count;         ///< Number of operations in the sequence
} EditResult;

/**
 * @brief Compute the minimum edit distance between two strings using dynamic programming
 * 
 * Implements the Levenshtein distance algorithm using space-optimized dynamic
 * programming. Uses only two rows of the DP matrix instead of the full matrix
 * to reduce memory usage from O(m*n) to O(min(m,n)).
 * 
 * Algorithm: Initialize first row with distances 0,1,2,...,n. For each character
 * in word1, compute the next row based on the recurrence relation. The final
 * value in the last position gives the minimum edit distance.
 * 
 * @param word1 First string (source)
 * @param word2 Second string (target)
 * @return Minimum number of operations to transform word1 into word2, or -1 on error
 * 
 * @pre word1 != NULL && word2 != NULL
 * 
 * Time Complexity: O(m * n) where m = len(word1), n = len(word2)
 * Space Complexity: O(min(m, n))
 * 
 * Example:
 * @code
 * int dist = compute_edit_distance("kitten", "sitting");
 * // Returns 3 (substitute k->s, substitute e->i, insert g)
 * @endcode
 */
int compute_edit_distance(const char* word1, const char* word2);

/**
 * @brief Compute edit distance and track the sequence of operations
 * 
 * Computes the minimum edit distance while maintaining the full DP matrix
 * to enable backtracking and reconstruction of the optimal operation sequence.
 * This provides detailed information about how to transform word1 into word2.
 * 
 * Algorithm: Build full DP matrix, then backtrack from bottom-right to top-left
 * to reconstruct the sequence of operations that achieves the minimum distance.
 * 
 * @param word1 First string (source)
 * @param word2 Second string (target)
 * @return EditResult containing distance and operations, or NULL on error
 * 
 * @pre word1 != NULL && word2 != NULL
 * @post Caller must call free_edit_result() on returned pointer
 * 
 * Time Complexity: O(m * n) where m = len(word1), n = len(word2)
 * Space Complexity: O(m * n) for the full DP matrix
 * 
 * Example:
 * @code
 * EditResult* result = compute_edit_operations("cat", "bat");
 * printf("Distance: %d\n", result->distance); // 1
 * printf("Operation: substitute 'c' -> 'b' at position 0\n");
 * free_edit_result(result);
 * @endcode
 */
EditResult* compute_edit_operations(const char* word1, const char* word2);

/**
 * @brief Get a single row of the edit distance matrix for partial computations
 * 
 * Computes one row of the edit distance DP matrix, useful for generating
 * suggestions within a specific edit distance threshold. This allows for
 * early termination when searching for words within a maximum distance.
 * 
 * @param word Source word
 * @param target Target word
 * @param target_len Length of target word
 * @return Array representing one row of the DP matrix, or NULL on error
 * 
 * @pre word != NULL && target != NULL && target_len >= 0
 * @post Caller must free the returned array
 * 
 * Time Complexity: O(target_len)
 * Space Complexity: O(target_len)
 * 
 * @note Useful for implementing suggestion generation with distance thresholds
 * 
 * Example:
 * @code
 * int* row = get_distance_row("hello", "help", 4);
 * // row[4] contains edit distance from "hello" to "help"
 * free(row);
 * @endcode
 */
int* get_distance_row(const char* word, const char* target, int target_len);

/**
 * @brief Free memory allocated for EditResult structure
 * 
 * Safely frees all memory associated with an EditResult, including the
 * operations array and the structure itself. Safe to call with NULL pointer.
 * 
 * @param result EditResult to free (can be NULL)
 * 
 * @post All memory associated with result is freed
 * @post result pointer becomes invalid
 * 
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 * 
 * @note Safe to call with NULL pointer (no-op)
 */
void free_edit_result(EditResult* result);

#endif // EDIT_DISTANCE_H