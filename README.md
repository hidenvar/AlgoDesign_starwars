# Algorithm Design Final Project

This project implements a **missile routing algorithm** using graph theory and the Boost Graph Library.  
It models missile paths between base cities and target cities while considering constraints such as:

- Radar detection  
- Missile range limitations  
- Spy presence in cities  
- Defense levels of targets  
- Missile inventory and availability  

The system supports multiple **scenarios (1–7)**, each adding complexity to the routing and attack strategies.

---

## ⚙️ Installation & Setup

### 1. Prerequisites
Before building the project, make sure you have the following installed on your system:

- **C++17 or higher** → Required for modern language features.  
- **GNU Make** → For building the project with the provided `Makefile`.  
- **g++ (or clang++)** → Your C++ compiler.  

To check if you already have them installed:
```bash
g++ --version
make --version
```
On Ubuntu/Debian:
```bash
sudo apt update
sudo apt install g++ make
```
On Fedora:
```bash
sudo dnf install gcc-c++ make
```
On macOS (with Homebrew):
```bash
brew install gcc make 
```
---

### 2. Cloning the Repository
Clone the repository to your local machine:
```bash
git clone https://github.com/hidenvar/AlgoDesign_starwars.git
cd AlgoDesign_starwars
```
---
### 3. Build Instructions
The project is built using a custom Makefile located at the project root.
Available Targets

``` make ```→ Builds the entire project (creates executables in bin/)

``` make clean ```→ Removes all object files and binaries (build/ and bin/)

``` make run (1-7)```→ Builds and runs the scenario provided (7 scenarios in total)

---
### 4. Testing
Additional testcases can be provided, only thing you need to do is to create the testcase and copy it in the testcase.txt for the desired scenario.