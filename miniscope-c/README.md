# 🐧 MiniScope

### See inside your Linux system, one process at a time.

**Lightweight Linux Process Monitor | C11 | `/proc` Filesystem**

> A terminal-based performance monitor written in C. Explore system CPU usage, RAM statistics and running processes without relying on a heavy framework or an external C library.

**Status:** Functional educational MVP  ·  **Language:** C  ·  **Platform:** Linux / WSL2  ·  **License:** MIT

---

## ✨ Highlights

| Feature | What it does |
| --- | --- |
| 🧠 CPU usage | Calculate system CPU utilization from two `/proc/stat` snapshots |
| 💾 RAM monitoring | Show memory usage using `MemTotal` and `MemAvailable` |
| ⚙️ Process inspector | Display PID, process names, CPU usage and resident memory |
| 📊 Sorting | List processes ordered by CPU usage |
| 🔁 Auto-refresh | Set the update frequency in seconds |
| 📝 CSV output | Append collected metrics to a CSV file |
| 📦 Minimal dependencies | Standard C / POSIX APIs, GCC and Make |
| 🧪 Smoke tests | Linux CLI test script and GitHub Actions |

## 🧰 Tech stack

`C11` · `Linux /proc` · `POSIX` · `GCC` · `Make` · `Shell` · `GitHub Actions`

## 🚀 Getting started

**Prerequisites:** Linux (Ubuntu recommended) or **WSL2** on Windows, GCC and Make.

For Ubuntu / WSL2:

```bash
sudo apt update
sudo apt install build-essential git
```

Clone and compile:

```bash
git clone https://github.com/YOUR_USERNAME/miniscope-c.git
cd miniscope-c
make
./miniscope
```

### Command examples

```bash
./miniscope --once                 # Print a single sample and exit
./miniscope --once --top 15        # Show the top 15 processes
./miniscope --interval 3 --top 25  # Refresh every 3 seconds
./miniscope --once --csv usage.csv # Append a CSV metric record
```

> **Windows note:** The `/proc` interface is Linux-specific. Use WSL2; this program is not designed for native Windows Command Prompt or PowerShell.

## 🧠 How it works

```text
       Linux /proc
          │
  ┌───────┼──────────────────┐
  ▼       ▼                  ▼
/proc/stat  /proc/meminfo   /proc/<pid>/stat
  │           │                  │
  └───────────┴──────────────────┘
              ▼
          C sampling
              ▼
     Sorted terminal output
              │
       Optional CSV record
```

1. Read two CPU time snapshots and calculate the change between them.
2. Calculate used RAM from total and available memory.
3. Scan numeric process directories in `/proc` and read process statistics.
4. Estimate per-process CPU activity and list the busiest processes.
5. Refresh the view or exit after one sample, depending on options.

CPU for a newly detected process can initially show zero until a second sample is available.

## 📁 Project structure

```text
miniscope-c/
├── src/miniscope.c          # Core Linux monitor implementation
├── tests/smoke.sh           # CLI smoke tests
├── Makefile                 # Build, test and clean targets
├── .github/workflows/ci.yml
├── .gitignore
└── LICENSE
```

## ✅ Build, test and clean

```bash
make
make test
make clean
```

CI compiles and runs smoke tests on Ubuntu.

## 🗺️ Roadmap (not implemented yet)

- [ ] Interactive keyboard controls and process filtering
- [ ] Per-core CPU display
- [ ] Configurable threshold alerts
- [ ] Historical graphs and richer logging
- [ ] Efficient tracking on systems with very high process counts

## 🔒 Limitations

MiniScope is a learning tool, not a complete replacement for `top` or `htop`. Some processes disappear during sampling; they are skipped. The implementation uses bounded process storage and is optimized for understanding Linux systems rather than production-scale profiling. It **does not terminate processes, modify kernel settings or require root permissions** for normal use.

## 📸 Screenshots

Run `./miniscope`, capture your real terminal output and add a screenshot in this section.

## 🤝 Contributions & license

Feedback and pull requests are welcome. MIT licensed — see [LICENSE](LICENSE).

---

**GitHub description:** A lightweight Linux system monitor built in C for live CPU, RAM, and process monitoring.
