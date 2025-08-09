#include <assert.h>
#include <stdio.h>
#include <string.h>

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

int jsn(char** inp, char** out);
int obj(char** inp, char** out);
int arr(char** inp, char** out);
int key(char** inp, char** out);
int str(char** inp, char** out);

void example_json()
{
    char* str = "{ \"name\": \"John\", \"country\": [ \"USA\", \"BRAZIL\" ] }";

    char out[64] = "";
    char* pout = out;
    jsn(&str, &pout);

    assert(strcmp(out, "\"John\"; \"USA\"; \"BRAZIL\"; ") == 0);
}

int jsn(char** inp, char** out)
{
    walker_space(inp);
    return obj(inp, out) || arr(inp, out) || str(inp, out);
}

int obj(char** inp, char** out)
{
    if (walker_matchc(inp, '{')) {
        if (key(inp, out)) {
            while (walker_matchc(inp, ',') && key(inp, out)) { }
        }
        walker_space(inp);
        return walker_matchc(inp, '}');
    }
    return 0;
}

int arr(char** inp, char** out)
{
    if (walker_matchc(inp, '[')) {
        if (jsn(inp, out)) {
            while (walker_matchc(inp, ',') && jsn(inp, out)) { }
        }
        walker_space(inp);
        return walker_matchc(inp, ']');
    }
    return 0;
}

int str(char** inp, char** out)
{
    auto m = walker_mark(*inp);
    if (walker_string(inp, '"')) {
        if (out) {
            int l = walker_mark_len(*inp, m);
            strncpy(*out, m, l);
            *out += l;
            strncpy(*out, "; ", 2);
            *out += 2;
        }
        return 1;
    }
    return 0;
}

int key(char** inp, char** out)
{
    walker_space(inp);
    return walker_string(inp, '"') && walker_matchc(inp, ':') && jsn(inp, out);
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

void test_0n()
{
    char* i = "abcabc";
    assert(walker_0n(walker_match(&i, "x")) == 1);
    assert(strlen(i) == 6);
    assert(walker_0n(walker_match(&i, "abc")) == 1);
    assert(strlen(i) == 0);
}

void test_1n()
{
    char* i = "abcABC";
    assert(walker_1n(walker_match(&i, "x")) == 0);
    assert(strlen(i) == 6);
    assert(walker_1n(walker_match(&i, "abc")) == 1);
    assert(strlen(i) == 3);
}

void test_peek()
{
    char* i = "ab";
    assert(walker_peekm(&i, walker_matchc(&i, 'x') && walker_matchc(&i, 'y')) == 0);
    assert(strlen(i) == 2);
    assert(walker_peekm(&i, walker_matchc(&i, 'a') && walker_matchc(&i, 'b')) == 1);
    assert(strlen(i) == 2);
}

void test_undo()
{
    char* i = "1+2";
    assert(walker_undom(&i, walker_matchc(&i, '1') && walker_matchc(&i, '+') && walker_matchc(&i, '3')) == 0);
    assert(strlen(i) == 3);
    assert(walker_undom(&i, walker_matchc(&i, '1') && walker_matchc(&i, '+') && walker_matchc(&i, '2')) == 1);
    assert(strlen(i) == 0);
}

void test_string()
{
    char* i = "\"\"";

    assert(walker_string(&i, '"') == 2);
    assert(strlen(i) == 0);

    i = "\"a\"";
    assert(walker_string(&i, '"') == 3);
    assert(strlen(i) == 0);

    i = "\"a\\\"b\\\"c\"";
    assert(walker_string(&i, '"') == 9);
    assert(strlen(i) == 0);

    i = "\"a\\\nb\\\"c\"";
    assert(walker_string(&i, '"') == 9);
    assert(strlen(i) == 0);

    i = "\"a";
    assert(walker_string(&i, '"') == 0);
    assert(strlen(i) == 2);

    i = "''";
    assert(walker_string(&i, '\'') == 2);
    assert(strlen(i) == 0);

    i = "'a'";
    assert(walker_string(&i, '\'') == 3);
    assert(strlen(i) == 0);

    i = "'a\\'bc'";
    assert(walker_string(&i, '\'') == 7);
    assert(strlen(i) == 0);
}

void test_line()
{
    char* i = "\n";
    assert(walker_line(&i) == 1);
    assert(strlen(i) == 0);

    i = "a\n";
    assert(walker_line(&i) == 2);
    assert(strlen(i) == 0);

    i = "aaa\n";
    assert(walker_line(&i) == 4);
    assert(strlen(i) == 0);

    i = "";
    assert(walker_line(&i) == 0);
    assert(strlen(i) == 0);

    i = "abc";
    assert(walker_line(&i) == 3);
}

void test_until()
{
    char* i = "abc123";

    assert(walker_until(&i, "abc") == 0);
    assert(strlen(i) == 6);

    assert(walker_until(&i, "123") == 3);
    assert(strlen(i) == 3);
}

void test_untilc()
{
    char* i = "abc0";

    assert(walker_untilc(&i, 'a') == 0);
    assert(strlen(i) == 4);

    assert(walker_untilc(&i, '0') == 3);
    assert(strlen(i) == 1);
}

void test_untilr()
{
    char* i = "abc0";

    assert(walker_untilr(&i, 'a', 'a') == 0);
    assert(strlen(i) == 4);

    assert(walker_untilr(&i, 'a', 'z') == 0);
    assert(strlen(i) == 4);

    assert(walker_untilr(&i, '0', '0') == 3);
    assert(strlen(i) == 1);

    i = "abc9";
    assert(walker_untilr(&i, '0', '9') == 3);
    assert(strlen(i) == 1);
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
    assert(walker_equal(i, "ona") == 0);
    assert(strlen(i) == 3);
    assert(walker_equal(i, "two") == 0);
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
    example_json();
    example();
    test_float_out();
    test_int_out();
    test_0n();
    test_1n();
    test_peek();
    test_undo();
    test_string();
    test_line();
    test_until();
    test_untilc();
    test_untilr();
    test_not();
    test_notc();
    test_notr();
    test_whiler();
    test_space();
    test_match();
    test_matchc();
    test_matchr();
    test_equal();
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
