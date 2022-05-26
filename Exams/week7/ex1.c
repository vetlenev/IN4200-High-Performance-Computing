#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

// --- a) ---

void a_before() {
    for (i=0; i < (int) sqrt(x); i++) {
        a[i] = 2.3 * x;
        if (i < 10) b[i] = a[i];
    }
}

void a_after() {
    // x is not written to, so it can be shared
    #pragma omp parallel for
    for (i=0; i < (int) sqrt(x); i++) {
        a[i] = 2.3 * x;
        if (i < 10) b[i] = a[i];
    }
}

void b() {
    // Since we don't know iteration schedule, some values of a might not have been assigned desired values,
    // as it might happen that some threads terminate entire loop before other threads have checked the condition
    flag = 0;  
    for (i = 0; (i<n) & (!flag); i++) {
        a[i] = 2.3 * i;
        if (a[i] < b[i]) flag += 1;
    }
}

void c() {
    // Without knowing the content of foo, we are not sure if there are
    // any dependencies between the elements of a[i] => no trivial way to do parallelization
    for (i = 0; i < n; i++)
        a[i] = foo(i);

    // Assuming foo does not manipulate the elements of a,
    // the loop is trivial to parallelize
    #pragma omp parallel for
    for (i = 0; i < n; i++)
        a[i] = foo(i);
}

void d() {
    // Can parallelize of foo not function of a[i+-k], no true dependency
    #pragma omp parallel for
    for (i = 0; i < n; i++) {
        a[i] = foo(i);
        if (a[i] < b[i]) a[i] = b[i];
    }
}

void e() {
    // Not possible to parallelize, since some threads may assign values to a[i],
    // for values of i that are AFTER the first i that breaks the statement, and
    // since threads execute in non-trivial orders, there is no way to circumvent this.
    for (i = 0; i < n; i++) {
        a[i] = foo(i);
        if (a[i] < b[i]) break;
    }
}

void f() {
    dotp = 0;
    // Using firstprivate is not necessary, since the value of the shared variable
    // will be made private when using reduction.
    #pragma omp parallel for reduction(+:dotp)
    for (i = 0; i < n; i++)
        dotp += a[i] * b[i];
}

void g() {
    // Assume k is int.
    // The loop has a backward dependency (read-after-write), and it is not predictable
    // what values have been updated before others
    for (i = k; i < 2*k; i++)
        a[i] = a[i] + a[i-k];

    // To parallelize, the true dependency must be resolved by calculating the dependent elements in a separate for loop
    #pragma omp parallel 
    {
    #pragma omp for
    for (i=k; i<2*k; i++)
        b[i] = a[i-k];

    #pragma omp for
    for (i=k; i<2*k; i++)
        a[i] = a[i] + b[i];

    }
}

void h() {
    #pragma omp parallel
    {
    #pragma omp for
    for (i = k; i < n; i++)
        c[i] = b * a[i-k];
    #pragma omp for
    for (i = k; i < n; i++)
        a[i] = c[i];
    }

    // OR, using wavefronts:
    // Spawn k threads, where each unique thread computes the set of elements that depend on each other could work,
    // but there are no way to explicitly assign specific indices to each thread.
    // Solution: Manually assign indices by start_idx and stride
    #pragma omp parallel 
    { // all variables defined within parallel region is private
    omp_set_num_threads(k);
    r = n % k;
    last_set_threads = k - r;

    my_start = (thread_id+1)*k; // 3, 6, 9, ... for k=3
    my_stop = (n-1) - (k - thread_id+1);
    
    if (last_set_threads > 0 & thread_id < last_set_threads) {
        my_stop = (n-1) - (last_set_threads - thread_id+1);
    }
    else {
        my_stop = (n-1) - last_set_threads - (k - thread_id+1);
    }
    #pragma omp for
    for (i = my_start; i < my_stop; i+=k) // No dependency among threads now!
        a[i] = b * a[i-k];
    }

}