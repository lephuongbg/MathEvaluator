#include <evaluator.h>
#include <big_num.h>

/********************************************************
 * 														*
 * FUNCTION: prior() - 	Compare priority between		*
 * 						two operator					*
 * 														*
 ********************************************************/
int precedence(char op1)
{
    switch (op1) {
    case '(':
    case ')':
        return 1;
    case '!':
        return 2;
    case '*':
    case '/':
        return 3;
    case '+':
    case '-':
        return 4;
    }
    return 0;
}

void do_math(char op, stack<big_num>& number)
{
    big_num  n1, n2;
    switch(op) {
    case '+':
        n1 = number.top();number.pop();
        n2 = number.top();number.pop();
        number.push(n2+n1);
        break;
    case '-':
        n1 = number.top();number.pop();
        n2 = number.top();number.pop();
        number.push(n2-n1);
        break;
    case '*':
        n1 = number.top();number.pop();
        n2 = number.top();number.pop();
        number.push(n2*n1);
        break;
    case '/':
        n1 = number.top();number.pop();
        n2 = number.top();number.pop();
        number.push(n2/n1);
        break;
    case '!':
        n1 = number.top();number.pop();
        number.push(n1.factorial());
    default:
        break;
    }
}

