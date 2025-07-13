# walker

A simple text parser in c.

## Example

```c
#include <stdio.h>

#include "walker.h"

int main()
{
    char* str = "point(10 20)\n"
                "vector(-30 -40)";

    while (walker_more(str)) {
        char* m = str;
        if (walker_while_range(&str, 'a', 'z')) {
            int len = walker_mark_len(str, m);
            int x = 0, y = 0;
            if (walker_match(&str, "(")
                && walker_int_out(&str, &x)
                && walker_space(&str)
                && walker_int_out(&str, &y)
                && walker_match(&str, ")")) {
                printf("name=%.*s x=%d y=%d\n", len, m, x, y);
            }
        }
        walker_next(&str);
    }

    // Output:
    // name=point x=10 y=20
    // name=vector x=-30 y=-40

    return 0;
}
```