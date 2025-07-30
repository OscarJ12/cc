/* Ancient C (1972 style) demonstration */
/* Global variables */
sum;
count;
buffer[20];

/* Function declarations */
printstr();
printnum();
fact();
power();

main() {
    auto i, j, k;
    
    i = 3;
    j = fact(i);
    k = power(2, i);
    
    printstr("Factorial of ");
    printnum(i);
    printstr(" is ");
    printnum(j);
    printstr("\n");
    
    printstr("2 raised to ");
    printnum(i);
    printstr(" is ");
    printnum(k);
    printstr("\n");
    
    /* Array demonstration */
    auto arr[5];
    arr[0] = 1;
    arr[1] = 2;
    arr[2] = arr[0] + arr[1];
    
    printstr("Array[2] = ");
    printnum(arr[2]);
    printstr("\n");
    
    /* Pointer operations */
    auto *ptr;
    ptr = &arr[0];
    *ptr = 5;
    
    printstr("Modified array[0] = ");
    printnum(arr[0]);
    printstr("\n");
}

/* Print number function */
printnum(n)
int n;
{
    auto i, digits;
    i = 0;
    
    /* Handle zero case */
    if (n == 0) {
        putchar('0');
        return;
    }
    
    /* Extract digits */
    while (n > 0) {
        buffer[i] = n % 10 + '0';
        n = n / 10;
        i++;
    }
    digits = i;
    
    /* Print in reverse order */
    for (i = digits-1; i >= 0; i--) {
        putchar(buffer[i]);
    }
}

/* String printing function */
printstr(s)
char s[];
{
    auto i;
    i = 0;
    while (s[i] != '\0') {
        putchar(s[i]);
        i++;
    }
}

/* Recursive factorial */
fact(n)
int n;
{
    if (n <= 1)
        return 1;
    return n * fact(n-1);
}

/* Power function */
power(base, exp)
int base, exp;
{
    auto result;
    result = 1;
    
    while (exp > 0) {
        result = result * base;
        exp--;
    }
    return result;
}
