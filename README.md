# Systems Programming - Parallel Implementations 🚀

## 📋 Περιεχόμενα

### 1. Πολυδιεργασιακή Υλοποίηση (Multi-processing & IPC)
Υπολογισμός του ολοκληρώματος της $f(x) = \sin(\cos(x))$ με τη μέθοδο **Monte Carlo**.
* **IPC Mechanisms:** FIFO (Named Pipes), Message Queues (POSIX/System V), Shared Memory (`mmap`) με και χωρίς σεμαφόρους.
* **Analysis:** Σύγκριση απόδοσης των μηχανισμών IPC και επίδραση του αριθμού των διεργασιών στον χρόνο εκτέλεσης.

### 2. Πολυνηματική Υλοποίηση (Pthreads)
Παραλληλοποίηση 16 διαφορετικών tasks ολοκλήρωσης με χρήση της βιβλιοθήκης **POSIX threads**.
* **Architectures:** * Εσωτερικός παραλληλισμός (Internal parallelization).
    * Δύο επίπεδα παραλληλισμού (Static Round-Robin).
    * Δυναμική ανάθεση εργασιών με **Task Queue** και Mutexes.
* **Features:** Thread-safe RNG (`drand48_r`), αποφυγή race conditions και ανάλυση Load Balancing.

### 3. Παραλληλισμός με OpenMP
Χρήση του προτύπου **OpenMP** για υψηλού επιπέδου παραλληλισμό.
* **Monte Carlo:** Υλοποίηση με εμφωλευμένο παραλληλισμό (`nested parallelism`) και ρήτρες `reduction`.
* **Machine Learning (Logistic Regression):** Παραλληλοποίηση τυχαίας αναζήτησης υπερπαραμέτρων (Random Search).
* **Features:** Δυναμική χρονοδρομολόγηση (`dynamic schedule`) και χρήση `erand48` για thread-safety.

## 📊 Αποτελέσματα & Απόδοση
Όλες οι υλοποιήσεις δοκιμάστηκαν σε σύστημα με **6 φυσικούς πυρήνες**. Τα κύρια συμπεράσματα περιλαμβάνουν:
* **Scalability:** Η μέγιστη επιτάχυνση (Speedup) επιτυγχάνεται όταν τα νήματα/διεργασίες ισούνται με τους φυσικούς πυρήνες ($T=6$).
* **Oversubscription:** Η χρήση περισσότερων νημάτων από τους διαθέσιμους πυρήνες προκαλεί καθυστέρηση λόγω Context Switching.
* **Efficiency:** Η χρήση Shared Memory (`mmap`) στην 1η εργασία και το υβριδικό μοντέλο στην 2η εργασία ($P=2, T=3$) έδωσαν τους βέλτιστους χρόνους.

## 🛠️ Τεχνολογίες
* **Language:** C (C11)
* **Parallelism:** POSIX Threads, OpenMP, `fork()`
* **OS:** Linux / WSL (Ubuntu)
* **Tools:** GCC Compiler, Makefiles

## 👤 Συγγραφέας
* **Σπυρίδων Μανταδάκης** (1100613)
* Φοιτητής 3ου έτους, Τμήμα Μηχανικών Ηλεκτρονικών Υπολογιστών και Πληροφορικής (CEID).
