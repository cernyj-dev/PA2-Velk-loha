#ifndef CEXP_ABSTRACT_HPP
#define CEXP_ABSTRACT_HPP
#include <variant>
#include <string>
#include <memory>
#include "../CPos.hpp"
#include <unordered_set>
#include "../pair_util.hpp"

class CSpreadsheet;

using CValue = std::variant<std::monostate, double, std::string>;
class CExpr
{
public:
    virtual CValue evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const = 0;
    virtual ~CExpr() = default;
    virtual std::shared_ptr<CExpr> clone() const = 0;
    virtual void updateReferencesBy(const CPos &offset) = 0;
    virtual void exprToString(std::string &output) const = 0;
};
#endif