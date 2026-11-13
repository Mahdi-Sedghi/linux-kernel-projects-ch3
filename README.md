# Linux Kernel Modules: Task Information & Kernel Data Structures

Implementations of **Project 2** and **Project 4** from Chapter 3 (*Processes*) of *Operating System Concepts* (10th Edition) by Silberschatz, Galvin, and Gagne.

## Overview

### 1. Project 2: Linux Kernel Module for Task Information (`project2_proc_pid/`)
Creates a `/proc/pid` kernel interface that allows user-space processes to query task information directly from the kernel's Process Control Block (`struct task_struct`).
- **Write Operation (`proc_write`)**: Accepts a PID string from user space via `copy_from_user()`, allocates kernel memory with `kmalloc(..., GFP_KERNEL)`, parses the integer via `kstrtol()`, and frees the buffer with `kfree()`.
- **Read Operation (`proc_read`)**: Looks up the `struct task_struct` using `pid_task(find_vpid(pid), PIDTYPE_PID)` and outputs the process command name (`comm`), PID (`pid`), and execution state (`__state`/`state`) via `copy_to_user()`.

### 2. Project 4: Kernel Data Structures (`project4_kernel_lists/`)
Explores the circular, doubly linked list (`struct list_head` in `<linux/list.h>`) embedded inside kernel structures:
- **Part I (`color_list.c`)**: Dynamically allocates four `struct color` nodes using `kmalloc()`, appends them using `list_add_tail()`, traverses them on load with `list_for_each_entry()`, and safely deletes/frees all nodes on exit using `list_for_each_entry_safe()`, `list_del()`, and `kfree()`.
- **Part II (`collatz.c`)**: Accepts a load-time parameter (`start`, defaulting to `25`) via `module_param(start, int, 0)`, generates the Collatz conjecture sequence in a kernel linked list, prints the sequence to the kernel ring buffer, and frees all kernel memory upon module removal.

---

## Prerequisites

Ensure you have the Linux kernel headers and build tools installed:

```bash
# Ubuntu / Debian
sudo apt update && sudo apt install build-essential linux-headers-$(uname -r)

# Fedora / RHEL
sudo dnf install kernel-devel kernel-headers make gcc
