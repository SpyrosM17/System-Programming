// integral_mc_seq_heavy16.c
// Sequential Monte Carlo integrations: 16 tasks from 4 "heavier" functions.
// Per-task seed = task index (0,1,2,...). Analytic references used for Error/Rel.Error.

#define _GNU_SOURCE // Λύση Προβλήματος : implicit declaration, το stdlib δεν περιεχει τα extensions για την δηλωση τωνω  drand48, srand48
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <stdint.h>





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

/* Monte Carlo on [a,b] with n samples (sequential) */
static double integrate_mc(double (*f)(double), double a, double b, unsigned long long n) {
  const double width = b - a;
  double s = 0.0;
  for (unsigned long long i = 0; i < n; ++i) {
    double xi = a + width * drand48();   // U[a,b)
    s += f(xi);
  }
  return (width / (double)n) * s;
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

  printf("Sequential Monte Carlo (n=%llu samples per task)\n", n);

  double Τ0 = get_wtime(); // Λύση Προβλήματος : variable Shadowing, μετονομασια σε Τ0 για να μη χρησιμοποιειται μεσα στο loop αντι για το εσωτερικο t0.
  const double tiny = 1e-14;
  for (int k = 0; k < ntasks; ++k) {
    const task_t *t = &tasks[k];

    /* Per-task seed: 0,1,2,... */
    srand48((long)k);

    double ref; int has_ref = compute_ref(t->fid, t->a, t->b, &ref);

    double t0 = get_wtime();
    double res = integrate_mc(t->f, t->a, t->b, n);
    double t1 = get_wtime();

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
  double Τ1 = get_wtime(); 
  printf("total time: %.6f s\n", Τ1-Τ0); 

  return 0;
}
