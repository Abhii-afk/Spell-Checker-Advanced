#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdbool.h>
#include "trie.h"

/**
 * @file file_io.h
 * @brief File I/O operations for dictionary loading and text processing
 * 
 * This module handles all file input/output operations for the spell checker,
 * including dictionary loading into Trie structures and text file tokenization
 * for spell checking. The module uses stream-based processing to handle large
 * files efficiently and provides robust error handling for various file formats.
 * 
 * Features:
 * - Stream-based file reading for memory efficiency
 * - Robust tokenization with word boundary detection
 * - Preservation of original text formatting for accurate reporting
 * - Comprehensive error handling for file access issues
 * - Support for various text encodings and line endings
 * 
 * Algorithm: State machine-based tokenization that identifies word boundaries
 * while preserving position information. Normalizes words for dictionary lookup
 * while maintaining original forms for user-friendly error reporting.
 * 
 * Usage Example:
 * @code
 * Trie* dictionary = trie_create();
 * if (load_dictionary("words.txt", dictionary)) {
 *     TextDocument* doc = load_text_file("document.txt");
 *     if (doc) {
 *         // Process document...
 *         free_text_document(doc);
 *     }
 * }
 * trie_destroy(dictionary);
 * @endcode
 */

/**
 * @brief TextToken structure representing a single token from text processing
 * 
 * Stores both normalized and original forms of words to enable accurate
 * dictionary lookup while preserving formatting for user-friendly reporting.
 * Includes position information for precise error location reporting.
 */
typedef struct TextToken {
    char* word;           ///< Normalized word (lowercase, no punctuation) for dictionary lookup
    char* original_word;  ///< Original word preserving case/punctuation for reporting
    int line_number;      ///< Line number where token was found (1-based)
    int position;         ///< Character position within the line (0-based)
} TextToken;

/**
 * @brief TextDocument structure containing all tokens from a processed text file
 * 
 * Represents a complete tokenized document with metadata for processing
 * and statistics. Maintains filename for error reporting and provides
 * both unique and total word counts for analysis.
 */
typedef struct TextDocument {
    TextToken* tokens;    ///< Array of tokens extracted from the document
    int token_count;      ///< Number of unique tokens in the document
    int total_words;      ///< Total word count including duplicates and skipped tokens
    char* filename;       ///< Source filename for error reporting
} TextDocument;

/**
 * @brief Load dictionary from file into Trie data structure
 * 
 * Reads a dictionary file (one word per line) and populates the provided
 * Trie with all valid words. Handles various line endings and performs
 * word validation and normalization during loading.
 * 
 * File Format: Plain text, one word per line, UTF-8 encoding
 * - Empty lines are skipped
 * - Words are normalized to lowercase
 * - Invalid characters are filtered out
 * - Duplicate words are handled gracefully
 * 
 * @param filename Path to dictionary file (must not be NULL)
 * @param trie Pointer to Trie to populate (must not be NULL)
 * @return true if loading successful, false on error (file not found, invalid format, memory allocation failure)
 * 
 * @pre filename != NULL && trie != NULL
 * @post On success, trie contains all valid words from the file
 * 
 * Time Complexity: O(n * m) where n = number of words, m = average word length
 * Space Complexity: O(n * m) for the trie storage
 * 
 * Example:
 * @code
 * Trie* dictionary = trie_create();
 * if (load_dictionary("english_words.txt", dictionary)) {
 *     printf("Dictionary loaded with %d words\n", dictionary->total_words);
 * } else {
 *     printf("Failed to load dictionary\n");
 * }
 * @endcode
 */
bool load_dictionary(const char* filename, Trie* trie);

/**
 * @brief Load and tokenize text file for spell checking
 * 
 * Reads a text file and tokenizes it into individual words while preserving
 * position information and original formatting. Uses stream-based processing
 * to handle large files efficiently.
 * 
 * Tokenization Rules:
 * - Word boundaries: whitespace, punctuation, line breaks
 * - Preserves original case and punctuation in original_word field
 * - Normalizes words for dictionary lookup in word field
 * - Tracks line numbers and character positions
 * - Handles various text encodings and line endings
 * 
 * @param filename Path to text file to process (must not be NULL)
 * @return Pointer to TextDocument structure, or NULL on error (file not found, memory allocation failure)
 * 
 * @pre filename != NULL
 * @post Caller must call free_text_document() on returned pointer
 * 
 * Time Complexity: O(n) where n = file size in characters
 * Space Complexity: O(w) where w = number of words in the file
 * 
 * Example:
 * @code
 * TextDocument* doc = load_text_file("essay.txt");
 * if (doc) {
 *     printf("Loaded %d tokens from %s\n", doc->token_count, doc->filename);
 *     free_text_document(doc);
 * }
 * @endcode
 */
TextDocument* load_text_file(const char* filename);

/**
 * @brief Normalize word by converting to lowercase and removing punctuation
 * 
 * Creates a normalized version of a word suitable for dictionary lookup.
 * Removes leading/trailing punctuation, converts to lowercase, and filters
 * out non-alphabetic characters while preserving the core word.
 * 
 * Normalization Rules:
 * - Convert to lowercase (A-Z → a-z)
 * - Remove leading/trailing punctuation
 * - Remove internal non-alphabetic characters
 * - Preserve apostrophes in contractions (don't → dont)
 * - Return NULL for words with no alphabetic characters
 * 
 * @param word Original word to normalize (must not be NULL)
 * @return Newly allocated normalized word string, or NULL on error or invalid input
 * 
 * @pre word != NULL
 * @post Caller must free the returned string
 * 
 * Time Complexity: O(m) where m = word length
 * Space Complexity: O(m) for the normalized word
 * 
 * Example:
 * @code
 * char* normalized = normalize_word("Hello!");
 * printf("Normalized: '%s'\n", normalized); // Output: "hello"
 * free(normalized);
 * @endcode
 */
char* normalize_word(const char* word);

/**
 * @brief Check if a token represents a valid word for spell checking
 * 
 * Determines whether a token should be included in spell checking based
 * on various criteria including length, character composition, and format.
 * Filters out numbers, URLs, email addresses, and other non-word tokens.
 * 
 * Validation Criteria:
 * - Contains at least one alphabetic character
 * - Length between 1 and 50 characters
 * - Not purely numeric
 * - Not a URL or email address
 * - Not excessive punctuation
 * 
 * @param word Word to validate (must not be NULL)
 * @return true if word should be spell checked, false otherwise
 * 
 * @pre word != NULL
 * 
 * Time Complexity: O(m) where m = word length
 * Space Complexity: O(1)
 * 
 * Example:
 * @code
 * if (is_valid_word("hello")) {
 *     // Proceed with spell checking
 * } else {
 *     // Skip this token
 * }
 * @endcode
 */
bool is_valid_word(const char* word);

/**
 * @brief Free all memory associated with a TextDocument
 * 
 * Recursively frees all memory allocated for a TextDocument including
 * all tokens, strings, and the document structure itself. Safe to call
 * with NULL pointer.
 * 
 * @param doc Pointer to TextDocument to free (can be NULL)
 * 
 * @post All memory associated with doc is freed
 * @post doc pointer becomes invalid
 * 
 * Time Complexity: O(n) where n = number of tokens
 * Space Complexity: O(1)
 * 
 * @note Safe to call with NULL pointer (no-op)
 */
void free_text_document(TextDocument* doc);

#endif // FILE_IO_H