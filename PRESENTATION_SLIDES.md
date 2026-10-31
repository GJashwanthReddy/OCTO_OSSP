# FORGESHELL: LINUX INTER-PROCESS COMMUNICATION (PIPES & FIFOS)
## Academic Presentation Slide Deck (15 Slides)

**Course**: Operating Systems and System Programming (OSSP)  
**Degree**: B.Tech in Computer Science and Engineering  
**Presenter**: G Jashwanth Reddy  

---

### SLIDE 1: Title Slide
- **Header / Title**: LINUX INTER-PROCESS COMMUNICATION USING PIPES AND FIFOS
- **Subtitle**: Interactive Visual Simulator & POSIX C Implementation Suite (OSSP Coursework)
- **Presenter Details**:
  - **Student Name**: G Jashwanth Reddy
  - **Roll Number**: [INSERT ROLL NUMBER]
  - **Branch / Course**: B.Tech CSE | Operating Systems & System Programming (OSSP)
  - **University / Institute**: [INSERT UNIVERSITY / COLLEGE NAME]
- **Visual Element**:
  - `[INSERT SCREENSHOT: Web Simulator Overview Banner & Brand Header]`

---

### SLIDE 2: Problem Statement
- **Title**: Problem Statement & OS Challenges
- **Key Bullet Points**:
  - **Invisibility of Kernel IPC State**: Data written to Linux pipes passes transiently through kernel memory buffers, making process interaction invisible to students.
  - **Rendezvous Synchronization Pitfalls**: Named FIFOs block on `open()` until both reader and writer connect—frequently misinterpreted as system deadlocks.
  - **Descriptor Leaks & Hanging Processes**: Failing to close unneeded read/write file descriptors after `fork()` prevents End-Of-File (EOF) delivery, causing receiving processes to block indefinitely.
- **Goal**: Build a dual visual simulator and POSIX C suite that exposes low-level kernel system calls and process states.

---

### SLIDE 3: Project Objectives
- **Title**: Measurable Project Objectives
- **Key Bullet Points**:
  - **Implement Native POSIX Anonymous Pipe IPC**: Build C programs utilizing `pipe()`, `fork()`, `write()`, `read()`, `close()`, and `wait()` for parent-child streaming.
  - **Implement Native POSIX Named FIFO IPC**: Build independent C binaries utilizing `mkfifo()`, `open()`, `write()`, `read()`, `close()`, and `unlink()` for unrelated process rendezvous.
  - **Develop Interactive Front-End Simulator**: Build a responsive web laboratory using Vite, ES6 JS, and CSS keyframes to model dynamic packet movement and live system call logs.
  - **Enforce Descriptor & Process Hygiene**: Eliminate zombie processes via `wait()` and prevent infinite blocking via structural descriptor closure.

---

### SLIDE 4: Project Overview & Dual Architecture
- **Title**: System Architecture Overview
- **Key Bullet Points**:
  - **Dual-Layer Architecture**: Separates educational front-end visual modeling from production back-end Linux execution.
  - **Layer 1 — Web Visual Simulator (`/src` & `index.html`)**:
    - Interactive single-page app with 7 tabbed views.
    - Real-time CSS packet glide animations across process boundaries.
    - Dynamic status indicator lights and 6-step system call activity checklist.
  - **Layer 2 — Linux C Source Suite (`/linux-implementation`)**:
    - Native POSIX C programs compiled with GCC (`-Wall -Wextra`).
    - Executes direct Linux kernel traps under Ubuntu / Linux / WSL environments.
- **Visual Element**:
  - `[INSERT SCREENSHOT: System Architecture Diagram from Web Simulator Overview Page]`

---

### SLIDE 5: Core System Concepts & System Calls
- **Title**: Core Linux IPC Primitives & System Calls
- **Key Bullet Points**:
  - **Anonymous Pipe (`pipe`)**: Unidirectional in-memory channel (64KB circular ring buffer) strictly shared between related parent-child processes created via `fork()`.
  - **Named FIFO (`mkfifo`)**: Persistent special file node in filesystem (`/tmp/myfifo`) enabling IPC between completely independent, unrelated processes.
  - **Key System Calls**:
    - `pipe(fd)`: Allocates read (`fd[0]`) and write (`fd[1]`) descriptor pair.
    - `fork()`: Clones parent process context and inherits file descriptor table.
    - `mkfifo(path, mode)`: Creates named FIFO file node in directory.
    - `open(path, flags)`: Connects process to FIFO (`O_WRONLY` / `O_RDONLY`).
    - `write()` / `read()`: Transfers data bytes into/out of kernel IPC buffer.
    - `close()` / `unlink()`: Releases file descriptors and deletes FIFO node.

---

### SLIDE 6: Selected Commands & Build System
- **Title**: Implemented Commands & Compilation Utility
- **Key Bullet Points**:
  - **C Executable Commands (Linux / WSL)**:
    - `./pipe_demo`: Runs Anonymous Pipe parent-child IPC demonstration.
    - `./fifo_writer`: Launches Writer Process A (creates `/tmp/myfifo` & writes payload).
    - `./fifo_reader`: Launches Reader Process B (reads payload & unlinks node).
  - **Build Automation (`Makefile`)**:
    - `make all`: Compiles all C source files via GCC (`gcc -Wall -Wextra`).
    - `make clean`: Deletes binaries and removes leftover `/tmp/myfifo` file nodes.
  - **Web Visual Simulator Commands**:
    - `npm run dev`: Launches Vite development server (`http://localhost:5173`).
    - `npm run build`: Bundles static web distribution in `/dist`.

---

### SLIDE 7: Command Execution Workflow
- **Title**: End-to-End Command Execution Workflow
- **Key Bullet Points**:
  - **Step 1: Input Acquisition**: User enters string via terminal `fgets()` or Web UI input box.
  - **Step 2: Channel Initialization**: System calls `pipe(fd)` or `mkfifo("/tmp/myfifo", 0666)`.
  - **Step 3: Process Attachment & Rendezvous**:
    - *Pipe*: `fork()` creates Child Process B; Parent closes `fd[0]`, Child closes `fd[1]`.
    - *FIFO*: Process A opens `O_WRONLY` (blocks); Process B opens `O_RDONLY` (unblocks handshake).
  - **Step 4: Data Transmission**: `write()` transfers bytes to kernel buffer; `read()` pulls bytes into receiving process memory.
  - **Step 5: Resource Cleanup**: `close(fd)` sends EOF signal; `wait()` reaps child process; `unlink()` deletes filesystem FIFO node.
- **Visual Element**:
  - `[INSERT DIAGRAM: Flowchart from User Input -> System Call Traps -> Process Output]`

---

### SLIDE 8: Implementation Details — Anonymous Pipe
- **Title**: Implementation: Anonymous Pipe (`pipe_demo.c`)
- **Key Bullet Points**:
  - **Parent Process A (Producer)**:
    - Closes unused read descriptor: `close(fd[0])`.
    - Writes payload string + null terminator: `write(fd[1], write_msg, len)`.
    - Closes write descriptor `close(fd[1])` to decrement write reference count to 0 (signals EOF).
    - Synchronizes via `wait(NULL)` to reap child process.
  - **Child Process B (Consumer)**:
    - Closes unused write descriptor: `close(fd[1])`.
    - Reads message from pipe: `read(fd[0], read_msg, sizeof(read_msg))`.
    - Displays message output and exits cleanly (`EXIT_SUCCESS`).
- **Code Snippet**:
  ```c
  pipe(fd);
  if (fork() > 0) {
      close(fd[0]); write(fd[1], msg, len); close(fd[1]); wait(NULL);
  } else {
      close(fd[1]); read(fd[0], buf, sizeof(buf)); close(fd[0]); exit(0);
  }
  ```

---

### SLIDE 9: Implementation Details — Named FIFO
- **Title**: Implementation: Named FIFO (`fifo_writer.c` & `fifo_reader.c`)
- **Key Bullet Points**:
  - **Writer Process A (`fifo_writer.c`)**:
    - Creates node: `mkfifo("/tmp/myfifo", 0666)` (handles `EEXIST`).
    - Opens FIFO: `open("/tmp/myfifo", O_WRONLY)` — **Blocks until Reader opens**.
    - Transmits payload bytes via `write(fd, msg, len)` and closes descriptor.
  - **Reader Process B (`fifo_reader.c`)**:
    - Opens FIFO: `open("/tmp/myfifo", O_RDONLY)` — Completes handshake.
    - Receives payload bytes via `read(fd, buffer, sizeof(buffer))`.
    - Prints output, closes descriptor, and deletes node via `unlink("/tmp/myfifo")`.
- **Visual Element**:
  - `[INSERT SCREENSHOT: Dual Terminal Window showing fifo_reader and fifo_writer Execution]`

---

### SLIDE 10: Interactive Web Simulator Engine
- **Title**: Interactive Front-End Visual Laboratory
- **Key Bullet Points**:
  - **Vite & Modular ES6 Architecture**: Powered by `main.js`, `pipe-simulator.js`, and `fifo-simulator.js`.
  - **Dynamic Packet Flight Animation**: Computes DOM element bounding boxes (`getBoundingClientRect`) and applies CSS cubic-bezier transforms.
  - **Real-Time System Call Checklist Log**:
    - Sequentially illuminates 6 live execution states: `mkfifo` $\rightarrow$ `open(O_WRONLY)` $\rightarrow$ `write` $\rightarrow$ `open(O_RDONLY)` $\rightarrow$ `read` $\rightarrow$ `unlink`.
  - **UX Protections**: Input trimming, empty string validation toast alerts, and button disabling during active animations.
- **Visual Element**:
  - `[INSERT SCREENSHOT: Web FIFO Simulator tab showing packet animation and illuminated 6-step checklist]`

---

### SLIDE 11: Comparative Analysis: Pipe vs FIFO
- **Title**: Comparative Analysis: Anonymous Pipe vs Named FIFO
- **Comparison Table**:

| Feature / Property | Anonymous Pipe (`pipe_demo`) | Named FIFO (`fifo_writer` / `fifo_reader`) |
| :--- | :--- | :--- |
| **Process Scope** | Strictly related processes (Parent-Child via `fork`) | Completely independent, unrelated processes |
| **Filesystem Identity** | No filesystem path (Anonymous kernel buffer) | Named special file node (e.g. `/tmp/myfifo`) |
| **Creation System Call** | `pipe(int fd[2])` | `mkfifo(const char *path, mode_t mode)` |
| **Lifespan / Persistence** | Transient (Destroyed when descriptors close) | Persistent (Exists on disk until `unlink()`) |
| **Opening Semantics** | Pre-opened upon allocation via `pipe()` | Requires explicit `open()`; blocks until pair attaches |
| **Channel Direction** | Unidirectional (Half-Duplex) | Unidirectional per node (Half-Duplex) |

---

### SLIDE 12: Test Cases and Empirical Results
- **Title**: Professional Test Matrix & Results
- **Key Test Summary Table**:

| Test ID | Target / Scenario | Expected Result | Actual Result | Status |
| :--- | :--- | :--- | :--- | :--- |
| **TC_PIPE_01** | `./pipe_demo` ("Hello Linux Pipe!") | Parent writes 18 bytes; Child reads 18 bytes & displays output | Transmitted 18 bytes; Child output matched exactly; Parent reaped child | **PASSED** |
| **TC_FIFO_01** | `./fifo_reader` & `./fifo_writer` | Reader blocks on `open()`; Writer attaches; 21 bytes read; unlinked | Rendezvous succeeded; 21 bytes read; `/tmp/myfifo` unlinked cleanly | **PASSED** |
| **TC_FIFO_02** | `./fifo_writer` (No Reader active) | Writer opens FIFO in `O_WRONLY` and blocks in kernel | Process A blocked cleanly without hanging system or crashing | **PASSED** |
| **TC_MAKE_01** | `make all` | Compiles C files with `-Wall -Wextra` without errors | Generated 3 binaries with 0 compiler warnings/errors | **PASSED** |
| **TC_WEB_02** | Web Simulator (Empty Input) | Shows toast validation message | Toast displayed: "Please enter a message..." | **PASSED** |

---

### SLIDE 13: Sample Outputs & Visual Proofs
- **Title**: Output Verification & Visual Demonstrations
- **Key Bullet Points**:
  - Verified clean binary compilation under GCC on Linux/WSL.
  - Verified exact byte transfer counts (`strlen(msg) + 1`) across kernel IPC channels.
  - Verified smooth 60fps packet animations and responsive tab navigation in Web Simulator.
- **Visual Placeholders**:
  - `[INSERT SCREENSHOT 1: Web Pipe Simulator Active Transmission & Step Timeline]`
  - `[INSERT SCREENSHOT 2: Terminal Output of ./pipe_demo]`
  - `[INSERT SCREENSHOT 3: Dual Terminal Output of ./fifo_reader and ./fifo_writer]`

---

### SLIDE 14: System Limitations & Future Enhancements
- **Title**: System Limitations & Future Roadmap
- **Current Limitations**:
  - Channels are half-duplex (unidirectional); full-duplex communication requires two separate pipes/FIFOs.
  - Restricted to processes running on the same local host operating system kernel.
  - Static 256-byte message buffer size in C binaries (bounded by 64KB Linux kernel limit).
- **Future Enhancements**:
  - **Full-Duplex Dual Channel Extensions**: Implement bidirectional conversation streams between processes.
  - **Unix Domain Sockets & Shared Memory**: Expand visual simulator to support `socketpair()` and POSIX shared memory (`shm_open`).
  - **Dynamic Buffer Pressure Controls**: Add controls to simulate kernel write blocking when buffer capacity is 100% full.

---

### SLIDE 15: Conclusion & Course Outcome Attainment
- **Title**: Conclusion & Course Outcomes (CO) Attainment
- **Key Bullet Points**:
  - **CO1 Attainment (Layered Abstraction & Syscalls)**: Successfully demonstrated user/kernel space traps via direct POSIX system call execution and step-by-step visual tracing.
  - **CO2 Attainment (Unix Process Control)**: Mastered process cloning (`fork`), synchronization (`wait`), descriptor table inheritance, and clean termination (`exit`).
  - **CO3 Attainment (Inter-Process Communication)**: Implemented both Anonymous Pipes (related process streaming) and Named FIFOs (unrelated process rendezvous) in production C and web simulator.
  - **Dual Deliverable Impact**: Combined robust POSIX C systems programming with an intuitive browser-based visual laboratory.
- **Thank You / Q&A Prompt**: "Thank you! Open for Questions."

---
