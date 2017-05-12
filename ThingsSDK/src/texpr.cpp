#include <stdio.h>
#include <string.h>
#include "include/texpr.h"
#include "include/tparser.h"
#include "include/tobject.h"

#include "include/cwlib.h"

#ifdef AVR
#include "include/cwlib.h"
#endif
/*
  reference: http://zh.wikipedia.org/wiki/%E8%B0%83%E5%BA%A6%E5%9C%BA%E7%AE%97%E6%B3%95
  reference: http://zh.wikipedia.org/wiki/%E9%80%86%E6%B3%A2%E5%85%B0%E8%A1%A8%E7%A4%BA%E6%B3%95
*/


void check_name_token(T_TOKEN *token, void *context) {
    TObject *value = NULL;
    if (token->type == T_TOKEN_NAME) {
        value = new TObject();
        if (strcmp(token->name, "true") == 0) {
            value->set(T_TRUE);
        } else if (strcmp(token->name, "false") == 0) {
            value->set(T_FALSE);
        } else {
            TObject *v = ((TExpr*)context)->context->member(token->name);
            value->assign(v);
        };
        free(token->name);
        token->name  = NULL;
        token->value = value;
        token->type  = T_TOKEN_VALUE;
    };
};


T_TOKEN *calc_dot(T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1, *t2;
    t1 = params[0];
    t2 = params[1];
    check_name_token(t1, context);
    TObject *o = new TObject();
    if ((t1->type == T_TOKEN_VALUE)
        && (t1->value->type() == T_OBJECT)
        && (t2->type == T_TOKEN_NAME)) {
        o->assign(t1->value->member(t2->name));
    }
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type  = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    // printf("%s.%s = %s\n",
    //        t1->value->to_string(),
    //        t2->value->to_string(),
    //        o->to_string());
    return token;
};
T_TOKEN *calc_power (T_TOKEN *params[], int n, void *context) {
    return NULL;
};
T_TOKEN *calc_times (T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1, *t2;
    t1 = params[0];
    t2 = params[1];
    check_name_token(t1, context);
    check_name_token(t2, context);
    TObject *o = obj_times(t1->value, t2->value);
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    printf("%s * %s = %s\n",
           params[0]->value->to_string(),
           params[1]->value->to_string(),
           o->to_string());
    return token;
};

T_TOKEN *calc_mod (T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1, *t2;
    t1 = params[0];
    t2 = params[1];
    check_name_token(t1, context);
    check_name_token(t2, context);
    TObject *o = obj_mod(t1->value, t2->value);
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type  = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    printf("%s %% %s = %s\n",
           params[0]->value->to_string(),
           params[1]->value->to_string(),
           o->to_string());
    return token;
};
T_TOKEN *calc_div (T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1, *t2;
    t1 = params[0];
    t2 = params[1];
    check_name_token(t1, context);
    check_name_token(t2, context);
    TObject *o = obj_div(t1->value, t2->value);
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    printf("%s %% %s = %s\n",
           params[0]->value->to_string(),
           params[1]->value->to_string(),
           o->to_string());
    return token;
};
T_TOKEN *calc_plus (T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1, *t2;
    t1 = params[0];
    t2 = params[1];
    check_name_token(t1, context);
    check_name_token(t2, context);
    TObject *o = obj_add(t1->value, t2->value);
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    printf("%s + %s = %s\n",
           params[0]->value->to_string(),
           params[1]->value->to_string(),
           o->to_string());
    return token;
};
T_TOKEN *calc_sub (T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1, *t2;
    t1 = params[0];
    t2 = params[1];
    check_name_token(t1, context);
    check_name_token(t2, context);
    TObject *o = obj_sub(t1->value, t2->value);
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    printf("%s - %s = %s\n",
           params[0]->value->to_string(),
           params[1]->value->to_string(),
           o->to_string());
    return token;
};
T_TOKEN *calc_neg          (T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1;
    t1 = params[0];
    check_name_token(t1, context);
    TObject *o = obj_neg(t1->value);
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    printf("-%s = %s\n",
           params[0]->value->to_string(),
           o->to_string());
    return token;
};
T_TOKEN *calc_bool_eq(T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1, *t2;
    t1 = params[0];
    t2 = params[1];
    check_name_token(t1, context);
    check_name_token(t2, context);
    TObject *o = obj_bool_eq(t1->value, t2->value);
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    printf("%s == %s = %s\n",
           params[0]->value->to_string(),
           params[1]->value->to_string(),
           o->to_string());
    return token;
};
T_TOKEN *calc_bool_ne(T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1, *t2;
    t1 = params[0];
    t2 = params[1];
    check_name_token(t1, context);
    check_name_token(t2, context);
    TObject *o = obj_bool_ne(t1->value, t2->value);
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    printf("%s != %s = %s\n",
           params[0]->value->to_string(),
           params[1]->value->to_string(),
           o->to_string());
    return token;
};
T_TOKEN *calc_bool_gt(T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1, *t2;
    t1 = params[0];
    t2 = params[1];
    check_name_token(t1, context);
    check_name_token(t2, context);
    TObject *o = obj_bool_gt(t1->value, t2->value);
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    printf("%s > %s = %s\n",
           params[0]->value->to_string(),
           params[1]->value->to_string(),
           o->to_string());
    return token;
};
T_TOKEN *calc_bool_lt(T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1, *t2;
    t1 = params[0];
    t2 = params[1];
    check_name_token(t1, context);
    check_name_token(t2, context);
    TObject *o = obj_bool_lt(t1->value, t2->value);
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    printf("%s < %s = %s\n",
           params[0]->value->to_string(),
           params[1]->value->to_string(),
           o->to_string());
    return token;
};
T_TOKEN *calc_bool_ge(T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1, *t2;
    t1 = params[0];
    t2 = params[1];
    check_name_token(t1, context);
    check_name_token(t2, context);
    TObject *o = obj_bool_ge(t1->value, t2->value);
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    printf("%s >= %s = %s\n",
           params[0]->value->to_string(),
           params[1]->value->to_string(),
           o->to_string());
    return token;
};
T_TOKEN *calc_bool_le(T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1, *t2;
    t1 = params[0];
    t2 = params[1];
    check_name_token(t1, context);
    check_name_token(t2, context);
    TObject *o = obj_bool_le(t1->value, t2->value);
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    printf("%s <= %s = %s\n",
           params[0]->value->to_string(),
           params[1]->value->to_string(),
           o->to_string());
    return token;
};
T_TOKEN *calc_string_match(T_TOKEN *params[], int n, void *context) {
    return NULL;
};
T_TOKEN *calc_bool_and(T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1, *t2;
    t1 = params[0];
    t2 = params[1];
    check_name_token(t1, context);
    check_name_token(t2, context);
    TObject *o = obj_bool_and(t1->value, t2->value);
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    printf("%s && %s = %s\n",
           params[0]->value->to_string(),
           params[1]->value->to_string(),
           o->to_string());
    return token;
};
T_TOKEN *calc_bool_or(T_TOKEN *params[], int n, void *context) {
    T_TOKEN *t1, *t2;
    t1 = params[0];
    t2 = params[1];
    check_name_token(t1, context);
    check_name_token(t2, context);
    TObject *o = obj_bool_or(t1->value, t2->value);
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_VALUE;
    token->value = o;
    token->name  = NULL;
    printf("%s || %s = %s\n",
           params[0]->value->to_string(),
           params[1]->value->to_string(),
           o->to_string());
    return token;
};
T_TOKEN *calc_array_in(T_TOKEN *params[], int n, void *context) {
    return NULL;
};


TExpr::TExpr(char *code) {
    context   = NULL;
    token_len = 0;
    rpn_len   = 0;
    pass1(&code);
    pass2();
};

TExpr::~TExpr() {
    int i;
    for (i = 0; i < rpn_len; i++) {
        if (rpn[i] != NULL) {
            free_token(&rpn[i]);
        }
    };
};

TObject *TExpr::eval(TObject *context_obj){
    context = context_obj;
    return pass3();
};


T_TOKEN *TExpr::parse_token_string(char **code) {
    TObject *v = parse_value(code);
    if (v == NULL) {
        return NULL;
    };
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->value = v;
    token->type  = T_TOKEN_VALUE;
    token->name  = NULL;
    return token;
};

T_TOKEN *TExpr::parse_token_number(char **code) {
    TObject *v = parse_value(code);
    if (v == NULL) {
        return NULL;
    };
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->value = v;
    token->type  = T_TOKEN_VALUE;
    token->name  = NULL;
    return token;
};

int TExpr::is_name_char(char ch) {
    if (((ch >= 'a') && (ch <= 'z'))
        || ((ch >= 'A') && (ch <= 'Z'))
        || (ch == '$')
        || (ch == '_')) {
        return 1;
    } else {
        return 0;
    }
};
T_TOKEN *TExpr::parse_token_name(char **code) {
    char *name = *code;
    int len = 0;
    while (is_name_char(*code[0])) {
        (*code)++;
        len++;
    }
    if (len == 0) {
        return NULL;
    }
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_NAME;
    token->name = (char*)malloc(len + 1);
    token->value  = NULL;
    strncpy(token->name, name, len);
    token->name[len] = 0;
    return token;
};

int TExpr::is_operator_prefix(char ch) {
    if (index("^().*%/+-=!><@&|", ch) == NULL) {
        return 0;
    } else {
        return 1;
    };
};

T_TOKEN *TExpr::parse_token_operator(char **code) {
    T_TOKEN *token = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    token->type = T_TOKEN_OPERATOR;
    token->name  = NULL;
    token->value = NULL;
    char ch1 = 0, ch2 = 0, ch3 = 0, ch4 = 0;
    ch1 = *code[0];
    if (ch1 != 0) { ch2 = (*code)[1]; };
    if (ch2 != 0) { ch3 = (*code)[2]; };
    if (ch3 != 0) { ch4 = (*code)[3]; };
    token->op = OP_NONE;
    (*code)++;
    switch(ch1) {
    case '(': token->type = T_TOKEN_LEFT_BRACKET; break;
    case ')': token->type = T_TOKEN_RIGHT_BRACKET; break;
    case '.': token->op = OP_DOT; break;
    case '*': token->op = OP_TIMES; break;
    case '%': token->op = OP_MOD; break;
    case '/': token->op = OP_DIV; break;
    case '+': token->op = OP_PLUS; break;
    case '-': token->op = OP_SUB; break;
    case '=': if (ch2 == '=') { token->op = OP_BOOL_EQ; (*code)++; }; break;
    case '!': if (ch2 == '=') { token->op = OP_BOOL_NE; (*code)++; }; break;
    case '>': token->op = OP_BOOL_GT; break;
    case '<': token->op = OP_BOOL_LT; break;
    case '&': if (ch2 == '&') { token->op = OP_BOOL_AND; (*code)++;}; break;
    case '|': 
        if (ch2 == '|') { token->op = OP_BOOL_OR;  (*code)++;}; 
        break;
    }
    if ((token->type == T_TOKEN_OPERATOR) && (token->op == OP_NONE)) {
        free(token);
        return NULL;
    } else {
        return token;
    }
};

void TExpr::pass1(char **code) {
    T_TOKEN *token;

    int error = 0;
    char ch = *code[0];
    while ((ch != 0) && (!error)) {
        if ((ch == '"') || (ch == '\'')) {
            token = parse_token_string(code);
        } else if ((ch >= '0') && (ch <= '9')) {
            token = parse_token_number(code);
        } else if (is_name_char(ch)) {
            token = parse_token_name(code);
        } else if (is_operator_prefix(ch)) {
            token = parse_token_operator(code);
        }
        if (token != NULL) {
            if (token_len >= TOKENS_MAX) {
                printf("tokens overflow in pass1\n");
                exit(0);
            };
            tokens[token_len] = token;
            token_len++;
            parse_skip_space(code);
            ch = *code[0];
        } else {
            error = 1;
        }
    };
    if (error) {
        printf("syntax error: %s", *code);
        exit(0);
    };
};


const char S_OP_NONE[]     = "[none]"; 
const char S_OP_DOT[]      = ".";     
const char S_OP_TIMES[]    = "*";     
const char S_OP_MOD[]      = "%";     
const char S_OP_DIV[]      = "/";     
const char S_OP_PLUS[]     = "+";     
const char S_OP_SUB[]      = "-";     
const char S_OP_NEG[]      = "-n";    
const char S_OP_BOOL_EQ[]  = "==";    
const char S_OP_BOOL_NE[]  = "!=";    
const char S_OP_BOOL_GT[]  = ">";     
const char S_OP_BOOL_LT[]  = "<";     
const char S_OP_BOOL_GE[]  = ">=";    
const char S_OP_BOOL_LE[]  = "<=";    
const char S_OP_BOOL_AND[] = "&&";    
const char S_OP_BOOL_OR[]  = "||";    


const char *op_name(T_OPERATOR op) {
    switch (op) {
    case OP_NONE:     return S_OP_NONE;       break;
    case OP_DOT:      return S_OP_DOT;        break;
    case OP_TIMES:    return S_OP_TIMES;      break;
    case OP_MOD:      return S_OP_MOD;        break;
    case OP_DIV:      return S_OP_DIV;        break;
    case OP_PLUS:     return S_OP_PLUS;       break;
    case OP_SUB:      return S_OP_SUB;        break;
    case OP_NEG:      return S_OP_NEG;        break;
    case OP_BOOL_EQ:  return S_OP_BOOL_EQ;    break;
    case OP_BOOL_NE:  return S_OP_BOOL_NE;    break;
    case OP_BOOL_GT:  return S_OP_BOOL_GT;    break;
    case OP_BOOL_LT:  return S_OP_BOOL_LT;    break;
    case OP_BOOL_GE:  return S_OP_BOOL_GE;    break;
    case OP_BOOL_LE:  return S_OP_BOOL_LE;    break;
    case OP_BOOL_AND: return S_OP_BOOL_AND;   break;
    case OP_BOOL_OR:  return S_OP_BOOL_OR;    break;
    default : return S_OP_NONE;
    };
};


void TExpr::pass2() {
    T_TOKEN *stack[TOKENS_MAX];
    int stack_len = 0;

    T_TOKEN *token;
    int pos = 0;
    while (pos < token_len) {
        token = tokens[pos];
        switch (token->type) {
        case T_TOKEN_NAME:
        case T_TOKEN_VALUE:
            rpn[rpn_len] = token;
            rpn_len++;
            pos++;
            break;
        case T_TOKEN_OPERATOR:
            T_TOKEN *o1, *o2;
            o1 = token;
            o2 = NULL;
            if (stack_len > 0) {
                o2 = stack[stack_len - 1];
            }
            int p1, p2;
            p1 = 0;
            p2 = 0;
            T_OPER_DIR o1_dir;
            T_OPER_DIR o2_dir;
            if (o2 != NULL) {
                p1 = operators[o1->op].priority;
                p2 = operators[o2->op].priority;
                o1_dir = operators[o1->op].dir;
                o2_dir = operators[o2->op].dir;
            }
            while ((o2 != NULL) && (o2->type == T_TOKEN_OPERATOR)
                   && (((o1_dir == T_OPER_LTR) && (p1 >= p2))
                       || ((o1_dir == T_OPER_RTL) && (p1 > p2)))) {
                stack_len--;
                rpn[rpn_len] = stack[stack_len];
                rpn_len++;
                //rpn.push(stack.pop());
                if (stack_len > 0) {
                    o2 = stack[stack_len - 1];
                } else {
                    o2 = NULL;
                }
                if (o2 != NULL) {
                    p2 = operators[o2->op].priority;
                }
            }
            stack[stack_len] = o1;
            stack_len++;
            pos++;
            break;
        case T_TOKEN_LEFT_BRACKET:
            stack[stack_len] = token;
            stack_len++;
            pos++;
            break;
        case T_TOKEN_RIGHT_BRACKET:
            free_token(&token);
            stack_len--;
            token = stack[stack_len];
            while ((stack_len > 0) && (token->type != T_TOKEN_LEFT_BRACKET)) {
                rpn[rpn_len] = token;
                rpn_len++;
                stack_len--;
                token = stack[stack_len];
            }
            if (token->type != T_TOKEN_LEFT_BRACKET) {
                printf("missing left brace\n");
                exit(0);
            } else {
                free_token(&token);
            };
            pos++;
            break;
        case T_TOKEN_COMMENT: pos++; break;
        }
    }
    if (stack_len > 0) {
        token = stack[stack_len - 1];
        if (token->type == T_TOKEN_LEFT_BRACKET) {
            printf("mismatch brace\n");
            exit(0);
        } else {
            while (stack_len > 0) {
                stack_len--;
                rpn[rpn_len] = stack[stack_len];
                rpn_len++;
            }
        }
    }
    //return rpn.reverse();
    int i = 0, j = rpn_len - 1;
    while (i < j) {
        token  = rpn[i];
        rpn[i] = rpn[j];
        rpn[j] = token;
        i++; j--;
    };
};


TObject *TExpr::pass3() {
    T_TOKEN *result[TOKENS_MAX];
    int result_len = 0;

    T_TOKEN *params[TOKENS_MAX];
    int params_len = 0;

    T_TOKEN *token;
    int rp_len = rpn_len;

    while (rp_len > 0) {
        rp_len--;
        if (rpn[rp_len] == NULL) {
            continue;
        } else {
            token = clone_token(rpn[rp_len]);
        }
        if ((token->type == T_TOKEN_VALUE)
            || (token->type == T_TOKEN_NAME)) {
            result[result_len] = token;
            result_len++;
        } else if (token->type == T_TOKEN_OPERATOR) {
            if (result_len < operators[token->op].oper_nums) {
                printf("not enough operator\n");
                exit(0);
            } else {
                int i;
                int params_num = operators[token->op].oper_nums;
                params_len = params_num;
                for (i = 0; i < params_num; i++) {
                    result_len--;  // reverse parameters order
                    params[params_num - i - 1] = result[result_len];
                    params_len++;
                    //params.push(result.pop());
                }
                token = operators[token->op].calc(params, params_num, this);
                for (i = 0; i < params_num; i++) {
                    free_token(&params[i]);
                }
                result[result_len] = token;
                result_len++;
            }
        }
    }
    TObject *r = new TObject();
    token = result[0];
    check_name_token(token, context);
    r->assign(token->value);
    free_token(&token);
    return r;
};
void TExpr::free_token(T_TOKEN **token) {
    int i;
    switch ((*token)->type) {
    case T_TOKEN_VALUE:
        if ((*token)->value != NULL) {
            delete (*token)->value;
        };
        break;
    case T_TOKEN_NAME:
        free((*token)->name);
        break;
    case T_TOKEN_OPERATOR:
    case T_TOKEN_COMMENT:
    case T_TOKEN_LEFT_BRACKET:
    case T_TOKEN_RIGHT_BRACKET:
        break;
    };
    free(*token);
    *token = NULL;
};


T_TOKEN *TExpr::clone_token(T_TOKEN *token) {
    T_TOKEN *result = (T_TOKEN*)malloc(sizeof(T_TOKEN));
    result->value = NULL;
    result->name  = NULL;

    result->type = token->type;
    result->op    = token->op;

    switch (token->type) {
    case T_TOKEN_VALUE:
        result->value = new TObject();
        result->value->assign(token->value);
        break;
    case T_TOKEN_NAME:
        result->name = (char*)malloc(strlen(token->name) + 1);
        strcpy(result->name, token->name);
        break;
    case T_TOKEN_OPERATOR:
    case T_TOKEN_COMMENT:
    case T_TOKEN_LEFT_BRACKET:
    case T_TOKEN_RIGHT_BRACKET:
        break;
    };
    return result;
};
