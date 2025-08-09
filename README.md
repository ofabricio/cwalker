# cwalker

General purpose text parser in C.

## Example

This example shows how to collect the names and the arguments of the statements.

```c
#include <stdio.h>
#include "walker.h"

int main()
{
    char* inp = "point(10 20)\n"
                "vector(-30 -40)";

    while (walker_more(inp)) {
        auto m = walker_mark(inp);
        if (walker_whiler(&inp, 'a', 'z')) {
            int len = walker_mark_len(inp, m);
            int x = 0, y = 0;
            if (walker_matchc(&inp, '(')
                && walker_int_out(&inp, &x)
                && walker_space(&inp)
                && walker_int_out(&inp, &y)
                && walker_matchc(&inp, ')')) {
                printf("name=%.*s, x=%d, y=%d\n", len, m, x, y);
            }
        }
        walker_next(&inp);
    }

    // Output:
    // name=point, x=10, y=20
    // name=vector, x=-30, y=-40

    return 0;
}
```

## Example: math expression

This example shows how to parse a math expression.
Note that this example is not production-ready.

```c
#include <stdio.h>
#include "walker.h"

int expr(char** inp, int* out);
int term(char** inp, int* out);
int fact(char** inp, int* out);

int main()
{
    char* inp = "(6-1)*4*2+(1+3)*(16/2)";

    int out = 0;
    int ok = expr(&inp, &out);
    printf("(%s) %d\n", ok ? "true" : "false", out);
    // (true) 72
    return 0;
}

int expr(char** inp, int* out)
{
    if (term(inp, out)) {
        int r;
        if (walker_matchc(inp, '+') && expr(inp, &r)) {
            *out += r;
        } else if (walker_matchc(inp, '-') && expr(inp, &r)) {
            *out -= r;
        }
        return 1;
    }
    return 0;
}

int term(char** inp, int* out)
{
    if (fact(inp, out)) {
        int r;
        if (walker_matchc(inp, '*') && term(inp, &r)) {
            *out *= r;
        } else if (walker_matchc(inp, '/') && term(inp, &r)) {
            *out /= r;
        }
        return 1;
    }
    return 0;
}

int fact(char** inp, int* out)
{
    return (walker_matchc(inp, '(') && expr(inp, out) && walker_matchc(inp, ')'))
        || walker_int_out(inp, out);
}
```

## Example: json

This example shows how to parse a json and get all string values.
Note that this example is not production-ready.

```c
#include <stdio.h>
#include <string.h>
#include "walker.h"

int jsn(char** inp, char** out);
int obj(char** inp, char** out);
int arr(char** inp, char** out);
int key(char** inp, char** out);
int str(char** inp, char** out);

int main()
{
    char* inp = "{ \"name\": \"John\", \"country\": [ \"USA\", \"BRAZIL\" ] }";

    char out[64] = "";
    char* pout = out;
    jsn(&inp, &pout);
    printf("%s\n", out);
    // "John"; "USA"; "BRAZIL";
    return 0;
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
        int l = walker_mark_len(*inp, m);
        strncpy(*out, m, l);
        *out += l;
        strncpy(*out, "; ", 2);
        *out += 2;
        return 1;
    }
    return 0;
}

int key(char** inp, char** out)
{
    walker_space(inp);
    return walker_string(inp, '"') && walker_matchc(inp, ':') && jsn(inp, out);
}
```

## Introduction

This library implements a Mark-Match-Move mechanism,
which is a simple way to parse and collect tokens.

All matching operations are based on this pattern:

```cpp
auto m = walker_mark(str);
if (walker_match(&str, "something")) {
    int l = walker_mark_len(str, m);
}
```

First a mark is set to the current position.
Then the parser advances on a match.
Finally, the mark is used to extract the matched token on success.
It could be used to move the parser back to the marked position if needed.

That's all about it.
