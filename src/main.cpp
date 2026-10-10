// STL includes
#include <chrono>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>

// External library includes (from src/libs/*)
#include "libs/dtl/dtl.hpp"                             // Diff Engine: dtl
// #include "libs/miniz/miniz.h"                           // Compression: miniz
#include "libs/rang/rang.hpp"                           // CLI Formatting / Terminal Colors: rang
#include "libs/cli11/CLI11.hpp"                         // Argument Parsing: CLI11
#include "libs/nlohmann/json.hpp"                       // Indexing / JSON: nlohmann/json
#include "libs/hash-library/sha1.h"                     // Hashing: stbrumme/hash-library

// Alias for convenience with std::filesystem
using std::cin, std::cout;
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
            
            return "";
        }

    public:
        Epoch() 
            : initialized(false)
            {}

        ~Epoch() 
            {}


        void init(){
            repoPath    = fs::current_path();
            epochPath   = repoPath  / ".epoch";             // .epoch
            headPath    = epochPath / "HEAD";               // .epoch/HEAD
            indexPath   = epochPath / "index";              // .epoch/index
            objectsPath = epochPath / "objects";            // .epoch/objects

            if (fs::exists(repoPath) && fs::exists(epochPath) && fs::exists(headPath) && fs::exists(indexPath) && fs::exists(objectsPath)){
                cout << "[!] Epoch is already initialized inside the current directory.\n";
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
                
                if (indexFile) indexFile.close();
            }

            // .epoch/objects dir
            if (fs::exists(indexPath) && !fs::exists(objectsPath)){
                fs::create_directory(objectsPath);
            }

            initialized = true;
        }

        string hashObject_fromData(const string& data){
            SHA1 sha1;

            return sha1(data);
        }

        // takes input a file path, computes its hash, stores the file's contents into your database, and prints the generated object hash.
        void hashObject_fromPath(const fs::path& filePath){
            if (!fs::exists(filePath)){
                cout << "[!] FilePath doesn't exist.\n";
                return;
            }
            
            // computing hash
            string fileData = getFileData(filePath);
            string fileHash = hashObject_fromData(fileData);

            // storing file's contents in .epoch/objects/
            fs::path objectPath = (objectsPath / fileHash);
            if (!fs::exists(objectPath)){
                std::ofstream outFile(objectPath, std::ios::binary);
                outFile << fileData;
                outFile.close();
            }

            cout << "Hash (sha1): " << fileHash << '\n';
        }


        // takes input an object hash, looks up the stored object in your database, retrieves it, and then prints its contents to the terminal.
        void catFile(const string& objectHash){
            string objectPath = (objectsPath / objectHash);

            if (fs::exists(objectPath)){
                cout << getFileData(objectPath);
            }
            else{
                cout << "[!] Hash doesnt exist.\n";
                return;
            }
        }

        void add(){
            
        }

        void commit(){
            
        }

        void log(){
            
        }

        void status(){
            
        }

        void diff(){
            
        }

        void branch(){
            
        }

        void merge(){
            
        }

        void checkout(){
            
        }
};



int main(int argc, char* argv[]){
    // CLI::App app{"Epoch - A simple git clone"};             // create the command parser

    // CLI11_PARSE(app, argc, argv);                           // running the parser

    // cout << fs::current_path();

    Epoch epoch;
    epoch.init();
    string str = "hakuna matata";
    // cout << epoch.hashObject_fromData(str);
    // epoch.hashObject_fromPath(fs::current_path());
    epoch.catFile("da39a3ee5e6b4b0d3255bfef95601890afd80709");
}