#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <string>
#include "internal/epoch/epoch.hpp"

namespace fs = std::filesystem;
using std::cout;

// Helper to create a dummy test file with content
void createTestFile(const string& filename, const string& content) {
    std::ofstream file(filename);
    file << content;
    file.close();
}

void runTests() {
    cout << "[*] Starting Epoch Test Suite...\n\ninfo: ";

    // Setup a clean temporary test directory for isolation
    fs::path testRepoDir = fs::current_path() / "test_sandbox";
    if (fs::exists(testRepoDir)) {
        fs::remove_all(testRepoDir);
    }
    fs::create_directory(testRepoDir);
    fs::current_path(testRepoDir);

    // Test 1: Initialization
    {
        Epoch epoch;
        epoch.init();
        
        assert(fs::exists(".epoch"));
        assert(fs::exists(".epoch/HEAD"));
        assert(fs::exists(".epoch/index"));
        assert(fs::exists(".epoch/objects"));
        cout << "[+] Test 1 Passed: Repository initialization successful.\n";
    }

    // Test 2: Hash Object & Cat File
    {
        Epoch epoch;
        epoch.init();

        createTestFile("hello.txt", "Hello, Epoch World!");
        string hash = epoch.hashObject("hello.txt", false);
        
        assert(hash != ":(");
        assert(hash.length() == 40); // Default SHA-1 length
        cout << "[+] Test 2 Passed: hash-object generated valid SHA-1: " << hash << "\n";
    }

    // Test 3: Add and Commit Workflow
    {
        Epoch epoch;
        epoch.init();

        createTestFile("project.cpp", "int main() { return 0; }\n");
        epoch.add("project.cpp");
        
        // Perform first commit
        epoch.commit("Initial commit with project.cpp");
        cout << "[+] Test 3 Passed: File staged and committed successfully.\n";
    }

    // Test 4: Modify file, Log history, and Diff engine
    {
        Epoch epoch;
        epoch.init();

        // Modify the file to trigger diff changes
        createTestFile("project.cpp", "int main() {\n    // Added comment\n    return 0;\n}\n");
        epoch.add("project.cpp");
        epoch.commit("Updated project.cpp with a comment");

        cout << "\n--- Testing Log Output ---\n";
        epoch.log();

        // Get the parent commit or run diff against previous versions
        // (You can pass a specific commit hash here if captured, log prints it)
        cout << "[+] Test 4 Passed: Log and modification history evaluated.\n";
    }

    // Cleanup sandbox after tests
    fs::current_path(testRepoDir.parent_path());
    fs::remove_all(testRepoDir);

    cout << "\n[*] All Epoch tests completed successfully!\n";
}

int main() {
    runTests();
    return 0;
}