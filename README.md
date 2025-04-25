# Excel-like Spreadsheet Engine (BI-PA2 Semester Project)

> 🧮 A C++ implementation of a simplified Excel-like spreadsheet engine with support for expressions, memory-efficient abstract syntax trees, and hexadecimal file serialization.

---

## Project Overview

This repository contains the solution to a **semester assignment** for the course **BI-PA2**. The task simulates the core logic of spreadsheet programs like Excel, focusing on how cell data and expressions are represented and evaluated in memory.

Each cell in the table — identified by a name such as `A1`, `B5`, or `Z999` — can contain:
- A **numeric value**
- A **string**
- Or an **expression**, starting with the `=` symbol (e.g., `=4 + A2 + B$7`)

Expressions are parsed and stored using a **polymorphic Abstract Syntax Tree (AST)** for efficient evaluation.

---

## Key Features

- **Expression Parsing**  
  Expressions are parsed from infix notation (e.g., `4 + A2 + B$7`) to an implicit postfix representation using a custom parser.  
  For example, the parser interprets: `4 A2 + B$7 +`
by calling a sequence of method implementations:
```cpp
valNumber(4)
valReference("A2")
opAdd()
valReference("B$7")
opAdd()
```

These method calls dynamically build the AST structure.

- Polymorphic AST
  - Each node in the AST represents a literal or an operation. The AST is composed of polymorphic classes representing both values and operators, which enable dynamic evaluation of complex expressions.

- Cell Storage & File I/O
  - The entire spreadsheet can be serialized to a file in hexadecimal format.
  - Files can be loaded back into memory for continued usage.
- Cycle Detection
  - The program passes both standard tests and an optional cycle detection test using a modified DFS algorithm to prevent circular dependencies between cells.

## Repository Structure ##
```
Velka_uloha/
├── build/                     # CMake build folder
├── headers/                   # Header files defining core classes
│   ├── CExpr                  # AST nodes representing literal values
│   │   ├── CExpr_abstract.hpp
│   │   ├── Literals.hpp       # AST nodes representing literal values
│   │   └── Operators.hpp      # AST nodes representing operators
│   ├── CMyOwnExprBuilder.hpp  # Specified class utilized by the parser to call appropriate methods
│   ├── CNode.hpp              # Represents the value of a spreadsheet cell
│   ├── CPos.hpp               # Represents the position of a spreadsheet cell in the spreadsheet
│   ├── CSpreadsheet.hpp       # The spreadsheet itself
│   └── pair_util.hpp          # helper function for hashing
├── i686-w64-mingw32
├── implementations/           # Has all the implementations of the header files
│   ├── CExpr                  
│   │   ├── Literals.cpp       
│   │   └── Operators.cpp      
│   ├── CMyOwnExprBuilder.cpp  
│   ├── CNode.cpp              
│   ├── CPos.cpp               
│   └── CSpreadsheet.cpp             
├── CMakeLists.txt             # CMake build configuration
├── all_in_one.cpp             # Final merged submission for Progtest
├── expression.h               # Abstract class utilized by the parser to call appropriate methods
└── test.cpp                   # Entry point with basic test cases
```

## Purpose ##

This project was designed to:
- Understand the internal logic behind spreadsheet software
- Practice building abstract syntax trees
- Learn advanced C++ programming with memory and object-oriented design
- Explore custom parsing and serialization techniques
