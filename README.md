
# 🐧 MiniScope
### Lightweight Linux System Monitor

> Monitor Your System. Understand Your Processes.

MiniScope is a lightweight terminal-based Linux system monitoring tool developed using the **C programming language**.

It provides useful information about CPU usage, memory consumption, and running processes using the Linux `/proc` filesystem.

The project focuses on operating systems, system-level programming, and performance monitoring.

---

## 🚀 Features

- 🧠 Live CPU utilization monitoring
- 💾 RAM usage statistics
- ⚙️ Running process information
- 🔢 Process ID and process names
- 📊 CPU-based process sorting
- 🔁 Configurable refresh intervals
- 📝 CSV metrics logging
- ⚡ Lightweight command-line interface
- 🧪 Automated smoke tests
- 🐧 Linux and WSL2 support

## 🛠️ Technology Stack

| Technology | Purpose |
|---|---|
| C11 | Core programming language |
| Linux /proc | System information |
| POSIX APIs | System programming |
| GCC | C compiler |
| Make | Build automation |
| Bash | Testing scripts |
| GitHub Actions | CI workflow |

## 📁 Project Structure

```text
miniscope-c/
├── src/
│   └── miniscope.c
├── tests/
│   └── smoke.sh
├── Makefile
├── .github/
│   └── workflows/
│       └── ci.yml
├── .gitignore
├── LICENSE
└── README.md
```

## ⚙️ Installation

### Requirements

- Linux or WSL2
- GCC compiler
- GNU Make
- Git

### Install Dependencies (Ubuntu)

```bash
sudo apt update
sudo apt install build-essential git
```

### Clone Repository

```bash
git clone https://github.com/YOUR_USERNAME/miniscope-c.git
cd miniscope-c
```

### Compile

```bash
make
```

### Run

```bash
./miniscope
```

## 💻 Command Examples

### Single System Snapshot

```bash
./miniscope --once
```

### Display Top 15 Processes

```bash
./miniscope --once --top 15
```

### Refresh Every 3 Seconds

```bash
./miniscope --interval 3 --top 25
```

### Export Metrics to CSV

```bash
./miniscope --once --csv usage.csv
```

## 🧠 How It Works

MiniScope collects information from Linux system files.

```text
Linux Operating System
         |
         v
     /proc Files
         |
         v
   C Monitoring Engine
         |
         v
 CPU / Memory / Process Data
         |
         v
    Terminal Display
         |
         v
   Optional CSV Logging
```

### Important Linux Files

| File | Information |
|---|---|
| /proc/stat | CPU usage statistics |
| /proc/meminfo | Memory statistics |
| /proc/[pid]/stat | Process information |

## 🧪 Testing

```bash
make test
```

### Clean Build Files

```bash
make clean
```

## 🔮 Future Improvements

- [ ] Interactive terminal controls
- [ ] Per-core CPU monitoring
- [ ] Process search and filtering
- [ ] Threshold-based alerts
- [ ] Historical resource graphs
- [ ] Extended performance reports

## ⚠️ Limitations

MiniScope is developed for Linux environments and is not compatible with native Windows system APIs.

It is intended as an educational monitoring tool rather than a complete replacement for top or htop.

## 🤝 Contributing

Suggestions and improvements are welcome.

## 📜 License

Licensed under the MIT License.

---

### ⭐ Support

If this project helps you understand Linux system programming, consider giving it a star!

**Built with C | Linux | Systems Programming**
