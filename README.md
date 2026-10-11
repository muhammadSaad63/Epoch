```text
  ███████╗██████╗  ██████╗  ██████╗██╗  ██╗
  ██╔════╝██╔══██╗██╔═══██╗██╔════╝██║  ██║
  █████╗  ██████╔╝██║   ██║██║     ███████║
  ██╔══╝  ██╔═══╝ ██║   ██║██║     ██╔══██║  
  ███████╗██║     ╚██████╔╝╚██████╗██║  ██║
  ╚══════╝╚═╝      ╚═════╝  ╚═════╝╚═╝  ╚═╝
```

**Epoch** is a lightweight, high-performance, Git-inspired version control system built completely from scratch in **C++17**. Because `git init` was too easy. 😉

## 🚀 Features & Architecture

* **Content-Addressable Storage**: Objects are securely stored and sharded based on their cryptographic hashes (`2-character directory prefixing` to optimize filesystem performance).
* **Dynamic Hashing**: Supports runtime toggling and validation between **SHA-1** and **SHA-256** hashing algorithms.
* **Efficient Compression**: Integrates the **`miniz`** library to deflate and compress object snapshots and commit logs while keeping the `index` and `HEAD` pointers transparently in plaintext.
* **JSON-Backed Indexing**: Powered by **`nlohmann/json`** to map file paths to hashes with $O(1)$ lookup performance.
* **Line-by-Line Diff Engine**: Leverages the Diff Template Library (**`dtl`**) alongside **`rang`** for colorized, terminal-native patch comparisons between working trees and historical commits.

## 🛠️ Tech Stack & Dependencies

Epoch relies on modern C++ features and clean, header-only external libraries housed in `src/libs/`:

| Library / Feature | Purpose |
| :--- | :--- |
| **C++17 `std::filesystem`** | Cross-platform file, directory navigation, and path management |
| **`CLI11`** | Robust, sleek command-line argument parsing |
| **`nlohmann/json`** | Fast and flexible JSON parsing for indices and commit metadata |
| **`miniz`** | Deflate/inflate compression for the object database |
| **`dtl`** | Diff Template Library for computing line-by-line patch changes |
| **`stbrumme/hash-library`** | Cryptographic hashing (`SHA-1` and `SHA-256`) |
| **`rang`** | Terminal color formatting for clean CLI output |

## 📋 Implementation Status

* [x] `init` — Initialize repository structure (`.epoch/`, `HEAD`, `index`, `objects`)
* [x] `hash-object` — Compute hashes, compress, and store blobs
* [x] `cat-file` — Retrieve, decompress, and inspect object contents
* [x] `add` — Stage files into the JSON index map
* [x] `commit` — Package staged snapshots into compressed commit objects and update `HEAD`
* [x] `log` — Traverse and print chronological ancestry graph backwards from `HEAD`
* [x] `diff` — Compare historical commit snapshots against the working directory with colorized diffs
* [ ] `status` — Inspect and report working tree vs. staging index status *(In Progress)*
* [ ] `branch` — Create, list, or delete lightweight reference pointers
* [ ] `checkout` — Switch active `HEAD` or restore working files
* [ ] `merge` — Combine branch histories

## 📚 Learning Resources & Inspirations

Building a custom VCS from scratch is an incredible systems-programming rite of passage. These resources guided the architecture:

* [Writing a Git Clone in Python (ugit)](https://www.leshenko.net/p/ugit/#)
* [Write Yourself a Git (wyag)](https://wyag.thb.lt/)
* [YouTube Video Guide](https://www.youtube.com/watch?v=SEc5PFVSfms)

## 💻 Usage Example

```cpp
Epoch epoch;
epoch.init();
epoch.hashObject("main.cpp", true);
epoch.commit("Initial commit of Epoch core engine!");
epoch.log();
```

*Built with passion, C++17, and a deep appreciation for low-level systems engineering.*