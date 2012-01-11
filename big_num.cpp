#include "big_num.h"
/************************************
 *        CLASS CONSTRUCTOR         *
 ************************************/
big_num::big_num()
{
    number = "";
    sign = 1;
}

big_num::big_num(int n) {
    std::stringstream convert;
    convert << n;
    number =  convert.str();
    sign = 1;
    if (n < 0)
    {
        number.erase(0,1);
        sign = -1;
    }
}

big_num::big_num(string &str) {
    if (str.at(0) == '-')
    {
        sign = -1;
        number = str.substr(1);
    } else {
        number = str;
        sign = 1;
    }
}
/************************************
 *        ASSIGNMENT OPERATOR       *
 ************************************/
big_num big_num::operator=(big_num op2)
{
    number = op2.number;
    sign = op2.sign;
    return *this;
}
/*****************************************************
 * FUNCTION: abs_compare()                           *
 *                                                   *
 * USE: to compare the absolute value of two number  *
 *                                                   *
 * PARAMETER: two number of type big_num             *
 *                                                   *
 * RETURN:                                           *
 * 	1		n1 > n2                                  *
 * -1		n2 < n2                                  *
 * 	0		n1 = n2                                  *
 *****************************************************/
int abs_compare (big_num n1, big_num n2)
{
    //Compare the lengths of these numbers
    if (n1.number.length() > n2.number.length())
        return 1;
    else if (n1.number.length() < n2.number.length())
        return -1;
    //If they have the same length, compare their digits from left to right
    else
    {
        for (unsigned int i = 0; i < n1.number.length(); i++)
        {
            if (n1.number.at(i) > n2.number.at(i))
                return 1;
            else if (n1.number.at(i) < n2.number.at(i))
                return -1;
        }
        return 0;
    }
}

/********************************************************
 * 														*
 * FUNCTION: s2n() - Convert string to number			*
 * 														*
 ********************************************************/
int s2n (const string& str)
{
    int n;
    istringstream convert(str);
    convert >> n;
    return n;
}

/********************************************************
 *                                                      *
 * FUNCTION: factorial() - The name means itself        *
 *                                                      *
 ********************************************************/
big_num big_num::factorial()
{
    big_num result(1);
    if (number == "0")
        return result;
    if (sign == -1)
    {
        result.number = "";
        result.sign = 0;
        return result;
    }
    for ( ; number != "0"; *this = *this - 1)
        result = result * *this;
    return result;
}
