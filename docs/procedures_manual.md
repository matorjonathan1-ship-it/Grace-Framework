# Grace: Procedures Manual ⚓

> "A ship in port is safe, but that is not what ships are built for." — Rear Admiral Grace Hopper

## I. The Routing Theorem
In Grace, we do not "guess" if a URL exists. We use C++20 Concepts to prove it at compile-time. 
* **Standard:** All handlers MUST satisfy the `GraceHandler` concept.
* **Objective:** Zero runtime 404s due to code logic.

## II. The Reflection Engine (`GRACE_REFLECT`)
To maintain a **12MB RSS** footprint, we use zero-copy reflection. 
* **Standard:** Use `std::tie` to map members.
* **Objective:** No intermediate JSON objects; stream data directly from the struct to the `io_uring` buffer.

## III. The Engine Room (`io_uring`)
Grace communicates directly with the Linux Kernel.
* **Standard:** Every I/O operation must be asynchronous.
* **Objective:** 1.85M req/s throughput on commodity hardware.
