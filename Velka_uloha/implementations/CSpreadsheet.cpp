#include "../headers/CSpreadsheet.hpp"
#include "../headers/CExpr/Literals.hpp"
#include <sstream>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include "../headers/pair_util.hpp"

// needed for unordered_map
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
        // write the CPos original string in hexadecimal format
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
    // write the checksum
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
            // add the node to the spreadsheet
            this->setCell(CPos(pos_str), cnode_str);

            // skip the newline character
            is.ignore();
        }
        // std::cout << "While loop stopped" << std::endl;
        unsigned int read_checksum;
        // read the checksum
        is >> read_checksum;

        // check if the read was successful and the checksums match
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