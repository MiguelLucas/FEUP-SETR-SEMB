### Fixed priorities scheduling

The schedule is built with the system working and is based on a static parameter. The ready tasks queue is sorted by descending priorities. The first to execute is the one with higher priority. When preemption is allowed, when a task arrives at the head of the queue with a higher priority than the one that is currently executing, this one is suspended and the new one is executed.

O(n)

This method is easily scalable, and accommodates sporadic tasks. Has a deterministic behavior under overloads as it just affects tasks with priority lower than that of the overload. On the other hand, is of a more complex implementation than the cyclic executive, and has a higher run time overhead. Also, overloads at high priority levels can block the whole system.

Assigning priorites can be:

* Inversely proportional to the period (RM - Rate Monotonic)
* Inversely proportional to the deadline (DM - Deadline Monotonic)
* Proportional to the tasks importance
