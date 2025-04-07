#ifndef CPOS_H
#define CPOS_H

#include <string>

long long calculate_width(const std::string &source);

void process_cpos(const std::string_view &source, std::string &norm_str, long long &width, long long &height);

class CPos
{
public:
    // CPos(std::string_view str
    CPos(std::string_view str = "");
    CPos(const long long &width, const long long &height);
    bool operator==(const CPos &rhs) const;
    long long getWidth() const;
    long long getHeight() const;
    friend CPos operator-(const CPos &lhs, const CPos &rhs);
    friend CPos operator+(const CPos &lhs, const CPos &rhs);
    std::string getNormStr() const;
    void setNormStr(const std::string &str);
    void mNormStrRecreation();

private:
    std::string m_norm_str;
    long long m_width;
    long long m_height;
};

#endif