// STL includes
#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

// External library includes (from src/libs/*)
#include "libs/dtl/dtl.hpp"                             // Diff Engine: dtl
#include "libs/miniz/miniz.h"                           // Compression: miniz
#include "libs/rang/rang.hpp"                           // CLI Formatting / Terminal Colors: rang
#include "libs/cli11/CLI11.hpp"                         // Argument Parsing: CLI11
#include "libs/nlohmann/json.hpp"                       // Indexing / JSON: nlohmann/json
#include "libs/hash-library/sha1.h"                     // Hashing: stbrumme/hash-library
#include "libs/hash-library/sha256.h"                     // Hashing: stbrumme/hash-library

// Alias for convenience with std::filesystem
using std::cin, std::cout;
using Json = nlohmann::json;
using std::string, std::vector;
namespace fs = std::filesystem;



class Epoch{
    private:
        fs::path repoPath;
        fs::path epochPath;
        fs::path headPath;
        fs::path indexPath;
        fs::path objectsPath;
        
        bool     initialized;
        bool     useSha256;

        // checks if epoch is initialized inside the current directory
        bool checkIfInitialized(){
            if (!initialized){
                cout << "[!] Epoch is not initialized inside the current directory.\n";
                return false;
            }

            return true;
        }

        // returns the string contents of a file located at <filePath>
        string getFileData(const fs::path& filePath){
            std::ifstream file(filePath, std::ios::in | std::ios::binary);
            
            if (file){
                std::stringstream fileData;
                fileData << file.rdbuf();
                
                return fileData.str();    
            }
            
            return ":(";
        }

        // takes input a string data, computes its hash, and then returns it.
        string generateHash(const string& data){
            if (!useSha256){
                SHA1 sha1;
                return sha1(data);                      // returns 40-char SHA-1 hash
            } 
            else{
                SHA256 sha256;
                return sha256(data);                    // returns 64-char SHA-256 hash
            }
        }

        // returns the first 2 chars of the hash string
        string getDirName(const string& hash){
            return hash.substr(0, 2);                       // from index 0 to 2 (exclusive) ie the first 2 chars of the hash string
        }

        // returns the last 38 chars of the hash string
        string getFileName(const string& hash){
            return hash.substr(2);                          // from index 2 to the very end of the hash string
        }

        bool updateIndexFile(const fs::path& filePath, const string& fileHash){
            Json jsonData;

            // accessing & parsing json data from indexFile
            const string& indexData = getFileData(indexPath);
            jsonData = Json::parse(indexData);

            // adding key-value pair
            jsonData[filePath.string()] = fileHash;

            // opening indexFile (in overwrite mode)
            std::ofstream indexFile(indexPath, std::ios::trunc);
            if (!indexFile){
                cout << "[!] Failed to open index file for writing.\n";
                return false;
            }

            // overwriting indexFile with updated jsonData
            indexFile << jsonData.dump(4);
            indexFile.close();

            return true;
        }

        // uses miniz library to compress/deflate uncompressed data
        string compressData(const string& data){
            if (data.empty()) 
                return "";
            
            uLongf destLen = compressBound(data.size());
            string dest(destLen, '\0');
            
            if (compress((Bytef*)dest.data(), &destLen, (const Bytef*)data.data(), data.size()) == Z_OK) {
                dest.resize(destLen);
                return dest;
            }

            return "";
        }

        // uses miniz library to uncompress/inflate compressed data
        string decompressData(const string& data){
            if (data.empty()) 
                return "";
            
            // Estimate uncompressed size (or grow buffer if needed)
            uLongf destLen = data.size() * 4; 
            string dest(destLen, '\0');
            
            int res = uncompress((Bytef*)dest.data(), &destLen, (const Bytef*)data.data(), data.size());
            while (res == Z_BUF_ERROR){
                destLen *= 2;
                dest.resize(destLen);
                res = uncompress((Bytef*)dest.data(), &destLen, (const Bytef*)data.data(), data.size());
            }
            
            if (res == Z_OK){
                dest.resize(destLen);
                return dest;
            }

            return "";
        }

        // updating the HEAD pointer with newCommitHash
        bool updateHEAD(const string& newCommitHash){
            std::ofstream headFile(headPath);
            
            if (!headFile){
                cout << "[!] Failed to update HEAD pointer.\n";
                return false;
            }

            headFile << newCommitHash;
            headFile.close();

            return true;
        }

        // returns the JSON object for the commit
        Json getJSONCommitObject(const string& parentHash, const string& commitMessage, const string& indexData, const string& timeStamp){
            Json commitObject;

            commitObject["snapShot"]      = Json::parse(indexData);
            commitObject["timeStamp"]     = timeStamp;
            commitObject["parentHash"]    = parentHash;
            commitObject["commitMessage"] = commitMessage;

            return commitObject;
        }

        // returns the current timeStamp in human-readable format
        string getCurrentTimeStamp(){
            auto now         = std::chrono::system_clock::now();
            auto now_time_t  = std::chrono::system_clock::to_time_t(now);
            string timeStamp = std::ctime(&now_time_t);
            
            if (!timeStamp.empty() && (timeStamp.back() == '\n'))
                timeStamp.pop_back();

            return timeStamp;
        }

        // add a new object entry to the objects directory
        bool storeObject(const string& fileHash, const string& fileData){
            // creating .epoch/objects/<dirName> dir if it no exists            
            fs::path objectPath = (objectsPath / getDirName(fileHash));
            if (!fs::exists(objectPath))
                fs::create_directory(objectPath);

            // creating .epoch/objects/<dirName>/<fileName> if it no exists
            objectPath /= getFileName(fileHash);
            std::ofstream outFile(objectPath, std::ios::binary);

            // storing file's contents in .epoch/objects/<dirName>/<fileName>
            {
                if (!outFile){
                    outFile << "[!] Failed to write commit object to database.\n";
                    return false;
                } 
                
                outFile << fileData;
                outFile.close();

                return true;
            }
        }

    public:
        Epoch() 
            : initialized(false)
            , useSha256(false)
            {}

        ~Epoch() 
            {}

        // initializes a new, empty epoch version control repository by creating the necessary .epoch directory structure, HEAD, index, and objects folders.
        void init(){
            // setting path attributes
            repoPath    = fs::current_path();
            epochPath   = repoPath  / ".epoch";             // .epoch
            headPath    = epochPath / "HEAD";               // .epoch/HEAD
            indexPath   = epochPath / "index";              // .epoch/index
            objectsPath = epochPath / "objects";            // .epoch/objects

            // checking if epoch was already initialized previously
            if (fs::exists(repoPath) && fs::exists(epochPath) && fs::exists(headPath) && fs::exists(indexPath) && fs::exists(objectsPath)){
                cout << "[!] Epoch is already initialized inside the current directory.\n";
                initialized = true;
                
                return;
            }

            // .epoch dir
            if (fs::exists(repoPath) && !fs::exists(epochPath)){
                fs::create_directory(epochPath);
            }

            // .epoch/HEAD file
            if (fs::exists(epochPath) && !fs::exists(headPath)){
                std::ofstream headFile(headPath);
                
                if (headFile) headFile.close();
            }

            // .epoch/index file
            if (fs::exists(headPath) && !fs::exists(indexPath)){
                std::ofstream indexFile(indexPath);
                
                if (indexFile){
                    indexFile << "{}";
                    indexFile.close();
                }
            }

            // .epoch/objects dir
            if (fs::exists(indexPath) && !fs::exists(objectsPath)){
                fs::create_directory(objectsPath);
            }

            initialized = true;
        }

        // takes input a file path, computes its hash, stores the file's contents into your database, and prints the generated object hash.
        string hashObject(const fs::path& filePath, const bool printHash){
            if (!checkIfInitialized()) return ":(";

            if (!fs::exists(filePath)){
                cout << "[!] File doesn't exist.\n";
                return ":(";
            }
            
            // computing hash & data
            string fileData           = getFileData(filePath);
            string fileHash           = generateHash(fileData);
            string compressedFileData = compressData(fileData);

            // storing data at .epoch/objects/<dirName>/<fileName>       
            if (!storeObject(fileHash, compressedFileData)) return ":(";


            if (printHash)
                cout << "Hash (sha1): " << fileHash << '\n';

            return fileHash;
        }

        // takes input an object hash, looks up the stored object in your database, retrieves it, and then prints its contents to the terminal.
        void catFile(const string& objectHash){
            if (!checkIfInitialized()) return;

            // validating objectHash's length
            if (objectHash.length() != 40) {
                cout << "[!] Invalid hash length.\n";
                return;
            }

            fs::path objectPath = (objectsPath / getDirName(objectHash) / getFileName(objectHash));

            // object with hash objectHash doesn't exist
            if (!fs::exists(objectPath)){
                cout << "[!] Hash doesn't exist.\n";
                return;
            }

            // object with hash objectHash exists
            string objectData             = getFileData(objectPath);
            string decompressedObjectData = decompressData(objectData);

            // displaying contents at objectHash
            cout << decompressedObjectData << '\n';
        }
        
        // takes input a file path, stages the file by hashing its content and recording an entry into the index file.
        void add(const fs::path& filePath){
            if (!checkIfInitialized()) return;

            if (!fs::exists(filePath)){
                cout << "[!] File doesn't exist.\n";
                return;
            }

            // calculates the hash and add its corresponding object to .epoch/objects/ 
            const string& fileHash = hashObject(filePath, false);
            if (fileHash == ":("){
                cout << "[!] Error hashing file.\n";
                return;
            }

            // updates the index file (staging area)
            bool indexed = updateIndexFile(filePath, fileHash);
            if (!indexed)
                cout << "[!] Error indexing file.\n";
        }

        // packages the staged entries from the index into a commit object and updates the HEAD pointer.
        void commit(const string& commitMessage){
            if (!checkIfInitialized()) return;

            // reading the current index file
            string indexData = getFileData(indexPath);
            if (indexData.empty() || indexData == "{}"){
                cout << "[!] Nothing to commit, working tree clean.\n";
                return;
            }

            // getting hash at HEAD
            string parentHash = getFileData(headPath);

            // getting current timeStamp
            string timeStamp = getCurrentTimeStamp();

            // making the commit json object
            Json commitObject = getJSONCommitObject(parentHash, commitMessage, indexData, timeStamp);

            // hashing and compressing commitString
            string commitString     = commitObject.dump();
            string compressedCommit = compressData(commitString);
            string commitHash       = generateHash(commitString);

            // saving the compressedCommit string into .epoch/objects/commitHash
            if (!storeObject(commitHash, compressedCommit)) return;

            // updating HEAD to now point to the (new) commitHash
            if (!updateHEAD(commitHash)) return;

            // printing details if successful
            cout << "[*] Committed successfully!"         << '\n';
            cout << "    Commit Hash : " << commitHash    << '\n';
            cout << "    Message     : " << commitMessage << '\n';
        }

        // inspects and displays differences between the working directory, the staging index, and the current HEAD commit.
        void status(){
            if (!checkIfInitialized()) return;

            //
        }

        // compares line-by-line text changes between files in the working tree, index, or specific historical commits.
        void diff(){
            if (!checkIfInitialized()) return;

            //
        }

        // traverses and prints the chronological commit history backwards starting from the current HEAD pointer.
        void log(){
            if (!checkIfInitialized()) return;

            //
        }

        // creates, lists, or deletes lightweight reference pointers to specific commit hashes.
        void branch(){
            if (!checkIfInitialized()) return;

            //
        }

        // switches the active HEAD pointer to a different branch or restores working directory files to a target commit.
        void checkout(){
            if (!checkIfInitialized()) return;

            //
        }

        // combines the history and changes of a specified branch into your current active branch.
        void merge(){
            if (!checkIfInitialized()) return;

            //
        }
};



int main(int argc, char* argv[]){
    // CLI::App app{"Epoch - A simple git clone"};             // create the command parser

    // CLI11_PARSE(app, argc, argv);                           // running the parser

    // cout << fs::current_path();

    Epoch epoch;
    epoch.init();
    string str = "hakuna matata";
    // cout << epoch.generateHash(str);
    epoch.hashObject("meow.txt", true);
    // epoch.catFile("da39a3ee5e6b4b0d3255bfef95601890afd80709");
}