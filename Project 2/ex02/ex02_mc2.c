// Spyros Mantadakis, 1100613

#define _GNU_SOURCE  // Για χρήση reentrant drand48_r, srand48_r
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <stdint.h>
#include <pthread.h>  // Προσθηκη POSIX threads
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ΠΡΟΣΘΗΚΗ Mutex για συγχρονισμό των printf από τα εξωτερικά νήματα
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static double get_wtime(void) {
  struct timeval t; gettimeofday(&t, NULL);
  return (double)t.tv_sec + (double)t.tv_usec * 1.0e-6;
}

/* Integrands (heavier but with elementary antiderivatives) */
static inline double f_log1px2(double x)   { return log(1.0 + x*x); }
static inline double f_sqrt1px2(double x)  { return sqrt(1.0 + x*x); }
static inline double f_lnsqrt(double x)    { return log(x) * sqrt(x); }    // requires x>0
static inline double f_exp_sin(double x)   { return exp(-x) * sin(x); }

/* Function IDs for reference computation */
typedef enum { FID_LOG1PX2, FID_SQRT1PX2, FID_LNSQRT, FID_EXP_SIN } fid_t;

/* Task description */
typedef struct {
  const char *name; 
  fid_t fid;
  double (*f)(double);
  double a, b;
} task_t;


// Δομή για το πέρασμα attributes στο κάθε εσωτερικο νήμα
typedef struct {
  int thread_id; // Αναγνωριστικό εσωτερικού νήματος
  int task_id;   // Αναγνωριστικό task
  unsigned long long thread_samples; // δείγματα νήματος
  double (*f)(double);  // συνάρτηση προς εκτέλεση απο το εσωτερικο νημα
  double a;  
  double b;
  double partial_sum;   // μερικό άθροισμα
} Inner_thread_attr_t;


// Δομή για το πέρασμα attributes στο κάθε εξωτερικο νήμα
typedef struct {
    int outer_id;           // Αναγνωριστικό εξωτερικού νήματος
    int total_outer_threads; // Συνολικός αριθμός εργατών (P)
    int inner_threads_T;     // Αριθμός εσωτερικών νημάτων (T)
    unsigned long long n;   // Συνολικά δείγματα ανά task
    const task_t *tasks;    // Pointer στον πίνακα των tasks
    int ntasks;             // Πλήθος tasks
} outer_thread_attr_t;

//Συναρτηση υλοποιησης monte carlo integration σε μερος δειγματων που εκτελει καθε εσωτερικο νημα
void * integrate_threaded_mc(void *args)  {
  Inner_thread_attr_t *data = (Inner_thread_attr_t *)args; //Αποθηκευση ορισματων στην μεταβλητη δομης εσωτερικου νηματος data
  
  struct drand48_data buffer;  //Δημιουργια thread-safe state buffer 

  // Συνδυασμος task id και thread id για τυχαιο seed
  long seed = (long)(data->task_id * 1000 + data->thread_id);
  // Παραγωγή τυχαίου αριθμού στο  με thread-safe τρόπο
  srand48_r(seed, &buffer);
  double width = data->b - data->a; 
  double x; //Μεταβλητη αποθηκευσης τυχαιου αριθμου
  double sum=0.0;
  
  //Μερικη αθροιση για το μερος των διεγματων του νηματος
  for (unsigned long long i = 0; i < data->thread_samples; ++i) {

        drand48_r(&buffer, &x); 

        double xi = data->a + width * x;
        sum += data->f(xi);
    }

    data->partial_sum = sum; //αποθηκευση μερικου αθροισματος  στο ορισμα του εσωτερικου νηματος
    return NULL;
}





/* Exact references (elementary closed forms) */
static int compute_ref(fid_t fid, double a, double b, double *ref_out) {
  switch (fid) {
    case FID_LOG1PX2: {   // ∫ ln(1+x^2) dx = x ln(1+x^2) - 2x + 2 arctan x
      double Fb =  b*log(1.0+b*b) - 2.0*b + 2.0*atan(b);
      double Fa =  a*log(1.0+a*a) - 2.0*a + 2.0*atan(a);
      *ref_out = Fb - Fa; return 1;
    }
    case FID_SQRT1PX2: {  // ∫ sqrt(1+x^2) dx = 0.5*( x*sqrt(1+x^2) + asinh x )
      double Fb = 0.5 * ( b*sqrt(1.0+b*b) + asinh(b) );
      double Fa = 0.5 * ( a*sqrt(1.0+a*a) + asinh(a) );
      *ref_out = Fb - Fa; return 1;
    }
    case FID_LNSQRT: {    // ∫ x^{1/2} ln x dx = x^{3/2} * ( (2/3) ln x - 4/9 ) , requires a,b>0
      if (a <= 0.0 || b <= 0.0) return 0;
      double Fb = pow(b,1.5) * ( (2.0/3.0)*log(b) - 4.0/9.0 );
      double Fa = pow(a,1.5) * ( (2.0/3.0)*log(a) - 4.0/9.0 );
      *ref_out = Fb - Fa; return 1;
    }
    case FID_EXP_SIN: {   // ∫ e^{-x} sin x dx = -0.5 e^{-x} (sin x + cos x)
      double F = -0.5 * exp(-b) * (sin(b) + cos(b))
                 +0.5 * exp(-a) * (sin(a) + cos(a));
      *ref_out = F; return 1;
    }
  }
  return 0;
}




// Συνάρτηση Νηματος Εργάτη
void * outer_worker(void *args) {
    outer_thread_attr_t *my_args = (outer_thread_attr_t *)args; //Αποθηκευση ορισματων στην μεταβλητη my_args
    int P = my_args->total_outer_threads; //αριθμος εξωτερικων νηματων
    int T = my_args->inner_threads_T;     //αριθμος εσωτερικων νηματων
    unsigned long long n = my_args->n;    //αριθμος δειγματων
    
    //κατανομή (Round Robin)
    for (int k = 0; k < my_args->ntasks; ++k) {
        if (k % P != my_args->outer_id) {
            continue; 
        }

        const task_t *t = &my_args->tasks[k];
        double t0 = get_wtime();

        //Δημιουργια πινακα εσωτερικων νηματων και πινακα των ορισματων τους
        pthread_t inner_Thread[T];
        Inner_thread_attr_t Inner_Thread_attr[T];

        unsigned long long samples_per_thread = n / T; //ομοιμορφη κατανομη των δειγματων σε καθε εσωτερικο νημα
        unsigned long long remainder = n % T; // υπολοιπα δειγματα

        //Αντιγραφη στοιχειων του τρεχον task στον πινακα των attributes του εσωτερικου νηματος
        for(int i = 0; i < T; i++){
            Inner_Thread_attr[i].thread_id = i;
            Inner_Thread_attr[i].task_id = k;
            Inner_Thread_attr[i].thread_samples = samples_per_thread;
            Inner_Thread_attr[i].f = t->f;
            Inner_Thread_attr[i].a = t->a;
            Inner_Thread_attr[i].b = t->b;
            Inner_Thread_attr[i].partial_sum = 0.0;

            //  προσθεσε το υπολοιπο στο μερος δειγματων του τελευταιο εσωτερικου νηματος
             if (i == T - 1) { 
                Inner_Thread_attr[i].thread_samples += remainder;
             }

            // Δημιουργια εσωτερικου νήματος
            pthread_create(&inner_Thread[i], NULL, integrate_threaded_mc, (void*)&Inner_Thread_attr[i]);
        }

        double total_sum = 0.0;
        for (int i = 0; i < T; i++) {
            pthread_join(inner_Thread[i], NULL); //Αναμονη εσωτερικων νηματων
            total_sum += Inner_Thread_attr[i].partial_sum; //Υπολογισμος συνολικου αθροισματος απο τα μερικα αθροισματα των εσωτερικων νηματων.
        }

        // Τελικός υπολογισμός Monte Carlo
        double width = t->b - t->a;
        double res = (width / (double)n) * total_sum;
        double t1 = get_wtime();

        
        double ref; int has_ref = compute_ref(t->fid, t->a, t->b, &ref);
        const double tiny = 1e-14;

        // Κλείδωμα για καθαρή εκτύπωση
        pthread_mutex_lock(&mutex);

        if (has_ref) {
            double abs_err = fabs(res - ref);
            if (fabs(ref) > tiny) {
                double rel_err = abs_err / fabs(ref);
                printf("[%-15s] [a=%.10g b=%.10g] n=%llu seed=%d  Result=%.16f  Error=%e  Rel.Error=%e  Time=%.6f s\n",
                       t->name, t->a, t->b, (unsigned long long)n, k, res, abs_err, rel_err, t1 - t0);
            } else {
                printf("[%-15s] [a=%.10g b=%.10g] n=%llu seed=%d  Result=%.16f  Error=%e  Rel.Error=NA  Time=%.6f s\n",
                       t->name, t->a, t->b, (unsigned long long)n, k, res, abs_err, t1 - t0);
            }
        } else {
            printf("[%-15s] [a=%.10g b=%.10g] n=%llu seed=%d  Result=%.16f  Error=NA  Rel.Error=NA  Time=%.6f s\n",
                   t->name, t->a, t->b, (unsigned long long)n, k, res, t1 - t0);
        }
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL;
}

int main(int argc, char *argv[]) {
  unsigned long long n = 200000000ULL; // Προσαρμογή σε 2e8 για χρόνο ~50s
  int P = 1; //default αριθμός εξωτερικων νημάτων: 1
  int T = 1; //default αριθμός εσωτερικων νημάτων: 1

  // Parsing ορισμάτων: -P <P> -T <T>
  if (argc >= 5) {
    
      for(int i=1; i<argc; i++){
        //Αναζητηση -P στα ορισματα γραμμης εντολων
          if(strcmp(argv[i], "-P") == 0 && i+1 < argc){
              P = atoi(argv[i+1]);
          }
          //Αναζητηση -Τ στα ορισματα γραμμης εντολων
          if(strcmp(argv[i], "-T") == 0 && i+1 < argc){
              T = atoi(argv[i+1]);
          }
      }
  } else {
      printf("Usage: %s -P <outer_threads> -T <inner_threads>\n", argv[0]);
      
  }

  /* Exactly 16 tasks: 4 per function, domain-respecting intervals */
  task_t tasks[] = {
    // 1) log(1+x^2)
    {"log1px2[0,1]",    FID_LOG1PX2,  f_log1px2,   0.0,   1.0},
    {"log1px2[0,3]",    FID_LOG1PX2,  f_log1px2,   0.0,   3.0},
    {"log1px2[-1,1]",   FID_LOG1PX2,  f_log1px2,  -1.0,   1.0},
    {"log1px2[2,5]",    FID_LOG1PX2,  f_log1px2,   2.0,   5.0},

    // 2) sqrt(1+x^2)
    {"sqrt1px2[0,1]",   FID_SQRT1PX2, f_sqrt1px2,  0.0,   1.0},
    {"sqrt1px2[0,5]",   FID_SQRT1PX2, f_sqrt1px2,  0.0,   5.0},
    {"sqrt1px2[1,3]",   FID_SQRT1PX2, f_sqrt1px2,  1.0,   3.0},
    {"sqrt1px2[-2,2]",  FID_SQRT1PX2, f_sqrt1px2, -2.0,   2.0},

    // 3) log(x)*sqrt(x)  (x>0)
    {"lnsqrt[1,2]",     FID_LNSQRT,   f_lnsqrt,    1.0,   2.0},
    {"lnsqrt[1,4]",     FID_LNSQRT,   f_lnsqrt,    1.0,   4.0},
    {"lnsqrt[2,3]",     FID_LNSQRT,   f_lnsqrt,    2.0,   3.0},
    {"lnsqrt[0.5,2]",   FID_LNSQRT,   f_lnsqrt,    0.5,   2.0},

    // 4) exp(-x)*sin(x)
    {"exp_sin[0,2]",    FID_EXP_SIN,  f_exp_sin,   0.0,   2.0},
    {"exp_sin[0,10]",   FID_EXP_SIN,  f_exp_sin,   0.0,  10.0},
    {"exp_sin[2,6]",    FID_EXP_SIN,  f_exp_sin,   2.0,   6.0},
    {"exp_sin[10,20]",  FID_EXP_SIN,  f_exp_sin,  10.0,  20.0}
  };
  const int ntasks = (int)(sizeof(tasks)/sizeof(tasks[0]));




  printf("Two-Level Parallel Monte Carlo (n=%llu samples per task, P=%d Outer threads, T=%d Inner threads per task)\n",n, P, T);

  double T0 = get_wtime(); // Λύση Προβλήματος : variable Shadowing, μετονομασια σε Τ0 για να μη χρησιμοποιειται μεσα στο loop αντι για το εσωτερικο t0.

  //Δημιουργια πινακα εξεωτερικων νηματων και πινακα των ορισματων τους
  pthread_t outer_Thread[P];
  outer_thread_attr_t outer_Thread_attr[P];


  for(int i=0; i<P; i++){
      outer_Thread_attr[i].outer_id = i;
      outer_Thread_attr[i].total_outer_threads = P;
      outer_Thread_attr[i].inner_threads_T = T;
      outer_Thread_attr[i].n = n;
      outer_Thread_attr[i].tasks = tasks;
      outer_Thread_attr[i].ntasks = ntasks;

      pthread_create(&outer_Thread[i], NULL, outer_worker, (void*)&outer_Thread_attr[i]);
  }

  // Αναμονή τερματισμού εξωτερικών νημάτων
  for(int i=0; i<P; i++){
      pthread_join(outer_Thread[i], NULL);
  }

  double T1 = get_wtime(); 
  printf("total time: %.6f s\n", T1-T0); 
  
  pthread_mutex_destroy(&mutex);

  return 0;
}