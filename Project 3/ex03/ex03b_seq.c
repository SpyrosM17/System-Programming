#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>


/* ----------- Configuration (similar to your Python script) ----------- */

#define N_SAMPLES   100000   // total samples (like n_samples=100000)
#define N_FEATURES  200      // number of features (like n_features=200)
#define TRAIN_RATIO 0.8      // 80% train, 20% validation
#define N_TRIALS    32       // number of random C trials
#define EPOCHS      100       // number of gradient descent passes
#define LEARNING_RATE 0.1    // learning rate for GD


/* ----------- Timer function  ----------- */

static double get_wtime(void) {
  struct timeval t; gettimeofday(&t, NULL);
  return (double)t.tv_sec + (double)t.tv_usec * 1.0e-6;
}

/* ----------- Utility RNG functions ----------- */

static double rand_uniform(void) {
    // Uniform in [0,1)
    return drand48();
}

static double randn(void) {
    // Box-Muller transform for standard normal
    double u1 = rand_uniform();
    double u2 = rand_uniform();
    double r = sqrt(-2.0 * log(u1 + 1e-12));
    double theta = 2.0 * M_PI * u2;
    return r * cos(theta);
}

/* ----------- Sigmoid function ----------- */

static double sigmoid(double z) {
    if (z >= 0) {
        double e = exp(-z);
        return 1.0 / (1.0 + e);
    } else {
        double e = exp(z);
        return e / (1.0 + e);
    }
}

/* ----------- Data generation ----------- */
/* We generate a linear decision boundary with Gaussian noise,
   somewhat similar in spirit to make_classification. */

void generate_data(double *X, int *y, int n_samples, int n_features) {
    // True underlying weights
    double *w_true = (double *)malloc(n_features * sizeof(double));
    if (!w_true) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < n_features; ++j) {
        w_true[j] = randn(); // random true weight
    }
    double bias_true = 0.0;

    for (int i = 0; i < n_samples; ++i) {
        double z = bias_true;
        for (int j = 0; j < n_features; ++j) {
            double xij = randn(); // feature ~ N(0,1)
            X[i * n_features + j] = xij;
            z += w_true[j] * xij;
        }
        // Add some noise and threshold
        z += 0.5 * randn();
        y[i] = (z > 0.0) ? 1 : 0;
    }

    free(w_true);
}

/* ----------- Standardization (like StandardScaler) ----------- */

void compute_mean_std(
    double *X,
    int n_samples,
    int n_features,
    double *mean,
    double *std
) {
    // Compute mean over samples (train only)
    for (int j = 0; j < n_features; ++j) {
        double sum = 0.0;
        for (int i = 0; i < n_samples; ++i) {
            sum += X[i * n_features + j];
        }
        mean[j] = sum / (double)n_samples;
    }

    // Compute std
    for (int j = 0; j < n_features; ++j) {
        double var = 0.0;
        for (int i = 0; i < n_samples; ++i) {
            double diff = X[i * n_features + j] - mean[j];
            var += diff * diff;
        }
        var /= (double)n_samples;
        std[j] = sqrt(var + 1e-8); // add epsilon to avoid div by zero
    }
}

void apply_standardization(
    double *X,
    int n_samples,
    int n_features,
    const double *mean,
    const double *std
) {
    for (int i = 0; i < n_samples; ++i) {
        for (int j = 0; j < n_features; ++j) {
            int idx = i * n_features + j;
            X[idx] = (X[idx] - mean[j]) / std[j];
        }
    }
}

/* ----------- Logistic Regression training (batch GD + L2 reg) ----------- */

double train_and_eval_logreg(
    double *X_train,
    int *y_train,
    int n_train,
    double *X_val,
    int *y_val,
    int n_val,
    int n_features,
    double C
) {
    // L2 regularization strength: lambda = 1/C (rough analogy)
    double lambda = 1.0 / C;

    // Model parameters
    double *w = (double *)calloc(n_features, sizeof(double));
    if (!w) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    double b = 0.0;

    // Gradient buffers
    double *grad_w = (double *)malloc(n_features * sizeof(double));
    if (!grad_w) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Training loop
    for (int epoch = 0; epoch < EPOCHS; ++epoch) {
        // Zero gradients
        for (int j = 0; j < n_features; ++j) {
            grad_w[j] = 0.0;
        }
        double grad_b = 0.0;

        // Compute gradients (batch)
        for (int i = 0; i < n_train; ++i) {
            double z = b;
            double *x_i = &X_train[i * n_features];
            for (int j = 0; j < n_features; ++j) {
                z += w[j] * x_i[j];
            }
            double p = sigmoid(z);
            double error = p - (double)y_train[i]; // dL/dz

            for (int j = 0; j < n_features; ++j) {
                grad_w[j] += error * x_i[j];
            }
            grad_b += error;
        }

        // Average gradient
        double inv_n = 1.0 / (double)n_train;
        for (int j = 0; j < n_features; ++j) {
            grad_w[j] = grad_w[j] * inv_n + lambda * w[j]; // L2 term
        }
        grad_b *= inv_n;

        // Gradient descent update
        for (int j = 0; j < n_features; ++j) {
            w[j] -= LEARNING_RATE * grad_w[j];
        }
        b -= LEARNING_RATE * grad_b;
    }

    // Evaluate accuracy on validation set
    int correct = 0;
    for (int i = 0; i < n_val; ++i) {
        double z = b;
        double *x_i = &X_val[i * n_features];
        for (int j = 0; j < n_features; ++j) {
            z += w[j] * x_i[j];
        }
        double p = sigmoid(z);
        int y_pred = (p >= 0.5) ? 1 : 0;
        if (y_pred == y_val[i]) {
            correct++;
        }
    }

    double acc = (double)correct / (double)n_val;

    free(w);
    free(grad_w);

    return acc;
}

/* ----------- Main: random search over C (log-uniform) ----------- */

int main(void) {
    srand48(1);

    int n_train = (int)(N_SAMPLES * TRAIN_RATIO);
    int n_val   = N_SAMPLES - n_train;

    // Allocate data
    double *X = (double *)malloc(N_SAMPLES * N_FEATURES * sizeof(double));
    int    *y = (int *)malloc(N_SAMPLES * sizeof(int));
    if (!X || !y) {
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FAILURE;
    }

    // 1. Generate data (like make_classification)
    generate_data(X, y, N_SAMPLES, N_FEATURES);

    // Train/validation split:
    // X_train: first n_train rows, X_val: last n_val rows
    double *X_train = X;
    int    *y_train = y;
    double *X_val   = X + (size_t)n_train * N_FEATURES;
    int    *y_val   = y + n_train;

    // 2. Standardization using only train statistics
    double *mean = (double *)malloc(N_FEATURES * sizeof(double));
    double *std  = (double *)malloc(N_FEATURES * sizeof(double));
    if (!mean || !std) {
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FAILURE;
    }

    compute_mean_std(X_train, n_train, N_FEATURES, mean, std);
    apply_standardization(X_train, n_train, N_FEATURES, mean, std);
    apply_standardization(X_val,   n_val,   N_FEATURES, mean, std);

    // 3. Random search over C (log-uniform between 1e-1 and 1e+1)
    double best_C = 0.0;
    double best_acc = -1.0;

    for (int i = 0; i < N_TRIALS; ++i) {
        double t0 = get_wtime();
        // Exponent in [-1, +1]
        double exponent = -1.0 + 2.0 * rand_uniform();
        double C = pow(10.0, exponent);

        double acc = train_and_eval_logreg(
            X_train, y_train, n_train,
            X_val,   y_val,   n_val,
            N_FEATURES,
            C
        );

        double t1 = get_wtime();
        printf("%02d: C=%.3e, acc=%.4f t=%.3f s\n", i, C, acc, t1-t0);

        if (acc > best_acc) {
            best_acc = acc;
            best_C   = C;
        }
    }

    printf("\nBest result:\n");
    printf("C=%.3e, acc=%.4f\n", best_C, best_acc);

    // Clean up
    free(X);
    free(y);
    free(mean);
    free(std);

    return 0;
}
