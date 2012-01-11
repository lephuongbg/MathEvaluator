#include "big_num.h"
#include "evaluator.h"

/********************************************
 *      OPERATOR OVERLOADING: DIVIDE		*
 ********************************************/

big_num big_num::operator/(big_num op2)
{
    big_num result;
    //Error handling
    if (this->sign == 0 || op2.sign == 0)
    {
        result.sign = 0;
        return result;
    }

    //Divide by zero error
    if (op2.number == "0")
    {
        result.sign = 0;
        return result;
    }
    /*Calculate the number of digits for the integer part*/
    int res_digits = 0;
    {
        big_num temp(op2);
        while(abs_compare(*this,temp) != -1)
        {
            res_digits++;
            temp = temp * 10;
        }
    }
    if (res_digits == 0)
    {
        return 0;
    }
    res_digits = res_digits * 2 + 1;	//Must have to make sure of the precision

    /*CREATE ARRAYS FOR USING FOURRIER ALGORITHM*/
    //Break dividend into two-digit pieces
    vector<int> dividend(res_digits-res_digits/2 + 1,0);	//Plus 1 to prevent out of range situation
    for (unsigned int i = 0; i < dividend.size(); i++)
    {
        if (i*2 < number.length()) {
            dividend.at(i) = s2n(number.substr(i*2,2));
            if (dividend.at(i) < 10)
                dividend.at(i)*=10;
        } else {
            dividend.at(i) = 0;
        }
    }

    //Break dividsor into two-digit pieces
    vector<int> divisor(res_digits-res_digits/2,0);
    for (unsigned int i = 0; i < divisor.size(); i++)
    {
        if (i*2 < op2.number.length())	{
            divisor.at(i) = s2n(op2.number.substr(i*2,2));
            if (divisor.at(i) < 10)
                divisor.at(i)*=10;
        } else {
            divisor.at(i) = 0;
        }
    }

    /*APPLYING FOURIER ALGORITHM*/
    //More info on wikipedia
    vector<int> remainder(1, (dividend.at(0)*100+dividend.at(1))%divisor.at(0));
    vector<int> result_temp(1, (dividend.at(0)*100+dividend.at(1))/divisor.at(0));
    int temp_int = 0;
    for ( int i = 1; i < res_digits-res_digits/2; i++)
    {
        for (int j = 1; j <= i; j++)
            temp_int += result_temp.at(i-j)*divisor.at(j);

        remainder.resize(remainder.size()+1, (remainder.at(i-1)*100+dividend.at(i+1)-temp_int)%divisor.at(0));
        if (remainder.at(i) < 0)
            remainder.at(i) += divisor.at(0);

        result_temp.resize(result_temp.size()+1, ((remainder.at(i-1)*100+dividend.at(i+1)-temp_int)-remainder.at(i))/divisor.at(0));

        temp_int = 0;
    }

    /*STORE RESULT*/
    result = result_temp.at(0);
    for (unsigned int i = 1; i < result_temp.size(); i++)
        result = result * 100 + result_temp.at(i);
    result.number.erase((res_digits-1)/2);
    result.sign = sign*op2.sign;

    return result;
}
