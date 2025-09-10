document.addEventListener('DOMContentLoaded', () => {

    // NEW: A simple state object to share data between pages
    const appState = {
        currentKey: null,
    };

    // --- GENERIC HELPER FUNCTIONS ---
    async function invokeAPI(operation, payload = {}) {
        try {
            const request = { operation, payload };
            const response = await window.invoke(request);
            return response;
        } catch (error) {
            console.error(`Failed during invoke operation '${operation}':`, error);
            return { status: 'error', error: `Frontend communication error: ${error.message}` };
        }
    }

    // Note: copyToClipboard is no longer used by the Key Gen page,
    // but we'll leave it for the other pages.
    function copyToClipboard(element, button) {
        if (!element || !element.value) return;
        element.select();
        element.setSelectionRange(0, 99999);
        try {
            if (document.execCommand('copy')) {
                button.textContent = 'Copied!';
            } else {
                button.textContent = 'Copy Failed!';
            }
        } catch (err) {
            console.error('Copy command failed:', err);
            button.textContent = 'Copy Failed!';
        }
        const originalText = button.textContent;
        setTimeout(() => {
            button.textContent = "Copy"; // Reset to a generic "Copy"
        }, 2000);
    }

    // --- PAGE INITIALIZATION ---
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

        showPage('page-home');
    }

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

    // =========================================================================
    // UPDATED KEY GENERATION LOGIC
    // =========================================================================
    function setupKeyGenPage() {
        const generateBtn = document.getElementById('generate-key-btn');
        const keyOutput = document.getElementById('key-output');
        const copyBtn = document.getElementById('copy-key-btn');
        // Get references to the other pages' input fields
        const hmacKeyInput = document.getElementById('hmac-key');
        const aesKeyInput = document.getElementById('aes-key');

        generateBtn.addEventListener('click', async () => {
            const originalText = generateBtn.textContent;
            keyOutput.value = 'Generating...';
            generateBtn.disabled = true;

            const response = await invokeAPI('generateKey');
            
            if (response.status === 'success') {
                const newKey = response.key;
                keyOutput.value = newKey;
                
                // 1. Update the shared state
                appState.currentKey = newKey;

                // 2. Automatically populate the key in other sections
                if (hmacKeyInput) hmacKeyInput.value = newKey;
                if (aesKeyInput) aesKeyInput.value = newKey;

                // 3. Provide user feedback
                generateBtn.textContent = 'Key Populated!';

            } else {
                keyOutput.value = `Error: ${response.error}`;
                generateBtn.textContent = 'Generation Failed!';
            }
            
            setTimeout(() => {
                generateBtn.textContent = originalText;
                generateBtn.disabled = false;
            }, 2000);
        });

        // The copy button on this page can still be used as a backup
        copyBtn.addEventListener('click', () => copyToClipboard(keyOutput, copyBtn));
    }
    // =========================================================================
    function setupAesPage() {
        const keyInput = document.getElementById('aes-key');
        const dataInput = document.getElementById('aes-input');
        const encryptBtn = document.getElementById('aes-encrypt-btn');
        const decryptBtn = document.getElementById('aes-decrypt-btn');
        const outputArea = document.getElementById('aes-output');
        const copyBtn = document.getElementById('copy-aes-output-btn');
        // The save button is in the HTML, but requires native file dialogs which is
        // beyond the scope of this simple webview setup. We will not wire it up.
        // const saveBtn = document.getElementById('save-aes-output-btn');

        encryptBtn.addEventListener('click', async () => {
            const key = keyInput.value;
            const textInput = dataInput.value;
            if (!key || !textInput) {
                outputArea.value = 'Error: Key and Input Data are required.';
                return;
            }
            outputArea.value = "Encrypting...";
            const response = await invokeAPI('aesEncrypt', { key, textInput });

            if (response.status === 'success') {
                // Format the output as a pretty-printed JSON object.
                // This is both human-readable and machine-parseable for decryption.
                const result = {
                    iv: response.iv,
                    ciphertext: response.ciphertext
                };
                outputArea.value = JSON.stringify(result, null, 2);
            } else {
                outputArea.value = `Error: ${response.error}`;
            }
        });

        decryptBtn.addEventListener('click', async () => {
            const key = keyInput.value;
            const textInput = dataInput.value;
            if (!key || !textInput) {
                outputArea.value = 'Error: Key and Input Data are required.';
                return;
            }

            outputArea.value = "Decrypting...";
            try {
                // We expect the input data to be the JSON object we created during encryption
                const inputData = JSON.parse(textInput);
                if (!inputData.iv || !inputData.ciphertext) {
                    throw new Error("Input for decryption must be a JSON object with 'iv' and 'ciphertext' keys.");
                }

                const response = await invokeAPI('aesDecrypt', {
                    key,
                    iv: inputData.iv,
                    ciphertext: inputData.ciphertext
                });

                if (response.status === 'success') {
                    outputArea.value = response.plaintext;
                } else {
                    outputArea.value = `Error: ${response.error}`;
                }
            } catch (e) {
                outputArea.value = `Error: Invalid input format for decryption. ${e.message}`;
            }
        });
        
        copyBtn.addEventListener('click', () => copyToClipboard(outputArea, copyBtn));
    }

    function setupSha256Page() {
        const generateBtn = document.getElementById('sha-generate-btn');
        const textInput = document.getElementById('sha-input');
        const resultOutput = document.getElementById('sha-output');
        const copyBtn = document.getElementById('copy-sha-output-btn');

        generateBtn.addEventListener('click', async () => {
            if (!textInput.value) {
                resultOutput.value = 'Please enter some text to hash.';
                return;
            }
            resultOutput.value = 'Generating...';
            const response = await invokeAPI('sha256', { textInput: textInput.value });
            if (response.status === 'success') {
                resultOutput.value = response.digest;
            } else {
                resultOutput.value = `Error: ${response.error}`;
            }
        });
        
        copyBtn.addEventListener('click', () => copyToClipboard(resultOutput, copyBtn));
    }

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

    // --- APP STARTUP ---
    setupNavigation();
    setupHomePage();
    setupKeyGenPage();
    setupSha256Page();
    setupHmacPage();
    setupAesPage();

});

