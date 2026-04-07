# Systems Programming - Parallel Implementations 🚀

## 📋 Table of Contents

### 1. Multi-processing & IPC Implementation
Calculation of the integral of $f(x) = \sin(\cos(x))$ using the **Monte Carlo** method.
* **IPC Mechanisms:** FIFO (Named Pipes), Message Queues (POSIX/System V), Shared Memory (`mmap`) with and without Semaphores.
* **Analysis:** Performance comparison of IPC mechanisms and the impact of process count on execution time.

### 2. Multi-threaded Implementation (Pthreads)
Parallelization of 16 different integration tasks using the **POSIX threads** (Pthreads) library.
* **Architectures:** * **Internal Parallelization:** Parallelizing within a single task.
    * **Static Round-Robin:** Two-level parallelization assigning tasks to threads.
    * **Dynamic Task Queue:** Dynamic task assignment using a shared queue and Mutexes.
* **Features:** Implementation of thread-safe Random Number Generation (`drand48_r`), race condition prevention, and Load Balancing analysis.

### 3. High-Level Parallelism with OpenMP
Leveraging the **OpenMP** API for shared-memory parallel programming.
* **Monte Carlo:** Implementation using nested parallelism and `reduction` clauses.
* **Machine Learning (Logistic Regression):** Parallelization of Hyperparameter Random Search.
* **Features:** Dynamic scheduling strategies and the use of `erand48` for thread-safe operations.

## 📊 Results & Performance Analysis
All implementations were benchmarked on a system with **6 physical cores**. Key findings include:
* **Scalability:** Maximum **Speedup** is achieved when the number of threads/processes equals the number of physical cores ($T=6$).
* **Oversubscription:** Utilizing more threads than available cores results in performance degradation due to increased **Context Switching** overhead.
* **Efficiency:** The Shared Memory (`mmap`) approach in Project 1 and the hybrid model in Project 2 ($P=2, T=3$) yielded the most optimized execution times.

## 🛠️ Technologies
* **Language:** C
* **Parallelism:** POSIX Threads (Pthreads), OpenMP, `fork()`
* **OS:** Linux / WSL (Ubuntu)
* **Tools:** GCC Compiler, Makefiles

