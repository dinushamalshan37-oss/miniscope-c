#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_PROCS 8192
#define COMM_LEN 80

typedef struct {
    int pid;
    char name[COMM_LEN];
    unsigned long long ticks;
    long rss_kib; 
    double cpu_percent;
} Process;

typedef struct {
    unsigned long long total;
    unsigned long long idle;
} CpuStat;

static volatile sig_atomic_t running = 1;
static void on_signal(int unused) { (void)unused; running = 0; }

static bool read_cpu(CpuStat *stat) {
    FILE *f = fopen("/proc/stat", "r");
    if (!f) return false;
    char line[512];
    bool ok = fgets(line, sizeof(line), f) != NULL;
    fclose(f);
    if (!ok) return false;
    unsigned long long v[10] = {0};
    int n = sscanf(line, "cpu %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
                   &v[0], &v[1], &v[2], &v[3], &v[4], &v[5], &v[6], &v[7], &v[8], &v[9]);
    if (n < 4) return false;
    stat->total = 0;
    for (int i = 0; i < n; ++i) stat->total += v[i];
    stat->idle = v[3] + (n >= 5 ? v[4] : 0);
    return true;
}

static bool read_mem(long *total_kib, long *available_kib) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) return false;
    char line[256];
    *total_kib = *available_kib = -1;
    while (fgets(line, sizeof(line), f)) {
        long value;
        if (sscanf(line, "MemTotal: %ld kB", &value) == 1) *total_kib = value;
        if (sscanf(line, "MemAvailable: %ld kB", &value) == 1) *available_kib = value;
    }
    fclose(f);
    return *total_kib > 0 && *available_kib >= 0;
}

static bool parse_process(int pid, Process *out) {
    char path[64], line[4096];
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    FILE *f = fopen(path, "r");
    if (!f) return false; /* Process may have exited while scanning. */
    bool ok = fgets(line, sizeof(line), f) != NULL;
    fclose(f);
    if (!ok) return false;
    char *left = strchr(line, '(');
    char *right = strrchr(line, ')'); /* Name may contain spaces and parentheses. */
    if (!left || !right || right <= left || right[1] != ' ') return false;
    size_t len = (size_t)(right - left - 1);
    if (len >= COMM_LEN) len = COMM_LEN - 1;
    memcpy(out->name, left + 1, len);
    out->name[len] = '\0';
    unsigned long long utime = 0, stime = 0;
    long rss_pages = 0;
    bool found_u = false, found_s = false, found_r = false;
    char *saveptr = NULL;
    int field_number = 3; /* first token after command is field #3 (state) */
    for (char *token = strtok_r(right + 2, " \n", &saveptr); token;
         token = strtok_r(NULL, " \n", &saveptr), ++field_number) {
        if (field_number == 14) { utime = strtoull(token, NULL, 10); found_u = true; }
        if (field_number == 15) { stime = strtoull(token, NULL, 10); found_s = true; }
        if (field_number == 24) { rss_pages = strtol(token, NULL, 10); found_r = true; break; }
    }
    if (!found_u || !found_s || !found_r) return false;
    long page_kib = sysconf(_SC_PAGESIZE) / 1024;
    out->pid = pid;
    out->ticks = utime + stime;
    out->rss_kib = rss_pages > 0 ? rss_pages * page_kib : 0;
    out->cpu_percent = 0;
    return true;
}

static size_t read_processes(Process *out, size_t capacity) {
    DIR *dir = opendir("/proc");
    if (!dir) return 0;
    size_t n = 0;
    struct dirent *entry;
    while (n < capacity && (entry = readdir(dir)) != NULL) {
        if (!isdigit((unsigned char)entry->d_name[0])) continue;
        bool digits = true;
        for (const char *c = entry->d_name; *c; ++c) {
            if (!isdigit((unsigned char)*c)) { digits = false; break; }
        }
        if (!digits) continue;
        long pid = strtol(entry->d_name, NULL, 10);
        if (pid <= 0 || pid > 2147483647L) continue;
        if (parse_process((int)pid, &out[n])) ++n;
    }
    closedir(dir);
    return n;
}

static double system_cpu_percent(CpuStat before, CpuStat after) {
    if (after.total <= before.total) return 0;
    unsigned long long diff_total = after.total - before.total;
    unsigned long long diff_idle = after.idle >= before.idle ? after.idle - before.idle : 0;
    return 100.0 * (double)(diff_total > diff_idle ? diff_total - diff_idle : 0) / (double)diff_total;
}

static void calculate_process_cpu(Process *now, size_t n, const Process *old, size_t old_n,
                                  CpuStat before, CpuStat after, long cores) {
    if (after.total <= before.total) return;
    double denominator = (double)(after.total - before.total);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < old_n; ++j) {
            if (now[i].pid == old[j].pid && now[i].ticks >= old[j].ticks) {
                now[i].cpu_percent = 100.0 * (double)(now[i].ticks - old[j].ticks) / denominator * (double)cores;
                break;
            }
        }
    }
}

static int compare_cpu(const void *a, const void *b) {
    const Process *x = a, *y = b;
    if (x->cpu_percent < y->cpu_percent) return 1;
    if (x->cpu_percent > y->cpu_percent) return -1;
    if (x->rss_kib < y->rss_kib) return 1;
    if (x->rss_kib > y->rss_kib) return -1;
    return x->pid - y->pid;
}

static void display(const Process *processes, size_t n, int top, bool clear,
                    double cpu_usage, long mem_total, long mem_available) {
    if (clear) printf("\033[2J\033[H");
    time_t time_now = time(NULL);
    struct tm tm_now;
    localtime_r(&time_now, &tm_now);
    char stamp[32];
    strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", &tm_now);
    printf("MiniScope  |  %s  |  Linux Process Monitor\n", stamp);
    printf("--------------------------------------------------------------\n");
    printf("CPU: %5.1f%%  |  RAM: %.1f / %.1f MiB  |  Processes: %zu\n\n",
           cpu_usage, (mem_total - mem_available) / 1024.0, mem_total / 1024.0, n);
    printf("%-8s %7s %10s  %s\n", "PID", "CPU %", "RAM MiB", "COMMAND");
    printf("--------------------------------------------------------------\n");
    size_t count = n < (size_t)top ? n : (size_t)top;
    for (size_t i = 0; i < count; ++i) {
        printf("%-8d %7.1f %10.1f  %s\n", processes[i].pid, processes[i].cpu_percent,
               processes[i].rss_kib / 1024.0, processes[i].name);
    }
    printf("\nPress Ctrl+C to exit. Process CPU%% is measured per logical core.\n");
    fflush(stdout);
}

static bool csv_log(const char *path, double cpu, long total, long available, size_t n) {
    FILE *f = fopen(path, "a+");
    if (!f) return false;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return false; }
    long length = ftell(f);
    if (length == 0) fprintf(f, "timestamp,cpu_percent,used_ram_mib,total_ram_mib,process_count\n");
    fprintf(f, "%lld,%.2f,%.2f,%.2f,%zu\n", (long long)time(NULL), cpu,
            (total - available) / 1024.0, total / 1024.0, n);
    return fclose(f) == 0;
}

static int parse_number(const char *text, int min, int max, const char *flag) {
    char *end;
    errno = 0;
    long n = strtol(text, &end, 10);
    if (errno || !text[0] || *end || n < min || n > max) {
        fprintf(stderr, "%s must be between %d and %d.\n", flag, min, max);
        exit(2);
    }
    return (int)n;
}

static void usage(const char *program) {
    printf("MiniScope - zero-dependency Linux process monitor\n"
           "Usage: %s [--once] [--interval seconds] [--top number] [--csv file]\n"
           "  --once           Take a 1-second sample, print once and exit\n"
           "  --interval N     Continuous refresh interval: 1-60 sec (default 2)\n"
           "  --top N          Show top N processes: 3-100 (default 12)\n"
           "  --csv FILE       Append system measurements to FILE\n"
           "  --help           Display this help\n", program);
}

int main(int argc, char **argv) {
    int interval = 2, top = 12;
    bool once = false;
    const char *csv = NULL;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) { usage(argv[0]); return 0; }
        else if (!strcmp(argv[i], "--once")) once = true;
        else if (!strcmp(argv[i], "--interval") && i + 1 < argc) interval = parse_number(argv[++i], 1, 60, "--interval");
        else if (!strcmp(argv[i], "--top") && i + 1 < argc) top = parse_number(argv[++i], 3, 100, "--top");
        else if (!strcmp(argv[i], "--csv") && i + 1 < argc) csv = argv[++i];
        else { fprintf(stderr, "Unknown/incomplete argument: %s\n", argv[i]); usage(argv[0]); return 2; }
    }
    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);
    Process *before = calloc(MAX_PROCS, sizeof(Process));
    Process *after = calloc(MAX_PROCS, sizeof(Process));
    if (!before || !after) { fprintf(stderr, "Out of memory.\n"); free(before); free(after); return 1; }
    CpuStat cpu_before, cpu_after;
    if (!read_cpu(&cpu_before)) { fprintf(stderr, "Unable to read /proc/stat. Run on Linux.\n"); free(before); free(after); return 1; }
    size_t old_n = read_processes(before, MAX_PROCS);
    long cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (cores < 1) cores = 1;
    int result = 0;
    do {
        unsigned int seconds = once ? 1U : (unsigned int)interval;
        while (seconds && running) seconds = sleep(seconds);
        if (!running) break;
        if (!read_cpu(&cpu_after)) { fprintf(stderr, "Cannot read /proc/stat.\n"); result = 1; break; }
        size_t n = read_processes(after, MAX_PROCS);
        long mem_total, mem_available;
        if (!read_mem(&mem_total, &mem_available)) { fprintf(stderr, "Cannot read /proc/meminfo.\n"); result = 1; break; }
        double utilization = system_cpu_percent(cpu_before, cpu_after);
        calculate_process_cpu(after, n, before, old_n, cpu_before, cpu_after, cores);
        qsort(after, n, sizeof(Process), compare_cpu);
        display(after, n, top, !once && isatty(STDOUT_FILENO), utilization, mem_total, mem_available);
        if (csv && !csv_log(csv, utilization, mem_total, mem_available, n)) {
            perror("Writing CSV failed"); result = 1; break;
        }
        // Sorted snapshot preserves PID + ticks, so next comparison is still valid.
        Process *tmp = before; before = after; after = tmp;
        old_n = n; cpu_before = cpu_after;
    } while (running && !once);
    free(before); free(after);
    return result;
}
