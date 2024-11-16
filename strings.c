#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <stdint.h>

#include "strings.h"

struct String_S
{
    size_t size;
    size_t len;
    char *chars;
} ;

size_t size_of_utf8(char *s)
{
    if ((uint8_t)s[0] <= 0x7F) return 1;
    if ((uint8_t)s[0] >= 0xC0 && (uint8_t)s[0] <= 0xDF) return 2;
    if ((uint8_t)s[0] >= 0xE0 && (uint8_t)s[0] <= 0xEF) return 3;
    if ((uint8_t)s[0] >= 0xF0 && (uint8_t)s[0] <= 0xF7) return 4;
    return 0;
}

size_t count_utf8(char *s)
{
    size_t count = 0;
    size_t pos = 0;

    while (s[pos])
    {
        pos += size_of_utf8(s + pos*sizeof(char));
        ++count;
    }

    return count;
}

int utf8_cmp(char *s1, char *s2, size_t len)
{
    size_t size1 = 0;
    size_t size2 = 0;

    size_t pos = 0;

    for (size_t i = 0; i < len; ++i)
    {
        size1 = size_of_utf8(s1 + pos);
        size2 = size_of_utf8(s2 + pos);

        if (size1 > size2) return 1;
        if (size1 < size2) return -1;

        int cmp = memcmp(s1 + pos, s2 + pos, size1);
        if (cmp) return cmp;

        pos += size1;
    }
    return 0;
}

size_t size_string(String_T *s) {return !s ? SIZE_MAX : s->size;}
size_t len_string(String_T *s) {return !s ? SIZE_MAX : s->len;}
bool equal_string(String_T *s1, String_T *s2)
{return s1->size == s2->size && s1->len == s2->len && !memcmp(s1->chars, s2->chars, s1->size);}


String_T *new_string(char *s)
{
    if (!s) return NULL;

    String_T *new = calloc(1, sizeof(String_T));
    if (!new)
    {
        perror("new_string: allocation failure\n");
        exit(1);
    }

    new->size = strlen(s);
    new->len = count_utf8(s);

    new->chars = calloc(new->size, sizeof(char));
    if (!new->chars)
    {
        perror("new_string: allocation failure\n");
        exit(1);
    }

    memcpy(new->chars, s, new->size);

    return new;
}

void print_string(String_T *s)
{
    if (!s) return ;

    if (s->size > INT_MAX)
    {
        perror("print_string: String too big to print");
        exit(1);
    }

    printf("%.*s\n", (int) s->size, s->chars);
}

bool equal_string(String_T *s1, String_T *s2){return s1->size == s2->size && !memcmp(s1->chars, s2->chars, s1->size);}

String_T *dup_string(String_T *s)
{
    if (!s) return NULL ;

    String_T *new = calloc(1, sizeof(String_T));
    if (!new) {
        perror("dup_string: allocation failure\n");
        exit(1);
    }

    new->size = s->size;
    new->len = s->len;

    new->chars = calloc(new->size, sizeof(char));
    if (!new->chars) {
        perror("dup_string: allocation failure\n");
        exit(1);        
    }

    memcpy(new->chars, s->chars, new->size);

    return new;
}

String_T *concat_string(String_T *s1, String_T *s2)
{
    if (!s1 || !s2) return NULL ;
    if (!s1->size) return dup_string(s2);
    if (!s2->size) return dup_string(s1);

    String_T *new = calloc(1, sizeof(String_T));
    if (!new)
    {
        perror("concat_string: allocation failure\n");
        exit(1);
    }

    new->size = s1->size + s2->size;
    new->len = s1->len + s2->len;
    new->chars = calloc(new->size, sizeof(char));
    if (!new->chars)
    {
        perror("concat_string: allocation failure\n");
        exit(1);
    }

    memcpy(new->chars, s1->chars, s1->size);
    memcpy(new->chars + s1->size * sizeof(char), s2->chars, s2->size);

    return new;
}

size_t find_string(String_T *s1, String_T *s2)
{
    if (!s1 || !s2 || !s1->len || s1->len > s2->len) return SIZE_MAX;

    for (size_t p = 0, i = 0; i + s1->len <= s2->len; ++i, p += size_of_utf8(s2->chars + p))
    {
        int cmp = utf8_cmp(s1->chars, s2->chars + p, s1->len);
        if (!cmp) return i;
    }
    
    return SIZE_MAX ;
}

String_T *take_string(String_T *s, size_t n)
{
    if (!s) return NULL;
    if (!n) return new_string("");
    if (s->len <= n) return dup_string(s);

    String_T *new = calloc(1, sizeof(String_T));
    if (!new)
    {
        perror("take_string: allocation failure\n");
        exit(1);
    }

    for (size_t i = 0; i < n; ++i) new->size += size_of_utf8(s->chars + new->size);
    new->len = n;

    new->chars = calloc(new->size, sizeof(char));
    if (!new->chars)
    {
        perror("take_string: allocation failure\n");
        exit(1);
    }

    memcpy(new->chars, s->chars, new->size);

    return new;
}

String_T *drop_string(String_T *s, size_t n)
{
    if (!s) return NULL;
    if (!n) return dup_string(s);
    if (s->len <= n) return new_string("");

    String_T *new = calloc(1, sizeof(String_T));
    if (!new)
    {
        perror("drop_string: allocation failure\n");
        exit(1);
    }

    new->len = s->len - n;

    char *start = s->chars;
    for (size_t i = 0; i < n; ++i) start += size_of_utf8(start);
    for (size_t j = 0; j < new->len; ++j) new->size += size_of_utf8(start + new->size);

    new->chars = calloc(new->size, sizeof(char));
    if (!new->chars)
    {
        perror("drop_string: allocation failure\n");
        exit(1);
    }

    memcpy(new->chars, start, new->size);

    return new;
}

int compare_string(String_T *s1, String_T *s2)
{
    if (!s1 || !s2) return 0;

    size_t min_len = (s1->len < s2->len ? s1->len : s2->len);
    int cmp = utf8_cmp(s1->chars, s2->chars, min_len);

    if (cmp) return cmp;
    if (s1->len > s2->len) return 1;
    if (s1->len < s2->len) return -1;
    return 0;
}

String_T *substring(String_T *s, size_t start, size_t len)
{
    if (!s) return NULL;
    if (start > s->len || !len) return new_string("");
    if (start + len > s->len) len = s->len - start;

    String_T *new = calloc(1, sizeof(String_T));
    if (!new) {
        perror("substring: Allocation failure\n");
        exit(1);
    }

    new->len = len;

    new->size = 0;
    size_t start_pos = 0;
    for (size_t i = 0; i < start; ++i) start_pos += size_of_utf8(s->chars + start_pos);
    for (size_t j = 0; j < len; ++j) new->size += size_of_utf8(s->chars + start_pos + new->size);

    new->chars = calloc(new->size, sizeof(char));
    if (!new->chars) {
        perror("substring: Allocation failure\n");
        exit(1);
    }

    memcpy(new->chars, s->chars + start, new->size);

    return new;
}

String_T *to_upper(String_T *s)
{
    if (!s) return NULL ;
    if (!s->len) return new_string("");

    String_T *new = calloc(1, sizeof(String_T));
    if (!new) {
        perror("to_upper: Allocation failure\n");
        exit(1);
    }

    new->len = s->len;
    new->size = s->size;

    new->chars = calloc(new->size, sizeof(char));
    if (!new->chars) {
        perror("to_upper: Allocation failure\n");
        exit(1);
    }

    for(size_t i = 0; i < new->size; ++i)
        new->chars[i] = toupper(s->chars[i]);
    
    return new;
}

String_T *to_lower(String_T *s)
{
    if (!s) return NULL ;
    if (!s->len) return new_string("");

    String_T *new = calloc(1, sizeof(String_T));
    if (!new) {
        perror("to_upper: Allocation failure\n");
        exit(1);
    }

    new->len = s->len;
    new->size = s->size;
    new->chars = calloc(new->size, sizeof(char));
    if (!new->chars) {
        perror("to_lower: Allocation failure\n");
        exit(1);
    }

    for(size_t i = 0; i < new->size; ++i)
        new->chars[i] = tolower(s->chars[i]);
    
    return new;
}

String_T *reverse_string(String_T *s)
{
    if (!s) return NULL;
    if (!s->len) return new_string("");

    String_T *new = calloc(1, sizeof(String_T));
    if (!new)
    {
        perror("reverse_string: Allocation failure\n");
        exit(1);
    }

    new->len = s->len;
    new->size = s->size;

    new->chars = calloc(new->size, sizeof(char));
    if (!new->chars)
    {
        perror("reverse_string: Allocation failure\n");
        exit(1);
    }

    size_t p = 0;
    size_t csize = 0;
    for (size_t i = 0; i < s->len; ++i)
    {
        csize = size_of_utf8(s->chars + p);
        memcpy(new->chars + new->size - p - csize, s->chars + p, csize);
        p += csize;
    }

    return new;
}


