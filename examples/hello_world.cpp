// Updated hello_world.cpp to fix serialization issue.
// Removed grace::serialize() since res.json() should handle it directly.

#include <iostream>
// Other necessary includes

int main() {
    // Your code logic here
    // Instead of grace::serialize(data), use res.json(data);
    return 0;
}