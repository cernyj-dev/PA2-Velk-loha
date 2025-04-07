#include "../../headers/CExpr/Literals.hpp"
#include "../../headers/CSpreadsheet.hpp"
#include "../../headers/CPos.hpp"

#include <variant>
#include <string>

using CValue = std::variant<std::monostate, double, std::string>;

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
    // normalizing the position string and then calling a process_cpos to fill the m_width and m_height numbers
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

// DFS implemented for cycle detection
CValue CReference::evaluate(CSpreadsheet &m_spreadsheet, std::unordered_set<std::pair<long long, long long>, pairHasher, pairEqual> &visited) const
{
    // std::cout << "Called evaluate on CReference" << std::endl;

    // adding the position into a set - this cell is now "open"
    auto insert_result = visited.insert(std::make_pair(m_width, m_height));

    // if it was already in the set, the insert returns a false
    if (!insert_result.second) // cycle detected
    {
        return CValue();
    }

    // go through with the evaluation
    auto result = m_spreadsheet.getValue(CPos(m_width, m_height));

    // erase from the set - this cell is now "closed"
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