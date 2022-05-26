** Exam 2020 **

*** Task 2 ***
- Nested for-loop is not O(N^2) iterations, but is definitely larger than initializatio loop O(N).
- No race condition in nested loop, since iter-variables are privatized

Speedup using more threads:
- Initialization loop linear increase in runtime for more threads, since the work assignment is trivial => minimum overhead
- For nested loop, assigning work chunks among threads is not trivial, and there might be significant overhead. Moreover, if using dynamic scheduling, the workload could be unevenly distributed among threads. Thus, the speedup is expected to decrease the more workers we use (it will still be faster, but slope of the speedup decreases).


*** Task 3b ***
- Actual performance is always less than theoretical performance, because there is always irreducible overhead involved when executing parallel code. Theoretical minimum time assumes optimal flow of data, no cache misses, etc..
- temp variables not counted for in code balance, since it is predefined an resides in register during inner loops.


*** Task 5 - Shared-memory NUMA ***
OpenMP:
- Advantages:
    - All data resides in memory addresses that each thread has access to without resorting to network communication. All data is easily accessed by any thread.
- Disadvantage:
    - Memory is accessed non-uniformly, which means the time it takes for threads to access data residing at different locations varies, and can quickly lead to overhead.
    - Threads need to go via the coherent memory link to access data residing on another socket. Can lead to race condition if multiple threads try to access same data address.

MPI:
- Advantages:
    - Each process communicates safely with neighboring MPI processes, mitigating the danger of race condition
- Disadvantage:
    - There are no distributed network to communicate over, which is the main purpose of MPI to simplify this communication. 