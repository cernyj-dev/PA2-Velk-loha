#ifndef CSPREADSHEET_HPP
#define CSPREADSHEET_HPP

#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <string>
#include <iostream>
#include <functional>
#include <cstdlib>
#include "CPos.hpp"
#include "CNode.hpp"
#include "pair_util.hpp"
using namespace std::literals;
using CValue = std::variant<std::monostate, double, std::string>;

constexpr unsigned SPREADSHEET_CYCLIC_DEPS = 0x01;
constexpr unsigned SPREADSHEET_FUNCTIONS = 0x02;
constexpr unsigned SPREADSHEET_FILE_IO = 0x04;
constexpr unsigned SPREADSHEET_SPEED = 0x08;
constexpr unsigned SPREADSHEET_PARSER = 0x10;

struct CPosHasher
{
    long long operator()(const CPos &cpos) const;
};

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CSpreadsheet
{
public:
    static unsigned capabilities()
    {
        return SPREADSHEET_CYCLIC_DEPS | SPREADSHEET_FUNCTIONS | SPREADSHEET_FILE_IO | SPREADSHEET_SPEED; //| SPREADSHEET_PARSER;
    }
    CSpreadsheet();
    // CSpreadsheet(const CSpreadsheet &other);
    // CSpreadsheet &operator=(const CSpreadsheet &other);
    bool load(std::istream &is);
    bool save(std::ostream &os) const;
    bool setCell(CPos pos,
                 std::string contents);
    CValue getValue(CPos pos);
    void copyRect(CPos dst,
                  CPos src,
                  int w = 1,
                  int h = 1);
    void print() const;

private:
    // model: mapa <<width, height>, contents> - O(logn) access - might be too slow
    // 		  2D vector
    //        unordered_map - O(1) search and access - might be the best option, but ill need my own hash functor and an operator== for CPos
    std::unordered_map<CPos, CNode, CPosHasher> m_spreadsheet; // done

    // used for DFS
    std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> visited;
};

#endif