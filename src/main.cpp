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


    public:
        Epoch()  
            {}

        ~Epoch() 
            {}


        void init(){
            repoPath    = fs::current_path();
            epochPath   = repoPath  / ".epoch";              // .epoch
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

            // .epoch/objects file
            if (fs::exists(indexPath) && !fs::exists(objectsPath)){
                std::ofstream objectsFile(objectsPath);
                
                if (objectsFile) objectsFile.close();
            }
        }

        string getFileData(const fs::path& filePath){
            if (fs::exists(filePath)){
                std::istream file(filePath);
                    return file.
            }
        }

        string hashObject_fromData(const string& data){
            SHA1 sha1;
    
            return sha1(data);
        }

        string hashObject_fromFile(const fs::path& filePath){
            SHA1 sha1;
            string data = getFileData(filePath);
    
            return sha1(data);
        }

        void catFile(const string& hash){
            string hashPath = (objectsPath / hash);

            if (fs::exists(hashPath)){

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
    epoch.init();

}