// Spyros Mantadakis, 1100613


#define _GNU_SOURCE  //Για χρηση reentrant drand48_r, srand48_r
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <stdint.h>
#include <pthread.h> // Προσθηκη POSIX threads
#include <string.h>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static double get_wtime(void) {
  struct timeval t; gettimeofday(&t, NULL);
  return (double)t.tv_sec + (double)t.tv_usec * 1.0e-6;
}

/* Integrands (heavier but with elementary antiderivatives) */
static inline double f_log1px2(double x)   { return log(1.0 + x*x); }
static inline double f_sqrt1px2(double x)  { return sqrt(1.0 + x*x); }
static inline double f_lnsqrt(double x)    { return log(x) * sqrt(x); }    // requires x>0
static inline double f_exp_sin(double x)   { return exp(-x) * sin(x); }


//Δομη για το περασμα attributes στο καθε νημα
typedef struct {
  int thread_id; //Αναγνωριστικο νηματος
  int task_id;   //Αναγνωριστικο task
  unsigned long long thread_samples; // δειγματα νηματος
  double (*f)(double);  // συναρτηση προς εκτελεση απο το νημα
  double a;  
  double b;
  double partial_sum;   //μερικο αθροισμα
} thread_attr_t;


//Συναρτηση υλοποιησης monte carlo integration σε μερος δειγματων που εκτελει καθε νημα
void * integrate_threaded_mc(void *args)  {
  thread_attr_t *data = (thread_attr_t *)args; //Αποθηκευση ορισματων στην μεταβλητη δομης νηματος data

  struct drand48_data buffer; //Δημιουργια thread-safe state buffer 

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

    data->partial_sum = sum; //αποθηκευση μερικου αθροισματος   στο ορισμα του νηματος
    return NULL;



}

/* Function IDs for reference computation */
typedef enum { FID_LOG1PX2, FID_SQRT1PX2, FID_LNSQRT, FID_EXP_SIN } fid_t;

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

/* Task description */
typedef struct {
  const char *name; 
  fid_t fid;
  double (*f)(double);
  double a, b;
} task_t;



int main(int argc, char *argv[]) {
  unsigned long long n = 480000000ULL; // default 48e7
  int T = 1; //default αριθμός νημάτων: 1

  // Ελεγχος flag: Αν (-Τ) τοτε το τριτο ορισμα ειναι ο αριθμος των νηματων
    if (argc == 3 && (strcmp(argv[1], "-T") == 0)) {
        T = atoi(argv[2]);
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

  

  
  printf("Multi-threaded Sequential Monte Carlo (n=%llu samples per task, T=%d threads per task)\n", n, T);

  double T0 = get_wtime(); // Λύση Προβλήματος : variable Shadowing, μετονομασια σε Τ0 για να μη χρησιμοποιειται μεσα στο loop αντι για το εσωτερικο t0.
  const double tiny = 1e-14;

  //Δημιουργια πινακα νηματων και πινακα των ορισματων τους
  pthread_t Thread[T]; 
  thread_attr_t Thread_attr[T]; 


  for (int k = 0; k < ntasks; ++k) {
    const task_t *t = &tasks[k];
    double t0 = get_wtime();

    unsigned long long samples_per_thread = n / T; //ομοιμορφη κατανομη των δειγματων σε καθε νημα
    unsigned long long remainder = n % T; // υπολοιπα δειγματα
    
    //Αντιγραφη στοιχειων του τρεχον task στον πινακα των attributes του νηματος
    for(int i=0; i<T; i++){
      Thread_attr[i].thread_id = i;
      Thread_attr[i].task_id = k;
      Thread_attr[i].thread_samples = samples_per_thread;
      Thread_attr[i].f = t->f;
      Thread_attr[i].a = t->a;
      Thread_attr[i].b = t->b;
      Thread_attr[i].partial_sum = 0.0;

      //  προσθεσε το υπολοιπο στο μερος δειγματων του τελευταιο νηματος
      if (i == T - 1) {
            Thread_attr[i].thread_samples += remainder;
          }

      // Δημιουργια νήματος
      pthread_create(&Thread[i], NULL, integrate_threaded_mc, (void*)&Thread_attr[i]);
    }
    
    double total_sum = 0.0;
    for (int i = 0; i < T; i++) {
        pthread_join(Thread[i], NULL); //Αναμονη νηματων
        total_sum += Thread_attr[i].partial_sum; //Υπολογισμος συνολικου αθροισματος απο τα μερικα αθροισματα των νηματων.
    }

    // Τελικός υπολογισμός Monte Carlo
    double width = t->b - t->a;
    double res = (width / (double)n) * total_sum;

    double t1 = get_wtime();


    
    double ref; int has_ref = compute_ref(t->fid, t->a, t->b, &ref);


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
  }
  double T1 = get_wtime(); 
  printf("total time: %.6f s\n", T1-T0); 

  return 0;
}
