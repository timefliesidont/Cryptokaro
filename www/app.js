document.addEventListener('DOMContentLoaded', () => {

    // --- GENERIC HELPER FUNCTIONS ---

    /**
     * A centralized function to communicate with the C++ backend.
     * @param {string} operation - The command to execute (e.g., 'sha256').
     * @param {object} payload - The data to send with the command.
     * @returns {Promise<object>} - The parsed JSON response from the backend.
     */
async function invokeAPI(operation, payload = {}) {
        try {
            const request = { operation, payload };
            // The webview library handles JSON conversion automatically.
            const response = await window.invoke(request);
            return response;
        } catch (error) {
            console.error(`Failed during invoke operation '${operation}':`, error);
            return { status: 'error', error: `Frontend communication error: ${error.message}` };
        }
    }

    /**
     * Copies the content of a given textarea or input element to the clipboard.
     * @param {string} elementId - The ID of the element to copy from.
     * @param {HTMLElement} button - The button element that was clicked.
     */
    function copyToClipboard(elementId, button) {
        const element = document.getElementById(elementId);
        if (element && element.value) {
            element.select();
            // This method is more reliable in the webview environment
            document.execCommand('copy');
            const originalText = button.textContent;
            button.textContent = 'Copied!';
            setTimeout(() => {
                button.textContent = originalText;
            }, 1500);
        }
    }


    // --- PAGE-SPECIFIC SETUP FUNCTIONS ---

    /**
     * Sets up navigation logic to show/hide pages.
     */
    function setupNavigation() {
        const navItems = document.querySelectorAll('.nav-item');
        const pages = document.querySelectorAll('.page');

        function showPage(pageId) {
            pages.forEach(page => page.classList.add('hidden'));
            const targetPage = document.getElementById(pageId);
            if (targetPage) {
                targetPage.classList.remove('hidden');
            }
            navItems.forEach(item => {
                item.classList.toggle('active', item.dataset.page === pageId);
            });
        }

        navItems.forEach(item => {
            item.addEventListener('click', () => showPage(item.dataset.page));
        });

        // Show the initial page
        showPage('page-home');
    }

    /**
     * Sets up the Home page, including the integration test.
     */
    function setupHomePage() {
        const runButton = document.getElementById('test-button');
        const resultDisplay = document.getElementById('result-display');

        runButton.addEventListener('click', async () => {
            resultDisplay.innerHTML = 'Calling C++...';
            const response = await invokeAPI('integrationTest');
            if (response.status === 'success') {
                resultDisplay.innerHTML = response.html;
            } else {
                resultDisplay.innerHTML = `<strong>❌ Test Failed!</strong><br><br>${response.error}`;
            }
        });
    }

    /**
     * Sets up the Symmetric Key Generation page.
     */
    function setupKeyGenPage() {
        const generateBtn = document.getElementById('generate-key-btn');
        const copyBtn = document.getElementById('copy-key-btn');
        const keyOutput = document.getElementById('key-output');

        generateBtn.addEventListener('click', async () => {
            keyOutput.value = 'Generating secure key...';
            const response = await invokeAPI('generateKey');
            if (response.status === 'success') {
                keyOutput.value = response.key;
            } else {
                keyOutput.value = `Error: ${response.error}`;
            }
        });

        copyBtn.addEventListener('click', () => {
            if (keyOutput.value && !keyOutput.value.startsWith('Error')) {
                copyToClipboard(keyOutput.value);
                // Optional: Provide user feedback
                const originalText = copyBtn.textContent;
                copyBtn.textContent = 'Copied!';
                setTimeout(() => { copyBtn.textContent = originalText; }, 1500);
            }
        });
    }
    
    /**
     * Sets up the AES Encrypt/Decrypt page.
     */
    function setupAesPage() {
        const encryptBtn = document.getElementById('aes-encrypt-btn');
        const decryptBtn = document.getElementById('aes-decrypt-btn');
        const aesOutput = document.getElementById('aes-output');
        const copyBtn = document.getElementById('copy-aes-output-btn');
        const saveBtn = document.getElementById('save-aes-output-btn');

        encryptBtn.addEventListener('click', () => {
            aesOutput.value = "Encryption is not yet implemented.";
        });

        decryptBtn.addEventListener('click', () => {
            aesOutput.value = "Decryption is not yet implemented.";
        });

        copyBtn.addEventListener('click', () => copyToClipboard('aes-output', copyBtn));

        saveBtn.addEventListener('click', () => {
            alert("Save-to-file is not yet implemented.");
        });
    }

    /**
     * Sets up the SHA-256 Digest page.
     */
    function setupSha256Page() {
        const generateBtn = document.getElementById('sha-generate-btn');
        const shaInput = document.getElementById('sha-input');
        const shaOutput = document.getElementById('sha-output');
        const copyBtn = document.getElementById('copy-sha-output-btn');

        generateBtn.addEventListener('click', async () => {
            const textToHash = shaInput.value;
            if (!textToHash) {
                shaOutput.value = 'Please enter some text to hash.';
                return;
            }
            shaOutput.value = 'Generating digest...';
            const response = await invokeAPI('sha256', { textInput: textToHash });

            if (response.status === 'success') {
                shaOutput.value = response.digest;
            } else {
                shaOutput.value = `Error: ${response.error}`;
            }
        });

        copyBtn.addEventListener('click', () => copyToClipboard('sha-output', copyBtn));
    }

    /**
     * Sets up the HMAC-SHA256 page.
     */
    function setupHmacPage() {
        const generateBtn = document.getElementById('hmac-generate-btn');
        const keyInput = document.getElementById('hmac-key');
        const textInput = document.getElementById('hmac-input');
        const resultOutput = document.getElementById('hmac-output');
        const copyBtn = document.getElementById('copy-hmac-output-btn');

        generateBtn.addEventListener('click', async () => {
            if (!keyInput.value || !textInput.value) {
                resultOutput.value = 'Please provide both a key and input data.';
                return;
            }
            resultOutput.value = 'Generating HMAC...';
            const response = await invokeAPI('hmac', { 
                key: keyInput.value,
                textInput: textInput.value 
            });

            if (response.status === 'success') {
                resultOutput.value = response.digest;
            } else {
                resultOutput.value = `Error: ${response.error}`;
            }
        });

        copyBtn.addEventListener('click', () => copyToClipboard(resultOutput, copyBtn));
    }


    // --- INITIALIZATION ---
    // Run all setup functions to wire up the application.
    setupNavigation();
    setupHomePage();
    setupKeyGenPage();
    setupAesPage();
    setupSha256Page();
    setupHmacPage();
});

