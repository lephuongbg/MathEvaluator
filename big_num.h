#ifndef BIG_NUM_H
#define BIG_NUM_H
#include <QString>
#include <QChar>
#include <QObject>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>

#define D2C(x) (x + 48)	//Digit to character
#define C2D(x) (x - 48)	//Character to digit
using namespace std;
class big_num {

public:
    string number;
    int sign;

    //Constructors
    big_num();
    big_num(int n);
    big_num(string &str);

    //Overloading operators:
    //(these operators are in separate source files)
    big_num operator=(big_num op2);
    big_num operator+(big_num op2);
    big_num operator+(int op2);
    big_num operator-(big_num op2);
    big_num operator*(big_num op2);
    big_num operator*(int op2);
    big_num operator/(big_num op2);

    //(these operators are in the class source file)
    big_num operator^(big_num op2);

    //Factorial
    big_num factorial();
};

/*Related functions*/
int abs_compare (big_num n1, big_num n2);
int s2n (const string& str);
#endif // BIG_NUM_H
