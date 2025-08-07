#include <assert.h>
#include <stdio.h>

#include "cwalker.h"

int expr(char** str, int* out);
int term(char** str, int* out);
int fact(char** str, int* out);

void example_expr()
{
    char* str = "(6-1)*4*2+(1+3)*(16/2)";
    int out = 0;
    int ok = expr(&str, &out);
    assert(ok == 1);
    assert(out == 72);
}

int expr(char** str, int* out)
{
    if (term(str, out)) {
        int r;
        if (walker_matchc(str, '+') && expr(str, &r)) {
            *out += r;
        } else if (walker_matchc(str, '-') && expr(str, &r)) {
            *out -= r;
        }
        return 1;
    }
    return 0;
}

int term(char** str, int* out)
{
    if (fact(str, out)) {
        int r;
        if (walker_matchc(str, '*') && term(str, &r)) {
            *out *= r;
        } else if (walker_matchc(str, '/') && term(str, &r)) {
            *out /= r;
        }
        return 1;
    }
    return 0;
}

int fact(char** str, int* out)
{
    return (walker_matchc(str, '(') && expr(str, out) && walker_matchc(str, ')'))
        || walker_int_out(str, out);
}

void example()
{
    char* str = "point(10 20)\n"
                "vector(-30 -40)";

    while (walker_more(str)) {
        auto m = walker_mark(str);
        if (walker_whiler(&str, 'a', 'z')) {
            int len = walker_mark_len(str, m);
            int x = 0, y = 0;
            if (walker_match(&str, "(")
                && walker_int_out(&str, &x)
                && walker_space(&str)
                && walker_int_out(&str, &y)
                && walker_match(&str, ")")) {
                printf("example(): name=%.*s, x=%d, y=%d\n", len, m, x, y);
            }
        }
        walker_next(&str);
    }
}

#define ASSERT_OUT(NAME, INP, LEN, OUT)  \
    {                                    \
        char* inp = INP;                 \
        out = 0;                         \
        assert(NAME(&inp, &out) == LEN); \
        assert(out == OUT);              \
    }

void test_float_out()
{
    float out;
    ASSERT_OUT(walker_float_out, "1", 1, 1.0f);
    ASSERT_OUT(walker_float_out, "+1", 2, 1.0f);
    ASSERT_OUT(walker_float_out, "-10", 3, -10.0f);
    ASSERT_OUT(walker_float_out, "1.5", 3, 1.5f);
    ASSERT_OUT(walker_float_out, ".35", 3, .35f);
    ASSERT_OUT(walker_float_out, "-.35", 4, -.35f);
    ASSERT_OUT(walker_float_out, "+.35", 4, .35f);
    ASSERT_OUT(walker_float_out, "4e2", 3, 400.0f);
    ASSERT_OUT(walker_float_out, "4.e2", 4, 400.0f);
    ASSERT_OUT(walker_float_out, "4.3e2", 5, 430.0f);
    ASSERT_OUT(walker_float_out, "4.3E2", 5, 430.0f);
    ASSERT_OUT(walker_float_out, "4.3e+2", 6, 430.0f);
    ASSERT_OUT(walker_float_out, "4.3e-2", 6, 0.043f);
    ASSERT_OUT(walker_float_out, "4.3.2", 3, 4.3f);
    ASSERT_OUT(walker_float_out, "4.3e", 0, 0);
    ASSERT_OUT(walker_float_out, "4.3e-", 0, 0);
    ASSERT_OUT(walker_float_out, ".e", 0, 0);
    ASSERT_OUT(walker_float_out, "..2", 0, 0);
}

void test_int_out()
{
    int out;
    ASSERT_OUT(walker_int_out, "2", 1, 2);
    ASSERT_OUT(walker_int_out, "+3", 2, 3);
    ASSERT_OUT(walker_int_out, "45", 2, 45);
    ASSERT_OUT(walker_int_out, "-678", 4, -678);
    ASSERT_OUT(walker_int_out, "-", 0, 0);
}

void test_not()
{
    char* i = "aNot";

    assert(walker_not(&i, "a") == 0);
    assert(strlen(i) == 4);
    assert(walker_not(&i, "Not") == 1);
    assert(strlen(i) == 3);
    assert(walker_not(&i, "Not") == 0);
    assert(strlen(i) == 3);
}

void test_notr()
{
    char* i = "a1";

    assert(walker_notr(&i, 'a', 'a') == 0);
    assert(strlen(i) == 2);
    assert(walker_notr(&i, 'a', 'z') == 0);
    assert(strlen(i) == 2);
    assert(walker_notr(&i, '0', '9') == 1);
    assert(strlen(i) == 1);
}

void test_notc()
{
    char* i = "ab";

    assert(walker_notc(&i, 'a') == 0);
    assert(strlen(i) == 2);
    assert(walker_notc(&i, 'x') == 1);
    assert(strlen(i) == 1);
}

void test_whiler()
{
    char* i = "abc123def";

    assert(walker_whiler(&i, 'a', 'z') == 3);
    assert(strlen(i) == 6);
    assert(walker_whiler(&i, '0', '9') == 3);
    assert(strlen(i) == 3);
}

void test_space()
{
    char* i = "   a";

    assert(walker_space(&i) == 3);
    assert(strlen(i) == 1);
}

void test_match()
{
    char* i = "onetwo";

    assert(walker_match(&i, "one") == 3);
    assert(strlen(i) == 3);
    assert(walker_match(&i, "three") == 0);
    assert(strlen(i) == 3);
    assert(walker_match(&i, "two") == 3);
    assert(strlen(i) == 0);
}

void test_matchc()
{
    char* i = "one";

    assert(walker_matchc(&i, 'o') == 1);
    assert(strlen(i) == 2);
    assert(walker_matchc(&i, 'n') == 1);
    assert(strlen(i) == 1);
    assert(walker_matchc(&i, 'x') == 0);
    assert(strlen(i) == 1);
    assert(walker_matchc(&i, 'e') == 1);
    assert(strlen(i) == 0);
}

void test_matchr()
{
    char* i = "am";

    assert(walker_matchr(&i, '0', '9') == 0);
    assert(strlen(i) == 2);
    assert(walker_matchr(&i, 'a', 'a') == 1);
    assert(strlen(i) == 1);
    assert(walker_matchr(&i, 'a', 'z') == 1);
    assert(strlen(i) == 0);
}

void test_equal()
{
    char* i = "one";

    assert(walker_equal(i, "one") == 3);
    assert(strlen(i) == 3);
    assert(walker_equal(i, "two") == 0);
    assert(strlen(i) == 3);
}

void test_equaln()
{
    char* i = "one";

    assert(walker_equaln(i, "one", 3) == 3);
    assert(strlen(i) == 3);
    assert(walker_equaln(i, "one", 2) == 2);
    assert(strlen(i) == 3);
    assert(walker_equaln(i, "one", 0) == 0);
    assert(strlen(i) == 3);
}

void test_equalc()
{
    char* i = "a";

    assert(walker_equalc(i, 'a') == 1);
    assert(strlen(i) == 1);
    assert(walker_equalc(i, 'b') == 0);
    assert(strlen(i) == 1);
}

void test_equalr()
{
    char* i = "a";

    assert(walker_equalr(i, '0', '9') == 0);
    assert(strlen(i) == 1);
    assert(walker_equalr(i, 'a', 'a') == 1);
    assert(strlen(i) == 1);
    assert(walker_equalr(i, 'a', 'z') == 1);
    assert(strlen(i) == 1);
}

void test_any()
{
    char* i = "hi";

    assert(walker_any(&i) == 1);
    assert(strlen(i) == 1);
    assert(walker_any(&i) == 1);
    assert(strlen(i) == 0);
    assert(walker_any(&i) == 0);
    assert(strlen(i) == 0);
}

void test_adv()
{
    char* i = "hello";

    assert(walker_adv(&i, 0) == 0);
    assert(strlen(i) == 5);
    assert(walker_adv(&i, 2) == 2);
    assert(strlen(i) == 3);
    assert(walker_adv(&i, 3) == 3);
    assert(strlen(i) == 0);
}

void test_next()
{
    char* i = "hi";
    walker_next(&i);
    assert(strlen(i) == 1);
    walker_next(&i);
    assert(strlen(i) == 0);
}

void test_curr()
{
    assert(walker_curr("a") == 'a');
    assert(walker_curr("") == '\0');
}

void test_more()
{
    assert(walker_more("a") == 1);
    assert(walker_more("") == 0);
}

void test_mark_len()
{
    char* i = "four";

    char* m = i;
    walker_next(&i);
    walker_next(&i);
    walker_next(&i);
    assert(walker_mark_len(i, m) == 3);
    walker_next(&i);
    assert(walker_mark_len(i, m) == 4);
}

int main()
{
    example_expr();
    example();
    test_float_out();
    test_int_out();
    test_not();
    test_notc();
    test_notr();
    test_whiler();
    test_space();
    test_match();
    test_matchc();
    test_matchr();
    test_equal();
    test_equaln();
    test_equalc();
    test_equalr();
    test_any();
    test_adv();
    test_next();
    test_curr();
    test_more();
    test_mark_len();
    return 0;
}