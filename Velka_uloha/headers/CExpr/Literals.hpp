#ifndef LITERAL_HPP
#define LITERAL_HPP
#include <variant>
#include <string>
#include <memory>
#include <unordered_set>
#include "../pair_util.hpp"
#include "./CExpr_abstract.hpp"
// #include "../CSpreadsheet.hpp"
//  #include "../CPos.hpp"

using CValue = std::variant<std::monostate, double, std::string>;

class CSpreadsheet;

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

#endif