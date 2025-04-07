#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <climits>
#include <cfloat>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <array>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
#include <stdexcept>
#include <variant>
#include <optional>
#include <compare>
#include <charconv>
#include <span>
#include <utility>
#include "expression.h"
using namespace std::literals;
using CValue = std::variant<std::monostate, double, std::string>;

constexpr unsigned SPREADSHEET_CYCLIC_DEPS = 0x01;
constexpr unsigned SPREADSHEET_FUNCTIONS = 0x02;
constexpr unsigned SPREADSHEET_FILE_IO = 0x04;
constexpr unsigned SPREADSHEET_SPEED = 0x08;
constexpr unsigned SPREADSHEET_PARSER = 0x10;
#endif /* __PROGTEST__ */

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////
// PAIR UTIL
////////////////////////////////////////////////////////////////////////////////////////////////////////

struct pairHasher
{
    long long operator()(const std::pair<long long, long long> pair) const
    {
        return (std::hash<long long>()(pair.first)) ^ (std::hash<long long>()(pair.second) << 1);
    }
};

struct pairEqual
{
    bool operator()(const std::pair<long long, long long> &lhs, const std::pair<long long, long long> &rhs) const
    {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPOS
////////////////////////////////////////////////////////////////////////////////////////////////////////

long long calculate_width(const std::string &source);

void process_cpos(const std::string_view &source, std::string &norm_str, long long &width, long long &height);

class CPos
{
public:
    // CPos(std::string_view str
    CPos(std::string_view str = "");
    CPos(const long long &width, const long long &height);
    bool operator==(const CPos &rhs) const;
    long long getWidth() const;
    long long getHeight() const;
    friend CPos operator-(const CPos &lhs, const CPos &rhs);
    friend CPos operator+(const CPos &lhs, const CPos &rhs);
    std::string getNormStr() const;
    void setNormStr(const std::string &str);
    void mNormStrRecreation();

private:
    std::string m_norm_str;
    long long m_width;
    long long m_height;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CEXPR ABSTRACT
////////////////////////////////////////////////////////////////////////////////////////////////////////

class CSpreadsheet;
class CExpr
{
public:
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const = 0;
    virtual ~CExpr() = default;
    virtual std::shared_ptr<CExpr> clone() const = 0;
    virtual void updateReferencesBy(const CPos &offset) = 0;
    virtual void exprToString(std::string &output) const = 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CNODE
////////////////////////////////////////////////////////////////////////////////////////////////////////

class CSpreadsheet;

class CNode
{
public:
    CNode();
    CNode(const std::string &str);
    CNode(const CNode &other); // deep copy may be needed?
    CValue evaluate(CSpreadsheet &spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const;
    std::shared_ptr<CExpr> getExpr() const;
    void updateReferencesBy(const CPos &offset);
    void exprToString(std::string &output) const;

private:
    // root of AST
    std::shared_ptr<CExpr> m_cexpr;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSPREADSHEET
////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> visited;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CEXPR BUILDER
////////////////////////////////////////////////////////////////////////////////////////////////////////

class CMyOwnExprBuilder : public CExprBuilder
{
public:
    CMyOwnExprBuilder();
    std::shared_ptr<CExpr> getExpr() const;

    virtual void opAdd() override;
    virtual void opSub() override;
    virtual void opMul() override;
    virtual void opDiv() override;
    virtual void opPow() override;
    virtual void opNeg() override;
    virtual void opEq() override;
    virtual void opNe() override;
    virtual void opLt() override;
    virtual void opLe() override;
    virtual void opGt() override;
    virtual void opGe() override;
    virtual void valNumber(double val) override;
    virtual void valString(std::string val) override;
    virtual void valReference(std::string val) override;
    virtual void valRange(std::string val) override;
    virtual void funcCall(std::string fnName,
                          int paramCount) override;

private:
    std::stack<std::shared_ptr<CExpr>> m_expr_stack;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// LITERALS
////////////////////////////////////////////////////////////////////////////////////////////////////////

class CNumber : public CExpr
{
public:
    CNumber(CValue value);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;

private:
    // double - but for further uses, variant is probably better
    CValue m_number;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CStr : public CExpr
{
public:
    CStr(CValue value);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;

private:
    // string - but for further uses, variant is probably better
    CValue m_string;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CReference : public CExpr
{
public:
    CReference(CValue value);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;
    std::string RefStrRecreation() const;

private:
    bool is_col_rel;
    bool is_row_rel;

    // holding the cell im pointing to - might prove useful for future copying
    long long m_width;
    long long m_height;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CEXPR OPERATORS
////////////////////////////////////////////////////////////////////////////////////////////////////////

class CAddition : public CExpr
{
public:
    CAddition(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;

private:
    std::shared_ptr<CExpr> m_left;
    std::shared_ptr<CExpr> m_right;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CSubtraction : public CExpr
{
public:
    CSubtraction(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;

private:
    std::shared_ptr<CExpr> m_left;
    std::shared_ptr<CExpr> m_right;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CMultiplication : public CExpr
{
public:
    CMultiplication(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;

private:
    std::shared_ptr<CExpr> m_left;
    std::shared_ptr<CExpr> m_right;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CDivision : public CExpr
{
public:
    CDivision(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;

private:
    std::shared_ptr<CExpr> m_left;
    std::shared_ptr<CExpr> m_right;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CPow : public CExpr
{
public:
    CPow(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;

private:
    std::shared_ptr<CExpr> m_left;
    std::shared_ptr<CExpr> m_right;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CNegation : public CExpr
{
public:
    CNegation(std::shared_ptr<CExpr> expr);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;

private:
    std::shared_ptr<CExpr> m_neg;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CEquality : public CExpr
{
public:
    CEquality(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;

private:
    std::shared_ptr<CExpr> m_left;
    std::shared_ptr<CExpr> m_right;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CInequality : public CExpr
{
public:
    CInequality(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;

private:
    std::shared_ptr<CExpr> m_left;
    std::shared_ptr<CExpr> m_right;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CLessThan : public CExpr
{
public:
    CLessThan(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;

private:
    std::shared_ptr<CExpr> m_left;
    std::shared_ptr<CExpr> m_right;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CLessEqual : public CExpr
{
public:
    CLessEqual(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;

private:
    std::shared_ptr<CExpr> m_left;
    std::shared_ptr<CExpr> m_right;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CGreaterThan : public CExpr
{
public:
    CGreaterThan(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;

private:
    std::shared_ptr<CExpr> m_left;
    std::shared_ptr<CExpr> m_right;
};

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class CGreaterEqual : public CExpr
{
public:
    CGreaterEqual(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right);
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const override;
    virtual std::shared_ptr<CExpr> clone() const override;
    virtual void updateReferencesBy(const CPos &offset) override;
    virtual void exprToString(std::string &output) const override;

private:
    std::shared_ptr<CExpr> m_left;
    std::shared_ptr<CExpr> m_right;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPOS
////////////////////////////////////////////////////////////////////////////////////////////////////////

long long calculate_width(const string &source)
{
    long long result = 0;
    for (char c : source)
    {
        result = result * 26 + c - 'A' + 1; // ASCII normalization into a number
    }
    return result - 1;
}

// thought I might use this function somewhere else than for the instances of CPos - didnt make it a method for that reason
void process_cpos(const string_view &source, string &norm_str, long long &width, long long &height)
{
    // [Idea] - potentionally save the original string and not the lower_case one

    if (source.length() == 0)
    {
        throw invalid_argument("CPos input is empty: CPos constructor");
    }
    // first : copy into a normal string? To use transform for example [low_PROBLEM] - i wont now
    // second : normalize it [low_PROBLEM]
    string tmp_cpos; // tmp - before assigning it directly to the member string, i gotta check it first and not change the member string if something goes bad
    bool letters_parsed = false;
    bool one_letter_parsed = false; // i dont really want to have a standard for cycle - check if something was parsed, if a letter wasnt parsed, it should be an error
    bool one_number_parsed = false;
    long long tmp_width = 0;
    string tmp_height; // concat to this until the source is parsed, then use stoull to create a long long from the string

    for (char c : source)
    {
        if (letters_parsed == false) // need to still parse through letters
        {
            if (islower(c))
            {
                char b = toupper(c);
                tmp_cpos += b; // normalize for future use
                one_letter_parsed = true;
            }
            else if (isupper(c))
            {

                tmp_cpos += c; // normalize for future use
                one_letter_parsed = true;
            }
            else if (isdigit(c))
            {
                tmp_width = calculate_width(tmp_cpos);
                letters_parsed = true;
                one_number_parsed = true;
                tmp_height += c;
                tmp_cpos += c;
            }
            else
            {
                throw invalid_argument("Something fishy among the letters of inputed src: CPos constructor");
            }
            if (one_letter_parsed == false)
            { // if no letter was parsed - meaning there was probably a number right off the bat - error
                throw invalid_argument("No letter was registered: CPos constructor");
            }
        }
        else // letters were parsed
        {
            if (isdigit(c))
            {
                tmp_height += c;
                tmp_cpos += c;
            }
            else // letter is not a digit even though there are supposed to be only digits now
            {
                throw invalid_argument("Something fishy among the numbers of inputed src: CPos constructor");
            }
        }
    }
    if (one_number_parsed == false)
    {
        throw invalid_argument("No number was registered: CPos constructor");
    }
    // third : assign values if everything went well
    height = stoull(tmp_height);
    width = tmp_width;
    norm_str = tmp_cpos;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CPos::CPos(string_view str) : m_norm_str(""), m_width(0), m_height(0)
{
    process_cpos(str, m_norm_str, m_width, m_height);
    // cout <<  ", m_width: " << m_width << ", m_height: " << m_height << endl; //[PRINT]
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CPos::CPos(const long long &width, const long long &height) : m_width(width), m_height(height)
{
    // cout << ", m_width: " << m_width << ", m_height: " << m_height << endl; //[PRINT]
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

long long CPos::getWidth() const
{
    return m_width;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

long long CPos::getHeight() const
{
    return m_height;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// for internal "sorting" mechanism of unordered_map: Hash
bool CPos::operator==(const CPos &rhs) const
{
    return (m_width == rhs.m_width && m_height == rhs.m_height);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CPos operator-(const CPos &lhs, const CPos &rhs)
{
    return CPos(lhs.m_width - rhs.m_width, lhs.m_height - rhs.m_height);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CPos operator+(const CPos &lhs, const CPos &rhs)
{
    return CPos(lhs.m_width + rhs.m_width, lhs.m_height + rhs.m_height);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

string CPos::getNormStr() const
{
    return m_norm_str;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CPos::setNormStr(const string &str)
{
    m_norm_str = str;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CPos::mNormStrRecreation()
{
    m_norm_str = "";
    long long tmp_col = m_width;
    while (tmp_col > 0)
    {
        m_norm_str = char('A' + tmp_col % 26) + m_norm_str;
        tmp_col--; // decrement column to make it 0-indexed
        tmp_col /= 26;
    }
    if (m_norm_str == "")
    {
        m_norm_str = "A";
    }
    // append the row number to the string
    m_norm_str += std::to_string(m_height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CNODE
////////////////////////////////////////////////////////////////////////////////////////////////////////

CNode::CNode() : m_cexpr(nullptr) {}

bool isValidDouble(const std::string &str)
{
    if (str.empty() || std::isspace(str[0]))
    {
        return false;
    }

    std::size_t pos;
    try
    {
        std::stod(str, &pos);
    }
    catch (const std::exception &)
    {
        return false;
    }
    return pos == str.size();
}

// crafting AST
CNode::CNode(const string &str) : m_cexpr(nullptr)
{
    if (!str.empty())
    {
        if (isValidDouble(str))
        {
            // cout << "Double detected: " << str << endl;
            double tmp = stod(str);
            m_cexpr = make_shared<CNumber>(CValue(tmp));
        }
        else
        {
            // is it an expression?
            if (str[0] == '=')
            {
                CMyOwnExprBuilder builder;
                // cout << "Parsing expression: " << str << endl;
                parseExpression(str, builder);
                // cout << "Expression parsed" << endl;
                m_cexpr = builder.getExpr();
            }
            else // its a string
            {
                // cout << "String detected with length: " << str.length() << ", : |" << str << "|" << endl;
                m_cexpr = make_shared<CStr>(CValue(str));
            }
        }
    }
}

CNode::CNode(const CNode &other) // deep copy may be needed?
{
    if (other.m_cexpr)
    {
        m_cexpr = other.m_cexpr->clone();
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CValue CNode::evaluate(CSpreadsheet &spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    if (m_cexpr)
    {
        // cout << "Called evaluate on CNode with expression" << endl;
        return m_cexpr->evaluate(spreadsheet, visited);
    }
    // if m_cexpr is nullptr, return m_cval
    return CValue();
}

std::shared_ptr<CExpr> CNode::getExpr() const
{
    return m_cexpr;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CNode::updateReferencesBy(const CPos &offset)
{
    if (m_cexpr)
    {
        // cout << "Updating references by offset" << endl;
        // cout << "Offset: " << offset.row << " " << offset.col << endl;
        m_cexpr->updateReferencesBy(offset);
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CNode::exprToString(std::string &output) const
{
    if (m_cexpr)
    {
        // cout << "Called exprToString on CNode with expression" << endl;
        m_cexpr->exprToString(output);
    }
    // if m_cexpr is nullptr, return m_cval
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CSPREADSHEET
////////////////////////////////////////////////////////////////////////////////////////////////////////

long long CPosHasher::operator()(const CPos &cpos) const
{
    return (std::hash<long long>()(cpos.getWidth())) ^ (std::hash<long long>()(cpos.getHeight()) << 1);
}

CSpreadsheet::CSpreadsheet() {}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CSpreadsheet::setCell(CPos pos, std::string contents)
{
    try
    {
        m_spreadsheet[pos] = CNode(contents);
    }
    catch (const std::exception &e)
    {
        return false;
    }
    return true;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CValue CSpreadsheet::getValue(CPos pos)
{
    auto iter = m_spreadsheet.find(pos);
    if (iter == m_spreadsheet.end())
    {
        return CValue();
    }
    return m_spreadsheet[pos].evaluate(*this, visited);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSpreadsheet::copyRect(CPos dst, CPos src, int w, int h)
{
    // create a copy to ensure the correct behaviour when it comes to two rectangles that could be overlapping
    CSpreadsheet tmp = *this;
    CPos offs = dst - src;
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            CPos offset{x, y};
            CPos from = src + offset;
            CPos to = dst + offset;
            auto iter = tmp.m_spreadsheet.find(from);
            if (iter != tmp.m_spreadsheet.end())
            {
                from.mNormStrRecreation();
                to.mNormStrRecreation();
                // std::cout << "Copying from: " << from.getNormStr() << " to: " << to.getNormStr() << std::endl; //[PRINT]
                // std::cout << tmp.m_spreadsheet[from].getOrigStr() << std::endl;

                m_spreadsheet[to] = tmp.m_spreadsheet[from];
                m_spreadsheet[to].updateReferencesBy(offs);
                // m_spreadsheet[to].setOrigStr("=" + to.getNormStr());
            }
            else
            {
                m_spreadsheet.erase(to);
            }
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool CSpreadsheet::save(std::ostream &os) const
{
    unsigned int checksum = 0;
    os << m_spreadsheet.size() << "\n";
    // //std::cout << "size: " << m_spreadsheet.size() << std::endl;
    for (const auto &pair : m_spreadsheet)
    {
        // Write the CPos original string in hexadecimal format
        const std::string &pos_str = pair.first.getNormStr();
        // std::cout << "pos_str: " << pos_str << std::endl;
        os << pos_str.size() << " ";
        for (char c : pos_str)
        {
            // std::cout << "c: " << c << ", ASCII: " << static_cast<int>(c) << std::endl;

            checksum += c;
            os << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(c);
        }

        os << " ";

        // Write the CNode original string in hexadecimal format
        std::string contents = "=";
        pair.second.exprToString(contents); // CNode -> AST exprToString ...
        os
            << std::dec << contents.size() << " ";
        for (char c : contents)
        {
            // std::cout << "c: " << c << ", ASCII: " << static_cast<int>(c) << std::endl;
            checksum += c;
            os << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(c);
        }
        os << "\n";
        // std::cout << "Checksum: " << checksum << std::endl;
    }
    // std::cout << "FinalChecksum: " << checksum << std::endl;
    //  Write the checksum
    os << std::dec << checksum;

    // Check if the write was successful
    if (os.fail())
    {
        return false;
    }

    return true;
}

bool CSpreadsheet::load(std::istream &is)
{
    try
    {
        m_spreadsheet.clear(); // Clear the current spreadsheet data

        unsigned int checksum = 0;
        size_t num_lines;
        is >> num_lines;
        is.ignore(); // skip newline character
        // size_t counter = 0;
        for (size_t line = 0; line < num_lines && is.good(); ++line)
        {
            size_t size_pos, size_cnode;
            is >> size_pos;
            // std::cout << "size CPos: " << size_pos << std::endl;
            std::string pos_str(size_pos, '\0');
            is.ignore();
            for (size_t i = 0; i < size_pos; ++i)
            {
                std::string hex_chars;
                hex_chars.resize(2);
                is.read(&hex_chars[0], 2);
                int value = std::stoi(hex_chars, nullptr, 16);
                pos_str[i] = static_cast<char>(value);
                checksum += pos_str[i];
            }
            // std::cout << "pos_str: " << pos_str << std::endl;

            is >> size_cnode;
            // std::cout << "size CNode: " << size_cnode << std::endl;
            std::string cnode_str(size_cnode, '\0');
            is.ignore();
            for (size_t i = 0; i < size_cnode; ++i)
            {
                std::string hex_chars;
                hex_chars.resize(2);
                is.read(&hex_chars[0], 2);
                int value = std::stoi(hex_chars, nullptr, 16);
                cnode_str[i] = static_cast<char>(value);
                checksum += cnode_str[i];
            }
            // std::cout << "cnode: " << cnode_str << std::endl;
            //  Add the node to the spreadsheet
            this->setCell(CPos(pos_str), cnode_str);

            // Skip the newline character
            is.ignore();
        }
        // std::cout << "While loop stopped" << std::endl;
        unsigned int read_checksum;
        // Read the checksum
        is >> read_checksum;

        // Check if the read was successful and the checksums match
        if (is.fail() && !is.eof())
        {
            // std::cout << "is.fail() && !is.eof()" << std::endl;
            return false;
        }

        if (checksum != read_checksum)
        {
            // std::cout << "checksum != read_checksum" << std::endl;
            // std::cout << "checksum: " << checksum << " read_checksum: " << read_checksum << std::endl;
            return false;
        }
        return true;
    }
    catch (const std::exception &e)
    {
        return false;
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CSpreadsheet::print() const
{
    std::cout << "Printing spreadsheet" << std::endl;
    std::cout << "---------------------------------------------" << std::endl;
    for (const auto &pair : m_spreadsheet)
    {
        std::string output = "=";
        pair.second.exprToString(output);
        std::cout
            << "|" << pair.first.getNormStr() << "| : |" << output << "|" << std::endl;
    }
    std::cout << "---------------------------------------------" << std::endl;
    std::cout << "End of spreadsheet" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CEXPR BUILDER
////////////////////////////////////////////////////////////////////////////////////////////////////////

CMyOwnExprBuilder::CMyOwnExprBuilder() : m_expr_stack()
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

std::shared_ptr<CExpr> CMyOwnExprBuilder::getExpr() const
{
    return m_expr_stack.top();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::opAdd()
{
    shared_ptr<CExpr> right = m_expr_stack.top();
    m_expr_stack.pop();
    shared_ptr<CExpr> left = m_expr_stack.top();
    m_expr_stack.pop();

    m_expr_stack.push(make_shared<CAddition>(left, right));
    // cout << "+" << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::opSub()
{

    shared_ptr<CExpr> right = m_expr_stack.top();
    m_expr_stack.pop();
    shared_ptr<CExpr> left = m_expr_stack.top();
    m_expr_stack.pop();

    m_expr_stack.push(make_shared<CSubtraction>(left, right));
    // cout << "-" << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::opMul()
{
    shared_ptr<CExpr> right = m_expr_stack.top();
    m_expr_stack.pop();
    shared_ptr<CExpr> left = m_expr_stack.top();
    m_expr_stack.pop();

    m_expr_stack.push(make_shared<CMultiplication>(left, right));
    // cout << "*" << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::opDiv()
{
    shared_ptr<CExpr> right = m_expr_stack.top();
    m_expr_stack.pop();
    shared_ptr<CExpr> left = m_expr_stack.top();
    m_expr_stack.pop();

    m_expr_stack.push(make_shared<CDivision>(left, right));
    // cout << "/" << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::opPow()
{
    shared_ptr<CExpr> right = m_expr_stack.top();
    m_expr_stack.pop();
    shared_ptr<CExpr> left = m_expr_stack.top();
    m_expr_stack.pop();

    m_expr_stack.push(make_shared<CPow>(left, right));
    // cout << "^" << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::opNeg()
{
    shared_ptr<CExpr> right = m_expr_stack.top();
    m_expr_stack.pop();

    m_expr_stack.push(make_shared<CNegation>(right));
    // cout << " -";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::opEq()
{
    shared_ptr<CExpr> right = m_expr_stack.top();
    m_expr_stack.pop();
    shared_ptr<CExpr> left = m_expr_stack.top();
    m_expr_stack.pop();

    m_expr_stack.push(make_shared<CEquality>(left, right));
    // cout << "=" << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::opNe()
{
    shared_ptr<CExpr> right = m_expr_stack.top();
    m_expr_stack.pop();
    shared_ptr<CExpr> left = m_expr_stack.top();
    m_expr_stack.pop();

    m_expr_stack.push(make_shared<CInequality>(left, right));
    // cout << "<>" << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::opLt()
{
    shared_ptr<CExpr> right = m_expr_stack.top();
    m_expr_stack.pop();
    shared_ptr<CExpr> left = m_expr_stack.top();
    m_expr_stack.pop();

    m_expr_stack.push(make_shared<CLessThan>(left, right));
    // cout << "<" << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::opLe()
{
    shared_ptr<CExpr> right = m_expr_stack.top();
    m_expr_stack.pop();
    shared_ptr<CExpr> left = m_expr_stack.top();
    m_expr_stack.pop();

    m_expr_stack.push(make_shared<CLessEqual>(left, right));
    // cout << "<=" << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::opGt()
{
    shared_ptr<CExpr> right = m_expr_stack.top();
    m_expr_stack.pop();
    shared_ptr<CExpr> left = m_expr_stack.top();
    m_expr_stack.pop();

    m_expr_stack.push(make_shared<CGreaterThan>(left, right));
    // cout << ">" << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::opGe()
{
    shared_ptr<CExpr> right = m_expr_stack.top();
    m_expr_stack.pop();
    shared_ptr<CExpr> left = m_expr_stack.top();
    m_expr_stack.pop();

    m_expr_stack.push(make_shared<CGreaterEqual>(left, right));
    // cout << ">=" << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::valNumber(double val)
{
    // cout << "valNumber called: " << val << endl;
    m_expr_stack.push(make_shared<CNumber>(CValue(val)));
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::valString(std::string val)
{
    // cout << "valString called: " << val << endl;
    m_expr_stack.push(make_shared<CStr>(CValue(val)));
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::valReference(std::string val)
{
    // need to pass a reference to the spreadsheet into the CReference I think
    // in the evaluate I will then be able to call getValue on the correct CPos
    m_expr_stack.push(make_shared<CReference>(CValue(val)));
    // cout << val << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::valRange(std::string val)
{
    // cout << val << endl;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CMyOwnExprBuilder::funcCall(std::string fnName,
                                 int paramCount)
{
    // cout << fnName << " , " << paramCount << endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// LITERALS
////////////////////////////////////////////////////////////////////////////////////////////////////////

CNumber::CNumber(CValue value) : m_number(value)
{
    // std::cout << "CNumber created: " <<  << std::endl;
}

CValue CNumber::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    // std::cout << "Called evaluate on CNumber" << std::endl;
    return m_number;
}

std::shared_ptr<CExpr> CNumber::clone() const
{
    // std::cout << "Cloning CNumber" << std::endl;
    return std::make_shared<CNumber>(*this);
}

void CNumber::updateReferencesBy(const CPos &offset)
{
    // std::cout << "Called updateReferencesBy on CNumber" << std::endl;
}

void CNumber::exprToString(std::string &output) const
{
    output += std::to_string(std::get<double>(m_number));
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CStr::CStr(CValue value) : m_string(value)
{
    // std::cout << "CStr created: " << get<std::string>(m_string) << std::endl;
}

CValue CStr::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    // std::cout << "Called evaluate on CStr" << std::endl;
    return m_string;
}

std::shared_ptr<CExpr> CStr::clone() const
{
    // std::cout << "Cloning CStr" << std::endl;
    return std::make_shared<CStr>(*this);
}

void CStr::updateReferencesBy(const CPos &offset)
{
    // std::cout << "Called updateReferencesBy on CStr" << std::endl;
}

void CStr::exprToString(std::string &output) const
{
    // size_t size = get<std::string>(m_string).length();
    output += "\"";
    for (const char &c : get<std::string>(m_string))
    {
        if (c == '\"')
        {
            // output += "\\\"";
            output += "\"\"";
        }
        else
        {
            output += c;
        }
    }
    output += "\"";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CReference::CReference(CValue value)
{
    // std::cout << "CReference created" << std::endl;
    is_col_rel = true;
    is_row_rel = true;
    std::string ref = std::get<std::string>(value);
    if (ref[0] == '$')
    {
        is_col_rel = false;
        ref.erase(0, 1);
    }
    size_t dollar_pos = ref.find('$');
    if (dollar_pos != std::string::npos)
    {
        is_row_rel = false;
        ref.erase(dollar_pos, 1);
    }
    std::string tmp = "";
    process_cpos(ref, tmp, m_width, m_height);
}

CValue CReference::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    // std::cout << "Called evaluate on CReference" << std::endl;

    auto insert_result = visited.insert(std::make_pair(m_width, m_height));

    if (!insert_result.second) // cycle detected
    {
        return CValue();
    }

    auto result = m_spreadsheet.getValue(CPos(m_width, m_height));

    visited.erase(std::make_pair(m_width, m_height));

    return result;
}

std::shared_ptr<CExpr> CReference::clone() const
{
    // std::cout << "Cloning CReference" << std::endl;
    return std::make_shared<CReference>(*this);
}

void CReference::updateReferencesBy(const CPos &offset)
{
    // std::cout << "Called updateReferencesBy on CReference" << std::endl;
    if (is_col_rel)
    {
        m_width += offset.getWidth();
    }
    if (is_row_rel)
    {
        m_height += offset.getHeight();
    }
}

std::string CReference::RefStrRecreation() const
{
    std::string output = "";
    long long tmp_col = m_width;
    while (tmp_col > 0)
    {
        output = char('A' + tmp_col % 26) + output;
        tmp_col--; // decrement column to make it 0-indexed
        tmp_col /= 26;
    }
    if (output == "")
    {
        output = "A";
    }
    if (!is_col_rel)
    {
        output = "$" + output;
    }
    if (!is_row_rel)
    {
        output += "$";
    }
    // append the row number to the string
    output += std::to_string(m_height);
    return output;
}

void CReference::exprToString(std::string &output) const
{
    output += this->RefStrRecreation();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
// CEXPR OPERATORS
////////////////////////////////////////////////////////////////////////////////////////////////////////

CAddition::CAddition(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right) : m_left(move(m_left)), m_right(move(m_right))
{
}
CValue CAddition::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    // cout << "Called evaluate on CAddition" << endl;
    CValue right_val = m_right->evaluate(m_spreadsheet, visited);
    CValue left_val = m_left->evaluate(m_spreadsheet, visited);

    if (holds_alternative<double>(left_val) && holds_alternative<double>(right_val))
    {
        // cout << "Addition of two doubles" << endl;
        return CValue(get<double>(left_val) + get<double>(right_val));
    }
    else if (holds_alternative<string>(left_val) && holds_alternative<string>(right_val))
    {
        // cout << "Addition of two strings" << endl;
        return CValue(get<string>(left_val) + get<string>(right_val));
    }
    else if (holds_alternative<string>(left_val) && holds_alternative<double>(right_val))
    {
        // cout << "Addition of a string and a double" << endl;
        return CValue(get<string>(left_val) + to_string(get<double>(right_val)));
    }
    else if (holds_alternative<double>(left_val) && holds_alternative<string>(right_val))
    {
        // cout << "Addition of a double and a string" << endl;
        // cout << "left_val: " << get<double>(left_val) << endl;
        // cout << "right_val: " << get<string>(right_val) << endl;
        return CValue(to_string(get<double>(left_val)) + get<string>(right_val));
    }
    // if the CValue is not a string or a double, return an empty CValue
    // // cout << "Addition of two different types" << endl;
    return CValue();
}

std::shared_ptr<CExpr> CAddition::clone() const
{
    // return std::make_shared<CAddition>(*this);

    // this should ensure an AST deep copy for copying cells
    return std::make_shared<CAddition>(m_left->clone(), m_right->clone());
}

void CAddition::updateReferencesBy(const CPos &offset)
{
    m_left->updateReferencesBy(offset);
    m_right->updateReferencesBy(offset);
}

void CAddition::exprToString(std::string &output) const
{
    output += "(";
    m_left->exprToString(output);
    output += "+";
    m_right->exprToString(output);
    output += ")";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CSubtraction::CSubtraction(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right) : m_left(move(m_left)), m_right(move(m_right))
{
}
CValue CSubtraction::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    CValue left_val = m_left->evaluate(m_spreadsheet, visited);
    CValue right_val = m_right->evaluate(m_spreadsheet, visited);

    if (holds_alternative<double>(left_val) && holds_alternative<double>(right_val))
    {
        return CValue(get<double>(left_val) - get<double>(right_val));
    }
    // if the CValue is not a double, return an empty CValue
    return CValue();
}

std::shared_ptr<CExpr> CSubtraction::clone() const
{
    return std::make_shared<CSubtraction>(m_left->clone(), m_right->clone());
}

void CSubtraction::updateReferencesBy(const CPos &offset)
{
    m_left->updateReferencesBy(offset);
    m_right->updateReferencesBy(offset);
}

void CSubtraction::exprToString(std::string &output) const
{
    output += "(";
    m_left->exprToString(output);
    output += "-";
    m_right->exprToString(output);
    output += ")";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CMultiplication::CMultiplication(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right) : m_left(move(m_left)), m_right(move(m_right))
{
}

CValue CMultiplication::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    // // cout << "Called evaluate on CMultiplication" << endl;
    CValue left_val = m_left->evaluate(m_spreadsheet, visited);
    CValue right_val = m_right->evaluate(m_spreadsheet, visited);
    // // cout << "mult" << endl;
    if (holds_alternative<double>(left_val) && holds_alternative<double>(right_val))
    {
        // // cout << "Multiplication of two doubles" << endl;
        return CValue(get<double>(left_val) * get<double>(right_val));
    }
    // if the CValue is not a double, return an empty CValue
    return CValue();
}

std::shared_ptr<CExpr> CMultiplication::clone() const
{
    return std::make_shared<CMultiplication>(m_left->clone(), m_right->clone());
}

void CMultiplication::updateReferencesBy(const CPos &offset)
{
    m_left->updateReferencesBy(offset);
    m_right->updateReferencesBy(offset);
}

void CMultiplication::exprToString(std::string &output) const
{
    output += "(";
    m_left->exprToString(output);
    output += "*";
    m_right->exprToString(output);
    output += ")";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CDivision::CDivision(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right) : m_left(move(m_left)), m_right(move(m_right))
{
}

CValue CDivision::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    CValue left_val = m_left->evaluate(m_spreadsheet, visited);
    CValue right_val = m_right->evaluate(m_spreadsheet, visited);

    // division by 0
    if (CValue(0.0) == right_val)
    {
        return CValue();
    }

    if (holds_alternative<double>(left_val) && holds_alternative<double>(right_val))
    {
        return CValue(get<double>(left_val) / get<double>(right_val));
    }
    // if the CValue is not a double, return an empty CValue
    return CValue();
}

std::shared_ptr<CExpr> CDivision::clone() const
{
    return std::make_shared<CDivision>(m_left->clone(), m_right->clone());
}

void CDivision::updateReferencesBy(const CPos &offset)
{
    m_left->updateReferencesBy(offset);
    m_right->updateReferencesBy(offset);
}

void CDivision::exprToString(std::string &output) const
{
    output += "(";
    m_left->exprToString(output);
    output += "/";
    m_right->exprToString(output);
    output += ")";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CPow::CPow(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right) : m_left(move(m_left)), m_right(move(m_right))
{
}

CValue CPow::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    CValue left_val = m_left->evaluate(m_spreadsheet, visited);
    CValue right_val = m_right->evaluate(m_spreadsheet, visited);

    if (holds_alternative<double>(left_val) && holds_alternative<double>(right_val))
    {
        return CValue(pow(get<double>(left_val), get<double>(right_val)));
    }
    // if the CValue is not a double, return an empty CValue
    return CValue();
}

std::shared_ptr<CExpr> CPow::clone() const
{
    return std::make_shared<CPow>(m_left->clone(), m_right->clone());
}

void CPow::updateReferencesBy(const CPos &offset)
{
    m_left->updateReferencesBy(offset);
    m_right->updateReferencesBy(offset);
}

void CPow::exprToString(std::string &output) const
{
    output += "(";
    m_left->exprToString(output);
    output += "^";
    m_right->exprToString(output);
    output += ")";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CNegation::CNegation(std::shared_ptr<CExpr> neg) : m_neg(move(neg))
{
}

CValue CNegation::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    CValue val = m_neg->evaluate(m_spreadsheet, visited);

    if (holds_alternative<double>(val))
    {
        return CValue(-get<double>(val));
    }
    // if the CValue is not a double, return an empty CValue
    return CValue();
}

std::shared_ptr<CExpr> CNegation::clone() const
{
    return std::make_shared<CNegation>(m_neg->clone());
}

void CNegation::updateReferencesBy(const CPos &offset)
{
    m_neg->updateReferencesBy(offset);
}

void CNegation::exprToString(std::string &output) const
{
    output += "(-";
    m_neg->exprToString(output);
    output += ")";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CEquality::CEquality(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right) : m_left(move(m_left)), m_right(move(m_right))
{
}

CValue CEquality::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    CValue left_val = m_left->evaluate(m_spreadsheet, visited);
    CValue right_val = m_right->evaluate(m_spreadsheet, visited);

    if (holds_alternative<double>(left_val) && holds_alternative<double>(right_val))
    {
        if (get<double>(left_val) == get<double>(right_val))
        {
            return CValue(1.0);
        }
        else
        {
            return CValue(0.0);
        }
    }
    else if (holds_alternative<string>(left_val) && holds_alternative<string>(right_val))
    {
        if (get<string>(left_val) == get<string>(right_val))
        {
            return CValue(1.0);
        }
        else
        {
            return CValue(0.0);
        }
    }

    // if the CValue is not a double, return an empty CValue
    return CValue();
}

std::shared_ptr<CExpr> CEquality::clone() const
{
    return std::make_shared<CEquality>(m_left->clone(), m_right->clone());
}

void CEquality::updateReferencesBy(const CPos &offset)
{
    m_left->updateReferencesBy(offset);
    m_right->updateReferencesBy(offset);
}

void CEquality::exprToString(std::string &output) const
{
    output += "(";
    m_left->exprToString(output);
    output += "="; // !!!! = not ==
    m_right->exprToString(output);
    output += ")";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CInequality::CInequality(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right) : m_left(move(m_left)), m_right(move(m_right))
{
}

CValue CInequality::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    CValue left_val = m_left->evaluate(m_spreadsheet, visited);
    CValue right_val = m_right->evaluate(m_spreadsheet, visited);

    if (holds_alternative<double>(left_val) && holds_alternative<double>(right_val))
    {
        if (get<double>(left_val) != get<double>(right_val))
        {
            return CValue(1.0);
        }
        else
        {
            return CValue(0.0);
        }
    }
    else if (holds_alternative<string>(left_val) && holds_alternative<string>(right_val))
    {
        if (get<string>(left_val) != get<string>(right_val))
        {
            return CValue(1.0);
        }
        else
        {
            return CValue(0.0);
        }
    }

    // if the CValue is not a double or a string, return an empty CValue
    return CValue();
}

std::shared_ptr<CExpr> CInequality::clone() const
{
    return std::make_shared<CInequality>(m_left->clone(), m_right->clone());
}

void CInequality::updateReferencesBy(const CPos &offset)
{
    m_left->updateReferencesBy(offset);
    m_right->updateReferencesBy(offset);
}

void CInequality::exprToString(std::string &output) const
{
    output += "(";
    m_left->exprToString(output);
    output += "<>"; // <> not !=
    m_right->exprToString(output);
    output += ")";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CLessThan::CLessThan(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right) : m_left(move(m_left)), m_right(move(m_right))
{
}

CValue CLessThan::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    CValue left_val = m_left->evaluate(m_spreadsheet, visited);
    CValue right_val = m_right->evaluate(m_spreadsheet, visited);

    if (holds_alternative<double>(left_val) && holds_alternative<double>(right_val))
    {
        if (get<double>(left_val) < get<double>(right_val))
        {
            return CValue(1.0);
        }
        else
        {
            return CValue(0.0);
        }
    }
    else if (holds_alternative<string>(left_val) && holds_alternative<string>(right_val))
    {
        if (get<string>(left_val) < get<string>(right_val))
        {
            return CValue(1.0);
        }
        else
        {
            return CValue(0.0);
        }
    }

    // if the CValue is not a double or a string, return an empty CValue
    return CValue();
}

std::shared_ptr<CExpr> CLessThan::clone() const
{
    return std::make_shared<CLessThan>(m_left->clone(), m_right->clone());
}

void CLessThan::updateReferencesBy(const CPos &offset)
{
    m_left->updateReferencesBy(offset);
    m_right->updateReferencesBy(offset);
}

void CLessThan::exprToString(std::string &output) const
{
    output += "(";
    m_left->exprToString(output);
    output += "<";
    m_right->exprToString(output);
    output += ")";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CLessEqual::CLessEqual(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right) : m_left(move(m_left)), m_right(move(m_right))
{
}

CValue CLessEqual::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    CValue left_val = m_left->evaluate(m_spreadsheet, visited);
    CValue right_val = m_right->evaluate(m_spreadsheet, visited);

    if (holds_alternative<double>(left_val) && holds_alternative<double>(right_val))
    {
        if (get<double>(left_val) <= get<double>(right_val))
        {
            return CValue(1.0);
        }
        else
        {
            return CValue(0.0);
        }
    }
    else if (holds_alternative<string>(left_val) && holds_alternative<string>(right_val))
    {
        if (get<string>(left_val) <= get<string>(right_val))
        {
            return CValue(1.0);
        }
        else
        {
            return CValue(0.0);
        }
    }

    // if the CValue is not a double or a string, return an empty CValue
    return CValue();
}

std::shared_ptr<CExpr> CLessEqual::clone() const
{
    return std::make_shared<CLessEqual>(m_left->clone(), m_right->clone());
}

void CLessEqual::updateReferencesBy(const CPos &offset)
{
    m_left->updateReferencesBy(offset);
    m_right->updateReferencesBy(offset);
}

void CLessEqual::exprToString(std::string &output) const
{
    output += "(";
    m_left->exprToString(output);
    output += "<=";
    m_right->exprToString(output);
    output += ")";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CGreaterThan::CGreaterThan(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right) : m_left(move(m_left)), m_right(move(m_right))
{
}

CValue CGreaterThan::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    CValue left_val = m_left->evaluate(m_spreadsheet, visited);
    CValue right_val = m_right->evaluate(m_spreadsheet, visited);

    if (holds_alternative<double>(left_val) && holds_alternative<double>(right_val))
    {
        if (get<double>(left_val) > get<double>(right_val))
        {
            return CValue(1.0);
        }
        else
        {
            return CValue(0.0);
        }
    }
    else if (holds_alternative<string>(left_val) && holds_alternative<string>(right_val))
    {
        if (get<string>(left_val) > get<string>(right_val))
        {
            return CValue(1.0);
        }
        else
        {
            return CValue(0.0);
        }
    }

    // if the CValue is not a double or a string, return an empty CValue
    return CValue();
}

std::shared_ptr<CExpr> CGreaterThan::clone() const
{
    return std::make_shared<CGreaterThan>(m_left->clone(), m_right->clone());
}

void CGreaterThan::updateReferencesBy(const CPos &offset)
{
    m_left->updateReferencesBy(offset);
    m_right->updateReferencesBy(offset);
}

void CGreaterThan::exprToString(std::string &output) const
{
    output += "(";
    m_left->exprToString(output);
    output += ">";
    m_right->exprToString(output);
    output += ")";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CGreaterEqual::CGreaterEqual(std::shared_ptr<CExpr> m_left, std::shared_ptr<CExpr> m_right) : m_left(move(m_left)), m_right(move(m_right))
{
}

CValue CGreaterEqual::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    CValue left_val = m_left->evaluate(m_spreadsheet, visited);
    CValue right_val = m_right->evaluate(m_spreadsheet, visited);

    if (holds_alternative<double>(left_val) && holds_alternative<double>(right_val))
    {
        if (get<double>(left_val) >= get<double>(right_val))
        {
            return CValue(1.0);
        }
        else
        {
            return CValue(0.0);
        }
    }
    else if (holds_alternative<string>(left_val) && holds_alternative<string>(right_val))
    {
        if (get<string>(left_val) >= get<string>(right_val))
        {
            return CValue(1.0);
        }
        else
        {
            return CValue(0.0);
        }
    }

    // if the CValue is not a double or a string, return an empty CValue
    return CValue();
}

std::shared_ptr<CExpr> CGreaterEqual::clone() const
{
    return std::make_shared<CGreaterEqual>(m_left->clone(), m_right->clone());
}

void CGreaterEqual::updateReferencesBy(const CPos &offset)
{
    m_left->updateReferencesBy(offset);
    m_right->updateReferencesBy(offset);
}

void CGreaterEqual::exprToString(std::string &output) const
{
    output += "(";
    m_left->exprToString(output);
    output += ">=";
    m_right->exprToString(output);
    output += ")";
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PROGTEST__

bool valueMatch(const CValue &r,
                const CValue &s)

{
    if (r.index() != s.index())
        return false;
    if (r.index() == 0)
        return true;
    if (r.index() == 2)
        return std::get<std::string>(r) == std::get<std::string>(s);
    if (std::isnan(std::get<double>(r)) && std::isnan(std::get<double>(s)))
        return true;
    if (std::isinf(std::get<double>(r)) && std::isinf(std::get<double>(s)))
        return (std::get<double>(r) < 0 && std::get<double>(s) < 0) || (std::get<double>(r) > 0 && std::get<double>(s) > 0);
    return fabs(std::get<double>(r) - std::get<double>(s)) <= 1e8 * DBL_EPSILON * fabs(std::get<double>(r));
}
int main()
{
    CSpreadsheet x0, x1;
    std::ostringstream oss;
    std::istringstream iss;
    std::string data;
    assert(x0.setCell(CPos("A1"), "10"));
    assert(x0.setCell(CPos("A2"), "20.5"));
    assert(x0.setCell(CPos("A3"), "3e1"));
    assert(x0.setCell(CPos("A4"), "=40"));
    assert(x0.setCell(CPos("A5"), "=5e+1"));
    assert(x0.setCell(CPos("A6"), "raw text with any characters, including a quote \" or a newline\n"));
    assert(x0.setCell(CPos("A7"), "=\"quoted string, quotes must be doubled: \"\". Moreover, backslashes are needed for C++.\""));
    assert(valueMatch(x0.getValue(CPos("A1")), CValue(10.0)));
    assert(valueMatch(x0.getValue(CPos("A2")), CValue(20.5)));
    assert(valueMatch(x0.getValue(CPos("A3")), CValue(30.0)));
    assert(valueMatch(x0.getValue(CPos("A4")), CValue(40.0)));
    assert(valueMatch(x0.getValue(CPos("A5")), CValue(50.0)));
    assert(valueMatch(x0.getValue(CPos("A6")), CValue("raw text with any characters, including a quote \" or a newline\n")));
    assert(valueMatch(x0.getValue(CPos("A7")), CValue("quoted string, quotes must be doubled: \". Moreover, backslashes are needed for C++.")));
    assert(valueMatch(x0.getValue(CPos("A8")), CValue()));
    assert(valueMatch(x0.getValue(CPos("AAAA9999")), CValue()));
    assert(x0.setCell(CPos("B1"), "=A1+A2*A3"));
    assert(x0.setCell(CPos("B2"), "= -A1 ^ 2 - A2 / 2   "));
    assert(x0.setCell(CPos("B3"), "= 2 ^ $A$1"));
    assert(x0.setCell(CPos("B4"), "=($A1+A$2)^2"));
    assert(x0.setCell(CPos("B5"), "=B1+B2+B3+B4"));
    assert(x0.setCell(CPos("B6"), "=B1+B2+B3+B4+B5"));
    assert(valueMatch(x0.getValue(CPos("B1")), CValue(625.0)));
    assert(valueMatch(x0.getValue(CPos("B2")), CValue(-110.25)));
    assert(valueMatch(x0.getValue(CPos("B3")), CValue(1024.0)));
    assert(valueMatch(x0.getValue(CPos("B4")), CValue(930.25)));
    assert(valueMatch(x0.getValue(CPos("B5")), CValue(2469.0)));
    assert(valueMatch(x0.getValue(CPos("B6")), CValue(4938.0)));
    assert(x0.setCell(CPos("A1"), "12"));
    assert(valueMatch(x0.getValue(CPos("B1")), CValue(627.0)));
    assert(valueMatch(x0.getValue(CPos("B2")), CValue(-154.25)));
    assert(valueMatch(x0.getValue(CPos("B3")), CValue(4096.0)));
    assert(valueMatch(x0.getValue(CPos("B4")), CValue(1056.25)));
    assert(valueMatch(x0.getValue(CPos("B5")), CValue(5625.0)));
    assert(valueMatch(x0.getValue(CPos("B6")), CValue(11250.0)));
    x1 = x0;
    assert(x0.setCell(CPos("A2"), "100"));
    assert(x1.setCell(CPos("A2"), "=A3+A5+A4"));
    assert(valueMatch(x0.getValue(CPos("B1")), CValue(3012.0)));
    assert(valueMatch(x0.getValue(CPos("B2")), CValue(-194.0)));
    assert(valueMatch(x0.getValue(CPos("B3")), CValue(4096.0)));
    assert(valueMatch(x0.getValue(CPos("B4")), CValue(12544.0)));
    assert(valueMatch(x0.getValue(CPos("B5")), CValue(19458.0)));
    assert(valueMatch(x0.getValue(CPos("B6")), CValue(38916.0)));
    assert(valueMatch(x1.getValue(CPos("B1")), CValue(3612.0)));
    assert(valueMatch(x1.getValue(CPos("B2")), CValue(-204.0)));
    assert(valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
    assert(valueMatch(x1.getValue(CPos("B4")), CValue(17424.0)));
    assert(valueMatch(x1.getValue(CPos("B5")), CValue(24928.0)));
    assert(valueMatch(x1.getValue(CPos("B6")), CValue(49856.0)));
    oss.clear();
    oss.str("");
    assert(x0.save(oss));
    data = oss.str();
    iss.clear();
    iss.str(data);
    assert(x1.load(iss));
    assert(valueMatch(x1.getValue(CPos("B1")), CValue(3012.0)));
    assert(valueMatch(x1.getValue(CPos("B2")), CValue(-194.0)));
    assert(valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
    assert(valueMatch(x1.getValue(CPos("B4")), CValue(12544.0)));
    assert(valueMatch(x1.getValue(CPos("B5")), CValue(19458.0)));
    assert(valueMatch(x1.getValue(CPos("B6")), CValue(38916.0)));
    assert(x0.setCell(CPos("A3"), "4e1"));
    assert(valueMatch(x1.getValue(CPos("B1")), CValue(3012.0)));
    assert(valueMatch(x1.getValue(CPos("B2")), CValue(-194.0)));
    assert(valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
    assert(valueMatch(x1.getValue(CPos("B4")), CValue(12544.0)));
    assert(valueMatch(x1.getValue(CPos("B5")), CValue(19458.0)));
    assert(valueMatch(x1.getValue(CPos("B6")), CValue(38916.0)));
    oss.clear();
    oss.str("");
    assert(x0.save(oss));
    data = oss.str();
    for (size_t i = 0; i < std::min<size_t>(data.length(), 10); i++)
        data[i] ^= 0x5a;
    iss.clear();
    iss.str(data);
    assert(!x1.load(iss));
    assert(x0.setCell(CPos("D0"), "10"));
    assert(x0.setCell(CPos("D1"), "20"));
    assert(x0.setCell(CPos("D2"), "30"));
    assert(x0.setCell(CPos("D3"), "40"));
    assert(x0.setCell(CPos("D4"), "50"));
    assert(x0.setCell(CPos("E0"), "60"));
    assert(x0.setCell(CPos("E1"), "70"));
    assert(x0.setCell(CPos("E2"), "80"));
    assert(x0.setCell(CPos("E3"), "90"));
    assert(x0.setCell(CPos("E4"), "100"));
    assert(x0.setCell(CPos("F10"), "=D0+5"));
    assert(x0.setCell(CPos("F11"), "=$D0+5"));
    assert(x0.setCell(CPos("F12"), "=D$0+5"));
    assert(x0.setCell(CPos("F13"), "=$D$0+5"));
    x0.copyRect(CPos("G11"), CPos("F10"), 1, 4);
    assert(valueMatch(x0.getValue(CPos("F10")), CValue(15.0)));
    assert(valueMatch(x0.getValue(CPos("F11")), CValue(15.0)));
    assert(valueMatch(x0.getValue(CPos("F12")), CValue(15.0)));
    assert(valueMatch(x0.getValue(CPos("F13")), CValue(15.0)));
    assert(valueMatch(x0.getValue(CPos("F14")), CValue()));
    assert(valueMatch(x0.getValue(CPos("G10")), CValue()));
    assert(valueMatch(x0.getValue(CPos("G11")), CValue(75.0)));
    assert(valueMatch(x0.getValue(CPos("G12")), CValue(25.0)));
    assert(valueMatch(x0.getValue(CPos("G13")), CValue(65.0)));
    assert(valueMatch(x0.getValue(CPos("G14")), CValue(15.0)));
    x0.copyRect(CPos("G11"), CPos("F10"), 2, 4);
    assert(valueMatch(x0.getValue(CPos("F10")), CValue(15.0)));
    assert(valueMatch(x0.getValue(CPos("F11")), CValue(15.0)));
    assert(valueMatch(x0.getValue(CPos("F12")), CValue(15.0)));
    assert(valueMatch(x0.getValue(CPos("F13")), CValue(15.0)));
    assert(valueMatch(x0.getValue(CPos("F14")), CValue()));
    assert(valueMatch(x0.getValue(CPos("G10")), CValue()));
    assert(valueMatch(x0.getValue(CPos("G11")), CValue(75.0)));
    assert(valueMatch(x0.getValue(CPos("G12")), CValue(25.0)));
    assert(valueMatch(x0.getValue(CPos("G13")), CValue(65.0)));
    assert(valueMatch(x0.getValue(CPos("G14")), CValue(15.0)));
    assert(valueMatch(x0.getValue(CPos("H10")), CValue()));
    assert(valueMatch(x0.getValue(CPos("H11")), CValue()));
    assert(valueMatch(x0.getValue(CPos("H12")), CValue()));
    assert(valueMatch(x0.getValue(CPos("H13")), CValue(35.0)));
    assert(valueMatch(x0.getValue(CPos("H14")), CValue()));
    assert(x0.setCell(CPos("F0"), "-27"));
    assert(valueMatch(x0.getValue(CPos("H14")), CValue(-22.0)));
    x0.copyRect(CPos("H12"), CPos("H13"), 1, 2);
    assert(valueMatch(x0.getValue(CPos("H12")), CValue(25.0)));
    assert(valueMatch(x0.getValue(CPos("H13")), CValue(-22.0)));
    assert(valueMatch(x0.getValue(CPos("H14")), CValue(-22.0)));

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
