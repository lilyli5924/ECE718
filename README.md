# ECE718-Lab1-starter
MacID: lim14

StudentID: 400011362

This is a simple cache simulator with customized block size, cache size, associativity, write-through/write-back functions and LRU replacement policy.

The basic logic is every time reading an instruction from the memory trace file, depending on the situation it enters the nested loop. For either read or write operation, first check if there is hit by matching tag and valid bit, if hit does not happen, then check again if there is still empty cache that can use for that set (check if valid bit is 0). If nothing left, then implement the LRU policy by finding the maximum age block in that set and update the tag there.
