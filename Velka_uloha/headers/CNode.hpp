#ifndef CNODE_H
#define CNODE_H

#include <variant>
#include <string>
#include <iostream>
#include <memory>
#include "CExpr/CExpr_abstract.hpp"
#include "CPos.hpp"
#include <unordered_set>
#include "pair_util.hpp"
// #include "CSpreadsheet.hpp"

class CSpreadsheet;

using CValue = std::variant<std::monostate, double, std::string>;
// angle for polymorphism for future AST? CNode -> CNum, CStr, CExp ?
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

#endif