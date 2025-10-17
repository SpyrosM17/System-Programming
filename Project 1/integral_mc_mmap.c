// Spyros Mantadakis, 1100613

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

double get_wtime(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1.0e-6;
}

inline double f(double x) {
    return sin(cos(x));
}

int main(int argc, char *argv[]) {
    double a = 0.0;
    double b = 1.0;
    unsigned long n = 24e7;
    const double h = (b-a)/n;
    const double ref = 0.73864299803689018;
    int nprocs = 4;
    
    if (argc >= 2) {
        n = atol(argv[1]);
    }
    if (argc >= 3) {
        nprocs = atoi(argv[2]);
    }

    double t0 = get_wtime();
    
    // Create shared memory array for results
    double *shared_results = mmap(NULL, nprocs * sizeof(double), 
                                 PROT_READ | PROT_WRITE, 
                                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    
    unsigned long iter_per_proc = n / nprocs;
    
    for (int i = 0; i < nprocs; i++) {
        pid_t pid = fork();
        
        if (pid == 0) { // Child process
            unsigned long start_iter = i * iter_per_proc;
            unsigned long end_iter = (i == nprocs-1) ? n : start_iter + iter_per_proc;
            unsigned long local_iter = end_iter - start_iter;
            
            srand48(getpid() + i);
            
            double local_res = 0.0;
            for (unsigned long j = 0; j < local_iter; j++) {
                double xi = drand48();
                local_res += f(xi);
            }
            
            // Store result in shared memory
            shared_results[i] = local_res;
            exit(0);
        }
    }
    
    // Wait for all children
    for (int i = 0; i < nprocs; i++) {
        wait(NULL);
    }
    
    // Sum all partial results
    double total_res = 0.0;
    for (int i = 0; i < nprocs; i++) {
        total_res += shared_results[i];
    }
    
    total_res *= h;
    double t1 = get_wtime();
    
    // Cleanup shared memory
    munmap(shared_results, nprocs * sizeof(double));
    
    printf("Result=%.16f Error=%e Rel.Error=%e Time=%lf seconds Processes=%d\n",
           total_res, fabs(total_res-ref), fabs(total_res-ref)/ref, t1-t0, nprocs);
    
    return 0;
}