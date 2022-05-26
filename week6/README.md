### Exercise 2

20 independent tasks, 2 equal workers (spends same time on a task).
- Total of $$\sum_{i=1}^{N=20} i$$ hours.
- Distribute heavy tasks as evenly as possible among the two workers, so that if worker one takes on the heaviest task, worker two takes on second most heavy task.
    - When worker two finishes (it will be first), it takes on the third most heavy task, and when first worker finishes, it takes the fourth most heavy task, and so on ...

Formula (T: total time for tasks (sequentially), W: nr workers):
- If $$T\%W = 0$$, tasks are evenly distributed among workers, and parallelized time to execute all tasks is $$T_p = T/W$$.
- If $$T\%W \ne 0$$, we get remainder tasks that must be distributed among some of the workers. Parallelized time is: $$T_p = T \\ W + T\% W$$.
- For $$T = \sum_{i=1}^{N=20} i = 21\cdot 20 / 2 = 210$$ and $W=2$ we have that tasks can be evenly distributed among the workers. Shortest possible time to execute all tasks in parallell is then: $$T_p = T/W = 210/2 = 105$$, i.e. half the original time.

Three equal workers:
- Can use same formula since we still don't get remainder:
- $$T_p = T/W = 210/3 = 70$$.


### Exercise 3

This problem is not fully parallelized, since workers have to wait on each other in certain cases. The tasks are instead executed concurrently.

- If a layer has nr tasks less than or equal to nr workers, the layer can be fully parallelized, and runtime for this layer equals runtime of one task.
- Once nr tasks exceeds nr workers, one or more task in layer is idle and one have to (at least) double the runtime of the layer.
    - If nr tasks is more than 2 times the nr workers, the runtime of this layer will be tripled (if $$N \% W \ne 0$$ there will be remainder tasks, and some workers can start on next layer will others finish remaining tasks in current layer).


### Exercise 4

Domain: M x N
Blocks: S x T

- If $$M \% S = 0$$ / $$N \% T = 0$$, blocks match perfectly in row/column-direction (no remainder columns/rows). All block sizes are equal and the work is distributed as evenly as possible among the (block)-workers.
- If one of dimensions get remainder ($$X \% Y \ne 0$$), the block dimensions must be changed to ensure as evenly workload as possible among workers.

Assume no remainder:
- Blocks are evenly distributed over entire domain - all workers get same workload

Assume remainder in one direction, no remainder in other:
- In direction with no remainder, block size fit perfectly into domain
- In direction with remainder, there will be $$r = X \% Y$$ nodes not occupied by a block. Since we have $$r < Y$$, we can assign the remaining nodes evenly out to $$r$$ of the $$Y$$ blocks, one node to each of the $$r$$ blocks. 


### Exercise 5

