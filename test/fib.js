
function fib(n)
{
    if (n == 0 || n == 1)
        return 0;
    else if (n == 2)
        return 1;

    return fib(n - 1) + fib(n - 2);
}

console.log(fib(10));


