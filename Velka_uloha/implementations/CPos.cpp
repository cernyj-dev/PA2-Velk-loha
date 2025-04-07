#include "../headers/CPos.hpp"

#include <stdexcept>
#include <iostream>

using namespace std;

long long calculate_width(const string &source)
{
    long long result = 0;
    for (char c : source)
    {
        result = result * 26 + c - 'A' + 1; // ASCII normalization into a number
    }
    return result - 1;
}

// Used to process the (for example "A1") position into a number to use later on in the unordered_map Spreadsheet
void process_cpos(const string_view &source, string &norm_str, long long &width, long long &height)
{

    if (source.length() == 0)
    {
        throw invalid_argument("CPos input is empty: CPos constructor");
    }
    string tmp_cpos; // tmp - before assigning it directly to the member string, i gotta check it first and not change the member string if something goes bad
    bool letters_parsed = false;
    bool one_letter_parsed = false; // i dont really want to have a standard for cycle - check if something was parsed, if a letter wasnt parsed, it should be an error
    bool one_number_parsed = false;
    long long tmp_width = 0;
    string tmp_height; // concat to this until the source is parsed, then use stoull to create a long long from the string

    for (char c : source)
    {
        if (letters_parsed == false) // need to still parse through letters
        {
            if (islower(c))
            {
                char b = toupper(c);
                tmp_cpos += b; // normalize for future use
                one_letter_parsed = true;
            }
            else if (isupper(c))
            {

                tmp_cpos += c; // normalize for future use
                one_letter_parsed = true;
            }
            else if (isdigit(c))
            {
                tmp_width = calculate_width(tmp_cpos);
                letters_parsed = true;
                one_number_parsed = true;
                tmp_height += c;
                tmp_cpos += c;
            }
            else
            {
                throw invalid_argument("Something fishy among the letters of inputed src: CPos constructor");
            }
            if (one_letter_parsed == false)
            { // if no letter was parsed - meaning there was probably a number right off the bat - error
                throw invalid_argument("No letter was registered: CPos constructor");
            }
        }
        else // letters were parsed
        {
            if (isdigit(c))
            {
                tmp_height += c;
                tmp_cpos += c;
            }
            else // letter is not a digit even though there are supposed to be only digits now
            {
                throw invalid_argument("Something fishy among the numbers of inputed src: CPos constructor");
            }
        }
    }
    if (one_number_parsed == false)
    {
        throw invalid_argument("No number was registered: CPos constructor");
    }
    // third : assign values if everything went well
    height = stoull(tmp_height);
    width = tmp_width;
    norm_str = tmp_cpos;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CPos::CPos(string_view str) : m_norm_str(""), m_width(0), m_height(0)
{
    process_cpos(str, m_norm_str, m_width, m_height);
    // cout <<  ", m_width: " << m_width << ", m_height: " << m_height << endl; //[PRINT]
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CPos::CPos(const long long &width, const long long &height) : m_width(width), m_height(height)
{
    // cout << ", m_width: " << m_width << ", m_height: " << m_height << endl; //[PRINT]
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

long long CPos::getWidth() const
{
    return m_width;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

long long CPos::getHeight() const
{
    return m_height;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// for internal "sorting" mechanism of unordered_map: Hash
bool CPos::operator==(const CPos &rhs) const
{
    return (m_width == rhs.m_width && m_height == rhs.m_height);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
CPos operator-(const CPos &lhs, const CPos &rhs)
{
    return CPos(lhs.m_width - rhs.m_width, lhs.m_height - rhs.m_height);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CPos operator+(const CPos &lhs, const CPos &rhs)
{
    return CPos(lhs.m_width + rhs.m_width, lhs.m_height + rhs.m_height);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

string CPos::getNormStr() const
{
    return m_norm_str;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void CPos::setNormStr(const string &str)
{
    m_norm_str = str;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Normalizing the processed position from a number into the string version again
void CPos::mNormStrRecreation()
{
    m_norm_str = "";
    long long tmp_col = m_width;
    while (tmp_col > 0)
    {
        m_norm_str = char('A' + tmp_col % 26) + m_norm_str;
        tmp_col--; // decrement column to make it 0-indexed
        tmp_col /= 26;
    }
    if (m_norm_str == "")
    {
        m_norm_str = "A";
    }
    // append the row number to the string
    m_norm_str += std::to_string(m_height);
}
