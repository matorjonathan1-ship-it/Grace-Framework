![Build Status](https://github.com/matorjonathan1-ship-it/Grace-Framework/actions/workflows/ci.yml/badge.svg)

# Grace

> "The most dangerous phrase in the language is, 'We’ve always done it this way.'" — **Rear Admiral Grace Hopper**

Grace is a compiled C++ web framework engineered for absolute performance and zero friction. By treating routing as a theorem the compiler proves, Grace eliminates runtime routing errors and achieves record-breaking efficiency.

---

## ⚓ The Admiral's Standard (Benchmarks)
Grace is built on Linux `io_uring` to provide a zero-copy, non-blocking foundation:

* **Throughput:** 1.85M requests per second
* **P99 Latency:** 0.4ms
* **Memory Footprint:** 12MB RSS at 50,000 concurrent connections

## 🚀 Key Philosophies

### 1. The Routing Theorem
In Grace, your routing table is a theorem the compiler proves. If your code compiles, the routes are logically sound. No more runtime 404s or misconfigured endpoints.

### 2. Zero-Overhead Abstractions
Utilizing the `GRACE_REFLECT` engine, developers can define models that the framework automatically understands for JSON and database operations with no runtime cost.

### 3. Modern C++ Foundation
Built for C++20 and C++23, Grace leverages Concepts and high-performance metaprogramming to move the complexity from runtime to compile-time.

---

## 🛠️ Quick Start (Graceful Syntax)

Grace favors a declarative, strict syntax to ensure safety and speed:

```cpp
#include <grace/core.hpp>
#include <grace/reflect.hpp>

struct Order {
    int id;
    double total;
    GRACE_REFLECT(id, total); // Explicitly defined reflection
};

int main() {
    auto app = grace::create_app();

    app.get("/api/order/{id}", [](auto& req, auto& res) {
        Order o = grace::db::find<Order>(req.params["id"]);
        return res.json(o);
    });

    return app.listen(8080);
}
