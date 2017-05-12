#ifndef CWLIB_H
#define CWLIB_H

#include <time.h>
#include <stdlib.h>

//#define AVR
//#ifdef AVR
char* index(const char* s, int c);
char* rindex(const char* s, const char c);  // changed to strrchr
//#endif

void tick_to_time_str(time_t *time, char *buf, int len);
time_t str_to_time_tick(char *time);

extern char* get_token_first(char* buf, const char deli);
extern char* get_token_next(char* buf, const char deli);
extern char* get_token_rest(char* buf);

class Buffer {
 private:
    char *_buf;
    int   _size;
 public:
    Buffer();
    Buffer(const char *s);
    ~Buffer();
    char *append(const char *s);
    char *append(const char *s1, const char *s2);
    char *to_string();
};

class List {
 private:
    void**_q;
    int _length;
 public:
    List();
    ~List();
    int length();
    int count();
    void *at(int index);
    void *get(int index);  // for none null element
    void pack();
    int push(void *ptr);
    void *del(int index);
};


#endif
