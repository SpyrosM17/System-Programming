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
    int nprocs = 4; // default 4 processes
    double t0, t1;

  // Αν έχω 2 ορίσματα τότε το δεύτερο όρισμα ειναι είναι ο αριθμός διεργασιών
    if (argc == 2) {
        nprocs = atol(argv[1]);
    }

    // Αν έχω 3 ορίσματα τότε το δεύτερο όρισμα ειναι είναι ο αριθμός διεργασιών και το τρίτο ο αριθμός δειγμάτων n
    if (argc == 3) {
        nprocs = atoi(argv[1]);
        n = atol(argv[2]);
    }

   

    // Δημιουργία κοινής μνήμης
    double *shared_results = mmap(NULL, nprocs * sizeof(double), 
                                 PROT_READ | PROT_WRITE, 
                                 MAP_SHARED | MAP_ANONYMOUS, 
                                 -1, 0);
    
    if (shared_results == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }


     

    // Υπολογισμός δειγμάτων ανά διεργασία
    unsigned long iter_per_proc = n / nprocs;

    t0 = get_wtime();
    
    // δημιουργία διεργασιών (παιδιά)
    for (int i = 0; i < nprocs; i++) {
        pid_t pid = fork();
        
        if (pid == 0) { // Αν είναι διεργασία παιδί 
            // υπολόγισε μέρος των δειγμάτων
            unsigned long start_iter = i * iter_per_proc;
            unsigned long end_iter = (i == nprocs-1) ? n : start_iter + iter_per_proc;
            unsigned long local_iter = end_iter - start_iter;
            
            // Διαφορετικός σπόρος
            srand48(getpid() + i);
            
            double local_res = 0.0;
            for (unsigned long j = 0; j < local_iter; j++) {
                double xi;
                xi = drand48();
                local_res += f(xi);
            }
            

            //Γράψε το αποτέλεσμα σε συγκεκριμένη θέση της κοινής μνήμης
            shared_results[i] = local_res;
            
            exit(0);
        }
    }
    
    // Περίμενε τις διεργασίες παιδιά να τελειώσουν
    for (int i = 0; i < nprocs; i++) {
        wait(NULL);
    }
    
    // Άθροιση όλων των μερικών αποτελεσμάτων
    double total_res = 0.0;
    for (int i = 0; i < nprocs; i++) {
        total_res += shared_results[i];
    }


    
    
    total_res *= h;

    
    
    t1 = get_wtime();
    

    
    // Άδειασε την κοινή μνήμη
    munmap(shared_results, nprocs * sizeof(double));
    
    printf("Result=%.16f Error=%e Rel.Error=%e Time=%lf seconds Processes=%d\n",
           total_res, fabs(total_res-ref), fabs(total_res-ref)/ref, t1-t0, nprocs);
    
    return 0;
}