#include "edit_distance.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Helper function to get minimum of three integers
 */
static int min3(int a, int b, int c) {
    int min_ab = (a < b) ? a : b;
    return (min_ab < c) ? min_ab : c;
}

/**
 * Compute the minimum edit distance between two strings using dynamic programming
 * Uses space-optimized approach with only two rows instead of full matrix
 */
int compute_edit_distance(const char* word1, const char* word2) {
    if (!word1 || !word2) {
        return -1; // Invalid input
    }
    
    int len1 = strlen(word1);
    int len2 = strlen(word2);
    
    // Handle edge cases
    if (len1 == 0) return len2;
    if (len2 == 0) return len1;
    
    // Allocate two rows for space optimization
    int* prev_row = (int*)malloc((len2 + 1) * sizeof(int));
    int* curr_row = (int*)malloc((len2 + 1) * sizeof(int));
    
    if (!prev_row || !curr_row) {
        free(prev_row);
        free(curr_row);
        return -1; // Memory allocation failed
    }
    
    // Initialize first row (transforming empty string to word2[0..j])
    for (int j = 0; j <= len2; j++) {
        prev_row[j] = j;
    }
    
    // Fill the matrix row by row
    for (int i = 1; i <= len1; i++) {
        curr_row[0] = i; // Transforming word1[0..i] to empty string
        
        for (int j = 1; j <= len2; j++) {
            if (word1[i-1] == word2[j-1]) {
                // Characters match, no operation needed
                curr_row[j] = prev_row[j-1];
            } else {
                // Choose minimum cost operation
                int substitute = prev_row[j-1] + 1;  // Substitute
                int insert = curr_row[j-1] + 1;      // Insert
                int delete = prev_row[j] + 1;        // Delete
                
                curr_row[j] = min3(substitute, insert, delete);
            }
        }
        
        // Swap rows for next iteration
        int* temp = prev_row;
        prev_row = curr_row;
        curr_row = temp;
    }
    
    int result = prev_row[len2];
    
    free(prev_row);
    free(curr_row);
    
    return result;
}/**
 * C
ompute edit distance and track the sequence of operations
 * Uses full matrix to enable backtracking for operation sequence
 */
EditResult* compute_edit_operations(const char* word1, const char* word2) {
    if (!word1 || !word2) {
        return NULL;
    }
    
    int len1 = strlen(word1);
    int len2 = strlen(word2);
    
    // Allocate result structure
    EditResult* result = (EditResult*)malloc(sizeof(EditResult));
    if (!result) {
        return NULL;
    }
    
    // Allocate full DP matrix for backtracking
    int** dp = (int**)malloc((len1 + 1) * sizeof(int*));
    if (!dp) {
        free(result);
        return NULL;
    }
    
    for (int i = 0; i <= len1; i++) {
        dp[i] = (int*)malloc((len2 + 1) * sizeof(int));
        if (!dp[i]) {
            // Clean up on allocation failure
            for (int k = 0; k < i; k++) {
                free(dp[k]);
            }
            free(dp);
            free(result);
            return NULL;
        }
    }
    
    // Initialize base cases
    for (int i = 0; i <= len1; i++) {
        dp[i][0] = i;
    }
    for (int j = 0; j <= len2; j++) {
        dp[0][j] = j;
    }
    
    // Fill the DP matrix
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            if (word1[i-1] == word2[j-1]) {
                dp[i][j] = dp[i-1][j-1]; // Match
            } else {
                int substitute = dp[i-1][j-1] + 1;
                int insert = dp[i][j-1] + 1;
                int delete = dp[i-1][j] + 1;
                dp[i][j] = min3(substitute, insert, delete);
            }
        }
    }
    
    result->distance = dp[len1][len2];
    
    // Backtrack to find operations
    int max_ops = len1 + len2; // Upper bound on operations
    result->operations = (EditOperation*)malloc(max_ops * sizeof(EditOperation));
    if (!result->operations) {
        // Clean up on allocation failure
        for (int i = 0; i <= len1; i++) {
            free(dp[i]);
        }
        free(dp);
        free(result);
        return NULL;
    }
    
    // Backtrack from dp[len1][len2] to dp[0][0]
    int i = len1, j = len2;
    int op_count = 0;
    
    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && word1[i-1] == word2[j-1]) {
            // Characters match
            result->operations[op_count].type = EDIT_MATCH;
            result->operations[op_count].from_char = word1[i-1];
            result->operations[op_count].to_char = word2[j-1];
            result->operations[op_count].position = i-1;
            op_count++;
            i--;
            j--;
        } else if (i > 0 && j > 0 && dp[i][j] == dp[i-1][j-1] + 1) {
            // Substitution
            result->operations[op_count].type = EDIT_SUBSTITUTE;
            result->operations[op_count].from_char = word1[i-1];
            result->operations[op_count].to_char = word2[j-1];
            result->operations[op_count].position = i-1;
            op_count++;
            i--;
            j--;
        } else if (j > 0 && dp[i][j] == dp[i][j-1] + 1) {
            // Insertion
            result->operations[op_count].type = EDIT_INSERT;
            result->operations[op_count].from_char = '\0';
            result->operations[op_count].to_char = word2[j-1];
            result->operations[op_count].position = i;
            op_count++;
            j--;
        } else if (i > 0 && dp[i][j] == dp[i-1][j] + 1) {
            // Deletion
            result->operations[op_count].type = EDIT_DELETE;
            result->operations[op_count].from_char = word1[i-1];
            result->operations[op_count].to_char = '\0';
            result->operations[op_count].position = i-1;
            op_count++;
            i--;
        }
    }
    
    result->operation_count = op_count;
    
    // Reverse operations array to get correct order
    for (int k = 0; k < op_count / 2; k++) {
        EditOperation temp = result->operations[k];
        result->operations[k] = result->operations[op_count - 1 - k];
        result->operations[op_count - 1 - k] = temp;
    }
    
    // Clean up DP matrix
    for (int i = 0; i <= len1; i++) {
        free(dp[i]);
    }
    free(dp);
    
    return result;
}

/**
 * Get a single row of the edit distance matrix for partial computations
 * Useful for generating suggestions within a specific edit distance threshold
 */
int* get_distance_row(const char* word, const char* target, int target_len) {
    if (!word || !target || target_len < 0) {
        return NULL;
    }
    
    int word_len = strlen(word);
    
    // Allocate row array
    int* row = (int*)malloc((target_len + 1) * sizeof(int));
    if (!row) {
        return NULL;
    }
    
    // Initialize first row
    for (int j = 0; j <= target_len; j++) {
        row[j] = j;
    }
    
    // If word is empty, return initialized row
    if (word_len == 0) {
        return row;
    }
    
    // Allocate previous row for computation
    int* prev_row = (int*)malloc((target_len + 1) * sizeof(int));
    if (!prev_row) {
        free(row);
        return NULL;
    }
    
    // Copy initial row to prev_row
    memcpy(prev_row, row, (target_len + 1) * sizeof(int));
    
    // Compute each row
    for (int i = 1; i <= word_len; i++) {
        row[0] = i;
        
        for (int j = 1; j <= target_len; j++) {
            if (word[i-1] == target[j-1]) {
                row[j] = prev_row[j-1];
            } else {
                int substitute = prev_row[j-1] + 1;
                int insert = row[j-1] + 1;
                int delete = prev_row[j] + 1;
                row[j] = min3(substitute, insert, delete);
            }
        }
        
        // Swap rows
        int* temp = prev_row;
        prev_row = row;
        row = temp;
    }
    
    // prev_row now contains the final result
    free(row);
    return prev_row;
}

/**
 * Free memory allocated for EditResult structure
 */
void free_edit_result(EditResult* result) {
    if (result) {
        free(result->operations);
        free(result);
    }
}