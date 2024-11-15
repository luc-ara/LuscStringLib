#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stddef.h>
#include <ctype.h>

#include "strings.h"

size_t size_of_utf8(char *s)
{
    if (((s[0] >> 3) & 0x1F) == 0x1E) return 4;
    if (((s[0] >> 4) & 0x0F) == 0x0E) return 3;
    if (((s[0] >> 5) & 0x07) == 0x06) return 2;
    return 1;
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

size_t size_string(String_T *s) {return !s ? SIZE_MAX : s->size;}
size_t len_string(String_T *s) {return !s ? SIZE_MAX : s->len;}

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

    if (s->len > INT_MAX)
    {
        perror("print_string: String too big to print");
        exit(1);
    }

    printf("%.*s\n", (int) s->len, s->chars);
}

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
    if (!s1 || !s2 || !s1->size || s1->size > s2->size) return SIZE_MAX;

    for (size_t p = 0; p + s1->size <= s2->size; ++p)
        if (!memcmp(s2->chars + p*sizeof(char), s1->chars, s1->size))
            return p;
    
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

    new->len = n;
    new->chars = calloc(n, sizeof(char));
    if (!new->chars)
    {
        perror("take_string: allocation failure\n");
        exit(1);
    }

    memcpy(new->chars, s->chars, n);

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
    new->chars = calloc(new->len, sizeof(char));
    if (!new->chars)
    {
        perror("drop_string: allocation failure\n");
        exit(1);
    }

    memcpy(new->chars, s->chars + n*sizeof(char), new->len);

    return new;
}

int compare_string(String_T *s1, String_T *s2)
{
    if (!s1 || !s2) return 0;

    size_t min_len = (s1->len < s2->len ? s1->len : s2->len);
    int cmp = memcmp(s1->chars, s2->chars, min_len);

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
    new->chars = calloc(len, sizeof(char));
    if (!new->chars) {
        perror("substring: Allocation failure\n");
        exit(1);
    }

    memcpy(new->chars, s->chars + start*sizeof(char), len);

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
    new->chars = calloc(new->len, sizeof(char));
    if (!new->chars) {
        perror("substring: Allocation failure\n");
        exit(1);
    }

    for(size_t i = 0; i < new->len; ++i)
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
    new->chars = calloc(new->len, sizeof(char));
    if (!new->chars) {
        perror("substring: Allocation failure\n");
        exit(1);
    }

    for(size_t i = 0; i < new->len; ++i)
        new->chars[i] = tolower(s->chars[i]);
    
    return new;
}




