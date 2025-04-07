#include "../headers/CMyOwnExprBuilder.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include "../headers/CExpr/Literals.hpp"
#include "../headers/CExpr/Operators.hpp"

using namespace std;

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