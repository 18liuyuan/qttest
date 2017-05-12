#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "src/include/cwlib.h"
// #include "console.h"

//#ifdef AVR

char* index(const char* s, int c) {
    int i = 0, len = strlen(s);
    while (i < len) {
        if (s[i] == c) {
            return (char*)&s[i];
        };
        i++;
    };
    return 0;
};

char* rindex(const char* s, const char c) {
    size_t i;
    i = strlen(s);
    if (i == 0) return NULL;
    i--;
    while ((s[i] != c) && (i >= 0)) {
        i--;
    };
    if (i < 0) return NULL;
    return (char*)&s[i];
};
//#endif


void tick_to_time_str(time_t *time, char *buf, int len) {
    // 2015-01-27 11:35:09
    if (len < 20) { return; };
    if (!*time) { buf[0] = 0; return; };
    struct tm *result = localtime(time);
    result->tm_year = result->tm_year + 1900;
    result->tm_mon  = result->tm_mon  + 1;
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
            result->tm_year,
            result->tm_mon,
            result->tm_mday,
            result->tm_hour,
            result->tm_min,
            result->tm_sec);
};

time_t str_to_time_tick(char *time) {
    time = get_token_first(time, '-');
    char *year   = time; time = get_token_next(time, '-'); 
    char *month  = time; time = get_token_next(time, ' '); 
    char *day    = time; time = get_token_next(time, ':'); 
    char *hour   = time; time = get_token_next(time, ':'); 
    char *minute = time; time = get_token_rest(time);
    char *second = time; 
    struct tm _time;
    _time.tm_year = strtol(year,   NULL, 10) - 1900;
    _time.tm_mon  = strtol(month,  NULL, 10) - 1;
    _time.tm_mday = strtol(day,    NULL, 10);
    _time.tm_hour = strtol(hour,   NULL, 10);
    _time.tm_min  = strtol(minute, NULL, 10);
    _time.tm_sec  = strtol(second, NULL, 10);
    _time.tm_isdst= 0;
    return mktime(&_time);
};


char* get_token_first(char* buf, const char deli) {
    if (buf == NULL) return NULL;
    size_t end = 0;
    while ((buf[end] != deli) && (buf[end] != 0)) end++;
    buf[end] = 0;
    return buf;
};
char* get_token_next(char* buf, const char deli) {
    if (buf == NULL) return NULL;
    size_t pos = (int)strlen(buf);
    pos++;
    size_t end = pos;
    while ((buf[end] != deli) && (buf[end] != 0)) end++;
    buf[end] = 0;
    return &buf[pos];
};
char* get_token_rest(char* buf) {
    if (buf == NULL) return NULL;
    size_t pos = (int)strlen(buf);
    pos++;
    return &buf[pos];
};



Buffer::Buffer() {
    _buf = NULL;
    _size = 0;
};
Buffer::Buffer(const char *s) {
    _buf = NULL;
    _size = 0;
    if (s) {
        _buf = strdup(s);
        _size = strlen(s) + 1;
    }
};
Buffer::~Buffer() {
    if (_buf) { free(_buf); _buf = NULL; };
};

char *Buffer::append(const char *s) {
    if (!s) { return _buf; };
    if (_buf == NULL) {
        _buf = strdup(s);
        _size = strlen(s) + 1;
    } else {
        int len = strlen(s);
        char *p  = (char*)realloc(_buf, _size + len);
        if (p) {
            _buf = p;
            strcpy(&_buf[_size - 1], s);
            _size = _size + len;
        }
    };
    return _buf;
};

char *Buffer::append(const char *s1, const char *s2) {
    if (s1) { append(s1); };
    if (s2) { append(s2); };
    return _buf;
};

char *Buffer::to_string() {
    return _buf;
};



List::List() {
    _q = NULL;
    _length = 0;
};
List::~List() {
    if (_q) { free(_q); };
};
int List::length() {
    return _length;
};
int List::count() {
    int i;
    int cnt = 0;
    for (i = 0; i < _length; i++) {
        if (_q[i]) { cnt++; };
    }
    return cnt;
};
void *List::at(int index) {
    if (index > _length) { return NULL; };
    return _q[index];
};
void *List::get(int index) {
    int i = 0;
    int cnt = 0;
    while (i < _length) {
        if (_q[i]) {
            if (cnt == index) {
                return _q[i];
            }
            cnt++;
        }
        i++;
    }
    return NULL;
};
void *List::del(int index) {
    int i = 0;
    int cnt = 0;
    while (i < _length) {
        if (_q[index]) {
            if (cnt == index) {
                void *e = _q[i];
                _q[i] = NULL;
                pack();
                return e;
            }
            cnt++;
        }
        i++;
    }
    return NULL;
};
int List::push(void *ptr) {
    _q = (void**)realloc(_q, sizeof(void*) * _length + 1);
    _q[_length] = ptr;
    _length++;
    return _length -1;
};
void List::pack() {
    int i = 0;
    int j = 0;
    while (i < _length) {
        if (_q[i]) {
            _q[j] = _q[i];
            i++; j++;
        } else {
            i++;
        }
    }
    _length = j;
    if (i != j) {
        _q = (void**)realloc(_q, sizeof(void*) * _length);
    };
};

