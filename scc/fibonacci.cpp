//Fibonacci Series using Recursion 
#include <iostream> 
#include <omp.h>
#include <math.h>

using namespace std;

int fib_seq(int n)
{
    if (n <= 1)
        return n;
    return fib_seq(n - 1) + fib_seq(n - 2);
}

inline uint64_t fib_iterative(const size_t n) {
    uint64_t fn0 = 0;
    uint64_t fn1 = 1;
    uint64_t fn2 = 0;
    if (n == 0) return fn0;
    if (n == 1) return fn1;

    for (int i = 2; i < (n + 1); i++) {
        fn2 = fn0 + fn1;
        fn0 = fn1;
        fn1 = fn2;
    }
    return fn2;
}

inline uint64_t fib_recursive(uint64_t n) {
    if (n == 0 || n == 1) return(n);
    return(fib_recursive(n - 1) + fib_recursive(n - 2));
}

// https://stackoverflow.com/q/17002380
int fib_recursive_omp(int n) {
    int i, j;
    if (n < 2)
        return n;
    else {
#pragma omp task shared(i) firstprivate(n)
        i = fib_recursive_omp(n - 1);

#pragma omp task shared(j) firstprivate(n)
        j = fib_recursive_omp(n - 2);

#pragma omp taskwait
        return i + j;
    }
}

int fib_recursive_omp_fix(int n) {
    int i, j;
    if (n < 2)
        return n;
    else {
        if (n < 20)
        {
            return(fib_recursive_omp_fix(n - 1) + fib_recursive_omp_fix(n - 2));
        }
        else {
#pragma omp task shared(i) firstprivate(n)
            i = fib_recursive_omp_fix(n - 1);

#pragma omp task shared(j) firstprivate(n)
            j = fib_recursive_omp_fix(n - 2);

#pragma omp taskwait
            return i + j;
        }
    }
}

// https://stackoverflow.com/a/58686146
int fib(int n, int rec_height)
{
    int x = 1, y = 1;
    if (n < 2)
        return n;

    //int ID = omp_get_thread_num();
    //cout << "Thread ID: " << ID << endl;

    if (rec_height > 0)   //Surprisingly without this check parallel code is slower than serial one (I believe it is not needed, I just don't know how to use OpneMP)
    {
        rec_height -= 1;
#pragma omp task shared(x)
        x = fib(n - 1, rec_height);
#pragma omp task shared(y)
        y = fib(n - 2, rec_height);
#pragma omp taskwait
    }
    else {
        x = fib(n - 1, rec_height);
        y = fib(n - 2, rec_height);
    }
    return x + y;
}


int main()
{
    int tot_thread = 8;
    int recDepth = (int) log2f(tot_thread);
    if (((int)pow(2, recDepth)) < tot_thread) recDepth += 1;
    cout << "recDepth: " << recDepth << endl;
    omp_set_max_active_levels(recDepth);
    omp_set_nested(recDepth - 1);
    //omp_set_nested(1);
    //cout << omp_get_nested() << endl;
    omp_set_num_threads(tot_thread);

    int n;
    cout << "Input Fibonacci n: ";
    cin >> n;

    double start_time = omp_get_wtime();
    cout << fib_seq(n) << endl;
    double time = omp_get_wtime() - start_time;
    cout << "Seq time(ms): " << time * 1000 << endl;

    // https://stackoverflow.com/a/22641198
    start_time = omp_get_wtime();
#pragma omp parallel
    {
#pragma omp single
        {
            //cout << fib_recursive_omp(n) << endl;
            cout << fib(n, recDepth) << endl;
        }
    }
    time = omp_get_wtime() - start_time;
    cout << "OpenMP time(ms): " << time * 1000 << endl;

    return 0;
}