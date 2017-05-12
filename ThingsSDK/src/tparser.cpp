#ifdef AVR
#include <arduino.h>
#endif

#include <stdio.h>

#include <src/include/tutils.h>
#include <src/include/tparser.h>
#include <src/include/tobject.h>

TObject *parse_value(char **code);


char char_escape(const char ch) {
    switch(ch) {
    case 't':  return TAB;  break;
    case 'n':  return LF;   break;
    case 'r':  return CR;   break;
    case '\\': return '\\'; break;
    case '\'': return '\''; break;
    case '"':  return '"';  break;
    default:
        return ch;
    }
};


char get_next_ch_with_escape(char **code) {
    char ch;
    ch = *code[0];
    if (ch == '\\') {
        (*code)++;
        ch = *code[0];
        switch(ch) {
        case 't':  return TAB;  break;
        case 'n':  return LF;   break;
        case 'r':  return CR;   break;
        case '\\': return '\\'; break;
        case '\'': return '\''; break;
        case '"':  return '"';  break;
        default:
            return ch;
        }
    } else {
        return ch;
    }
};

char get_next_ch(char **code) {
    return *code[0];
};

void parse_skip_space(char **code) {
    char ch;
    ch = get_next_ch(code);
    while (ch != 0) {
        switch(ch) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            (*code)++; ch = *code[0];
            break;
        default:
            ch = 0;
        }
    }
};
void parse_skip_token_name(char **code) {
    // tempory for object property name
    char ch;
    ch = get_next_ch(code);
    while (ch != 0) {
        switch(ch) {
        case ' ':
        case ':':
        case '\t':
        case '\n':
        case '\r':
            ch = 0;
            break;
        default:
            (*code)++; ch = *code[0];
        }
    }
};

int parse_match_string(char **code, const char *str) {
    int i = 0;
    char ch;
    ch = get_next_ch(code);
    while (ch == str[i]) {
        i++;
        if (str[i] == 0) { (*code)++; return 1; };
        (*code)++;
        ch = get_next_ch(code);
    }
    return 0;
};
void parse_move_to_next(char **code, char to_ch) {
    char ch;
    ch = get_next_ch(code);
    while ((ch != 0) && (ch != to_ch)) {
        (*code)++;
        ch = get_next_ch(code);
    }
};

char* parse_property_name(char **code) {
    char ch;
    parse_skip_space(code);
    ch = get_next_ch(code);
    if ((ch == ',') || (ch == ':') || (ch == '}')) {
        return NULL;
    };
    if ((ch == '"') || (ch == '\'')) {
        (*code)++;
    } else {
        ch = 0;
    }
    char *result = *code;
    if (ch != 0) {
        parse_move_to_next(code, ch);
    } else {
        parse_skip_token_name(code);
    };
    if (*code[0] == 0) { return NULL; };
    if (*code[0] != ':') {
        *code[0] = 0;
        (*code)++;
    };
    return result;
};

TObject *parse_boolean(char **code) {
    int result = -1;
    char ch;
    ch = *code[0]; 
    if (ch == 't') {
        if (parse_match_string(code, "true")) {
            result = 1;
        } else {
            result = -1;
        }
    } else if (ch == 'f') {
        if (parse_match_string(code, "false")) {
            result = 0;
        } else {
            result = -1;
        }
    }
    if (result == 1) {
        return new TObject(T_TRUE);
    } else if (result == 0) {
        return new TObject(T_FALSE);
    } else {
        return NULL;
    };
};
TObject *parse_null(char **code) {
    int result = -1;
    if (parse_match_string(code, "null")) {
        return new TObject((char*)NULL);
    } else {
        return NULL;
    };
};
TObject *parse_string(char **code) {
    char ch, deli;
    deli = get_next_ch(code);
    if ((deli != '"') && (deli != '\'')) {
        return NULL;
    }
    (*code)++;
    char *result = *code;
    char *end = *code;
    ch = get_next_ch(code);
    while ((ch != 0) && (ch != deli)) {
        if (ch == '\\') {
            (*code)++;
            ch = get_next_ch(code);
            ch = char_escape(ch);
        };
        end[0] = ch;
        (*code)++;
        end++;
        ch = get_next_ch(code);
    }
    if (*code[0] == 0) { return NULL; };
    end[0] = 0;
    (*code)++;
    return new TObject(result);
};
TObject *parse_array(char **code) {
    TObject *result = new TObject();
    char* property_name;
    TObject *value;
    char ch;
    ch = get_next_ch(code);
    if (ch != '[') { return NULL; };
    (*code)++;
    ch = get_next_ch(code);
    int valid = 1;
    while (ch != 0) {
        parse_skip_space(code);
        value = parse_value(code);
        if (value == NULL) {valid = 0; break;};
        result->push(value);
        parse_skip_space(code);
        ch = get_next_ch(code);
        if (ch == ',') {
            (*code)++;
        } else if ((ch == ']') || (ch == 0)) {
            (*code)++; break;
        } else {
            valid = 0;
            break;
        }
    }
    if (valid) {
        return result;
    } else {
#ifdef AVR
        Serial.print("[invalid code]");
        Serial.println(*code);
#else
        printf("\n[invalid code] %s\n", *code);
#endif
        delete result;
        return NULL;
    }
};
TObject *parse_number(char **code) {
    char ch;
    int sign         = 1;
    int int_part     = 0;
    int digits       = 1;
    double frac_part = 0;
    int dot_cnt      = 0;
    int state        = 0; // 0 - int part 1 - frac part
    ch = get_next_ch(code);
    if (ch == '-') {
        sign = -1;
        (*code)++;
    }
    ch = get_next_ch(code);
    while (true) {
        if ((ch >= '0') && (ch <= '9')) {
            if (state == 0) { // int part
                int_part = (int_part * 10) + (ch - '0');
            } else {  // frac part
                frac_part = (frac_part * 10) + (ch - '0');
                digits = digits * 10;
            };
        } else if (ch == '.') {
            if (state == 0) { // int_part -> frac_part
                state = 1;
            } else {
                break;
            }
        } else {
            break;
        }
        (*code)++;
        ch = get_next_ch(code);
    }
    TObject *result = NULL;
    if (state == 0) {
        result =  new TObject(sign * int_part);
    } else if (state == 1) {
        result =  new TObject(sign * (int_part + (frac_part / digits)));
    };
    return result;
};

TObject *parse_object(char **code) {
    char ch;
    ch = get_next_ch(code);
    if (ch != '{') { return NULL; };
    (*code)++;
    TObject *result = new TObject();
    char* property_name;
    TObject *value;
    int valid = 1;
    parse_skip_space(code);
    ch = get_next_ch(code);
    if (ch == '}') {
        (*code)++;
        result->set((char*)NULL, NULL);
        return result;
    };
    while (ch != 0) {
        property_name = parse_property_name(code);
        if (property_name == NULL) { valid = 0; break; };
        parse_skip_space(code);
        ch = get_next_ch(code);
        if (ch != ':') { valid = 0; break; };
        *code[0] = 0;
        (*code)++;
        parse_skip_space(code);
        value = parse_value(code);
        if (value == NULL) { valid = 0; break; };
        result->set(property_name, value);
        parse_skip_space(code);
        ch = get_next_ch(code);
        if (ch == ',') {
            (*code)++;
        } else if ((ch == '}') || (ch == 0)) {
            (*code)++;
            break;
        } else {
            valid = 0;
            break;
        }
    }
    if (valid) {
        return result;
    } else {
#ifdef AVR
        Serial.print("[invalid code]");
        Serial.println(*code);
#else
        printf("\n[invalid code] %s", *code);
#endif
        delete result;
        return NULL;
    }
};

TObject *parse_value(char **code) {
    TObject *result;
    char ch;
    ch = get_next_ch(code);
    switch (ch) {
    case 'n':  result = parse_null(code);    break;
    case 't':  result = parse_boolean(code); break;
    case 'f':  result = parse_boolean(code); break;
    case '"':  result = parse_string(code);  break;
    case '\'': result = parse_string(code);  break;
    case '[':  result = parse_array(code);   break;
    case '{':  result = parse_object(code);  break;
    default:   result = parse_number(code);  break;
    }
    return result;
};

void parse_block(char **code) {
    T_PARSE_STATE state = T_PARSE_SPACE;
    parse_skip_space(code);
    char ch, ch_next;
    ch = get_next_ch(code);
    switch (ch) {
    case '{':
        parse_object(code);
        break;   
    case '[':
        parse_array(code);
        break;   
    }
};

TObject *parse(char **code){
    parse_block(code);
    return NULL;
};

TObject *parse_json(const char *code) {
  char *p = (char*)malloc(tx_strlen(code) + 1);
  tx_strcpy(p, code);
  char *s = p;
  TObject *result = parse_json(&s);
  free(p);
  return result;
};

TObject *parse_json(char **code){
    parse_skip_space(code);
    return parse_object(code);
};
