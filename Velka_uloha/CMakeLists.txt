cmake_minimum_required(VERSION 3.5)
project(VelkaUloha VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
    add_compile_definitions(/W4 /WX)
else()
    add_compile_options(-Wall -pedantic -g)
endif()


add_executable(
    VelkaUloha 
    test.cpp
    implementations/CPos.cpp
    implementations/CNode.cpp
    implementations/CMyOwnExprBuilder.cpp
    implementations/CSpreadsheet.cpp
    implementations/CExpr/Literals.cpp
    implementations/CExpr/Operators.cpp
)


find_library(EXPRESSION_PARSER_LIB expression_parser PATHS ./i686-w64-mingw32)

target_link_libraries(VelkaUloha ${EXPRESSION_PARSER_LIB})
