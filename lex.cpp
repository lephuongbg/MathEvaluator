#include "big_num.h"
#include "evaluator.h"
#include "lex.h"
/********************************************************
 *              CONSTRUCTOR FOR CLASS LEX               *
 ********************************************************/
lex::lex()
{
    input = "";
}
void lex::set(const string &str)
{
    input = str;
    lexed_result = lex_analyse(input);
}

/********************************************************
 * FUNCTION: lex_analyse()								*
 * USE TO: 												*
 * 		- break the input string into token				*
 * 		- standardize the expression and check for it	*
 * 		validity before evaluating						*
 * PARAMETER: a string of math expression				*
 * RETURN: 	an array of tokens, each token represents	*
 * 			an operator or a number						*
 ********************************************************/
vector<lex::lex_sequence> lex::lex_analyse(const string& str)
{
    vector<lex_sequence> lexed;
    vector<lex_sequence> &result = lexed;

    unsigned int i = 0;
    while (i < str.length())
    {
        switch (str.at(i))
        {
        case '+':
            lexed.resize(lexed.size()+1);
            lexed.at(lexed.size()-1).math_op = '+';
            lexed.at(lexed.size()-1).element = op;
            i++;
            break;
        case '-':
            lexed.resize(lexed.size()+1);
            lexed.at(lexed.size()-1).math_op = '-';
            lexed.at(lexed.size()-1).element = op;
            i++;
            break;
        case '*':
            lexed.resize(lexed.size()+1);
            lexed.at(lexed.size()-1).math_op = '*';
            lexed.at(lexed.size()-1).element = op;
            i++;
            break;
        case '/':
            lexed.resize(lexed.size()+1);
            lexed.at(lexed.size()-1).math_op = '/';
            lexed.at(lexed.size()-1).element = op;
            i++;
            break;
        case '(':
            lexed.resize(lexed.size()+1);
            lexed.at(lexed.size()-1).math_op = '(';
            lexed.at(lexed.size()-1).element = op;
            i++;
            break;
        case ')':
            lexed.resize(lexed.size()+1);
            lexed.at(lexed.size()-1).math_op = ')';
            lexed.at(lexed.size()-1).element = op;
            i++;
            break;
        case '!':
            lexed.resize(lexed.size()+1);
            lexed.at(lexed.size()-1).math_op = '!';
            lexed.at(lexed.size()-1).element = op;
            i++;
            break;
        case '^':
            lexed.resize(lexed.size()+1);
            lexed.at(lexed.size()-1).math_op = '^';
            lexed.at(lexed.size()-1).element = op;
            i++;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            lexed.resize(lexed.size()+1);
            lexed.at(lexed.size()-1).element = no;
            while (i < str.length() && str.at(i) >= '0' && str.at(i) <= '9')
            {
                lexed.at(lexed.size()-1).number.number.append(1, str.at(i));
                i++;
            }
            break;
        default:
            i++;
            break;
        }
    }

    /*CHECK THE VALIDITY OF THE RESULT - PHASE 1*/
    lex_check(result);

    /*STANDARDIZE THE RESULT*/
    lex_standardize(result);

    /*CHECK THE VALIDITY OF THE RESULT - PHASE 2*/
    lex_validate(result);

    return result;
}


/****************************************************
 * FUNCTION: lex_check() - Check the validity of	*
 * the lexical analysed result	- *PHASE 1*			*
 * FEATURE:											*
 * 		- Check for unbalanced parentheses			*
 * 		- Integrate the sign into the number when	*
 * 		detected									*
 ****************************************************/
void lex::lex_check(vector<lex_sequence> &lx)
{
    int i = 0;
    int open_paren = 0, close_paren = 0;
    while (i < (signed)lx.size())
    {
        //Check for unbalanced parentheses' pairs.
        if (lx.at(i).element == op && lx.at(i).math_op == '(')
            open_paren++;
        else if (lx.at(i).element == op && lx.at(i).math_op == ')')
            close_paren++;

        //Check for negative or positive number with sign
        if (lx.at(i).element == no)
            if (i - 1 >= 0 && (lx.at(i-1).math_op == '+' || lx.at(i-1).math_op == '-'))
                if ( (i - 2 >= 0 && lx.at(i-2).element == op &&lx.at(i-2).math_op != '!') || i - 2 < 0)
                {
                    if (lx.at(i-1).math_op == '-')
                    {
                        if (lx.at(i-2).math_op == '^') {
                            lx.at(i).number.sign = -1;
                            lx.erase(lx.begin()+i-1);
                        } else {
                            lx.at(i-1).number.sign = -1;
                            lx.at(i-1).number.number = "1";
                            lx.at(i-1).element = no;
                            lx.at(i-1).math_op = '\0';
                            lex_sequence temp;
                            temp.element = op;
                            temp.math_op = '*';
                            lx.insert(lx.begin()+i,temp);
                        }
                    }
                }
        i++;
    }

    //Report error after checking
    if (open_paren != close_paren)
    {
        lx.resize(0);
        display_error(1);
    }
}

/****************************************************
 * FUNCTION: lex_standardize()						*
 * USE: Standardize the lexically analysed result	*
 * i.e. Adding multiply operator when meeting 		*
 * implicated condition								*
 ****************************************************/
void lex::lex_standardize(vector<lex_sequence> &lx)
{
    int i = 0;
    while (i < (signed)lx.size())
    {
        if (
                // 1) A number beside an open paren
                (lx.at(i).element == no && i + 1 < (signed)lx.size() && lx.at(i+1).math_op == '(') ||
                // 2)  Close paren or factorial operator beside open paren or a number
                ((lx.at(i).math_op == ')' || lx.at(i).math_op == '!') && i + 1 < (signed)lx.size()
                    && (lx.at(i+1).math_op == '(' || lx.at(i+1).element == no)) ||
                // 3) Minus and plus beside open paren and after another operators not parentheses or power operator
                ((lx.at(i).math_op == '+' || lx.at(i).math_op == '-')
                    && i + 1 < (signed)lx.size() && lx.at(i+1).math_op == '('
                 && ((i - 1 >= 0 && lx.at(i-1).element == op && lx.at(i-1).math_op > ')' && lx.at(i-1).math_op != '^') || i - 1 < 0))
            )
        {
            //Add multiply operator
            lex_sequence omultiply;
            omultiply.element = op;
            omultiply.math_op = '*';
            lx.insert(lx.begin()+i+1, 1, omultiply);

            //For condition 3: replace the operator by a number (1 or -1)
            if (lx.at(i).math_op == '+'){
                lx.at(i).element = no;
                lx.at(i).math_op = '\0';
                lx.at(i).number = 1;
            }
            if (lx.at(i).math_op == '-') {
                lx.at(i).element = no;
                lx.at(i).math_op = '\0';
                lx.at(i).number = -1;
            }
        }
        i++;
    }
}

/****************************************************
 * FUNCTION: lex_validate() - Check the validity of	*
 * the lexical analysed result	- *PHASE 2*			*
 * FEATURE:											*
 * 		- Check for unbalanced parentheses			*
 * 		- Integrate the sign into the number when	*
 * 		detected									*
 ****************************************************/
void lex::lex_validate(vector<lex_sequence> &lx)
{

    /*VALIDITY CHECK*/
    int i = 0;
    while (i < (signed)lx.size())
    {
        switch (lx.at(i).element) {
        case no:
            //At the left side of a number must be an operator not close paren
            if (!((i - 1 < 0)||(i - 1 >= 0 && lx.at(i-1).element == op && lx.at(i-1).math_op != ')')))
            {   lx.resize(0); display_error(2);     }
            //At the right side of a number must be an operator not open paren
            if (!((i + 1 >= (signed)lx.size())||(i + 1 < (signed)lx.size() && lx.at(i+1).element == op && lx.at(i+1).math_op != '(')))
            {   lx.resize(0); display_error(2);     }
            break;
        case op:
            switch (lx.at(i).math_op) {
            case '+':
            case '-':
            case '*':
            case '/':
                //They must not be at the first position of the expression
                //and at their left must be a number, a right parentheses or a factorial operator
                if (!(i-1 >= 0 && (lx.at(i-1).element == no || lx.at(i-1).math_op == ')' || lx.at(i-1).math_op == '!')))
                {    lx.resize(0); display_error(3);    }
                //They must not be at the last position of the expression
                //and at their right must be a number of a left parentheses
                if (!(i + 1 < (signed) lx.size() && (lx.at(i+1).element == no || lx.at(i+1).math_op == '(')))
                {    lx.resize(0); display_error(3);    }
                break;
            case '!':
                //It must not be at the first position of the expression
                //and at its left must be a number or a close parentheses
                if (!(i-1 >= 0 && (lx.at(i-1).element == no || lx.at(i-1).math_op == ')')))
                {    lx.resize(0); display_error(3);    }
                //It can be at the last position of the expression,
                //or else, at its right must be an operator
                if (!( i + 1 >= (signed)lx.size() || (i + 1 < (signed)lx.size() && lx.at(i+1).element == op)))
                {    lx.resize(0); display_error(3);    }
                break;
            case '(':
                //It can be at the first position of the expression,
                //or else, at its left must be an operator (not right parentheses)
                if (!((i - 1 < 0)||(i - 1 >= 0 && lx.at(i-1).element == op && lx.at(i-1).math_op != ')')))
                {    lx.resize(0); display_error(4);    }
                //It can not be at the two last positions of the expression,
                //and at their right must be a number of another left parentheses)
                if (!(i < (signed)lx.size() - 2 && (lx.at(i+1).element == no || lx.at(i+1).math_op == '(')))
                {    lx.resize(0); display_error(4);    }
                break;
            case ')':
                //It can not be at the first position of the expression,
                //and at its left must be a number, another right parentheses or a factorial operator
                if (!(i - 1 > 0 && (lx.at(i-1).element == no || lx.at(i-1).math_op == ')' || lx.at(i-1).math_op == '!')))
                {    lx.resize(0); display_error(5);    }
                //It can be at the last position of the expression,
                //ore else, at its right must be an operator (not left parentheses)
                if (!((i + 1 >= (signed)lx.size()) || (i + 1 < (signed)lx.size()  && lx.at(i+1).element == op && lx.at(i+1).element != '(')))
                {    lx.resize(0); display_error(5);    }
                break;
            case '^':
                //It can not be at the first position of the expression,
                //and at its left must be a number, an open parentheses or factorial operator
                if (!(i - 1 >= 0 && (lx.at(i-1).element == no || lx.at(i-1).math_op == ')' || lx.at(i-1).math_op == '!')))
                {    lx.resize(0); display_error(6);     }
                //It can not be at the last position of the expression,
                //and at its right must be a number or a close parentheses
                if (!(i + 1 < (signed)lx.size() && (lx.at(i+1).element == no || lx.at(i+1).math_op == '(')))
                {    lx.resize(0); display_error(6);     }
                break;
            default:
                break;
            };
            break;
        }
        i++;
    }

}

/********************************************************
 * FUNCTION: evaluate()									*
 * USE TO: 	evaluate an mathematical expression and		*
 * 			return the result							*
 * PARAMETER: a mathematical expression in string form	*
 * RETURN: the math result								*
 ********************************************************/
void lex::evaluate()
{
    vector<lex_sequence> lx = lexed_result;	//Break string into tokens, check and standardize them
    //Error handling
    if (lx.size() == 0)
    {
        result.sign = 0;
        return;
    }
    stack<big_num> no_stack;		//Number stack
    stack<char> op_stack;			//Operator stack
    unsigned int i = 0;

    while (i < lx.size())
    {
        //Read a token
        switch (lx.at(i).element) {
        case op:
            switch (lx.at(i).math_op) {
            //If it's a left parentheses, push it into op_stack
            case '(':
                op_stack.push('(');
                i++;
                break;

            // If it's an operator, compare its priority with the op_stack's top operator's priority
            // =>if smaller or equal, pop that top operator out and do the math with two top numbers
            // in the number stack, then push back the result into the stack.
            // =>do the operation until op_stack is empty or encouter smaller priority operator, then push
            // this operator into op_stack.
            case '+':
            case '-':
            case '*':
            case '/':
            case '!':
            case '^':
                while (op_stack.size() > 0 && op_stack.top() != '(' && (precedence(op_stack.top()) <= precedence(lx.at(i).math_op)) ) {
                    if ((lx.at(i).math_op == '!' || lx.at(i).math_op == '^') && op_stack.top() == '^')
                        break;
                    do_math(op_stack.top(),no_stack);
                    op_stack.pop();
                }
                op_stack.push(lx.at(i).math_op);
                i++;
                break;
            // If it's an right parentheses, pop out all operators in operator stack until encounter
            // a left parentheses.
            case ')':
                while (op_stack.top() != '(') {
                    do_math(op_stack.top(), no_stack);
                    op_stack.pop();
                }
                op_stack.pop();
                i++;
                break;
            default:
                break;
            };
            break;
        // If it's a number, push it into the number stack
        case no:
            no_stack.push(lx.at(i).number);
            i++;
            break;
        default:
            i++;
            break;
        }
    }

    //When there's no token left, pop out all operators from the op_stack
    while (op_stack.size() != 0) {
        do_math(op_stack.top(), no_stack);
        op_stack.pop();
    }

    //The result is the only number left in number stack
    result = no_stack.top();
    no_stack.pop();
}

/********************************************************
 *              Display error (emit SIGNALS)            *
 ********************************************************/
void lex::display_error(int error_code)
{
    QString info = "<span style='font-weight:bold;color:red;'>";
    switch (error_code)
    {
    case 1:
        info.append(tr("Unbalanced parentheses!!!"));
        break;
    case 2:
        info.append(tr("Error beside a number!!!"));
        break;
    case 3:
        info.append(tr("Error beside an operator!!!"));
        break;
    case 4:
        info.append(tr("Error beside an open parentheses!!!"));
        break;
    case 5:
        info.append(tr("Error beside a close parantheses!!!"));
        break;
    case 6:
        info.append(tr("Error beside a power operator"));
        break;
    default:
        break;
    }
    info.append("</span>");
    emit error(info);
}

/***************************************************
 * FUNCTION: standardized()                        *
 *          - return the standardized expression   *
 ***************************************************/
QString lex::standardized()
{
    QString output = "";
    QString temp;
    for (int i = 0; i < (signed) lexed_result.size(); i++)
    {
        if (lexed_result.at(i).element == op)
            output.append(lexed_result.at(i).math_op);
        if (lexed_result.at(i).element == no)
        {
            if (lexed_result.at(i).number.sign == -1)
                output.append('-');
            temp = QString::fromStdString(lexed_result.at(i).number.number);
            output.append(temp);
        }
        output.append(' ');
    }
    return output;
}

/***************************************************
 * FUNCTION: execute()                             *
 *          - excute necessary steps               *
 ***************************************************/
void lex::excute()
{
    evaluate();
    if (!(result.sign == 0))
    {
        QString out = QString::fromStdString(result.number);
        if (result.sign == -1)
            out.insert(0,'-');
        emit result_str(out);

        //Enable view standard expression after done calculation
        emit success();
    }
}
