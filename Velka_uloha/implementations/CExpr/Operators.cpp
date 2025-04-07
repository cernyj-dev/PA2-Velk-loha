#include "../../headers/CExpr/Operators.hpp"
#include <memory>
#include <iostream>
#include <cmath>
#include <variant>
#include <string>
using CValue = std::variant<std::monostate, double, std::string>;
using namespace std;

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
