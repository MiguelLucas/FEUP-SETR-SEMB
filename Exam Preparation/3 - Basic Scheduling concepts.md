### Basic Scheduling concepts

**Task scheduling** is a sequence of tasks execution in one or more processors. It is generally represented in a Gantt chart.

A **scheduling algorithm** is a method to solve a scheduling problem.

* **EDD - Earliest Due Date**
Single job tasks and triggered synchronously.

Lmax(J) = max_i(f_i - d_i)
O(n*log(n))

<img src="Images/edd.PNG" width="550">

J3 é o primeiro a ser executado pois é o que tem menor due date. De seguida J2, J1, e J4.
