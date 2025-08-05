/* Simple 1972 C program - factorial calculator */

fact(n)
int n;
{
    if(n <= 1)
        return 1;
    return n * fact(n-1);
}

main() {
    auto i, result;
    
    i = 5;
    result = fact(i);
    
    /* In 1972 C, we'd typically use putchar for output */
    putchar('F');
    putchar('a');
    putchar('c');
    putchar('t');
    putchar('(');
    putchar('0' + i);
    putchar(')');
    putchar(' ');
    putchar('=');
    putchar(' ');
    
    /* Simple digit output - only works for small numbers */
    if(result >= 100) {
        putchar('0' + result/100);
        result = result % 100;
    }
    if(result >= 10) {
        putchar('0' + result/10);
        result = result % 10;
    }
    putchar('0' + result);
    putchar('\n');
    
    return 0;
}