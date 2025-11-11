// ===== Spell Checker with Trie + API Integration =====

class SpellCheckerApp {
    constructor() {
        // Simulated Trie dictionary (local) - Expanded with common words
        this.localDictionary = new Set([
            // Common words
            'hello', 'world', 'healer', 'heal', 'health', 'healthy', 'help', 'helper',
            'the', 'and', 'for', 'are', 'but', 'not', 'you', 'all', 'can', 'her', 'was',
            'one', 'our', 'out', 'day', 'get', 'has', 'him', 'his', 'how', 'man', 'new',
            'now', 'old', 'see', 'two', 'way', 'who', 'boy', 'did', 'its', 'let', 'put',
            'say', 'she', 'too', 'use',
            // Technical terms
            'computer', 'science', 'algorithm', 'data', 'structure', 'programming',
            'software', 'hardware', 'network', 'database', 'application', 'development',
            'testing', 'debugging', 'function', 'variable', 'array', 'string', 'integer',
            'boolean', 'class', 'object', 'method', 'interface', 'inheritance',
            'polymorphism', 'encapsulation', 'abstraction', 'recursion', 'iteration',
            'loop', 'condition', 'statement', 'expression', 'operator', 'keyword',
            'syntax', 'semantic', 'compiler', 'interpreter', 'runtime', 'memory',
            'pointer', 'reference', 'stack', 'heap', 'queue', 'tree', 'graph', 'node',
            'edge', 'vertex', 'path', 'cycle', 'sort', 'search', 'binary', 'linear',
            // More common words
            'about', 'after', 'again', 'also', 'another', 'because', 'before', 'between',
            'both', 'come', 'could', 'down', 'each', 'even', 'find', 'first', 'from',
            'give', 'good', 'great', 'hand', 'have', 'here', 'high', 'into', 'just',
            'know', 'large', 'last', 'leave', 'life', 'little', 'long', 'look', 'make',
            'many', 'more', 'most', 'move', 'much', 'must', 'name', 'need', 'never',
            'next', 'only', 'other', 'over', 'part', 'people', 'place', 'point', 'right',
            'same', 'seem', 'should', 'show', 'small', 'some', 'still', 'such', 'take',
            'tell', 'than', 'that', 'their', 'them', 'then', 'there', 'these', 'they',
            'thing', 'think', 'this', 'those', 'through', 'time', 'under', 'very',
            'want', 'well', 'what', 'when', 'where', 'which', 'while', 'with', 'work',
            'would', 'year', 'your'
        ]);

        // API Key (your Merriam-Webster key)
        this.apiKey = '732e1fc6-b4b9-40ff-b398-138388b25198';
        this.apiBaseUrl = 'https://www.dictionaryapi.com/api/v3/references/collegiate/json/';

        this.initializeElements();
        this.attachEventListeners();
        this.startTypingAnimation();
    }

    initializeElements() {
        this.wordInput = document.getElementById('wordInput');
        this.checkButton = document.getElementById('checkButton');
        this.resultsSection = document.getElementById('resultsSection');
        this.statusCard = document.getElementById('statusCard');
        this.statusIcon = document.getElementById('statusIcon');
        this.statusTitle = document.getElementById('statusTitle');
        this.statusMessage = document.getElementById('statusMessage');
        this.loadingSpinner = document.getElementById('loadingSpinner');
        this.suggestionsCard = document.getElementById('suggestionsCard');
        this.suggestionsList = document.getElementById('suggestionsList');
        this.definitionCard = document.getElementById('definitionCard');
        this.definitionHeader = document.getElementById('definitionHeader');
        this.definitionContent = document.getElementById('definitionContent');
        this.successSound = document.getElementById('successSound');
    }

    attachEventListeners() {
        this.checkButton.addEventListener('click', () => this.checkSpelling());
        this.wordInput.addEventListener('keypress', (e) => {
            if (e.key === 'Enter') this.checkSpelling();
        });
        this.definitionHeader.addEventListener('click', () => this.toggleDefinition());
    }

    // ===== Typing Animation =====
    startTypingAnimation() {
        const text = 'Spell Checker using Trie & Merriam-Webster API';
        const typingElement = document.getElementById('typingText');
        let index = 0;

        const type = () => {
            if (index < text.length) {
                typingElement.textContent += text.charAt(index);
                index++;
                setTimeout(type, 80);
            }
        };

        type();
    }

    // ===== Main Spell Check Function =====
    async checkSpelling() {
        const word = this.wordInput.value.trim().toLowerCase();

        if (!word) {
            this.showError('Please enter a word');
            return;
        }

        // Show results section
        this.resultsSection.style.display = 'block';
        this.hideAllResults();

        // Step 1: Check in local Trie dictionary
        console.log('🔍 Step 1: Checking in local Trie dictionary...');
        const inLocalDict = this.searchInTrie(word);

        if (inLocalDict) {
            // Word found in local dictionary
            this.showCorrectStatus(word, 'Found in local Trie dictionary');
            this.playSuccessSound();
        } else {
            // Word not found in local dictionary
            console.log('❌ Not found in local Trie');
            this.showIncorrectStatus(word, 'Not found in local dictionary. Checking online...');

            // Step 2: Check via Merriam-Webster API
            await this.checkViaAPI(word);
        }
    }

    // ===== Trie Search Simulation =====
    searchInTrie(word) {
        // Simulates O(m) Trie lookup where m = word length
        console.log(`Trie Lookup: Searching for "${word}"`);
        return this.localDictionary.has(word);
    }

    // ===== API Check =====
    async checkViaAPI(word) {
        // Show loading spinner
        this.loadingSpinner.style.display = 'block';

        try {
            console.log('🌐 Step 2: Querying Merriam-Webster API...');
            
            // Make API request
            const url = `${this.apiBaseUrl}${encodeURIComponent(word)}?key=${this.apiKey}`;
            const response = await fetch(url);
            
            if (!response.ok) {
                throw new Error(`API Error: ${response.status}`);
            }

            const data = await response.json();
            console.log('API Response:', data);

            // Hide loading spinner
            this.loadingSpinner.style.display = 'none';

            // Check if word was found
            if (Array.isArray(data) && data.length > 0) {
                const firstResult = data[0];

                // Check if it's a valid word entry (object) or suggestions (string)
                if (typeof firstResult === 'object' && firstResult.meta) {
                    // IMPORTANT: Verify the API returned the EXACT word we searched for
                    const apiWord = firstResult.meta.id.split(':')[0].toLowerCase(); // Remove any variant markers
                    const searchWord = word.toLowerCase();
                    
                    console.log(`Comparing: searched="${searchWord}" vs api="${apiWord}"`);
                    
                    // Only accept if it's an EXACT match
                    if (apiWord === searchWord) {
                        // Word found in API - exact match!
                        console.log('✅ Word verified by API (exact match)');
                        this.showVerifiedStatus(word, 'Verified via Merriam-Webster API');
                        this.showDefinition(firstResult);
                        this.playSuccessSound();
                    } else {
                        // API returned a different word (like "absolute" for "absoluter")
                        // This means the searched word is NOT valid
                        console.log(`❌ Word not found - API returned different word: ${apiWord}`);
                        this.showIncorrectStatus(word, 'Word not found in dictionary');
                        
                        // Use API suggestions if they're strings, otherwise generate local
                        if (typeof data[1] === 'string') {
                            this.generateSuggestions(word, data);
                        } else {
                            this.generateSuggestions(word, []);
                        }
                    }
                } else {
                    // Word not found, API returned suggestions (array of strings)
                    console.log('❌ Word not found in API, showing suggestions');
                    this.showIncorrectStatus(word, 'Word not found in dictionary');
                    this.generateSuggestions(word, data);
                }
            } else {
                // No results
                this.showIncorrectStatus(word, 'Word not found in dictionary');
                this.generateSuggestions(word, []);
            }

        } catch (error) {
            console.error('API Error:', error);
            this.loadingSpinner.style.display = 'none';
            
            // Fallback to local suggestions
            this.showIncorrectStatus(word, 'API unavailable. Showing local suggestions...');
            this.generateSuggestions(word, []);
        }
    }

    // ===== Generate Suggestions using Edit Distance + Similarity Matrix =====
    generateSuggestions(word, apiSuggestions) {
        console.log('🔧 Step 3: Generating suggestions using Edit Distance + Similarity Matrix');

        let suggestions = [];

        // Use API suggestions if available (these are actual suggestions from API)
        if (apiSuggestions && apiSuggestions.length > 0 && typeof apiSuggestions[0] === 'string') {
            // API returned string suggestions (word not found)
            // Take only top 3 API suggestions
            const validApiSuggestions = apiSuggestions
                .filter(s => typeof s === 'string' && s.length > 0)
                .slice(0, 3); // MAX 3 suggestions
            
            suggestions = validApiSuggestions.map((s, index) => ({
                word: s,
                score: 98 - (index * 2) // High scores for API suggestions (98, 96, 94)
            }));
            
            console.log(`Found ${suggestions.length} API suggestions`);
        } else {
            // Generate local suggestions using Edit Distance algorithm
            const candidates = [];

            for (let dictWord of this.localDictionary) {
                const distance = this.calculateEditDistance(word, dictWord);
                
                // VERY STRICT: only edit distance of 1 or 2
                if (distance >= 1 && distance <= 2) {
                    const lengthDiff = Math.abs(word.length - dictWord.length);
                    
                    // STRICT: Only suggest words with very similar length (within 1 character)
                    if (lengthDiff <= 1) {
                        // High-accuracy scoring algorithm
                        let score = 100;
                        score -= distance * 30; // Very high penalty for edit distance
                        score -= lengthDiff * 15; // High penalty for length difference
                        
                        // Bonus for words that start with the same letter
                        if (word[0] === dictWord[0]) {
                            score += 15;
                        }
                        
                        // Bonus for words with same length
                        if (lengthDiff === 0) {
                            score += 10;
                        }
                        
                        score = Math.max(0, Math.min(100, score)); // Clamp between 0-100
                        
                        candidates.push({
                            word: dictWord,
                            distance: distance,
                            score: Math.round(score)
                        });
                    }
                }
            }

            // Sort by score (descending), then by edit distance (ascending)
            candidates.sort((a, b) => {
                if (b.score !== a.score) return b.score - a.score;
                return a.distance - b.distance;
            });
            
            // STRICT FILTERING: Only show suggestions with score >= 75 (high accuracy)
            // And limit to TOP 3 suggestions only
            suggestions = candidates
                .filter(c => c.score >= 75)
                .slice(0, 3); // MAX 3 suggestions
            
            console.log(`Found ${suggestions.length} high-accuracy local suggestions`);
        }

        if (suggestions.length > 0) {
            this.showSuggestions(suggestions);
        } else {
            console.log('No high-accuracy suggestions found');
            // Show a message when no suggestions are available
            this.suggestionsCard.style.display = 'block';
            this.suggestionsList.innerHTML = '<p style="color: var(--text-secondary); text-align: center; padding: 20px;">No similar words found in dictionary</p>';
        }
    }

    // ===== Edit Distance Algorithm (Levenshtein) =====
    calculateEditDistance(word1, word2) {
        const m = word1.length;
        const n = word2.length;
        const dp = Array(m + 1).fill(null).map(() => Array(n + 1).fill(0));

        // Initialize base cases
        for (let i = 0; i <= m; i++) dp[i][0] = i;
        for (let j = 0; j <= n; j++) dp[0][j] = j;

        // Fill DP table
        for (let i = 1; i <= m; i++) {
            for (let j = 1; j <= n; j++) {
                if (word1[i - 1] === word2[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1];
                } else {
                    dp[i][j] = 1 + Math.min(
                        dp[i - 1][j],     // deletion
                        dp[i][j - 1],     // insertion
                        dp[i - 1][j - 1]  // substitution
                    );
                }
            }
        }

        return dp[m][n];
    }

    // ===== Similarity Matrix Calculation =====
    calculateSimilarity(word1, word2) {
        // Keyboard proximity and phonetic similarity
        const keyboardMap = {
            'q': ['w', 'a'], 'w': ['q', 'e', 's'], 'e': ['w', 'r', 'd'],
            'r': ['e', 't', 'f'], 't': ['r', 'y', 'g'], 'y': ['t', 'u', 'h'],
            'u': ['y', 'i', 'j'], 'i': ['u', 'o', 'k'], 'o': ['i', 'p', 'l'],
            'p': ['o'], 'a': ['q', 's', 'z'], 's': ['w', 'a', 'd', 'x'],
            'd': ['e', 's', 'f', 'c'], 'f': ['r', 'd', 'g', 'v'],
            'g': ['t', 'f', 'h', 'b'], 'h': ['y', 'g', 'j', 'n'],
            'j': ['u', 'h', 'k', 'm'], 'k': ['i', 'j', 'l'],
            'l': ['o', 'k'], 'z': ['a', 'x'], 'x': ['s', 'z', 'c'],
            'c': ['d', 'x', 'v'], 'v': ['f', 'c', 'b'], 'b': ['g', 'v', 'n'],
            'n': ['h', 'b', 'm'], 'm': ['j', 'n']
        };

        let similarityScore = 100;
        const maxLen = Math.max(word1.length, word2.length);

        for (let i = 0; i < maxLen; i++) {
            const char1 = word1[i];
            const char2 = word2[i];

            if (char1 !== char2) {
                // Check keyboard proximity
                if (keyboardMap[char1]?.includes(char2)) {
                    similarityScore -= 5; // Small penalty for nearby keys
                } else {
                    similarityScore -= 10; // Larger penalty for distant keys
                }
            }
        }

        return Math.max(0, similarityScore);
    }

    // ===== UI Update Functions =====
    showCorrectStatus(word, message) {
        this.statusCard.style.display = 'flex';
        this.statusIcon.className = 'status-icon correct';
        this.statusTitle.textContent = `"${word}" is Correct ✅`;
        this.statusTitle.className = 'status-title correct';
        this.statusMessage.textContent = message + ' - This word is spelled correctly!';
    }

    showIncorrectStatus(word, message) {
        this.statusCard.style.display = 'flex';
        this.statusIcon.className = 'status-icon incorrect';
        this.statusTitle.textContent = `"${word}" is Incorrect ❌`;
        this.statusTitle.className = 'status-title incorrect';
        this.statusMessage.textContent = message;
    }

    showVerifiedStatus(word, message) {
        this.statusCard.style.display = 'flex';
        this.statusIcon.className = 'status-icon verified';
        this.statusTitle.textContent = `"${word}" Verified via API ✅`;
        this.statusTitle.className = 'status-title verified';
        this.statusMessage.textContent = message + ' - This word is spelled correctly!';
    }

    showSuggestions(suggestions) {
        this.suggestionsCard.style.display = 'block';
        this.suggestionsList.innerHTML = '';

        suggestions.forEach((suggestion, index) => {
            const item = document.createElement('div');
            item.className = 'suggestion-item';
            item.style.animationDelay = `${index * 0.1}s`;
            
            item.innerHTML = `
                <span class="suggestion-word">${suggestion.word}</span>
                <span class="suggestion-score">Score: ${suggestion.score}%</span>
            `;

            item.addEventListener('click', () => {
                this.wordInput.value = suggestion.word;
                this.checkSpelling();
            });

            this.suggestionsList.appendChild(item);
        });
    }

    showDefinition(apiData) {
        this.definitionCard.style.display = 'block';

        const word = apiData.meta?.id || apiData.hwi?.hw || 'Unknown';
        const pos = apiData.fl || 'N/A';
        const definition = apiData.shortdef?.[0] || 'No definition available';

        document.getElementById('defWord').textContent = word;
        document.getElementById('defPos').textContent = pos;
        document.getElementById('defMeaning').textContent = definition;
    }

    toggleDefinition() {
        this.definitionHeader.classList.toggle('expanded');
        this.definitionContent.classList.toggle('expanded');
    }

    hideAllResults() {
        this.statusCard.style.display = 'none';
        this.loadingSpinner.style.display = 'none';
        this.suggestionsCard.style.display = 'none';
        this.definitionCard.style.display = 'none';
    }

    showError(message) {
        alert(message);
    }

    playSuccessSound() {
        try {
            this.successSound.currentTime = 0;
            this.successSound.play().catch(e => console.log('Audio play failed:', e));
        } catch (e) {
            console.log('Audio not supported');
        }
    }
}

// ===== Initialize App =====
document.addEventListener('DOMContentLoaded', () => {
    const app = new SpellCheckerApp();
    console.log('🚀 Spell Checker App Initialized');
    console.log('📚 Local Dictionary Size:', app.localDictionary.size);
    console.log('🔑 API Key:', app.apiKey ? 'Configured' : 'Not configured');
});