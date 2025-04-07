#ifndef OPERATORS_HPP
#define OPERATORS_HPP

#include "./CExpr_abstract.hpp"
#include <memory>
#include "../CSpreadsheet.hpp"
#include <unordered_set>

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

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif