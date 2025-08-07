# cwalker

General purpose text parser in C.

## Example

This example shows how to collect the names and the arguments of the statements.

```c
#include <stdio.h>
#include "walker.h"

int main()
{
    char* str = "point(10 20)\n"
                "vector(-30 -40)";

    while (walker_more(str)) {
        auto m = walker_mark(str);
        if (walker_while_range(&str, 'a', 'z')) {
            int len = walker_mark_len(str, m);
            int x = 0, y = 0;
            if (walker_match(&str, "(")
                && walker_int_out(&str, &x)
                && walker_space(&str)
                && walker_int_out(&str, &y)
                && walker_match(&str, ")")) {
                printf("name=%.*s, x=%d, y=%d\n", len, m, x, y);
            }
        }
        walker_next(&str);
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

int expr(char** str, int* out);
int term(char** str, int* out);
int fact(char** str, int* out);

int main()
{
    char* str = "(6-1)*4*2+(1+3)*(16/2)";

    int out = 0;
    int ok = expr(&str, &out);
    printf("(%s) %d\n", ok ? "true" : "false", out);
    // (true) 72
    return 0;
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
