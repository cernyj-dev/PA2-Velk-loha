#ifndef CMYOWNEXPRBUILDER_H
#define CMYOWNEXPRBUILDER_H

#include "../expression.h"
#include <stack>
#include <memory>
#include "CExpr/CExpr_abstract.hpp"
#include "./CSpreadsheet.hpp"

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

#endif