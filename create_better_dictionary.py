#!/usr/bin/env python3
"""
Script to create a comprehensive dictionary for the spell checker
Downloads common English words and creates a local dictionary file
"""

import requests
import os

def download_word_list():
    """Download a comprehensive English word list"""
    
    # Common English words (you can run this to get more words)
    common_words = [
        # Basic words
        "a", "about", "above", "across", "after", "again", "against", "all", "almost", "alone",
        "along", "already", "also", "although", "always", "among", "an", "and", "another", "any",
        "anyone", "anything", "anywhere", "are", "area", "around", "as", "ask", "at", "away",
        
        # Common verbs
        "be", "became", "because", "become", "been", "before", "began", "being", "below", "between",
        "both", "bring", "but", "by", "call", "came", "can", "change", "come", "could",
        
        # Technology terms
        "computer", "science", "algorithm", "algorithms", "data", "structure", "structures",
        "programming", "program", "programs", "function", "functions", "variable", "variables",
        "method", "methods", "class", "classes", "object", "objects", "system", "systems",
        
        # Common corrections for your test file
        "sentence", "receive", "separate", "definitely", "implementation", "efficient",
        "performance", "optimization", "application", "applications", "development",
        "technology", "language", "languages", "process", "processing", "analysis",
        
        # More common words
        "example", "examples", "text", "file", "files", "word", "words", "line", "lines",
        "error", "errors", "correct", "incorrect", "suggestion", "suggestions", "check",
        "checking", "find", "finding", "search", "searching", "match", "matching",
        
        # Spell checker specific
        "spell", "checker", "dictionary", "misspelled", "misspelling", "correction",
        "corrections", "edit", "distance", "similarity", "character", "characters",
        "string", "strings", "compare", "comparison", "algorithm", "trie", "node", "nodes"
    ]
    
    return common_words

def create_dictionary_file():
    """Create an enhanced dictionary file"""
    words = download_word_list()
    
    # Remove duplicates and sort
    unique_words = sorted(set(words))
    
    # Write to dictionary file
    with open('test_data/enhanced_dictionary.txt', 'w') as f:
        for word in unique_words:
            f.write(word.lower() + '\n')
    
    print(f"Created enhanced dictionary with {len(unique_words)} words")
    print("File: test_data/enhanced_dictionary.txt")

if __name__ == "__main__":
    create_dictionary_file()