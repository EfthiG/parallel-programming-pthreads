# Parallel Programming with POSIX Threads

A collection of four C programming experiments exploring multithreading, synchronization, memory-layout effects, and reader–writer concurrency using **POSIX Threads (Pthreads)**. Developed as a **collaborative university assignment** for a Parallel Systems course.

## Overview

| Exercise | Topic | Main focus |
| --- | --- | --- |
| 1.1 | Monte Carlo estimation of π | Serial vs. multithreaded computation |
| 1.2 | Atomic operations vs. mutexes | Synchronization overhead on a shared counter |
| 1.3 | False sharing | Cache-line padding and memory contention |
| 1.4 | Reader–writer synchronization | Custom reader-priority and writer-priority locks |

### 1.1 — Parallel Monte Carlo estimation of π

Estimates π by generating random points in a square and counting those that fall inside the unit circle. Compares serial execution with a Pthreads implementation that divides the work across threads and combines their local counts.

### 1.2 — Atomic operations vs. mutexes

Compares two ways to synchronize updates to a shared counter: C atomic operations and a `pthread_mutex_t`. Execution time is measured for different thread counts and workloads.

### 1.3 — False sharing and cache-line padding

Investigates the performance impact of false sharing when threads update different elements of a shared array. Compares adjacent counters with counters separated by padding based on a 64-byte cache-line assumption.

### 1.4 — Reader–writer locks

Runs concurrent search, insertion, and deletion operations on a sorted linked list. The project includes a custom reader–writer lock built using Pthreads mutexes and condition variables, with separate **reader-priority** and **writer-priority** policies.

## Technologies

- **C (C99)**
- **POSIX Threads (Pthreads)**
- C atomic operations (`stdatomic.h`)
- GCC and Make
- Bash for automated experiments
- Python, NumPy, and Matplotlib for plotting

## Project structure

```text
.
├── src/
│   ├── 1_1.c
│   ├── 1_2.c
│   ├── 1_3.c
│   ├── 1_4/
│   │   ├── custom_rw_lock.c
│   │   ├── my_rand.c
│   │   └── pth_ll_rwl.c
│   └── common.c
├── include/
│   ├── common.h
│   ├── custom_rw_lock.h
│   ├── my_rand.h
│   └── timer.h
├── Makefile
├── tests.sh
└── graphs.py
```

Additional supporting materials and figures may be included separately. The `bin/`, `build/`, and `results/` directories are created during compilation and experimentation.

## Build and run

**Requirements:** Linux or WSL with GCC, Make, and Pthreads support.

From the repository root:

```bash
make all
```

The Makefile builds four executables in `bin/`.

Example commands:

```bash
# 1.1: 4 threads, 1 million Monte Carlo throws
./bin/1_1 4 1000000

# 1.2: 4 threads, 1 million iterations; 0 = atomics, 1 = mutex
./bin/1_2 4 1000000 0
./bin/1_2 4 1000000 1

# 1.3: 4 threads, 1 million iterations; 0 = no padding, 1 = padding
./bin/1_3 4 1000000 0
./bin/1_3 4 1000000 1

# 1.4: 4 threads, 1000 initial keys, 50000 operations,
#      95% searches, 5% inserts, 0 = writer priority, 1 = reader priority
./bin/1_4 4 1000 50000 0.95 0.05 1
```

To remove compiled files:

```bash
make clean
```

## Benchmarking and visualization

The repository includes `tests.sh`, which rebuilds the programs and runs repeated experiments with **1, 2, 4, 8, and 16 threads** across the four exercises. Results are written to files under `results/`, and `graphs.py` produces performance plots.

> **Note:** The automated benchmark uses large workloads and may take considerable time. It also clears existing files in `results/` before running. Run it only when you want to regenerate the full experiment data.

```bash
bash tests.sh
```

To regenerate plots from existing results, install the Python plotting dependencies and run:

```bash
python3 -m pip install numpy matplotlib
python3 graphs.py
```

Performance depends on the processor, operating system, thread count, workload, and scheduling behavior. No universal speedup is implied by the included experiments.

## Academic context

This repository presents work completed **collaboratively** as part of a university Parallel Systems course.
