#ifndef WALKER_H
#define WALKER_H

#include <stdlib.h>
#include <string.h>

int walker_space(char** str);
int walker_int_out(char** str, int* out);
int walker_float_out(char** str, float* out);
int walker_int(char** str);
int walker_float(char** str);
int walker_while_range(char** str, char min, char max);
int walker_match(char** str, const char* pattern);
int walker_matchc(char** str, const char c);
int walker_equal(const char* str, const char* pattern);
int walker_equaln(const char* str, const char* pattern, int n);
int walker_equalc(const char* str, const char c);
int walker_adv(char** str, size_t n);
int walker_next(char** str);
int walker_more(const char* str);
int walker_mark_len(const char* str, const char* mark);
int walker_mark_reset(char** str, char* mark);

int walker_int_out(char** str, int* out)
{
    char* m = *str;
    if (walker_int(str)) {
        *out = atoi(m);
        return walker_mark_len(*str, m);
    }
    return 0;
}

int walker_float_out(char** str, float* out)
{
    char* m = *str;
    if (walker_float(str)) {
        *out = atof(m);
        return walker_mark_len(*str, m);
    }
    return 0;
}

int walker_int(char** str)
{
    char* m = *str;
    walker_matchc(str, '-') || walker_matchc(str, '+');
    if (walker_while_range(str, '0', '9'))
        return walker_mark_len(*str, m);
    return walker_mark_reset(str, m);
}

int walker_float(char** str)
{
    char* m = *str;
    walker_matchc(str, '-') || walker_matchc(str, '+');
    if (walker_matchc(str, '.') && !walker_while_range(str, '0', '9'))
        return walker_mark_reset(str, m);
    if (walker_while_range(str, '0', '9'))
        walker_matchc(str, '.') && walker_while_range(str, '0', '9');
    if (walker_matchc(str, 'e') || walker_matchc(str, 'E')) {
        walker_matchc(str, '-') || walker_matchc(str, '+');
        if (!walker_while_range(str, '0', '9'))
            return walker_mark_reset(str, m);
    }
    return walker_mark_len(*str, m);
}

int walker_while_range(char** str, char min, char max)
{
    char* m = *str;
    while (**str >= min && **str <= max)
        walker_next(str);
    return walker_mark_len(*str, m);
}

int walker_space(char** str)
{
    char* m = *str;
    while (walker_more(*str) && **str <= ' ')
        walker_next(str);
    return walker_mark_len(*str, m);
}

int walker_match(char** str, const char* pattern)
{
    int n = walker_equal(*str, pattern);
    return walker_adv(str, n);
}

int walker_matchc(char** str, const char c)
{
    int n = walker_equalc(*str, c);
    return walker_adv(str, n);
}

int walker_equal(const char* str, const char* pattern)
{
    return walker_equaln(str, pattern, strlen(pattern));
}

int walker_equaln(const char* str, const char* pattern, int n)
{
    return strncmp(str, pattern, n) == 0 ? n : 0;
}

int walker_equalc(const char* str, const char c)
{
    return *str == c;
}

int walker_adv(char** str, size_t n)
{
    *str += n;
    return n;
}

int walker_next(char** str)
{
    (*str)++;
    return 1;
}

int walker_more(const char* str)
{
    return *str != '\0';
}

int walker_mark_len(const char* str, const char* mark)
{
    return str - mark;
}

int walker_mark_reset(char** str, char* mark)
{
    *str = mark;
    return 0;
}

#endif
