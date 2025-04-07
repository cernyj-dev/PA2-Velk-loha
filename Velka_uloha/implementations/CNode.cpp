#include "../headers/CNode.hpp"
#include "../headers/CExpr/CExpr_abstract.hpp"
#include "../headers/CExpr/Literals.hpp"
#include "../headers/CMyOwnExprBuilder.hpp"
#include "../headers/pair_util.hpp"

using namespace std;

CNode::CNode() : m_cexpr(nullptr) {}

// to correctly decide if the setValue input is a number or a string, stod second argument is the position at which stod stopped reading the string because of a character it couldnt transform into a double
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