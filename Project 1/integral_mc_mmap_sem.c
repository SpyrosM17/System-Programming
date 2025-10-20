// Spyros Mantadakis, 1100613


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>

double get_wtime(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1.0e-6;
}

inline double f(double x) {
    return sin(cos(x));
}


// Δομή Κοινής μνήμης με σεμαφόρο
typedef struct {
    sem_t semaphore;
    double total_result;
} shared_data_t;

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


    
    // Δημιουργία κοινής μνήμης για δομή με σεμαφόρο και αποτέλεσμα
    shared_data_t *shared_data = mmap(NULL, sizeof(shared_data_t), 
                                     PROT_READ | PROT_WRITE, 
                                     MAP_SHARED | MAP_ANONYMOUS, 
                                     -1, 0);
    
    if (shared_data == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }

    if (sem_init(&shared_data->semaphore, 1, 1) == -1) {
        perror("sem_init failed");
        exit(1);
    }

    // Αρχικοποίησε συνολικό αποτέλεσμα
    shared_data->total_result = 0.0;

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
            
            //Ανανέωσε το κοινό αποτέλεσμα
            sem_wait(&shared_data->semaphore); // Lock
            shared_data->total_result += local_res;
            sem_post(&shared_data->semaphore); // Unlock
            
            exit(0);
        }
    }

    // Περίμενε τις διεργασίες παιδιά να τελειώσουν
    for (int i = 0; i < nprocs; i++) {
        wait(NULL);
    }
    
    double total_res = shared_data->total_result * h;
    t1 = get_wtime();

    
    
    
    
    
    
    // // Άδειασε την κοινή μνήμη και κλείσε σημαφόρους
    sem_destroy(&shared_data->semaphore);
    munmap(shared_data, sizeof(shared_data_t));
    
    printf("Result=%.16f Error=%e Rel.Error=%e Time=%lf seconds Processes=%d\n",
           total_res, fabs(total_res-ref), fabs(total_res-ref)/ref, t1-t0, nprocs);
    
    return 0;
}