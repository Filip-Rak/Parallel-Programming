# Overview
This repository contains solutions to lab exercises for the **Parallel and Distributed Programming** course during the fifth semester at **AGH University of Krakow**. The exercises cover various parallel programming techniques and paradigms using C and Java. Each lab focuses on a specific topic.

![Coffee shop simulation – clients ordering and drinking coffee](media/coffe_shop.gif)

*Simulation of a multi-threaded coffee shop where 300 clients (threads) take a mug (limited resource), use one of 4 coffee machines (limited resource), drink their coffee (wait), and repeat the process multiple times before leaving. While not waiting for resources, threads perform 'work' (simulated with busy-waiting and addition operation).*

---

# Technologies and Topics Covered

## C
  - **Clone and Fork**: Process creation and management, focusing on understanding process isolation and resource sharing.
  - **Pthreads**: Multithreading with POSIX threads, emphasizing synchronization mechanisms like **mutexes**, **barriers**, and
    **condition variables** to handle **RAW (Read-After-Write)**, **WAR (Write-After-Read)**, **and WAW (Write-After-Write)** dependencies.
  - **OpenMP**: Parallel programming with OpenMP directives for shared memory systems, optimizing performance through **loop parallelization**, **task scheduling**, and **data sharing**.
  - **OpenMPI**: Message Passing Interface (MPI) for distributed computing, focusing on **inter-process communication**, **synchronization**, and **scalability**.

## Java
  - **Thread and Runnable**: Basic concepts of multithreading, including thread lifecycle management and synchronization.
  - **Executor and ThreadPool**: Efficient thread pooling and task execution, ensuring optimal resource utilization and task scheduling.
  - **Callable and Future**: Tasks that return results, with asynchronous retrieval using **Future**, enabling better control over task execution and results.
  - **RecursiveTask and RecursiveAction**: Parallel programming with Java's Fork/Join framework for **divide-and-conquer tasks**, focusing on **work-stealing algorithms** and **load balancing**.

## Key Focus Areas
- **Synchronization**: Ensuring thread safety and avoiding race conditions through proper use of synchronization primitives like **mutexes**, **locks**, and **barriers**.
- **Performance Optimization**: Writing efficient parallel code by minimizing overhead, reducing contention, and maximizing resource utilization.
- **Dependency Management**: Understanding and resolving **RAW**, **WAR**, and **WAW** dependencies to ensure correct and predictable execution.
- **Code Safety**: Designing robust and maintainable parallel programs that are free from deadlocks, livelocks, and data corruption.

---

# Code Quality Note

Some of the exercises in the repository involve modifying or extending code provided as part of the course. 
While I strive to write clean, well-structured, and maintainable code, the starting code for certain exercises may not reflect my personal coding style or standards. 
These files are included as-is to provide context for the solutions and to demonstrate how the exercises were approached.

---

# Installation and Compilation
- All C code was originally written on Linux and exercises using **Clone**, **Fork** and **Pthreads** will not work outside of it as they rely on POSIX-specific APIs.
- Each lab directory involving C code should have a working makefile, making use of GCC compiler (MPICC in case of OpenMPI).
- OpenMPI may require a separate installation of MPICC not provided in the repository.
- For Java, only source code is provided and can be compiled with any IDE or directly from the command line.
