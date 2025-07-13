#include <assert.h>
#include <stdio.h>

#include "cwalker.h"

void example()
{
    char* src = "point(10 20)\n"
                "vector(-30 -40)";

    while (walker_more(src)) {
        char* m = src;
        if (walker_while_range(&src, 'a', 'z')) {
            int len = walker_mark_len(src, m);
            int x = 0, y = 0;
            if (walker_match(&src, "(")
                && walker_int_out(&src, &x)
                && walker_space(&src)
                && walker_int_out(&src, &y)
                && walker_match(&src, ")")) {
                printf("Example match: name=%.*s x=%d y=%d\n", len, m, x, y);
            }
        }
        walker_next(&src);
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

void test_while_range()
{
    char* i = "abc123def";

    assert(walker_while_range(&i, 'a', 'z') == 3);
    assert(strlen(i) == 6);
    assert(walker_while_range(&i, '0', '9') == 3);
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

    assert(walker_next(&i) == 1);
    assert(strlen(i) == 1);
    assert(walker_next(&i) == 1);
    assert(strlen(i) == 0);
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
    example();
    test_float_out();
    test_int_out();
    test_while_range();
    test_space();
    test_match();
    test_matchc();
    test_equal();
    test_equaln();
    test_equalc();
    test_adv();
    test_next();
    test_more();
    test_mark_len();
    return 0;
}