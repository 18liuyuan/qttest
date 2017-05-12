#define _GNU_SOURCE
#include <stdlib.h>


#include <string.h>
#include <stdint.h>

#include <src/include/tobject.h>
#include <src/include/tutils.h>
#include <src/include/cwlib.h>

#ifdef AVR
#include <arduino.h>
#else
#include <stdio.h>

#endif


TObject *obj_add(TObject *o1, TObject *o2) {
    TObject *result = new TObject();
    if ((o1 == NULL) || (o2 == NULL)) {
        return result;
    };
    OBJECT_TYPE t1, t2;
    t1 = o1->type();
    t2 = o2->type();
    if ((t1 == T_UNDEFINE) || (t2 == T_UNDEFINE)) {  // undefine
    } else if ((t1 == T_STRING) || (t2 == T_STRING)
               || (t1 == T_OBJECT) || (t2 == T_OBJECT)
               || (t1 == T_ARRAY) || (t2 == T_ARRAY)) { // string
        char *s1, *s2, *s3;
        int  l1, l2;
        s1 = o1->to_string();
        s2 = o2->to_string();
        l1 = strlen(s1);
        l2 = strlen(s2);
        s3 = (char*)malloc(l1 + l2 + 1);
        strcpy(&s3[0], s1);
        strcpy(&s3[l1], s2);
        result->set(s3);
        free(s3);
    } else if ((t1 == T_FLOAT) || (t2 == T_FLOAT)) {
        double f;
        f = o1->to_float() + o2->to_float();
        result->set(f);
    } else {  // int
        int i;
        i = o1->to_int() + o2->to_int();
        result->set(i);
    }
    return result;
};

TObject *obj_times(TObject *o1, TObject *o2) {
    TObject *result = new TObject();
    if ((o1 == NULL) || (o2 == NULL)) {
        return result;
    };
    OBJECT_TYPE t1, t2;
    t1 = o1->type();
    t2 = o2->type();
    if ((t1 == T_UNDEFINE) || (t2 == T_UNDEFINE)
        || (t1 == T_STRING) || (t2 == T_STRING)
        || (t1 == T_OBJECT) || (t2 == T_OBJECT)
        || (t1 == T_ARRAY) || (t2 == T_ARRAY)) { // string
    } else if ((t1 == T_FLOAT) || (t2 == T_FLOAT)) {
        double f;
        f = o1->to_float() * o2->to_float();
        result->set(f);
    } else {  // int
        int i;
        i = o1->to_int() * o2->to_int();
        result->set(i);
    }
    return result;
};

TObject *obj_mod(TObject *o1, TObject *o2) {
    TObject *result = new TObject();
    if ((o1 == NULL) || (o2 == NULL)) {
        return result;
    };
    OBJECT_TYPE t1, t2;
    t1 = o1->type();
    t2 = o2->type();
    if ((t1 == T_UNDEFINE) || (t2 == T_UNDEFINE)
        || (t1 == T_STRING) || (t2 == T_STRING)
        || (t1 == T_OBJECT) || (t2 == T_OBJECT)
        || (t1 == T_ARRAY) || (t2 == T_ARRAY)) { // string
    } else {  // int , float
        int i;
        i = o1->to_int() % o2->to_int();
        result->set(i);
    }
    return result;
};

TObject *obj_div(TObject *o1, TObject *o2) {
    TObject *result = new TObject();
    if ((o1 == NULL) || (o2 == NULL)) {
        return result;
    };
    OBJECT_TYPE t1, t2;
    t1 = o1->type();
    t2 = o2->type();
    if ((t1 == T_UNDEFINE) || (t2 == T_UNDEFINE)
        || (t1 == T_STRING) || (t2 == T_STRING)
        || (t1 == T_OBJECT) || (t2 == T_OBJECT)
        || (t1 == T_ARRAY) || (t2 == T_ARRAY)) { // string
    } else {  // int , float
        double f;
        f = o1->to_float() / o2->to_float();
        result->set(f);
    }
    return result;
};

TObject *obj_sub(TObject *o1, TObject *o2) {
    TObject *result = new TObject();
    if ((o1 == NULL) || (o2 == NULL)) {
        return result;
    };
    OBJECT_TYPE t1, t2;
    t1 = o1->type();
    t2 = o2->type();
    if ((t1 == T_UNDEFINE) || (t2 == T_UNDEFINE)
        || (t1 == T_STRING) || (t2 == T_STRING)
        || (t1 == T_OBJECT) || (t2 == T_OBJECT)
        || (t1 == T_ARRAY) || (t2 == T_ARRAY)) { // string
    } else if ((t1 == T_FLOAT) || (t2 == T_FLOAT)) {
        double f;
        f = o1->to_float() - o2->to_float();
        result->set(f);
    } else {  // int
        int i;
        i = o1->to_int() - o2->to_int();
        result->set(i);
    }
    return result;
};

TObject *obj_neg (TObject *o1) {
    TObject *result = new TObject();
    if (o1 == NULL) {
        return result;
    };
    OBJECT_TYPE t1;
    t1 = o1->type();
    if ((t1 == T_UNDEFINE)
        || (t1 == T_STRING)
        || (t1 == T_OBJECT)
        || (t1 == T_ARRAY)) {
    } else if (t1 == T_FLOAT) {
        double f;
        f = -o1->to_float();
        result->set(f);
    } else {  // int
        int i;
        i = -o1->to_int();
        result->set(i);
    };
    return result;
};
TObject *obj_bool_eq(TObject *o1, TObject *o2) {
    TObject *result = new TObject();
    if ((o1 == NULL) || (o2 == NULL)) {
        return result;
    };
    if (strcmp(o1->to_string(), o2->to_string()) == 0) {
        result->set(T_TRUE);
    } else {
        result->set(T_FALSE);
    };
    return result;
};

TObject *obj_bool_ne(TObject *o1, TObject *o2) {
    TObject *result = new TObject();
    if ((o1 == NULL) || (o2 == NULL)) {
        return result;
    };
    OBJECT_TYPE t1, t2;
    t1 = o1->type();
    t2 = o2->type();
    if ((t1 == t2) &&
        ((t1 == T_INT) || t1 == T_FLOAT || t1 == T_BOOL || t1 == T_NULL_VALUE)) {
        if (o1->to_float() == o2->to_float()) {
            result->set(T_FALSE);
        } else {
            result->set(T_TRUE);
        }
    } else {
        int b = strcmp(o1->to_string(), o2->to_string());
        if (b == 0) {
            result->set(T_FALSE);
        } else {
            result->set(T_TRUE);
        };
    }
    return result;
};
TObject *obj_bool_gt(TObject *o1, TObject *o2) {
    TObject *result = new TObject();
    if ((o1 == NULL) || (o2 == NULL)) {
        return result;
    };
    OBJECT_TYPE t1, t2;
    t1 = o1->type();
    t2 = o2->type();
    if ((t1 == t2) &&
        ((t1 == T_INT) || t1 == T_FLOAT || t1 == T_BOOL || t1 == T_NULL_VALUE)) {
        if (o1->to_float() > o2->to_float()) {
            result->set(T_TRUE);
        } else {
            result->set(T_FALSE);
        }
    } else {
        int b = strcmp(o1->to_string(), o2->to_string());
        if (b > 0) {
            result->set(T_TRUE);
        } else {
            result->set(T_FALSE);
        };
    }
    return result;
};
TObject *obj_bool_lt(TObject *o1, TObject *o2) {
    TObject *result = new TObject();
    if ((o1 == NULL) || (o2 == NULL)) {
        return result;
    };
    OBJECT_TYPE t1, t2;
    t1 = o1->type();
    t2 = o2->type();
    if ((t1 == t2) &&
        ((t1 == T_INT) || t1 == T_FLOAT || t1 == T_BOOL || t1 == T_NULL_VALUE)) {
        if (o1->to_float() < o2->to_float()) {
            result->set(T_TRUE);
        } else {
            result->set(T_FALSE);
        }
    } else {
        int b = strcmp(o1->to_string(), o2->to_string());
        if (b < 0) {
            result->set(T_TRUE);
        } else {
            result->set(T_FALSE);
        };
    }
    return result;
};
TObject *obj_bool_ge      (TObject *o1, TObject *o2) {
    TObject *result = new TObject();
    if ((o1 == NULL) || (o2 == NULL)) {
        return result;
    };
    OBJECT_TYPE t1, t2;
    t1 = o1->type();
    t2 = o2->type();
    if ((t1 == t2) &&
        ((t1 == T_INT) || t1 == T_FLOAT || t1 == T_BOOL || t1 == T_NULL_VALUE)) {
        if (o1->to_float() >= o2->to_float()) {
            result->set(T_TRUE);
        } else {
            result->set(T_FALSE);
        }
    } else {
        int b = strcmp(o1->to_string(), o2->to_string());
        if (b >= 0) {
            result->set(T_TRUE);
        } else {
            result->set(T_FALSE);
        };
    }
    return result;
};
TObject *obj_bool_le      (TObject *o1, TObject *o2) {
    TObject *result = new TObject();
    if ((o1 == NULL) || (o2 == NULL)) {
        return result;
    };
    OBJECT_TYPE t1, t2;
    t1 = o1->type();
    t2 = o2->type();
    if ((t1 == t2) &&
        ((t1 == T_INT) || t1 == T_FLOAT || t1 == T_BOOL || t1 == T_NULL_VALUE)) {
        if (o1->to_float() <= o2->to_float()) {
            result->set(T_TRUE);
        } else {
            result->set(T_FALSE);
        }
    } else {
        int b = strcmp(o1->to_string(), o2->to_string());
        if (b <= 0) {
            result->set(T_TRUE);
        } else {
            result->set(T_FALSE);
        };
    }
    return result;
};
TObject *obj_bool_and(TObject *o1, TObject *o2) {
    TObject *result = new TObject();
    if ((o1 == NULL) || (o2 == NULL)) {
        return result;
    };
    if (o1->to_bool() && o2->to_bool()) {
        result->set(T_TRUE);
    } else {
        result->set(T_FALSE);
    }
    return result;
};
TObject *obj_bool_or      (TObject *o1, TObject *o2) {
    TObject *result = new TObject();
    if ((o1 == NULL) || (o2 == NULL)) {
        return result;
    };
    if (o1->to_bool() || o2->to_bool()) {
        result->set(T_TRUE);
    } else {
        result->set(T_FALSE);
    }
    return result;
};


TObject::TObject() {
    _init();
    set();
};
TObject::TObject(const char *value) {
    _init();
    set(value);
};
TObject::TObject(const char *value, T_FLAGS flags) {
    _init();
    set(value, flags);
};
TObject::TObject(const T_BOOLEAN value) {
    _init();
    set(value);
};
TObject::TObject(const T_BOOLEAN value, T_FLAGS flags) {
    _init();
    set(value, flags);
};
TObject::TObject(const int value) {
    _init();
    set(value);
};
TObject::TObject(const int value, T_FLAGS flags) {
    _init();
    set(value, flags);
};
TObject::TObject(const double value) {
    _init();
    set(value);
};
TObject::TObject(const double value, T_FLAGS flags) {
    _init();
    set(value, flags);
};

TObject::TObject(const char *member_name, TObject *value) { // for object
    _init();
    set(member_name, value);
};
TObject::TObject(const char *member_name, TObject *value, T_FLAGS flags) {
    _init();
    set(member_name, value, flags);
};

TObject::~TObject() {
#ifdef T_DEBUG   
    char *t = type_name();
#endif
    release();
    if (_callback_cnt > 0) {
#ifdef T_DEBUG
        _update_memory_usage(-(sizeof(T_CALLBACK) * _callback_cnt));
#endif
        free(_event_callbacks);
        _event_callbacks = NULL;
        _callback_cnt    = 0;
    }
#ifdef T_DEBUG   
    Serial.print(F("v.type = "));
    Serial.println(t);
    Serial.print(F("v.mem_alloc = "));
    Serial.println(memory_alloc);
    Serial.print(F("v.mem_free  = "));
    Serial.println(memory_release);
    Serial.print(F("v.mem_leak = "));
    Serial.println(memory_usage);
#endif
};
#ifdef T_DEBUG   
void TObject::_update_memory_usage(int size) {
    if (size > 0) {
        memory_alloc   = memory_alloc + size;
        memory_usage   = memory_usage + size;
    } else {
        memory_release = memory_release + size;
        memory_usage   = memory_usage + size;
    };
};
#endif

void TObject::_init() {
#ifdef T_DEBUG   
    memory_alloc   = 0;
    memory_release = 0;
    memory_usage   = 0;
#endif
    _ref_cnt         = 0;
    _type            = T_UNDEFINE;
    _value_string    = NULL;
    _event_callbacks = NULL;
    _callback_cnt    = 0;
    _flags           = 0;
};

void TObject::ref(TObject *obj) {
    _ref_cnt++;
};
void TObject::unref(TObject *obj) {
    _ref_cnt--;
    if (_ref_cnt == 0) {
        delete this;
    }
};

void TObject::_free_string(char **str) {
    if (*str != NULL) {
#ifdef T_DEBUG   
        _update_memory_usage(-(tx_strlen(*str) + 1));
#endif
        free(*str);
        *str = NULL;
    };
};
void TObject::release() {
    int i;
    switch (_type) {
    case T_UNDEFINE :
        break;
    case T_BOOL :
        break;
    case T_NULL_VALUE :
        break;
    case T_STRING:
        _free_string(&_value.str);
        break;
    case T_INT:
        _free_string(&_value_string);
        break;
    case T_FLOAT:
        _free_string(&_value_string);
        break;
    case T_ARRAY:
        _free_string(&_value_string);
        for (i = 0; i < _member_array_len; i++) {
            if (_value.members[i].value->_type == T_OBJECT) {
                _value.members[i].value->unref(this);
            } else {
                delete _value.members[i].value;
            }
        }
        if (_value.members != NULL) {
#ifdef T_DEBUG
            _update_memory_usage(-(sizeof(MemberRecord) * _member_array_len));
#endif
            free(_value.members);
            _member_array_len = 0;
            _value.members = NULL;
        }
        break;
    case T_OBJECT:
        for (i = 0; i < _member_array_len; i++) {
            if (_value.members[i].name != NULL) {
                _free_string(&_value.members[i].name);
                if (_value.members[i].value->_type == T_OBJECT) {
                    _value.members[i].value->unref(this);
                } else {
                    delete _value.members[i].value;
                }
            }
        }
        if (_value.members != NULL) {
#ifdef T_DEBUG
            _update_memory_usage(-(sizeof(MemberRecord) * _member_array_len));
#endif
            free(_value.members);
            _member_array_len = 0;
            _value.members = NULL;
        }
        _free_string(&_value_string);
        break;
    }
    _type = T_UNDEFINE;
    _value_string = STR_UNDEFINED;;
};

TObject *TObject::set() {
    if (_type  == T_UNDEFINE) { return this;};
    _flags = _flags | T_FLAGS_CHANGED;
    release();
    _type  = T_UNDEFINE;
    _value_string = STR_UNDEFINED;;
    _call_after_change();
    return this;
};
TObject *TObject::set(const char *value) {
    if (_flags & T_FLAGS_READONLY) { return this;};
    return set(value, T_FLAGS_NONE);
};
TObject *TObject::set(const char *value, T_FLAGS flags){
    _flags = _flags | flags;
    if (value == NULL) { // set T_NULL_VALUE
        if (_type  == T_NULL_VALUE) { return this; };
        release();
        _type  = T_NULL_VALUE;
        _value_string = STR_NULL;
        _call_after_change();
        _flags = _flags | T_FLAGS_CHANGED;
        return this;
    }
    if (_type  == T_STRING) {
        if (tx_strcmp(value, _value.str) == 0) {
            return this;
        }
    }
    _flags = _flags | T_FLAGS_CHANGED;
    release();
    _type  = T_STRING;
    size_t len = tx_strlen(value) + 1;
    _value.str = (char*)malloc(len);
#ifdef T_DEBUG    
    _update_memory_usage(len);
#endif
    tx_strncpy(_value.str, value, len);
    _value_string = _value.str;
    _call_after_change();
    return this;
};
TObject *TObject::set(const int value) {
    set((long)value);
};
TObject *TObject::set(const int value, T_FLAGS flags) {
    set((long)value, flags);
};
TObject *TObject::set(const long value) {
    if (_flags & T_FLAGS_READONLY) { return this; };
    return set(value, T_FLAGS_NONE);
};
TObject *TObject::set(const long value, T_FLAGS flags) {
    _flags = _flags | flags;
    if ((_type == T_INT) && (value == _value.i)) {
        return this;
    }
    _flags = _flags | T_FLAGS_CHANGED;
    release();
    _type         = T_INT;
    _value.i      = value;
    _value_string = NULL;
    _call_after_change();
    return this;
};

TObject *TObject::set(const T_BOOLEAN value) {
    if (_flags & T_FLAGS_READONLY) { return this; };
    return set(value, T_FLAGS_NONE);
};
TObject *TObject::set(const T_BOOLEAN value, T_FLAGS flags) {
    _flags = _flags | flags;
    if ((_type == T_BOOL) && (value == _value.b)) {
        return this;
    }
    _flags = _flags | T_FLAGS_CHANGED;
    release();
    _type  = T_BOOL;
    _value.b = value;
    if (_value.b) {
        _value_string = STR_TRUE;
    } else {
        _value_string = STR_FALSE;
    }
    _call_after_change();
    return this;
};
TObject *TObject::set(const char *member_name, TObject  *value) {
    if (_flags & T_FLAGS_READONLY) { return this; };
    return set(member_name, value, T_FLAGS_NONE);
};

int TObject::find_member_first_level(const char *path,
                                     Name_mode mode) {
    if (_type != T_OBJECT) { return -1; };
    if (path == NULL) { return -1; };
    int i, l1, l2;
    char *name;
    l1 = -1;
    if (mode == BY_PATH) {
        l1 = tx_index(path, DELI_PATH);
    };
    if (l1 < 0) {
        l1 = tx_strlen(path);
    };
    for (i = 0; i < _member_array_len; i++) {
        name = _value.members[i].name;
        l2 = tx_strlen(name);
        if ((l1 == l2) && (tx_strncmp(path, name, l1) == 0)) {
            return i;
        }
    };
    return -1;
};

TObject *TObject::set(const char *member_name,
                      TObject  *value,
                      T_FLAGS flags) {
    _flags = _flags | flags;
    if (_type != T_OBJECT) {
        release();
        _type          = T_OBJECT;
        _value.members = NULL;
        _member_array_len    = 0;
        _value_string  = NULL;
    } else {
        _free_string(&_value_string);
    }
    if (member_name == NULL) {
        return this;
    };
    int index = find_member_first_level(member_name, BY_NAME);
    if (index >= 0) {
        if (_value.members[index].value == value) {
            return this;
        }
        _flags = _flags | T_FLAGS_CHANGED;
        _value.members[index].value->unref(this);
        _value.members[index].value = value;
        value->ref(this);
    } else if (value != NULL) {
        _flags = _flags | T_FLAGS_CHANGED;
        int i, found = 0; // found not used position
        for (i = 0; i < _member_array_len; i++) {
            if (_value.members[i].name == NULL) {
                found = 1;
                break;
            }
        };
        if (!found) {
            _member_array_len++;
            _value.members =
                (MemberRecord*)realloc(_value.members,
                                       sizeof(MemberRecord) * _member_array_len);
#ifdef T_DEBUG
            _update_memory_usage(sizeof(MemberRecord));
#endif
            i = _member_array_len - 1;
            //memset(&_value.members[i], 0, sizeof(MemberRecord));
        };
        // if (mode == BY_PATH) {
        //     index = tx_index(member_name, DELI_PATH);
        // } else {
        index = tx_strlen(member_name);
        // };
        _value.members[i].name = (char*)malloc(index + 1);

#ifdef T_DEBUG
        _update_memory_usage(index + 1);
#endif
        tx_strncpy(_value.members[i].name, member_name, index);
        _value.members[i].value = value;
        value->ref(this);
    };
    _call_after_change();
    return this;
};


TObject *TObject::set(const double value) {
    if (_flags & T_FLAGS_READONLY) { return this; };
    return set(value, T_FLAGS_NONE);
};
TObject *TObject::set(const double value, T_FLAGS flags) {
    _flags = _flags | flags;
    if ((_type == T_FLOAT) && (value == _value.f)) {
        return this;
    }
    _flags = _flags | T_FLAGS_CHANGED;
    release();
    _type  = T_FLOAT;
    _value.f = value;
    _value_string = NULL;
    _call_after_change();
    return this;
};

void TObject::del(const char *member_name){
    if (_type != T_OBJECT) { return; };
    int index = 0;
    for (int i = 0; i < _member_array_len; i++) {
        if (_value.members[i].name == NULL) { continue; };
        if (tx_strcmp(_value.members[i].name, member_name) == 0) {
            del(index);
            return;
        }
        index++;
    }
};
void TObject::del(const int index) {
    int pos = index;
    if (_type == T_OBJECT) {
        int i, cnt = -1;
        for (i = 0; i < _member_array_len; i++) {
            if (_value.members[i].name == NULL) { continue; };
            cnt++;
            if (cnt == index) { break; };
        };
        if (cnt == index) {
            pos = i;
        } else {
            return;
        }
    };

    if ((pos >= _member_array_len) || (pos < 0)) { return; };
    _free_string(&_value_string);
    _free_string(&_value.members[pos].name);
    TObject *v = _value.members[pos].value;
    if (v) {
        _flags = _flags | T_FLAGS_CHANGED;
        if (v->type() == T_OBJECT || v->type() == T_ARRAY) {
            v->unref(this);
        } else {
            delete _value.members[pos].value;
        };
        _value.members[pos].value = NULL;
    };
    if (_type == T_ARRAY) {
        int i;
        for (i = pos; i < _member_array_len - 1; i++) {
            _value.members[i].value = _value.members[i + 1].value;
        };
        _flags = _flags | T_FLAGS_CHANGED;
        _member_array_len--;
        _value.members =
            (MemberRecord*)realloc(_value.members,
                                   sizeof(MemberRecord) * _member_array_len);
    };
};

TObject *TObject::push(char *value) {
    TObject *v = new TObject(value);
    push(v);
};
TObject *TObject::push(TObject *value) {
    if (value == NULL) { return NULL; };
    if (_flags & T_FLAGS_READONLY) { return NULL; };
    if (_type != T_ARRAY) {
        release();
        _type          = T_ARRAY;
        _value.members = NULL;
        _member_array_len = 0;
        _value_string  = NULL;
    } else {
        _free_string(&_value_string);
    }
    _flags = _flags | T_FLAGS_CHANGED;
    _member_array_len++;
    _value.members =
        (MemberRecord*)realloc(_value.members,
                               sizeof(MemberRecord) * _member_array_len);
    _value.members[_member_array_len - 1].value = value;
    value->ref(this);
#ifdef T_DEBUG
    _update_memory_usage(sizeof(MemberRecord));
#endif
    _call_after_change();
    return value;
};

void TObject::_call_before_change() {};

void TObject::_call_after_change() {
    T_CALLBACK *cbs; int i;
    TObject *v = this;
    cbs = v->_event_callbacks;
    if (!cbs) { return; };
    for (i = 0; i < v->_callback_cnt; i++) {
        if (cbs[i].event == T_ON_AFTER_CHANGE) {
            cbs[i].callback(v, cbs[i].context);
        }
    }
};

size_t TObject::str_len_with_escape(char *str) {
    size_t len = 0;
    while (str[0] != 0) {
        len++;
        switch (str[0]) {
        case '"':  case '\\': case '/': case '\b':
        case '\f': case '\n': case '\r': case '\t':
            len++;
        }
        str++;
    }
    return len;
};

int TObject::need_quote() {
    switch (_type) {
    case T_UNDEFINE:
    case T_STRING:
        return 1;
    case T_ARRAY:
    case T_OBJECT:
    case T_NULL_VALUE:
    case T_BOOL:
    case T_INT:
    case T_FLOAT:
        return 0;
    }
}

size_t TObject::calc_to_string_length(int escape, T_FLAGS flags) {
    size_t len = 0;
    int i;
    MemberRecord *m;
    char buf[20];
    switch (_type) {
    case T_UNDEFINE :
        return tx_strlen(STR_UNDEFINED);
        break;
    case T_STRING:
        if (escape) {
            return str_len_with_escape(_value.str);
        } else {
            return tx_strlen(_value.str);
        }
        break;
    case T_NULL_VALUE:
        return tx_strlen(STR_NULL);
        break;
    case T_BOOL:
        if (_value.b) {
            return tx_strlen(STR_TRUE);
        } else {
            return tx_strlen(STR_FALSE);
        };
        break;
    case T_INT:
        return tx_int_to_str(buf, _value.i);
        break;
    case T_FLOAT:
        return tx_float_to_str(buf, _value.f, -1);
        break;
    case T_ARRAY:
        for (i = 0; i < _member_array_len; i++) {
            m = &_value.members[i];
            len = len + m->value->calc_to_string_length(true, flags);
            if (m->value->need_quote()) {
                len = len + 2;
            }
            len = len + 1;  // for ","
        };
        len = len + 2; // for [];
        return len;
        break;
    case T_OBJECT:
        for (i = 0; i < _member_array_len; i++) {
            m = &_value.members[i];
            if (m->name == NULL) { continue; };
            if ((flags == T_FLAGS_NONE) || (m->value->flags_match(flags))) {
                len = len + tx_strlen(m->name);
                len = len + m->value->calc_to_string_length(true, flags);
                if (m->value->need_quote()) {
                    len = len + 2;
                };
                len = len + 4;  // for "var":true,
            }
        };
        len = len + 2; // for {};
        return len;
        break;
    }
#ifdef AVR    
    Serial.println(F("Internal error"));
#endif
};

int TObject::put_value_string_to_buf(char *buf, size_t len, bool escape, T_FLAGS flags) {
    size_t pos = 0;
    int i;
    char *str = NULL;
    MemberRecord *m;
    switch (_type) {
    case T_UNDEFINE : str = STR_UNDEFINED; break;
    case T_NULL_VALUE : str = STR_NULL; break;
    case T_STRING   : str = _value.str; break;
    case T_BOOL:
        if (_value.b) {
            str = STR_TRUE;
        } else {
            str = STR_FALSE;
        };
        break;
    case T_INT:
        // todo: if value length > len
        return tx_int_to_str(buf, _value.i);
        break;
    case T_FLOAT:
        return tx_float_to_str(buf, _value.f, -1);
        break;
    case T_ARRAY:
        buf[pos] = '['; pos++;
        for (i = 0; i < _member_array_len; i++) {
            m = &_value.members[i];
            int need_quote = m->value->need_quote();
            if (need_quote) { buf[pos] = '"';  pos++; };
            pos = pos + m->value->put_value_string_to_buf(
                                                          &buf[pos],
                                                          len - pos,
                                                          true,
                                                          flags);
            if (need_quote) { buf[pos] = '"';  pos++; };
            buf[pos] = ',';  pos++;
        };
        if (pos > 2) { // for last ","
            pos--;
        };
        buf[pos] = ']'; pos++;
        buf[pos] = 0;
        break;
    case T_OBJECT:
        buf[pos] = '{'; pos++;
        for (i = 0; i < _member_array_len; i++) {
            m = &_value.members[i];
            if (m->name == NULL) { continue; };
            if ((flags == T_FLAGS_NONE) || (m->value->flags_match(flags))) {
                buf[pos] = '"'; pos++;
                pos = pos + tx_strcpy(&buf[pos], m->name); 
                buf[pos] = '"'; pos++;
                buf[pos] = ':'; pos++;
                int need_quote = m->value->need_quote();
                if (need_quote) { buf[pos] = '"';  pos++; };
                pos = pos + m->value->put_value_string_to_buf(&buf[pos],
                                                              len - pos,
                                                              true, flags);
                if (m->value->need_quote()) {
                    buf[pos] = '"';  pos++;
                };
                buf[pos] = ',';  pos++;
            };
        };
        if (pos > 2) { // for last ","
            pos--;
        };
        buf[pos] = '}'; pos++;
        buf[pos] = 0;
        break;
    };
    if (str != NULL) {
        if (escape) {
            while (str[0] != 0) {
                buf[pos] = '\\';
                switch (str[0]) {
                case '"': 
                    pos++; buf[pos] = '"'; break;
                case '\\':
                    pos++; buf[pos] = '\\'; break;
                case '\b':
                    pos++; buf[pos] = 'b'; break;
                case '\f':
                    pos++; buf[pos] = 'f'; break;
                case '\n':
                    pos++; buf[pos] = 'n'; break;
                case '\r':
                    pos++; buf[pos] = 'r'; break;
                case '\t':
                    pos++; buf[pos] = 't'; break;
                default :
                    buf[pos] = str[0];
                }
                pos++;
                str++;
            }
            buf[pos] = 0;
        } else {
            pos = tx_strcpy(buf, str);
        };
    }
    return pos;
};

void TObject::convert_value_to_string(T_FLAGS flags) {
    size_t len;
    if (_value_string != NULL) {
        _free_string(&_value_string);
    };
    switch (_type) {
    case T_UNDEFINE : _value_string = STR_UNDEFINED; break;
    case T_STRING   : _value_string = _value.str;    break;
    case T_NULL_VALUE     : _value_string = STR_NULL;      break;
    case T_BOOL:
        if (_value.b) {
            _value_string = STR_TRUE;
        } else {
            _value_string = STR_FALSE;
        };
        break;
    case T_INT:
        len = calc_to_string_length(false, flags) + 1;
#ifdef T_DEBUG    
        _update_memory_usage(len);
#endif
        _value_string = (char*)malloc(len);
        tx_int_to_str(_value_string, _value.i);
        break;
    case T_FLOAT: {
        len = calc_to_string_length(false, flags) + 1;
#ifdef T_DEBUG    
        _update_memory_usage(len);
#endif
        _value_string = (char*)malloc(len);
        tx_float_to_str(_value_string, _value.f, -1);
        break;
    }
    case T_ARRAY:  // array and object use same method
    case T_OBJECT:
        len = calc_to_string_length(true, flags) + 1;
#ifdef T_DEBUG    
        _update_memory_usage(len);
#endif
        _value_string = (char*)malloc(len);
        put_value_string_to_buf(_value_string, len, true, flags);
        break;
    }
};

char* TObject::type_name() {
    switch(_type) {
    case T_UNDEFINE: return STR_UNDEFINED;   break; 
    case T_NULL_VALUE:     return STR_NULL;        break; 
    case T_INT:      return STR_TYPE_INT;    break;
    case T_BOOL:     return STR_TYPE_BOOL;   break;
    case T_FLOAT:    return STR_TYPE_FLOAT;  break;
    case T_STRING:   return STR_TYPE_STRING; break;
    case T_ARRAY:    return STR_TYPE_ARRAY;  break;
    case T_OBJECT:   return STR_TYPE_OBJECT; break;
    default:         return STR_INTERNAL_ERROR;
    }
};

char* TObject::to_string() {
    return to_string(T_FLAGS_NONE);
};
char* TObject::to_string(const char *path) {
    if (!has_member(path)) { return NULL; };
    return member(path)->to_string();
};
char* TObject::to_string(T_FLAGS flags) {
    if (changed()) {
        pack();
    };
    if (_value_string == NULL) {
        convert_value_to_string(flags);
    }
    return _value_string;
};
T_BOOLEAN TObject::to_bool() {
    T_BOOLEAN result = T_FALSE;;
    if (_type == T_BOOL) {
        result = _value.b;
    } else if (_type == T_FLOAT) {
        if ((int)_value.f) {
            result = T_TRUE;
        }
    } else if (_type == T_INT) {
        if (_value.i) {
            result = T_TRUE;
        }
    } else if (_type == T_STRING) {
        if (_value.str != NULL) {
            result = T_TRUE;
        }
    } else if ((_type == T_OBJECT) || (_type == T_OBJECT))  {
        if (member_cnt() > 0) {
            result = T_TRUE;
        }
    }
    return result;
};

long TObject::to_int() {
    if (_type == T_INT) {
        return _value.i;
    } else if (_type == T_FLOAT) {
        return (int)_value.f;
    } else if (_type == T_NULL_VALUE) {
        return 0;
    } else if (_type == T_BOOL) {
        if (_value.b == T_TRUE) {
            return 1;
        } else {
            return 0;
        }
    } else if (_type == T_STRING) {
        return strtol(_value_string, NULL, 10);
    } else {
        return 0;
    }
};
double TObject::to_float() {
    if (_type == T_INT) {
        return (double)_value.i;
    } else if (_type == T_FLOAT) {
        return _value.f;
    } else if (_type == T_NULL_VALUE) {
        return 0;
    } else if (_type == T_BOOL) {
        if (_value.b == T_TRUE) {
            return (double)1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
};
void TObject::pack() {
    _flags = _flags & (~T_FLAGS_NEED_PACK);
    int i; int tail = 0;
    if (_value_string != NULL) {
        switch(_type) {
        case T_INT:
        case T_FLOAT:
        case T_ARRAY:
        case T_OBJECT:
            _free_string(&_value_string);
            break;
        case T_UNDEFINE:
        case T_NULL_VALUE:
        case T_BOOL:
        case T_STRING:
            break;
        };
    };
    if (_type == T_ARRAY) {
        for (i = 0; i < _member_array_len; i++) {
            _value.members[i].value->pack();
        }
    } else if (_type == T_OBJECT) {
        for (i = 0; i < _member_array_len; i++) {
            if (_value.members[i].name != NULL) {
                _value.members[i].value->pack();
                if (tail < i) {
                    _value.members[tail].name = _value.members[i].name;
                    _value.members[tail].value = _value.members[i].value;
                    _value.members[i].name = NULL;
                }
                tail++;
            }
        };
        if (tail != _member_array_len - 1) {
            _value.members =
                (MemberRecord*)realloc(_value.members,
                                       sizeof(MemberRecord) * tail);
#ifdef T_DEBUG    
            _update_memory_usage(-(sizeof(MemberRecord) * (_member_array_len - tail)));
#endif
            _member_array_len = tail;
        }
    };
};
size_t TObject::size() {
    int i;
    size_t size = 0;
    size = size + sizeof(TObject);
    if (_value_string != NULL) {
        size = size + tx_strlen(_value_string) + 1;
    }
    
    switch(_type) {
    case T_UNDEFINE: break; 
    case T_NULL_VALUE:     break; 
    case T_INT:      break;
    case T_BOOL:     break;
    case T_FLOAT:    break;
        break;
    case T_STRING:
        if (_value.str != NULL) {
            size = size + tx_strlen(_value.str) + 1;
        }
        break;
    case T_ARRAY:
        for (i = 0; i < _member_array_len; i++) {
            size = size + sizeof(MemberRecord);
            size = size + _value.members[i].value->size();
        };
        break;
    case T_OBJECT:
        for (i = 0; i < _member_array_len; i++) {
            size = size + sizeof(MemberRecord);
            if (_value.members[i].name != NULL) {
                size = size + tx_strlen(_value.members[i].name) + 1;
                size = size + _value.members[i].value->size();
            }
        };
        break;
    };
    return size;
};

void TObject::on(T_VAR_EVENT event,
                 TObject_callback callback, void *context) {
    int i, found = 0;
    if (callback == NULL) { // delete callback
        if (_callback_cnt == 0) { return; };
        for (i = 0; i < _callback_cnt; i++) {
            if (_event_callbacks[i].event == event) {
                found = 1;
            };
            if (found && (i >= 1)) {
                _event_callbacks[i].event    = _event_callbacks[i-1].event;
                _event_callbacks[i].callback = _event_callbacks[i-1].callback;
                _event_callbacks[i].context  = _event_callbacks[i-1].context;
            }
        };
        _callback_cnt--;
        if (_callback_cnt == 0) {
            free(_event_callbacks);
            _event_callbacks = NULL;
        } else {
            _event_callbacks = (T_CALLBACK *)realloc(_event_callbacks, sizeof(T_CALLBACK) * _callback_cnt);
        }
    } else {  //insert/update callback
        for (i = 0; i < _callback_cnt; i++) {
            if (_event_callbacks[i].event == event) {
                found = 1;
                break;
            }
        };
        if (found) {
            _event_callbacks[i].callback = callback;
            _event_callbacks[i].context  = context;
        } else {
            _callback_cnt++;
            _event_callbacks = (T_CALLBACK *)realloc(_event_callbacks, sizeof(T_CALLBACK) * _callback_cnt);

            i = _callback_cnt - 1;
            _event_callbacks[i].event    = event;
            _event_callbacks[i].callback = callback;
            _event_callbacks[i].context  = context;
        }
    }
};

/**
   @todo need comlete
*/
TObject *TObject::get_by_path(const char *path) {
    return get_by_path(path, BY_PATH);
};
TObject *TObject::get_by_path(const char *path, Name_mode mode) {
    if (path == NULL) { return this; };
    if (path[0] == DELI_PATH && path[1] == 0) { return this; };
    int found = 0, i;
    char *name;
    TObject *result = this;
    while (true) {
        if (result->_type != T_OBJECT) { return NULL; };
        int path_pattern_len = 0;
        while (1) {
            if (path[path_pattern_len] == 0) { break; };
            if (path[path_pattern_len] == DELI_PATH) { break; };
            path_pattern_len++;
        }
        found = false;
        int len_name_min = -1;
        int found_index  = -1;
        for (i = 0; i < result->_member_array_len; i++) {
            name = result->_value.members[i].name;
            if (name == NULL) { continue; };
            int len_name = strlen(name);
            if (mode == BY_NAME) {
                if (len_name != path_pattern_len) { continue; };
                if (tx_strncmp(path, name, path_pattern_len) == 0) {
                    found       = true;
                    found_index = i;
                    break;
                }
            } else { // mode == BY_PATH
                if (tx_strncmp(path, name, path_pattern_len) == 0) {
                    if ((found_index < 0)
                        || (len_name < len_name_min)) {
                        found        = true;
                        found_index  = i;
                        len_name_min = len_name;
                    }
                }
            };
        };
        if (!found) { return NULL; };
        result = result->_value.members[found_index].value;
        path = path + path_pattern_len;
        if (path[0] == DELI_PATH) {
            path++;
        } else if (path[0] == 0) {
            return result;
        } else {
            return NULL;
        };
    };    
    return NULL;
};
TObject *TObject::create_value(const char *path, Name_mode mode) {
    int found = 0, i, l1, l2;
    char *name = NULL;
    TObject *obj = NULL;
    TObject *result = this;
    l1 = -1;
    if (mode == BY_PATH) {
        l1 = tx_index(path, DELI_PATH);
    };
    if (l1 < 0) { l1 = tx_strlen(path); };
    while (l1 > 0) {
        if (result->_type != T_OBJECT) {
            obj = new TObject((char*)NULL);

//            char *s = strndup(path, l1);
//            result->set(s, obj);
//            result = obj;
//            free(s);

            char* p = new char[l1+1];
            memset(p,0,l1+1);
            memcpy(p,path,l1);
            result->set(p, obj);
            result = obj;
            delete p;

        } else {
            int index = result->find_member_first_level(path, mode);
            if (index >= 0) { // found
                result = result->_value.members[index].value;
            } else { // not found
                obj = new TObject((char*)NULL);
//                char *s = strndup(path, l1);
//                result->set(s, obj);
//                result = obj;
//                free(s);
                char* p = new char[l1+1];
                memset(p,0,l1+1);
                memcpy(p,path,l1);
                result->set(p, obj);
                result = obj;
                delete p;

            }
        };
        if (mode == BY_PATH) {
            path = path + l1;
            if (path[0] == DELI_PATH) { path++; };
            l1 = tx_index(path, DELI_PATH);
            if (l1 < 0) { l1 = tx_strlen(path); };
        } else {
            l1 = -1;
        }
    };    
    return result;
};
TObject *TObject::assign_by_path(const char *path, TObject *value) {
    set(NULL, NULL);
    TObject *result = get_by_path(path, BY_PATH);
    if (result == NULL) {
        result = create_value(path, BY_PATH);
    }
    result->assign(value);
    return result;
};
int TObject::flags_match(T_FLAGS flags) {
    if (_flags & flags) {
        return 1;
    };
    if (_type != T_OBJECT) {
        return 0;
    };
    int i;
    MemberRecord *m;
    for (i = 0; i < _member_array_len; i++) {
        m = &_value.members[i];
        if (m->name != NULL)  {
            if (m->value->flags_match(flags)) {
                return 1;
            }
        }
    };
    return 0;
};

void TObject::flags_set(T_FLAGS flags) {
    _flags = _flags | flags;
};

void TObject::flags_clear(T_FLAGS flags) {
    _flags = _flags & (!flags);
};


/**
   @todo  Change assign to reference

   @note  assign clone the value now, it's not the best way to do this
*/
TObject *TObject::assign(TObject *value) {
    if (_flags & T_FLAGS_READONLY) { return this; };
    if (value == NULL) {
        set();
        return this;
    }
    int i, cnt;
    char *name;
    switch (value->type()) {
    case T_UNDEFINE : set();                  break;
    case T_NULL_VALUE     : set((char*)NULL);       break;
    case T_BOOL     : set(value->_value.b);   break;
    case T_INT      : set(value->_value.i);   break;
    case T_FLOAT    : set(value->_value.f);   break;
    case T_STRING   : set(value->_value.str); break;
    case T_ARRAY:
        set();
        push((TObject*)NULL);
        cnt = value->member_count();
        for (i = 0; i < cnt; i++) {
            TObject *o = new TObject();
            o->assign(value->get_member_value(i));
            push(o);
        };
        break;
    case T_OBJECT:
        set();
        set(NULL, NULL);
        cnt = value->member_count();
        for (i = 0; i < cnt; i++) {
            name = value->member_name(i);
            int index = find_member_first_level(name, BY_NAME);
            TObject *o;
            if (index >= 0) {
                o = _value.members[index].value;
            } else {
                o = new TObject();
                set(name, o);
            };
            o->assign(value->get_member_value(i));
        };
        break;
    }
    return this;
};

OBJECT_TYPE TObject::type() {
    return _type;
};

int TObject::member_count() {
    int i, cnt = 0;
    switch (_type) {
    case T_OBJECT:
        for (i = 0; i < _member_array_len; i++) {
            if (_value.members[i].name != NULL) {
                cnt++;
            }
        };
        return cnt;
        break;
    case T_ARRAY:
        return _member_array_len;
        break;
    default: 
        return 0;
    }
};

char *TObject::member_name(int index) {
    if (_type != T_OBJECT) { return NULL; };
    if (_flags & T_FLAGS_NEED_PACK) {
        pack();
    };
    if ((index < 0) || (index >= _member_array_len)) {
        return NULL;
    };
    int i, cnt = -1;
    for (i = 0; i < _member_array_len; i++) {
        if (_value.members[i].name == NULL) { continue; };
        cnt++;
        if (cnt == index) { break; };
    };
    if (cnt == index) {
        return _value.members[i].name;
    } else {
        return NULL;
    }
};
TObject *TObject::get_member_value(int index) {
    if ((_type != T_OBJECT) && (_type != T_ARRAY)) {
        return NULL;
    };
    if ((index < 0) || (index >= _member_array_len)) {
        return NULL;
    };
    switch (_type) {
    case T_OBJECT : {
        int i, cnt = -1;
        for (i = 0; i < _member_array_len; i++) {
            if (_value.members[i].name == NULL) { continue; };
            cnt++;
            if (cnt == index) { break; };
        };
        if (cnt == index) {
            return _value.members[i].value;
        } else {
            return NULL;
        }
        break;
    };
    case T_ARRAY :
        return _value.members[index].value;
        break;
    default :
        return NULL;
    }
};

uint8_t TObject::flags() {
    return _flags;
};

void TObject::commit() {
    flags_clear(T_FLAGS_CHANGED);
    if (_type != T_OBJECT) { return; };
    int i;
    MemberRecord *m;
    for (i = 0; i < _member_array_len; i++) {
        m = &_value.members[i];
        if (m->name != NULL)  {
            m->value->commit();
        }
    };
};

int TObject::changed() {
    return flags_match(T_FLAGS_CHANGED);
};

void TObject::to_int(int *i) {
    if (_type == T_INT) {
        *i = _value.i;
    } else if (_type == T_FLOAT) {
        *i = _value.f;
    } else {
        i = NULL;
    }
};

int TObject::member_cnt() {
    int i, cnt = 0;
    switch (_type) {
    case T_OBJECT :
        for (i = 0; i < _member_array_len; i++) {
            if (_value.members[i].name != NULL) {
                cnt++;
            }
        };
        return cnt;
        break;
    case T_ARRAY :
        return _member_array_len;
        break;
    default:
        return 0;
    }
};

TObject *TObject::update_with(TObject *delta, int auto_insert) {
    return update_with(delta, auto_insert, BY_NAME);
}
TObject *TObject::update_with(TObject *delta,
                              int auto_insert,
                              Name_mode mode) {
    if (delta == NULL) { return this; };
    if (delta->type() == T_OBJECT) {
        if (type() != T_OBJECT) {
            if (auto_insert) {
                set((char*)NULL, NULL);
            } else {
                return this;
            };
        };
        int i, n;
        n = delta->member_cnt();
        for (i = 0; i < n; i++) {
            char *name     = delta->member_name(i);
            TObject *value = delta->get_member_value(i);
            TObject *v     = get_by_path(name, mode);
            if (v == NULL) {
                if (auto_insert) {
                    v = new TObject((char*)NULL, NULL);
                    set(name, v);
                    v->update_with(value, auto_insert, mode);
                }
            } else {
                v->update_with(value, auto_insert, mode);
            }
        };
        return this;
    } else if (delta->type() == T_ARRAY) {
        int is_equal = false;
        while (true) {
            if (type() != T_ARRAY) { break; };
            if (member_cnt() != delta->member_cnt()) { break; };
            int i, n;
            n = delta->member_cnt();
            for (i = 0; i < n; i++) {
                TObject *d = delta->member(i);
                TObject *v = member(i);
                v->update_with(d, auto_insert, mode);
            };
            if (i >= n) { is_equal = true; };
            break;
        };
        if (!is_equal) {
            assign(delta);
        }
    } else {
        assign(delta);
        return this;
    }
};

TObject *TObject::clone() {
    return clone(NULL);
};
TObject *TObject::clone(char *jnames_str) {
    TObject *result = new TObject();
    if (jnames_str == NULL) {  // clone all;
        result->assign(this);
    } else {
        char *jnames = jnames_str;
        char *jname  = jnames;
        while (jnames[0] != 0) {
            while ((jnames[0] != 0) && (jnames[0] != ';')) {
                jnames++;
            };
            if (jnames[0] == ';') {
                jnames[0] = 0;
                jnames++;
            }
            if (jnames != jnames_str) { // jname now is a valid jname;
                // jnames for next;
                char *jpath     = jname;
                char *jpath_end = jname;
                while ((jpath_end[0] != ':') && (jpath_end[0] != 0)) {
                    jpath_end++;
                };
                if (jpath_end[0] == 0) {
                    TObject *v = this->get_by_path(jpath, BY_NAME);
                    if (v != NULL) {
                        result->assign_by_path(jpath, v);
                    };
                } else {
                    jpath_end[0] = DELI_PATH;
                    jpath_end++;
                    char *jfield = jpath_end;
                    while (jfield[0] != 0) {
                        char *jfield_end = jfield;
                        while ((jfield_end[0] != 0) && (jfield_end[0] != ':')) {
                            jfield_end++;
                        }
                        char *p1 = jpath_end;
                        char *p2 = jfield;
                        char *p3 = jfield_end;
                        if (jfield_end[0] != 0) {
                            jfield_end++;
                        }
                        jfield = jfield_end;

                        while (p2 < p3) {
                            p1[0] = p2[0];
                            p1++; p2++;
                        };
                        p1[0] = 0;
                        TObject *v = this->get_by_path(jpath, BY_NAME);
                        if (v != NULL) {
                            result->assign_by_path(jpath, v);
                        };
                    };
                };
            };
            jname = jnames;
        }
    }
    return result;
};

TObject *TObject::member(const char *name) {
    return get_by_path(name, BY_PATH);
};
TObject *TObject::member(const char *path, Name_mode mode) {
    return get_by_path(path, mode);
};
TObject *TObject::new_member(const char *path, Name_mode mode) {
    return create_value(path, mode);
};

TObject *TObject::new_member(const char *name) {
    new_member(name, BY_PATH);
};

TObject *TObject::member(int index) {
    return get_member_value(index);
};

int TObject::has_member(const char *name) {
    return has_member(name, T_ANY_VALUE, BY_PATH);
};
int TObject::has_member(const char *name, OBJECT_TYPE type) {
    return has_member(name, type, BY_PATH);
};
int TObject::has_member(const char *path,
                        OBJECT_TYPE type,
                        Name_mode mode) {
    TObject *o = this->member(path, mode);
    if (!o) { return false; };
    if (type == T_ANY_VALUE) {
        return true;
    } else  {
        if (type == o->type()) {
            return true;
        } else {
            return false;
        }
    }

};
