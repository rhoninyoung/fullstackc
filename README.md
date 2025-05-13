# fullstackc
Just for fun and practice(C语言复健✗(真C✓))

This project developed from a deep curiosity about compilers and programming language design, particularly fueled by an interest in domain-specific languages (DSLs). It represents an exploration of foundational C programming through unconventional methods – I believe a true understanding only solidifies by building systems from the ground up.

---

### 📦 Dependencies

Install the required libraries before building:

```bash
sudo apt-get update
sudo apt-get install -y libjson-c-dev libsqlite3-dev
```

### Usage

Compile and run the server and client using the commands below:

```bash
gcc -o server server.c database.c -ljson-c -lsqlite3
gcc -o client client.c -ljson-c
./server
./client
```

Future directions may include:

### TODOLIST
- [ ] Enhancing extensibility
- [ ] Porting the frontend to WebAssembly
      
#### Strengthening backend robustness(adopting common networking patterns:
- [ ] concurrency/connection
- [ ] pooling
- [ ] caching
- [ ] load balancing
- [ ] logging/error handling
- [ ] security hardening

---
#### Personal musings (feel free to skip):
The increasing specialization across various domains seems to point towards an inevitable fragmentation of computing. Just as Domain-Specific Languages (DSLs) complement general-purpose languages, we might see the evolution of Domain-Specific Processing Units (DSPUs) alongside heterogeneous processors (XPUs). Even the memory-centric von Neumann architecture will likely transform as computing expands into new frontiers.

Does this imply that general-purpose processors and languages will become obsolete? I believe they will persist in a state of equilibrium (perhaps around a 20%/80% split) – the majority of users won't require extreme specialization after all. As for programming languages, their true value and the rationale for their adoption only become apparent through prolonged, in-depth engagement (Rust except).
