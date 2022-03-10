#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stack>
using namespace std;



typedef struct Exp_TreeNode
{
    char ch[100]; //符号或标识符信息
    struct Exp_TreeNode* left;
    struct Exp_TreeNode* right;
} *Exp_ptr;


typedef struct AST {
    int tag;  //结点的类型
    union //联合体
    {
        int type;    //变量类型（如果该结点是变量类型结点的话）
        char info[100];  //结点的信息
        Exp_ptr t; //表达式树
    };
    AST * child1, * child2, * child3;
}AST;


char token_text[10000];
char token_text0[10000];
int token_code[10000];
int w = 0, w0 = 0;
int line_number = 0;
int error = 0;
int fun_mark = 0;
FILE* fp;
FILE* f_out;

char key_word_table[100][100] = {"void","char","short","int","long","float","double","sizeof",\
                                "signed","unsigned","struct","union","enum","typedef",\
                                "if","else","for","while","do","switch","case","fault","goto","break","continue","return" };

enum Token_kind
{
    ERROR_TOKEN, IDENT, VOID, CHAR, SHORT, INT, LONG, FLOAT, DOUBLE, SIZEOF, SIGNED, UNSIGNED, STRUCT, UNION, ENUM, TYPEDEF, \
    IF, ELSE, FOR, WHILE, DO, SWITCH, CASE, FAULT, GOTO, BREAK, CONTINUE, RETURN, CHAR_CONST, INT_CONST, FLOAT_CONST, ARRAY,\
    EQ, ASSIGN, PLUS, PLUSPLUS, MINUS, MINUSMINUS, MULTIPLY, DIVIDE, PLUSEQ,  MINUSEQ, MULTIPLYEQ, DIVIDEEQ, MOD, \
    GREATER_THAN, GREATER_THAN_OR_EQUAL_TO, LESS_THAN, LESS_THAN_OR_EQUAL_TO, BITWISE_AND, BITWISE_OR, BITWISE_NOT, \
    BITWISE_LSHIFT, BITWISE_RSHIFT, AND, OR, NOT, LP, RP, XOR, LINE_COMMENT, BLOCK_COMMENT_BEGIN, BLOCK_COMMENT_END, \
    LBRACKET, RBRACKET, LBRACE, RBRACE, SEMI, COMMA, DEFINE, INCLUDE, FUNCTION, LONG_CONST
}token_kind;



AST* statement(void);
AST* statement_list(void);
AST* ExtVarList(void);
AST* ExtVarDef(int w0);
AST* parameterList();
AST* parameterDef();
AST* FunctionUseParameterList();
AST* FunctionUseParameterDef();

void clear(AST* &t);
void clear_exp(Exp_ptr &t);

int is_keyword(char text[])
{
    for (int i = 0; i < 26; i++)
    {
        if (!strcmp(text, key_word_table[i])) return i;
    }
    return -1;
}

int is_int_or_float_or_long(char text[])
{
    if (text[strlen(text) - 1] == 'L') return LONG;
    for (int i = 0; i < strlen(text); i++)
    {
        if (text[i] == '.') return FLOAT;
    }
    return INT;
}

char* Token_to_Sign(int a)
{
    char *str=(char *)malloc(sizeof(char)*100);
    switch (a)
    {
        case EQ:
            strcpy(str, "==");
            return str;
        case ASSIGN:
            strcpy(str, "=");
            return str;
        case PLUS:
            strcpy(str, "+");
            return str;
        case MINUS:
            strcpy(str, "-");
            return str;
        case MULTIPLY:
            strcpy(str, "*");
            return str;
        case DIVIDE:
            strcpy(str, "/");
            return str;
        case GREATER_THAN:
            strcpy(str, ">");
            return str;
        case LESS_THAN:
            strcpy(str, "<");
            return str;
        case NOT:
            strcpy(str, "!=");
            return str;
        case AND:
            strcpy(str, "&&");
            return str;
        case OR:
            strcpy(str, "||");
            return str;
        case MOD:
            strcpy(str, "%");
            return str;
        case IDENT:
            strcpy(str, "标识符");
            return str;
        case LP:
            strcpy(str, "左小括号");
            return str;
        case RP:
            strcpy(str, "右小括号");
            return str;
        case LBRACE:
            strcpy(str, "左大括号");
            return str;
        case RBRACE:
            strcpy(str, "右大括号");
            return str;
        case FUNCTION:
            strcpy(str, "函数");
            return str;
        case COMMA:
            strcpy(str, "逗号");
            return str;
        case SEMI:
            strcpy(str, "分号");
            return str;
        case FOR:
            strcpy(str, "for");
            return str;
        case IF:
            strcpy(str, "if");
            return str;
        case ELSE:
            strcpy(str, "else");
            return str;
        case WHILE:
            strcpy(str, "while");
            return str;
        case BREAK:
            strcpy(str, "break");
            return str;
        case CONTINUE:
            strcpy(str, "continue");
            return str;
        case RETURN:
            strcpy(str, "return");
            return str;
        case INCLUDE:
            strcpy(str, "include");
            return str;
        case DEFINE:
            strcpy(str, "define");
            return str;
        case LINE_COMMENT:
            strcpy(str, "行注释");
            return str;
        case BLOCK_COMMENT_BEGIN:
            strcpy(str, "块注释");
            return str;
        case INT:
            strcpy(str, "int");
            return str;
        case CHAR:
            strcpy(str, "char");
            return str;
        case LONG:
            strcpy(str, "long");
            return str;
        case SHORT:
            strcpy(str, "short");
            return str;
        case FLOAT:
            strcpy(str, "float");
            return str;
        case DOUBLE:
            strcpy(str, "double");
            return str;
        case INT_CONST:
            strcpy(str, "整型常量");
            return str;
        case FLOAT_CONST:
            strcpy(str, "浮点常量");
            return str;
        case LONG_CONST:
            strcpy(str, "长整常量");
            return str;
        case ARRAY:
            strcpy(str, "数组");
            return str;
    }
}

char* Var_name(int a)
{
    char* str = (char*)malloc(sizeof(char) * 100);
    switch (a)
    {
    case INT:
        strcpy(str, "int");
        return str;
    case CHAR:
        strcpy(str, "char");
        return str;
    case LONG:
        strcpy(str, "long");
        return str;
    case FLOAT:
        strcpy(str, "float");
        return str;
    case DOUBLE:
        strcpy(str, "double");
        return str;
    case SHORT:
        strcpy(str, "short");
        return str;
    }
}



int  gettoken(void) {  // fp也可以作为全局变量，该函数不使用参数
    token_text[0] = '\0';  //重置tokentext
    char c;
    while ((c = fgetc(fp)) == ' ' || c == '\n')//过滤掉空白符号，如果考虑报错位置，
    {
        if (c == '\n')                         //对回车需要单独处理，每次回车，设置一个行数计数器加1
        {
            line_number++;
        }
    }

    if (isupper(c) || islower(c))
    {
        do {
            char temp[10];
            temp[0] = c; temp[1] = '\0';
            strcat(token_text, temp);
        } while (isalnum((c = fgetc(fp)))|| c == '_');//拼标识符串
        ungetc(c, fp);   //退回多读的字符到文件输入缓冲区 标识符可能是关键字，需要判定并返回对应种类码，符号串在token_text中可以将所有关键字做成一个查找表，当标识符和某个关键字相等时，返回关键字的种类编码，否则返回IDENT
        if (is_keyword(token_text) != -1) return is_keyword(token_text) + 2;
        else if ((c = fgetc(fp)) == '[')
        {
            do {
                char temp[10];
                temp[0] = c; temp[1] = '\0';
                strcat(token_text, temp);
            } while ((c = fgetc(fp)) != ']');
            char temp[10];
            temp[0] = ']'; temp[1] = '\0';
            strcat(token_text, temp);
            return ARRAY;
        }
        else
        {
            ungetc(c, fp);
            if ((c = fgetc(fp)) == '(')
            {
                return FUNCTION;
            }
            ungetc(c, fp);
            return IDENT;
        }
    }

    if (c == '_')
    {
        do {
            char temp[10];
            temp[0] = c; temp[1] = '\0';
            strcat(token_text, temp);
        } while (isalnum((c = fgetc(fp))) || c == '_');//拼标识符串
        ungetc(c, fp);   //退回多读的字符到文件输入缓冲区 标识符可能是关键字，需要判定并返回对应种类码，符号串在token_text中可以将所有关键字做成一个查找表，当标识符和某个关键字相等时，返回关键字的种类编码，否则返回IDENT
        if ((c = fgetc(fp)) == '[')
        {
            do {
                char temp[10];
                temp[0] = c; temp[1] = '\0';
                strcat(token_text, temp);
            } while ((c = fgetc(fp)) != ']');
            char temp[10];
            temp[0] = ']'; temp[1] = '\0';
            strcat(token_text, temp);

            return ARRAY;
        }
        else
        {
            ungetc(c, fp);
            if ((c = fgetc(fp)) == '(')
            {
                return FUNCTION;
            }
            ungetc(c, fp);
            return IDENT;
        }
    }

    if (isdigit(c)) 
    {
        int mark = 1;
        do{
            if (c == '.' && mark == 1) mark = 0;
            else if (c == '.' && mark == 0) return ERROR_TOKEN;

            if (c == 'L' && mark == 1) mark = 0;
            else if (c == 'L' && mark == 0) return ERROR_TOKEN;

            char temp[10];
            temp[0] = c; temp[1] = '\0';
            strcat(token_text, temp);

        } while (isdigit((c = fgetc(fp))) || (token_text[0] == '0' && c == 'x' && strlen(token_text) == 1) || (c == '.')|| (c == 'L')); //拼数字串
        ungetc(c, fp);   //退回多读的字符   数字串在token_text中，返回INT_CONST。
        if (is_int_or_float_or_long(token_text) == INT)
            return INT_CONST;
        else if (is_int_or_float_or_long(token_text) == LONG)
            return LONG_CONST;
        else return FLOAT_CONST;
    }
    if (c == '#')
    {
        for (int j = 0; j < 6; j++)
        {
            c = fgetc(fp);
            char temp[10];
            temp[0] = c; temp[1] = '\0';
            strcat(token_text, temp);
        }
        if (!strcmp((token_text), "define"))
        {
            while ((c = fgetc(fp)) != '\n')
            {
                char temp[10];
                temp[0] = c; temp[1] = '\0';
                strcat(token_text, temp);
            }
            line_number++;
            return DEFINE;  
        }

        c = fgetc(fp);
        char temp[10];
        temp[0] = c; temp[1] = '\0';
        strcat(token_text, temp);
        if (!strcmp((token_text), "include"))
        {
            while ((c = fgetc(fp)) != '\n')
            {
                char temp[10];
                temp[0] = c; temp[1] = '\0';
                strcat(token_text, temp);
            }
            line_number++;
            return INCLUDE;
        }
        return ERROR_TOKEN;
    }

    switch(c)
    {

        case '=':  
            c = fgetc(fp);
            if (c == '=')
                return EQ;//返回相等运算符编码EQ；
            ungetc(c,fp);
            return ASSIGN;
        case '+':
            c = fgetc(fp);
            if (c == '+')
                return PLUSPLUS;//返回自增运算符编码；
            if (c == '=')
                return PLUSEQ;
            ungetc(c, fp);
            return PLUS;
        case '-':
            c = fgetc(fp);
            if (c == '-')
                return MINUSMINUS;//返回自减运算符编码；
            if (c == '=')
                return MINUSEQ;
            ungetc(c, fp);
            return MINUS;
        case '*':
            c = fgetc(fp);
            if (c == '=')
                return MULTIPLYEQ;
            else if (c == '/')
                return BLOCK_COMMENT_END;
            ungetc(c, fp);
            return MULTIPLY;
        case '/':
            c = fgetc(fp);
            if (c == '=')
                return DIVIDEEQ;
            else if (c == '/')
                return LINE_COMMENT;
            else if (c == '*')
                return BLOCK_COMMENT_BEGIN;
            ungetc(c, fp);
            return DIVIDE;
        case '>':
            c = fgetc(fp);
            if (c == '=')
                return GREATER_THAN_OR_EQUAL_TO;
            else if (c == '>')
                return BITWISE_RSHIFT;
            ungetc(c, fp);
            return GREATER_THAN;
        case '<':
            c = fgetc(fp);
            if (c == '=')
                return LESS_THAN_OR_EQUAL_TO;
            else if (c == '<')
                return BITWISE_LSHIFT;
            ungetc(c, fp);
            return LESS_THAN;
        case '!':
            c = fgetc(fp);
            if (c == '=')
                return NOT;
            ungetc(c, fp);
            return BITWISE_NOT;
        case '&':
            c = fgetc(fp);
            if (c == '&')
                return AND;
            ungetc(c, fp);
            return BITWISE_AND;
        case '|':
            c = fgetc(fp);
            if (c == '|')
                return OR;
            ungetc(c, fp);
            return BITWISE_OR;
        case '%':
            return MOD;
        case '^':
            return XOR;
        case '(':
            return LP;
        case ')':
            return RP;
        case '[':
            return LBRACKET;
        case ']':
            return RBRACKET;
        case '{':
            return LBRACE;
        case '}':
            return RBRACE;
        case ',':
            return COMMA;
        case ';':
            return SEMI;
        default:  
            if (feof(fp))   return EOF;
            else return ERROR_TOKEN;    //报错；错误符号
    }
}


void block_comment_judge()
{
    if (w == BLOCK_COMMENT_BEGIN)
    {
        char c;
        while (c = fgetc(fp) != '*');
        if (c = fgetc(fp) == '/')
        {
            w = gettoken();
            return;
        }
        else
        {
            block_comment_judge();
        }
    }
    return;
}



Exp_ptr Exp(int endsym1, int endsym2, int w)   //表达式结束符号endsym可以是分号，如表达式语句，可以是反小括号，作为条件时使用                           
{  //调用该算法时，在调用者已经读入了第一个单词在w中
    char precede[200][200];
    precede[PLUS][PLUS] = '>';        precede[PLUS][MINUS] = '>';        precede[PLUS][MULTIPLY] = '<';        precede[PLUS][DIVIDE] = '<';        precede[PLUS][LP] = '<';        precede[PLUS][RP] = '>';        precede[PLUS][GREATER_THAN] = '>';        precede[PLUS][LESS_THAN] = '>';         precede[PLUS][EQ] = '>';        precede[PLUS][NOT] = '>';        precede[PLUS][AND] = '>';        precede[PLUS][OR] = '>';        precede[PLUS]['#'] = '>';        precede[PLUS][MOD] = '<';
    precede[MINUS][PLUS] = '>';       precede[MINUS][MINUS] = '>';       precede[MINUS][MULTIPLY] = '<';       precede[MINUS][DIVIDE] = '<';       precede[MINUS][LP] = '<';       precede[MINUS][RP] = '>';       precede[MINUS][GREATER_THAN] = '>';       precede[PLUS][LESS_THAN] = '>';         precede[MINUS][EQ] = '>';       precede[MINUS][NOT] = '>';       precede[MINUS][AND] = '>';       precede[MINUS][OR] = '>';       precede[MINUS]['#'] = '>';       precede[MINUS][MOD] = '<';
    precede[MULTIPLY][PLUS] = '>';    precede[MULTIPLY][MINUS] = '>';    precede[MULTIPLY][MULTIPLY] = '>';    precede[MULTIPLY][DIVIDE] = '>';    precede[MULTIPLY][LP] = '<';    precede[MULTIPLY][RP] = '>';    precede[MULTIPLY][GREATER_THAN] = '>';    precede[MULTIPLY][LESS_THAN] = '>';     precede[MULTIPLY][EQ] = '>';    precede[MULTIPLY][NOT] = '>';    precede[MULTIPLY][AND] = '>';    precede[MULTIPLY][OR] = '>';    precede[MULTIPLY]['#'] = '>';    precede[MULTIPLY][MOD] = '>';
    precede[DIVIDE][PLUS] = '>';      precede[DIVIDE][MINUS] = '>';      precede[DIVIDE][MULTIPLY] = '>';      precede[DIVIDE][DIVIDE] = '>';      precede[DIVIDE][LP] = '<';      precede[DIVIDE][RP] = '>';      precede[DIVIDE][GREATER_THAN] = '>';      precede[DIVIDE][LESS_THAN] = '>';       precede[DIVIDE][EQ] = '>';      precede[DIVIDE][NOT] = '>';      precede[DIVIDE][AND] = '>';      precede[DIVIDE][OR] = '>';      precede[DIVIDE]['#'] = '>';      precede[DIVIDE][MOD] = '>';
    precede[LP][PLUS] = '<';          precede[LP][MINUS] = '<';          precede[LP][MULTIPLY] = '<';          precede[LP][DIVIDE] = '<';          precede[LP][LP] = '<';          precede[LP][RP] = '=';          precede[LP][GREATER_THAN] = '>';          precede[LP][LESS_THAN] = '>';           precede[LP][EQ] = '>';          precede[LP][NOT] = '>';          precede[LP][AND] = '>';          precede[LP][OR] = '>';          precede[LP]['#'] = '>';          precede[LP][MOD] = '<';
    precede[RP][PLUS] = '>';          precede[RP][MINUS] = '>';          precede[RP][MULTIPLY] = '>';          precede[RP][DIVIDE] = '>';          precede[RP][LP] = '>';                                          precede[RP][GREATER_THAN] = '<';          precede[RP][LESS_THAN] = '>';           precede[RP][EQ] = '>';          precede[RP][NOT] = '>';          precede[RP][AND] = '>';          precede[RP][OR] = '>';          precede[RP]['#'] = '>';          precede[RP][MOD] = '>';
    precede[ASSIGN][PLUS] = '<';      precede[ASSIGN][MINUS] = '<';      precede[ASSIGN][MULTIPLY] = '<';      precede[ASSIGN][DIVIDE] = '<';      precede[ASSIGN][LP] = '<';                                      precede[ASSIGN][GREATER_THAN] = '<';      precede[ASSIGN][LESS_THAN] = '<';       precede[ASSIGN][EQ] = '<';      precede[ASSIGN][NOT] = '<';      precede[ASSIGN][AND] = '<';      precede[ASSIGN][OR] = '<';      precede[ASSIGN]['#'] = '>';      precede[ASSIGN][MOD] = '<';     precede[ASSIGN][ASSIGN] = '<';
    precede[GREATER_THAN][PLUS] = '<';precede[GREATER_THAN][MINUS] = '<';precede[GREATER_THAN][MULTIPLY] = '<';precede[GREATER_THAN][DIVIDE] = '<';precede[GREATER_THAN][LP] = '<';precede[GREATER_THAN][RP] = '>';precede[GREATER_THAN][GREATER_THAN] = '>';precede[GREATER_THAN][LESS_THAN] = '>'; precede[GREATER_THAN][EQ] = '>';precede[GREATER_THAN][NOT] = '>';precede[GREATER_THAN][AND] = '>';precede[GREATER_THAN][OR] = '>';precede[GREATER_THAN]['#'] = '>';precede[GREATER_THAN][MOD]= '<';
    precede[LESS_THAN][PLUS] = '<';   precede[LESS_THAN][MINUS] = '<';   precede[LESS_THAN][MULTIPLY] = '<';   precede[LESS_THAN][DIVIDE] = '<';   precede[LESS_THAN][LP] = '<';   precede[LESS_THAN][RP] = '>';   precede[LESS_THAN][GREATER_THAN] = '>';   precede[LESS_THAN][LESS_THAN] = '>';    precede[LESS_THAN][EQ] = '>';   precede[LESS_THAN][NOT] = '>';   precede[LESS_THAN][AND] = '>';   precede[LESS_THAN][OR] = '>';   precede[LESS_THAN]['#'] = '>';   precede[LESS_THAN][MOD] = '<';
    precede[EQ][PLUS] = '<';          precede[EQ][MINUS] = '<';          precede[EQ][MULTIPLY] = '<';          precede[EQ][DIVIDE] = '<';          precede[EQ][LP] = '<';          precede[EQ][RP] = '>';          precede[EQ][GREATER_THAN] = '<';          precede[EQ][LESS_THAN] = '<';           precede[EQ][EQ] = '>';          precede[EQ][NOT] = '>';          precede[EQ][AND] = '>';          precede[EQ][OR] = '>';          precede[EQ]['#'] = '>';          precede[EQ][MOD] = '<';
    precede[NOT][PLUS] = '<';         precede[NOT][MINUS] = '<';         precede[NOT][MULTIPLY] = '<';         precede[NOT][DIVIDE] = '<';         precede[NOT][LP] = '<';         precede[NOT][RP] = '>';         precede[NOT][GREATER_THAN] = '<';         precede[NOT][LESS_THAN] = '<';          precede[NOT][EQ] = '>';         precede[NOT][NOT] = '>';         precede[NOT][AND] = '>';         precede[NOT][OR] = '>';         precede[NOT]['#'] = '>';         precede[NOT][MOD] = '<';
    precede[AND][PLUS] = '<';         precede[AND][MINUS] = '<';         precede[AND][MULTIPLY] = '<';         precede[AND][DIVIDE] = '<';         precede[AND][LP] = '<';         precede[AND][RP] = '>';         precede[AND][GREATER_THAN] = '<';         precede[AND][LESS_THAN] = '<';          precede[AND][EQ] = '>';         precede[AND][NOT] = '<';         precede[AND][AND] = '>';         precede[AND][OR] = '>';         precede[AND]['#'] = '>';         precede[AND][MOD] = '<';
    precede[OR][PLUS] = '<';          precede[OR][MINUS] = '<';          precede[OR][MULTIPLY] = '<';          precede[OR][DIVIDE] = '<';          precede[OR][LP] = '<';          precede[OR][RP] = '>';          precede[OR][GREATER_THAN] = '<';          precede[OR][LESS_THAN] = '<';           precede[OR][EQ] = '>';          precede[OR][NOT] = '<';          precede[OR][AND] = '<';          precede[OR][OR] = '>';          precede[OR]['#'] = '>';          precede[OR][MOD] = '<';
    precede[MOD][PLUS] = '>';         precede[MOD][MINUS] = '>';         precede[MOD][MULTIPLY] = '>';         precede[MOD][DIVIDE] = '>';         precede[MOD][LP] = '<';         precede[MOD][RP] = '>';         precede[MOD][GREATER_THAN] = '>';         precede[MOD][LESS_THAN] = '>';          precede[MOD][EQ] = '>';         precede[MOD][NOT] = '>';         precede[MOD][AND] = '>';         precede[MOD][OR] = '>';         precede[MOD]['#'] = '>';         precede[MOD][MOD] = '>';


    precede['#'][PLUS] = '<';         precede['#'][MINUS] = '<';         precede['#'][MULTIPLY] = '<';         precede['#'][DIVIDE] = '<';         precede['#'][LP] = '<';                                         precede['#'][GREATER_THAN] = '<';         precede['#'][LESS_THAN] = '<';          precede['#'][EQ] = '<';         precede['#'][NOT] = '<';         precede['#'][AND] = '<';         precede['#'][OR] = '<';         precede['#']['#'] = '=';         precede['#'][MOD] = '<';        precede['#'][ASSIGN] = '<';

    
    stack<char> op;                 //定义运算符栈 op; 并初始化，将起止符#入栈
    stack<Exp_ptr> opn;              //定义操作数栈opn，元素是结点的指针
    op.push('#');
    //int error = 0;                   //错误标记error设置为0
    //int w = gettoken();
        
    while ((w != '#'|| op.top() != '#') && !error)  //当运算符栈栈顶不是起止符号，并没有错误时
    {
        if (w == IDENT || w == INT_CONST || w == FLOAT_CONST)//w是标识符或常数等操作数时
        {
            Exp_ptr p = (Exp_ptr)malloc(sizeof(Exp_TreeNode));//根据w生成一个结点，结点指针进栈opn
            strcpy(p->ch, token_text);
            p->left = p->right = NULL;
            opn.push(p);
            w = gettoken();
        }
        else if (w == FUNCTION)
        {
            Exp_ptr p2 = (Exp_ptr)malloc(sizeof(Exp_TreeNode));//根据w生成一个结点
            Exp_ptr p1 = (Exp_ptr)malloc(sizeof(Exp_TreeNode));
            strcpy(p2->ch, token_text);
            p2->left = p2->right = NULL;
            p1 = opn.top();
            p1->left = p2;
            fun_mark = 1;
            return p1;
        }
        else if (w >= EQ && w <= RP && w != endsym1 && w != endsym2)
        {
            switch (precede[op.top()][w])
            {
                case '<':
                    op.push(w);
                    w = gettoken();
                    break;
                case '=':
                {
                    if (op.empty())  error++;
                    char t = op.top();
                    op.pop();
                    w = gettoken();
                    break;   //去括号
                }
                case '>':
                {
                    Exp_ptr t1 = NULL, t2 = NULL;
                    if (opn.empty()) error++;
                    else
                    {
                        t2 = opn.top();
                        opn.pop();
                    }

                    if (opn.empty()) error++;
                    else
                    {
                        t1 = opn.top();
                        opn.pop();
                    }
                    if (op.empty()) error++;
                    char t = op.top();
                    op.pop();
                    Exp_ptr t3 = (Exp_ptr)malloc(sizeof(Exp_TreeNode));  //根据运算符栈退栈得到的运算符t和操作数的结点指针t1和t2
                    strcpy(t3->ch, Token_to_Sign(t));
                    t3->left = t1;
                    t3->right = t2;                                     //完成建立生成一个运算符的结点，结点指针进栈opn
                    opn.push(t3);
                    break;
                }
                default:
                    if (w == endsym1 || w == endsym2) w = '#'; //遇到结束标记），w被替换成#
                    else error = 1;
                }
            }
            else if (w == endsym1 || w == endsym2) w = '#';//遇到结束标记分号，w被替换成#
            else error = 1;
    }
    if (opn.size()==1 && op.top() == '#'  && error == 0)
        return opn.top();    //成功返回表达式语法树的根结点指针
    else
    {
        printf("line%d ", line_number);
        printf("表达式有错！");
        return NULL;                  //表达式分析有错
    }
}


AST* complex_statement(void)
{
    if (error != 0) return NULL;
    AST* root = (AST*)malloc(sizeof(AST)); //生成复合语句结点；注意其中局部变量说明和语句序列都可以为空
    root->child1 = root->child2 = root->child3 = NULL; root->tag = 4; //初始化
    strcpy(root->info, "复合语句结点");
    w = gettoken();
    w0 = w;

    if (w == LINE_COMMENT)
    {
        char c;
        while ((c = fgetc(fp)) != '\n' && c != EOF);
        line_number++;
        w = gettoken();
    }
    block_comment_judge();


    if (w == INT || w == SHORT || w == CHAR || w == LONG || w == FLOAT || w == DOUBLE)//w是类型关键字
    {
        w0 = w;
        w = gettoken();
        strcpy(token_text0, token_text);
        w = gettoken();
        root->child1 = ExtVarDef(w0);//调用处理局部变量说明序列子程序，得到返回的子树根结点作为root的第一个孩子 
        if (error != 0) return NULL;
    }
    else
    {
        root->child1 = NULL;    //无局部变量说明，root的第一个孩子设置为空指针 
    }
    root->child2 = statement_list();  //调用处理语句序列子程序，返回子树根结点指针，作为root的第2个孩子
    if (error != 0) return NULL;
    if(w != RBRACE)                   //不是反大括号
    {
        error++;
        printf("line%d ", line_number);
        printf("复合语句有错！");
        clear(root);
        return NULL;                    //返回空指针，报错并释放结点
    }
    w = gettoken();
    return root;                        // 返回复合语句的子树根指针。
}


AST* statement_list(void)
{
    if (error != 0) return NULL;
    AST* root = (AST*)malloc(sizeof(AST));
    root->child1 = root->child2 = root->child3 = NULL; root->tag = 5; //初始化子树，根指针root = NULL；
    //strcpy(root->info, "语句序列结点");
    //w = gettoken();
    AST* r1 = statement();                       // 调用处理一条语句的子程序；返回其子树根指针r1；
    if (error != 0) return NULL;
    if (r1 == NULL)                              //没有分析到1条语句，errors>0时处理错误，
    {
        if (error != 0)
        {
            printf("line%d ", line_number);
            printf("出现错误！");
            return NULL;
        }
        else return NULL;       //否则表示语句序列已结束,返回NULL
    }
    else 
    {
        AST* statement_list_root = (AST*)malloc(sizeof(AST)); //生成语句序列的结点root
        statement_list_root->child1 = r1;
        statement_list_root->child2 = statement_list();            //递归调用处理语句序列子程序后的返回值
        if (error != 0) return NULL;
        statement_list_root->child3 = NULL;
        statement_list_root->tag = 3;
        strcpy(statement_list_root->info, "语句序列结点");
        return statement_list_root;                 //返回root
    }
}


AST *statement(void)
{
    if (error != 0) return NULL;
    AST* t = (AST*)malloc(sizeof(AST));
    t->child1 = t->child2 = t->child3 = NULL; t->tag = 0;
    //w = gettoken();
    switch(w)
    {
        case IF://分析条件语句
        {
            w = gettoken();
            if (w != LP)      //w不是左小括号,报错并返回空
            {
                error++;
                return NULL;
            }
            w = gettoken();
            Exp_ptr exp_t = Exp(RP,-1,w);  //调用处理表达式的子程序（结束符号为反小括号），正确时得到条件表达式子树结点指针
            AST* if_exp = (AST*)malloc(sizeof(AST));
            if_exp->t = exp_t; if_exp->tag = 1; if_exp->child1 = if_exp->child2 = if_exp->child3 = NULL;

            w = gettoken();
            if (w == LINE_COMMENT)
            {
                char c;
                while ((c = fgetc(fp)) != '\n' && c != EOF);
                line_number++;
                w = gettoken();
            }
            block_comment_judge();

            AST* if_statement = statement(); //调用处理一条语句的子程序，得到IF子句的子树根指针。                
            if (w == ELSE)
            {
                AST* else_statement = (AST*)malloc(sizeof(AST));  //else语句
                else_statement->child1 = else_statement->child2 = else_statement->child3 = NULL; //else语句下挂语句或者语句序列
                else_statement->tag = 16;

                w = gettoken();
                AST* else_statement_child = statement(); //调用处理一条语句的子程序，得到ELSE子句的子树根指针。
                strcpy(else_statement->info, "else"); 
                else_statement->child1 = else_statement_child;
                t->child1 = if_exp;                  //生成IF - ELSE结点，下挂条件、IF子句、ELSE子句3棵子树
                t->child2 = if_statement;
                t->child3 = else_statement;
                strcpy(t->info, "if-else语句");
                t->tag = 15;
            }
            else
            {
                t->child1 = if_exp;                 //生成IF结点，下挂条件、IF子句2棵子树 
                t->child2 = if_statement;
                strcpy(t->info, "if语句");
                t->tag = 15;
            }
            return t;
        }
        case  LBRACE:     //遇到{，调用处理复合语句子程序,返回得到的子树根指针
            return complex_statement();
            break;
        case  WHILE://分析while循环语句
        {
            w = gettoken();
            if (w != LP)      //w不是左小括号,报错并返回空
            {
                error++;
                return NULL;
            }
            w = gettoken();
            Exp_ptr exp_t = Exp(RP,-1,w);  //调用处理表达式的子程序（结束符号为反小括号），正确时得到条件表达式子树结点指针
            AST* while_exp = (AST*)malloc(sizeof(AST));
            while_exp->t = exp_t; while_exp->tag = 1; while_exp->child1 = while_exp->child2 = while_exp->child3 = NULL;

            w = gettoken();
            AST* while_statement = statement(); //调用处理一条语句的子程序，得到WHILE子句的子树根指针。 
            if (error != 0) return NULL;
            t->child1 = while_exp;
            t->child2 = while_statement;
            strcpy(t->info, "while语句");
            t->tag = 18;
            return t;
            break;
        }

        case FOR://分析for循环语句
        {
            w = gettoken();
            if (w != LP)      //w不是左小括号,报错并返回空
            {
                error++;
                return NULL;
            }
            w = gettoken();

            AST* for_exp = (AST*)malloc(sizeof(AST));
            for_exp->tag = 0; for_exp->child1 = for_exp->child2 = for_exp->child3 = NULL; ;
            strcpy(for_exp->info, "for语句循环条件表达式结点");

            if (w != SEMI)
            {
                for_exp->child1 = (AST*)malloc(sizeof(AST));
                Exp_ptr exp_t = Exp(SEMI, -1,w);  //调用处理表达式的子程序（结束符号为分号），正确时得到条件表达式子树结点指针
                for_exp->child1->t = exp_t;
                for_exp->child1->child1 = for_exp->child1->child2 = for_exp->child1->child3 = NULL;
                for_exp->child1->tag = 1;
            }
            w = gettoken();
            if (w != SEMI)
            {
                for_exp->child2 = (AST*)malloc(sizeof(AST));
                Exp_ptr exp_t = Exp(SEMI, -1, w);  //调用处理表达式的子程序（结束符号为分号），正确时得到条件表达式子树结点指针
                for_exp->child2->t = exp_t;
                for_exp->child2->child1 = for_exp->child2->child2 = for_exp->child2->child3 = NULL;
                for_exp->child2->tag = 1;
            }
            w = gettoken();
            if (w != RP)
            {
                for_exp->child3 = (AST*)malloc(sizeof(AST));
                Exp_ptr exp_t = Exp(RP, -1, w);  //调用处理表达式的子程序（结束符号为反小括号），正确时得到条件表达式子树结点指针
                for_exp->child3->t = exp_t;
                for_exp->child3->child1 = for_exp->child3->child2 = for_exp->child3->child3 = NULL;
                for_exp->child3->tag = 1;
            }
            
            w = gettoken();
            AST* for_statement = statement(); //调用处理一条语句的子程序，得到for子句的子树根指针。
            if (error != 0) return NULL;
            t->child1 = for_exp;
            t->child2 = for_statement;
            strcpy(t->info, "for语句");
            t->tag = 17;
            return t;
            break;
        }

        case LP:    
        {
            w = gettoken();
            Exp_ptr exp_t = Exp(RP,-1,w);
            AST* statement_exp = (AST*)malloc(sizeof(AST));   //调用处理表达式的子程序（结束符号为反小括号），正确时得到条件表达式子树结点指针
            statement_exp->t = exp_t; statement_exp->tag = 1; 
            statement_exp->child1 = statement_exp->child2 = statement_exp->child3 = NULL;
            return statement_exp;
            break;              //各种表达式语句，含赋值等，形式为表达式以分号结束
        }
        case IDENT:         //标识符
        {
            Exp_ptr exp_t = Exp(SEMI, -1, w);     //调用处理表达式的子程序（结束符号为分号），正确时得到条件表达式子树结点指针
            AST* statement_exp = (AST*)malloc(sizeof(AST));
            statement_exp->child1 = statement_exp->child2 = statement_exp->child3 = NULL;   
            if (fun_mark == 0)
            {
                statement_exp->t = exp_t; statement_exp->tag = 23;
                w = gettoken();  
            }
            else
            {
                fun_mark = 0;
                strcpy(statement_exp->info, "变量赋值语句");
                statement_exp->tag = 11;
                statement_exp->child1 = (AST*)malloc(sizeof(AST));
                statement_exp->child2 = (AST*)malloc(sizeof(AST));
                statement_exp->child1->child1 = statement_exp->child1->child2 = statement_exp->child1->child3 = NULL;
                statement_exp->child2->child1 = statement_exp->child2->child2 = statement_exp->child2->child3 = NULL;
                statement_exp->child1->tag = 26; statement_exp->child2->tag=22;
                strcpy(statement_exp->child1->info, exp_t->ch);
                strcpy(statement_exp->child2->info, exp_t->left->ch);
                w = gettoken();
                strcpy(token_text0, token_text);
                statement_exp->child2->child1 = FunctionUseParameterList();
                w = gettoken();
            }
            return statement_exp;
            break;
        }

                                
        case INT:
            w0 = INT;
            w = gettoken();
            strcpy(token_text0, token_text);
            w = gettoken();
            return ExtVarDef(w0);
            break;
        case CHAR:
            w0 = CHAR;
            w = gettoken();
            strcpy(token_text0, token_text);
            w = gettoken();
            return ExtVarDef(w0);
            break;
        case SHORT:
            w0 = SHORT;
            w = gettoken();
            strcpy(token_text0, token_text);
            w = gettoken();
            return ExtVarDef(w0);
            break;
        case FLOAT:
            w0 = FLOAT;
            w = gettoken();
            strcpy(token_text0, token_text);
            w = gettoken();
            return ExtVarDef(w0);
            break;
        case LONG:
            w0 = LONG;
            w = gettoken();
            strcpy(token_text0, token_text);
            w = gettoken();
            return ExtVarDef(w0);
            break;
        case DOUBLE:
            w0 = DOUBLE;
            w = gettoken();
            strcpy(token_text0, token_text);
            w = gettoken();
            return ExtVarDef(w0);
            break;

        case RETURN:
        {
     
            w = gettoken();
            Exp_ptr exp_t = Exp(SEMI, -1, w);  //调用处理表达式的子程序（结束符号为分号），正确时得到条件表达式子树结点指针
            AST* return_exp = (AST*)malloc(sizeof(AST));
            strcpy(return_exp->info, "return语句结点");
            return_exp->tag = 20; return_exp->child1 = return_exp->child2 = return_exp->child3 = NULL;

            return_exp->child1 = (AST*)malloc(sizeof(AST));
            return_exp->child1->child1 = return_exp->child1->child2 = return_exp->child1->child3 = NULL;
            return_exp->child1->tag = 1;
            return_exp->child1->t = exp_t;
            w = gettoken();
            return return_exp;
            break;
        }
        case CONTINUE:
        {
            t->tag = 19;
            w = gettoken();
            if (w !=SEMI)      //w不是分号,报错并返回空
            {
                error++;
                printf("line%d ", line_number);
                printf("continue语句错误！");
                return NULL;
            }
            strcpy(t->info, "continue语句结点");
            w = gettoken();

            if (w == LINE_COMMENT)
            {
                char c;
                while ((c = fgetc(fp)) != '\n' && c != EOF);
                line_number++;
                w = gettoken();
            }
            block_comment_judge();
            return t;
            break;
        }

        case BREAK:
        {
            t->tag = 24;
            w = gettoken();
            if (w != SEMI)      //w不是分号,报错并返回空
            {
                error++;
                printf("line%d ", line_number);
                printf("break语句错误！");
                return NULL;
            }
            strcpy(t->info, "break语句结点");
            w = gettoken();

            if (w == LINE_COMMENT)
            {
                char c;
                while ((c = fgetc(fp)) != '\n' && c != EOF);
                line_number++;
                w = gettoken();
            }
            block_comment_judge();
            return t;
            break;
        }
        case FUNCTION:
        {
            AST* root = (AST*)malloc(sizeof(AST));
            root->tag = 27;
            root->child1 = root->child2 = root->child3 = NULL;
            strcpy(root->info, "函数调用结点");
            root->child1 = (AST*)malloc(sizeof(AST));
            root->child1->child1 = root->child1->child2 = root->child1->child3 = NULL;
            strcpy(root->child1->info, token_text);
            root->child1->tag = 22;
            w = gettoken();
            strcpy(token_text0, token_text);
            root->child2 = FunctionUseParameterList();
            w = gettoken();
            return root;
        }

        case LINE_COMMENT:
        {
            char c;
            while ((c = fgetc(fp)) != '\n' && c != EOF);
            line_number++;
            w = gettoken();
            return NULL;
        }

        case BLOCK_COMMENT_BEGIN:
        {
            block_comment_judge();
        }

        case RBRACE: //}语句序列结束符号，如果语言支持switch语句，结束符号还会有case 和default    
            //w = gettoken();
            return NULL;
        default:
            error++;            //报错并返回NULL
            printf("line%d ", line_number);
            printf("语句有错！");
            return NULL;
        }
}


AST *ExtVarList(void)    // 初始时，tokenText0保存了第一个变量名
{  	
    if (error != 0) return NULL;
    AST *root = (AST*)malloc(sizeof(AST));
    root->child1 = root->child2 = root->child3 = NULL;
    root->tag = 21;    
    strcpy(root->info, "变量序列结点");//由保存在tokenText0的第一个变量名生成一个变量名结点，作为root的第一个孩子；
    AST* var_node = (AST*)malloc(sizeof(AST));
    var_node->child1 = var_node->child2 = var_node->child3 = NULL;
    strcpy(var_node->info, token_text0);
    var_node->tag = 12;
    root->child1 = var_node;
    //w = gettoken();       //开始识别后续的变量名
    if (w != COMMA && w != SEMI && w != ASSIGN)
    {
        error++;
        printf("line%d ", line_number);
        printf("变量声明错误！");  //报错，释放root为根的全部结点，返回空指针
        clear(root);
        return NULL;
    }

    if (w == ASSIGN)
    {
        //strcat(var_node->info, "赋值语句");
        var_node->tag = 26;
        var_node->child1 = (AST*)malloc(sizeof(AST));
        var_node->child1->child1 = var_node->child1->child2 = var_node->child1->child3 = NULL;
        
        w = gettoken();
        if (w == INT_CONST || w == FLOAT_CONST || w == LONG_CONST)
        {
            var_node->child1->tag = 1;  var_node->child1->t = (Exp_ptr)malloc(sizeof(Exp_TreeNode));
            var_node->child1->t->left = var_node->child1->t->right = NULL;
            strcpy(var_node->child1->t->ch, token_text);
            w = gettoken();
        }
        else if (w == FUNCTION)
        {
            root->tag = 11;
            var_node->child1->tag = 27; 
            w = gettoken();
            strcpy(token_text0, token_text);
            var_node->child1 = FunctionUseParameterList();
        }
        
        //return root;
    }
    if (w == SEMI)
    {
        w = gettoken();
        if(w == LINE_COMMENT)
        {
            char c;
            while ((c = fgetc(fp)) != '\n' && c != EOF);
            line_number++;
            w = gettoken();
        }
        block_comment_judge();
        return root;              //返回根结点root；
    }
    w = gettoken();
    w0 = w;
    if (w != IDENT) //w不是标识符,报错，释放root为根的全部结点，返回空指针
    {
        error++;
        clear(root);
        printf("line%d ", line_number);
        printf("变量定义错误！");
        return NULL;
    }
    
    strcpy(token_text0, token_text);// 将变量名w保存在tokenText0中；
    w = gettoken();
    root->child2 = ExtVarList(); //   调用ExtVarList，得到的子树作为root的第二棵子树，返回root；
    if (error != 0) return NULL;
    return root;
}

AST *ExtVarDef(int w0)
{
    if (error != 0) return NULL;
    AST* root = (AST*)malloc(sizeof(AST));      //root = 生成一个外部变量定义结点；
    root->child1 = root->child2 = root->child3 = NULL;
    root->tag = 6;
    strcpy(root->info, "变量定义结点");
    AST* var_node = (AST*)malloc(sizeof(AST));//根据已读入的外部变量的类型，生成外部变量类型结点，作为root的第一个孩子
    var_node->child1 = var_node->child2 = var_node->child3 = NULL;
    var_node->tag = 2; //var_node->type = w0;
    strcpy(var_node->info, "变量类型结点");
    root->child1 = var_node;    
    root->child1->type = w0;
    root->child2 = ExtVarList();    //调用ExtVarList，得到的子树根作为root的第二个子树
    if (error != 0) return NULL;
    return root;   //返回root；
}


AST *funcDef(int w0)
{
    if (error != 0) return NULL;
    AST* root = (AST*)malloc(sizeof(AST)); //生成函数定义结点root
    root->child1 = root->child2 = root->child3 = NULL;
    root->tag = 7; 

    AST* return_type = (AST*)malloc(sizeof(AST)); //生成返回值类型结点，作为root的第一个孩子
    return_type->child1 = return_type->child2 = return_type->child3 = NULL;
    return_type->tag = 2; return_type->type = w0;
    root->child1 = return_type;

    root->child2 = (AST*)malloc(sizeof(AST)); 
    root->child2->child1 = root->child2->child2 = root->child2->child3 = NULL;
    strcpy(root->child2->info, token_text0); root->child2->tag = 22;

    w = gettoken();
    //strcpy(token_text0, token_text);
    root->child2->child1 = parameterList();//调用形参子程序，得到参数部分的子树根指针，无参函数得到NULL，该子树作为root的第二棵子树
    
    //w = gettoken();//读入符号，如果分号，就是函数原型声明，函数体子树为空；

    if (w == LINE_COMMENT)
    {
        char c;
        while ((c = fgetc(fp)) != '\n' && c != EOF);
        line_number++;
        w = gettoken();
    }
    block_comment_judge();
    if (w == SEMI)
    {
        root->tag = 25;
        strcpy(root->info, "函数原型声明结点");
        w = gettoken();
    }
    else if (w == LBRACE)
    {
        strcpy(root->info, "函数定义结点");
        root->child3 = complex_statement();//正大括号，则调用函数体（复合语句）子程序，得到函数体子树根指针，得到的函数体子树作为root的第三棵子树
        if (error != 0) return NULL;
    }
    else
    {
        error++;//其它符号表示有错。
        printf("line%d ", line_number);
        printf("函数定义或声明错误！");
        clear(root);
        return NULL;
    }
    return root;// 返回root
}


AST* parameterList()
{
    if (error != 0) return NULL;
    if (w == RP)
    {
        w = gettoken();
        return NULL;
    }

    AST* root = (AST*)malloc(sizeof(AST));      //root = 生成形参序列结点；
    root->child1 = root->child2 = root->child3 = NULL;
    root->tag = 8;
    strcpy(root->info, "形参序列结点");

    AST* parameter_node = parameterDef();//根据已读入的形参的类型，生成形参结点，作为root的第一个孩子
    if (error != 0) return NULL;
    root->child1 = parameter_node;

    w = gettoken();
    if (w != COMMA && w != RP)
    {
        error++;
        printf("line%d ", line_number);
        printf("形参定义错误！");  //报错，释放root为根的全部结点，返回空指针
        clear(root);
        return NULL;
    }
    if (w == RP)
    {
        w = gettoken();
        return root;              //返回根结点root；
    }
    w = gettoken();
    if (w != INT && w != SHORT && w != LONG && w != CHAR && w != DOUBLE && w != FLOAT) //w不是变量类型,报错，释放root为根的全部结点，返回空指针
    {
        error++;
        clear(root);
        printf("line%d ", line_number);
        printf("形参定义错误！");
        return NULL;
    }

    root->child2 = parameterList();    //调用parameterList()，得到的子树根作为root的第二个子树
    if (error != 0) return NULL;
    return root;   //返回root；
}


AST* parameterDef()
{
    if (error != 0) return NULL;
    AST* root = (AST*)malloc(sizeof(AST));      //root = 生成形参变量结点；
    root->child1 = root->child2 = root->child3 = NULL;
    root->tag = 9;
    strcpy(root->info, "形参变量结点");

    root->child1= (AST*)malloc(sizeof(AST));
    root->child1->child1 = root->child1->child2 = root->child1->child3 = NULL;
    strcpy(root->child1->info, "形参变量类型"); root->child1->tag = 2; root->child1->type = w;

    w = gettoken();
    strcpy(token_text0, token_text);
    root->child2 = (AST*)malloc(sizeof(AST));
    root->child2->child1 = root->child2->child2 = root->child2->child3 = NULL;
    strcpy(root->child2->info, token_text0); 
    root->child2->tag = 12;
    return root;
}

AST* FunctionUseParameterList()
{
    if (error != 0) return NULL;
    if (w == RP)
    {
        w = gettoken();
        return NULL;
    }

    AST* root = (AST*)malloc(sizeof(AST));      //root = 生成函数调用结点；
    root->child1 = root->child2 = root->child3 = NULL;
    root->tag = 10;
    strcpy(root->info, "函数调用参数序列结点");

    AST* parameter_node = FunctionUseParameterDef();//根据已读入的参数，生成参数结点，作为root的第一个孩子
    root->child1 = parameter_node;

    w = gettoken();
    if (w != COMMA && w != RP)
    {
        error++;
        printf("line%d ", line_number);
        printf("函数调用参数错误！");  //报错，释放root为根的全部结点，返回空指针
        clear(root);
        return NULL;
    }
    if (w == RP)
    {
        w = gettoken();
        return root;              //返回根结点root；
    }
    w = gettoken();
    strcpy(token_text0, token_text);
    root->child2 = FunctionUseParameterList();    //调用FunctionUseParameterList()，得到的子树根作为root的第二个子树
    return root;   //返回root；
}

AST* FunctionUseParameterDef()
{
    if (error != 0) return NULL;
    AST* root = (AST*)malloc(sizeof(AST));      //root = 生成函数调用参数结点；
    root->child1 = root->child2 = root->child3 = NULL;
    root->tag = 28;
    strcpy(root->info, token_text0);
    return root;
}




AST *ExtDef()//处理外部定义序列，正确时，返回子树根结点指针，否则返回NULL
{
    if (error != 0) return NULL;
    if (w == LINE_COMMENT)
    {
        char c;
        while ((c = fgetc(fp)) != '\n' && c != EOF);
        line_number++;
        w = gettoken();
    }
    block_comment_judge();
    if (w != INT && w != SHORT && w != LONG && w != CHAR && w != DOUBLE && w != FLOAT && w!= VOID)//w不是类型关键字,报错并返回NULL
    {
        error++;
        printf("line%d ", line_number);
        printf("外部定义中，读到非类型关键字!");
        return NULL;
    }
    int w0 = w;    //保存类型说明符
    w = gettoken();
    if (w != IDENT && w != ARRAY && w != FUNCTION)
    {
        error++;//报错并返回NULL
        printf("line%d ", line_number);
        printf("外部定义中，读到非标识符!");
        return NULL;
    }
    strcpy(token_text0, token_text);  // 保存第一个变量名或函数名到tokenText0
    //w = gettoken();

    AST* root = (AST*)malloc(sizeof(AST));      
    root->child1 = root->child2 = root->child3 = NULL;
    root->tag = 4;
    strcpy(root->info, "外部定义结点");

    if (w != FUNCTION)
    {
        w = gettoken();
        root = ExtVarDef(w0);       //调用外部变量定义子程序
        if (error != 0) return NULL;
    }
    else root = funcDef(w0);      //调用函数定义子程序
    if (root) return root;//如果返回值p非空，表示成功完成一个外部定义的处理，返回p
}

AST *ExtDefList()//处理外部定义序列，正确时，返回子树根结点指针，否则错误处理
{
    if (error != 0) return NULL;
    if (w == EOF) return NULL;
    AST* root = (AST*)malloc(sizeof(AST));//生成一个外部定义序列结点root
    root->child1 = root->child2 = root->child3 = NULL;
    root->tag = 3;
    strcpy(root->info, "外部定义序列结点");    
    root->child1 = ExtDef();    //处理一个外部定义，得到一棵子树，作为root的第一棵子树
    root->child2 = ExtDefList();  //得到的子树，作为root的第二棵子树
    return root;
}

AST* DefineAndInclude()
{
    if (error != 0) return NULL;
    AST* root = (AST*)malloc(sizeof(AST));     
    root->child1 = root->child2 = root->child3 = NULL;
    root->tag = 13;
    strcpy(root->info, "文件包含和宏定义结点");
    if (w != DEFINE && w != INCLUDE)
    {
        clear(root);
        return NULL;
    }
    else
    {
        root->child1 = (AST*)malloc(sizeof(AST));
        root->child1->child1 = root->child1->child2 = root->child1->child3 = NULL;
        root->child1->tag = 12;
        strcpy(root->child1->info, token_text);
        w = gettoken();
        root->child2 = DefineAndInclude();
    }
    return root;
}




AST *Program()
{
    w = gettoken();
    AST* root = (AST*)malloc(sizeof(AST));      //root = 生成形参变量结点；
    root->child1 = root->child2 = root->child3 = NULL;
    root->tag = 14;
    strcpy(root->info, "程序结点");
    root->child1 = DefineAndInclude(); //第一棵子树是宏定义与文件包含
    root->child2 = ExtDefList();
    if(error==0)
    {
        return root;
    }
    else
    {
        clear(root);
        return NULL;
    }
}

void clear(AST* &t)
{
    if (t == NULL)
        return;
    if (t->child1 != NULL) {
        clear(t->child1);
        t->child1 = NULL;
    }
    if (t->child2 != NULL) {
        clear(t->child2);
        t->child2 = NULL;
    }
    if (t->child3 != NULL) {
        clear(t->child3);
        t->child3 = NULL;
    }
    if (t != NULL) {
       free(t);
        t = NULL;
    }
}

void clear_exp(Exp_ptr &t)
{
    if (t)
    {
        clear_exp(t->left);
        clear_exp(t->right);
        free(t);
        t = NULL;
    }
    return;
}

void pre_exp(Exp_ptr t, int l)
{
    if (t)
    {
        //for (int i = 1; i <= l - 1; i++)printf("    ");//4个空格
        pre_exp(t->left, l + 1);
        fprintf(f_out, "%s", t->ch);
        pre_exp(t->right, l + 1);
    }
    return;
}

void pre(AST *t,int l)
{        
    
    if (t) 
    {
        for (int i = 1; i <= l - 1; i++)printf("    ");//4个空格
        if (t->tag == 1||t->tag == 23)
        {
            printf("表达式语句");
            //if (t->t != NULL) pre_exp(t->t, 1);
            printf("\n");
        }
        else if (t->tag == 2)
        {
            printf("变量类型为%s\n", Var_name(t->type));
        }
        else printf("%s\n", t->info);
        pre(t->child1,l + 1);
        pre(t->child2,l + 1);
        pre(t->child3,l + 1);
    }
    return;
}


void output(AST* t, int l)
{
    if (t)
    {
        if (t->tag == 1)
        {
            pre_exp(t->t, 1);
        }
        else if (t->tag == 2)
        {
            //for (int i = 1; i <= l - 1; i++)printf("    ");//4个空格
            fprintf(f_out,"%s ", Var_name(t->type));
        }
        
        else if (t->tag == 3)
        {
            for (int i = 1; i <= l - 1; i++) fprintf(f_out,"    ");//4个空格
            output(t->child1, l);
            output(t->child2, l);
        }
        else if (t->tag == 4)
        {
            for (int i = 1; i <= l - 1; i++) fprintf(f_out, "    ");//4个空格
            fprintf(f_out, "{\n");
            if(t->child1) for (int i = 1; i <= l; i++)fprintf(f_out, "    ");//4个空格
            output(t->child1, l + 1);
            output(t->child2, l + 1);
            for (int i = 1; i <= l - 1; i++)fprintf(f_out, "    ");//4个空格
            fprintf(f_out, "}\n");
        }
        else if (t->tag == 5)
        {
            output(t->child1, l);
            output(t->child2, l);
        }
        else if (t->tag == 6)
        {
            output(t->child1, l);
            output(t->child2, l);
        }
        else if (t->tag == 7)
        {
            output(t->child1, l);
            output(t->child2, l);
            fprintf(f_out, "\n");
            output(t->child3, l);
        }
        else if (t->tag == 8)
        {

            output(t->child1, l);
            if (t->child2 == NULL) fprintf(f_out, ")");
            else
            {
                fprintf(f_out, ", ");
                output(t->child2, l);
            }
        }
        else if (t->tag == 9)
        {
            output(t->child1, l);
            output(t->child2, l);
        }
        else if (t->tag == 10)
        {
            output(t->child1, l);
            if (t->child2 == NULL) fprintf(f_out, ");\n");
            else
            {
                fprintf(f_out, ", ");
                output(t->child2, l);
            }
        }
        else if (t->tag == 11)
        {
            output(t->child1, l);
            output(t->child2, l);
            output(t->child3, l);
        }
        else if (t->tag == 12)
        {
            //for (int i = 1; i <= l - 1; i++)printf("    ");//4个空格
            fprintf(f_out, t->info);
            output(t->child1, l);
            
        }
        else if (t->tag == 13)
        {
            fprintf(f_out, "#");
            output(t->child1, l);
            fprintf(f_out, "\n");
            output(t->child2, l);
        }
        else if (t->tag == 14)
        {
            output(t->child1, l);
            output(t->child2, l);
        }
        
        else if (t->tag == 15)
        {
            //for (int i = 1; i <= l - 1; i++)printf("    ");//4个空格
            fprintf(f_out, "if(");
            output(t->child1, l);
            fprintf(f_out, ")\n");
            if (t->child2->tag != 4) for (int i = 1; i <= l; i++)fprintf(f_out, "    ");
            output(t->child2, l);
            output(t->child3, l);
        }
        else if (t->tag == 16)
        {
            for (int i = 1; i <= l - 1; i++)fprintf(f_out, "    ");//4个空格
            fprintf(f_out, "else\n");
            if (t->child1->tag != 4) for (int i = 1; i <= l; i++)fprintf(f_out, "    ");
            output(t->child1, l);
        }
        
        else if (t->tag == 17)
        {
            //for (int i = 1; i <= l - 1; i++)printf("    ");//4个空格
            fprintf(f_out, "for(");
            if (t->child1->child1 != NULL)
            {
                pre_exp(t->child1->child1->t, 1);
            }
            fprintf(f_out, ";");
            if (t->child1->child2 != NULL)
            {
                pre_exp(t->child1->child2->t, 1);
            }
            fprintf(f_out, ";");
            if (t->child1->child3 != NULL)
            {
                pre_exp(t->child1->child3->t, 1);
            }
            fprintf(f_out, ")\n");
            if (t->child2 && t->child2->tag != 4) for (int i = 1; i <= l; i++)fprintf(f_out , "    ");
            output(t->child2, l);
        }
        else if (t->tag == 18)
        {
            //for (int i = 1; i <= l - 1; i++)printf("    ");//4个空格
            fprintf(f_out, "while(");
            output(t->child1, l);
            fprintf(f_out, ")\n");
            if (t->child2 && t->child2->tag != 4) for (int i = 1; i <= l; i++)fprintf(f_out, "    ");
            output(t->child2, l);
        }
        else if (t->tag == 19)
        {
            //for (int i = 1; i <= l - 1; i++)printf("    ");//4个空格
            fprintf(f_out, "continue;\n");
        }
        else if (t->tag == 20)
        {
            //for (int i = 1; i <= l - 1; i++)printf("    ");//4个空格
            fprintf(f_out, "return ");
            output(t->child1, l);
            fprintf(f_out, ";\n");
        }
        else if (t->tag == 21)
        {
            output(t->child1, l);
            if (t->child2 == NULL) fprintf(f_out, ";\n");
            else
            {
                fprintf(f_out, ", ");
                output(t->child2, l);
            }
        }
        else if (t->tag == 22)
        {
            fprintf(f_out, t->info);
            fprintf(f_out, "(");
            if(t->child1==NULL) fprintf(f_out, ")");
            else output(t->child1, l);

        }
        else if (t->tag == 23)
        {
            //for (int i = 1; i <= l - 1; i++)printf("    ");//4个空格
            pre_exp(t->t, 1);
            fprintf(f_out, ";\n");
        }
        else if (t->tag == 24)
        {
            //for (int i = 1; i <= l - 1; i++)printf("    ");//4个空格
            fprintf(f_out, "break;\n");
        }
        
        else if (t->tag == 25)
        {
            for (int i = 1; i <= l - 1; i++)fprintf(f_out, "    ");//4个空格
            output(t->child1, l);
            output(t->child2, l);
            fprintf(f_out, ";\n");
        }
        else if (t->tag == 26)
        {
        //for (int i = 1; i <= l - 1; i++)printf("    ");//4个空格
            fprintf(f_out, t->info);
            fprintf(f_out, "=");
            output(t->child1, l);
        }
        else if (t->tag == 27)
        {
            output(t->child1, l);
            output(t->child2, l);
        }
        else if (t->tag == 28)
        {
            fprintf(f_out, "%s", t->info);
        }
    }
    return;
}

void show_token_kind()
{
    while (!feof(fp))
    {
        w = gettoken();
        if (w == EOF) return;
        printf("%s ", Token_to_Sign(w));
    }
    return;
}


int main()
{
    int x, flag = 1;
    char s[100];
    AST* t = NULL;
    while (flag)
    {
        printf("\n/**********欢迎使用高级语言源程序格式处理工具**********/\n");
        printf("0.退出程序\n1.程序使用说明\n2.词法分析\n3.抽象语法树生成\n4.缩进编排源文件\n");
        printf("请选择程序功能：");
        scanf("%d", &x);
        switch (x)
        {
        case 0:
            flag = 0;
            printf("退出成功！");
            break;
        case 1:
            printf("1.若选择功能4，则需先使用功能三\n2.能够识别的有:\n  1）基本数据类型的变量、常量，以及数组。不包含指针、结构，枚举等。\n  2）双目算术运算符（ + -*/ % ），关系运算符、逻辑与（ && ）、逻辑或（ || ）、赋值运算符。不包含逗号运算符、位运算符、各种单目运算符等等。\n  3）函数定义、声明与调用。\n  4）表达式语句、复合语句、if语句的2种形式、while语句、for语句，return语句、break语句、continue语句、外部变量说明语句、局部变量说明语句。\n  5）编译预处理（宏定义，文件包含）\n  6）注释（块注释与行注释）\n");
            break;
        case 2:
            printf("请输入文件名，需要和可执行文件放在同一目录下：");
            scanf("%s", s);
            fp = fopen(s, "r");
            show_token_kind();
            break;
        case 3:
            printf("请输入文件名，需要和可执行文件放在同一目录下：");
            scanf("%s", s);
            fp = fopen(s, "r");
            t = Program();
            if(error==0) pre(t,1);
            break;
        case 4:
            printf("请输入输出文件文件名：");
            scanf("%s", s);
            f_out = fopen(s, "w");
            //AST* t = Program();
            output(t, 1);
            fclose(f_out);
            printf("\n输出成功\n");
            break;
        }
    }
    fclose(fp);
    return 0;
}