#ifndef SPELL_CHECK_H
#define SPELL_CHECK_H

#include <stdbool.h>
#include <stddef.h>
#include "trie.h"
#include "file_io.h"

/**
 * Structure representing a single spelling error
 */
typedef struct SpellError {
    char* misspelled_word;      // The misspelled word
    char* original_word;        // Original word with case/punctuation
    int line_number;            // Line number where error was found
    int position;               // Position within the line
    char** suggestions;         // Array of suggested corrections
    float* suggestion_scores;   // Scores for each suggestion
    int suggestion_count;       // Number of suggestions
} SpellError;

/**
 * Structure containing spell check results
 */
typedef struct SpellCheckResult {
    SpellError* errors;         // Array of spelling errors
    int error_count;            // Number of errors found
    int total_words_checked;    // Total words processed
    double processing_time;     // Time taken (simplified)
    size_t memory_used;         // Memory used (simplified)
} SpellCheckResult;

/**
 * Perform spell checking on a text document
 */
SpellCheckResult* spell_check_document(TextDocument* doc, Trie* dictionary);

/**
 * Generate spelling suggestions for a misspelled word
 */
char** generate_suggestions(const char* misspelled_word, Trie* dictionary, int* count);

/**
 * Check if a word is likely a proper noun
 */
bool is_likely_proper_noun(const char* word, const TextToken* token);

/**
 * Free spell check result
 */
void free_spell_check_result(SpellCheckResult* result);

#endif // SPELL_CHECK_H