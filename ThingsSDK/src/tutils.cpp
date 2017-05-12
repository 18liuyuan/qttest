#ifdef AVR
#include <arduino.h>
#endif
#include <stdlib.h>
#include "src/include/tutils.h"

int tx_index(const char *str, const char ch) {
    if (str == NULL) { return -1; };
    int index = 0;
    while (true) {
        if (str[index] == 0) {
            return -1;
        } else if (str[index] == ch) {
            return index;
        } else {
            index++;
        }
    };
};

size_t tx_strcpy(char *dst, const char *src) {
    if ((src == NULL) || (dst == NULL)) { return 0; };
    size_t len = 0;
    while(src[0] != 0) {
        dst[0] = src[0];
        dst++; src++; len++;
    }
    dst[0] = 0;
    return len;
};
size_t tx_strncpy(char * dst, const char * src, size_t n) {
    if ((src == NULL) || (dst == NULL)) { return 0; };
    size_t len = 0;
    while((src[0] != 0) && (len < n)) {
        dst[0] = src[0];
        dst++; src++; len++;
    }
    dst[0] = 0;
    return len;
};
size_t tx_strlen(const char *src) {
    if (src == NULL) { return 0; };
    size_t len = 0;
    while(src[0] != 0) {
        src++; len++;
    }
    return len;
};
int  tx_strcmp(const char *s1, const char *s2) {
    if ((s1 == NULL) || (s2 == NULL)) {
        return 1;
    };
    while(s1[0] == s2[0]) {
        if (s1[0] == 0) {
            return 0;
        }
        s1++; s2++;
    }
    if (s1[0] > s2[0]) {
        return 1;
    } else {
        return -1;
    }
};

int tx_strncmp(const char *s1, const char *s2, size_t len) {
    if ((s1 == NULL) || (s2 == NULL)) {
        return 1;
    };
    while(s1[0] == s2[0]) {
        if (len == 0) { return 0; };
        s1++; s2++;
        len--;
    }
    if (len == 0) { return 0; };
    if (s1[0] > s2[0]) {
        return 1;
    } else {
        return -1;
    }
};

short int tx_int_to_str(char *buf, long i) {
    short int end = 0, start = 0, len = 0;
    if (i == 0) {
        buf[0] = '0'; buf[1] = 0; return 1;
    }
    if (i < 0) {
        buf[end] = '-';
        end++;
        i = -i;
        start = 1;
    };
    while (i > 0) {
        buf[end] = (i % 10) + '0';
        end++;
        i = i / 10;
    };
    buf[end] = 0;
    len = end;
    end--;
    char ch;
    while (start < end) {
        ch = buf[start];
        buf[start] = buf[end];
        buf[end] = ch;
        start++;
        end--;
    }
    return len;
};

short int tx_float_to_str(char *buf, double f, int n) {
    long i; 
    i = f;
    short int len = tx_int_to_str(buf, i);
    if (f < 0) {
        f = -f;
    }
    i = f;
    f = f - i;
    if (f <= 0) {
        return len;
    };
    buf[len] = '.'; len++;
    if (n < 0) { n = 2; };
    while ((n > 0) && (f > 0)) {
        f = f * 10;
        i = f;
        buf[len] = '0' + i; len++;
        f = f - i;
        n--;
    };
    buf[len] = 0;
    return len;
};

