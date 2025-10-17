// Panagiotis Hadijdoukas, 3456789
// Giorgos Papadimitrou Ilias, 1234567


//Προβλημα READ,WRITE δες το chat: FIFO πρόγραμμα σύγκρισης

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

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
    int nprocs = 4; // default 4 διεργασίες
    double t0, t1;

    // Αν έχω 2 ορίσματα τότε το δεύτερο όρισμα ειναι είναι ο αριθμός διεργασιών
    if (argc == 2) {
        nprocs = atol(argv[1]);
    }

    // Αν έχω 3 ορίσματα τότε το δεύτερο όρισμα ειναι είναι ο αριθμός διεργασιών και το τρίτο ο αριθμός δειγμάτων n
    if (argc == 3) {
        nprocs = atoi(argv[1]);
        n = atoi(argv[2]);
    }

    // Δημιουργεία αρχείου pipe για FIFO με δικαιώματα ανάγνωσης/εγγραφής (0666)
    char *fifo_name = "/tmp/integral_fifo";
    mkfifo(fifo_name, 0666); 
    
    t0 = get_wtime();
    
    // Υπολογισμός δειγμάτων ανά διεργασία
    unsigned long iter_per_proc = n / nprocs;
    
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
            
            // Γράψε το αποτέλεσμα στο αρχείο FIFO
            int file = open(fifo_name, O_RDWR);       //Λύση Προβλήματος: αλλαγη από O_WRONLY σε O_RDWR
            write(file, &local_res, sizeof(local_res));
            close(file);
            
            exit(0);
        }
    }
    
    // Διάβασε αποτελέσματα απο αρχείο FIFO   // Λύση Προβλήματος: ανοιγμα και κλεισμο αρχειου FIFO έξω απο τον βροχο
    double total_res = 0.0;
    int file = open(fifo_name, O_RDONLY);

    for (int i = 0; i < nprocs; i++) {
        double partial_res;
        
        read(file, &partial_res, sizeof(partial_res));
        
        total_res += partial_res;
    }
    close(file);
    
    total_res *= h;
    t1 = get_wtime();
    
    // Περίμενε τις διεργασίες παιδιά να τελειώσουν
    for (int i = 0; i < nprocs; i++) {
        wait(NULL);
    }
    
    // Άδειασε το αρχείο FIFO
    unlink(fifo_name);
    
    printf("Result=%.16f Error=%e Rel.Error=%e Time=%lf seconds Processes=%d\n",
           total_res, fabs(total_res-ref), fabs(total_res-ref)/ref, t1-t0, nprocs);
    
    return 0;
}