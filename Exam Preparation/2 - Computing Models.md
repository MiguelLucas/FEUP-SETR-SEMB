### Computing Models

* Transformational model - A program starts and finished, transforming input data in output results
* Reactive model - A program executes indefinitely a sequence of interactions, operating over a continuous input data stream
* **Real time model** - Reactive model in which the output stream has to be synchronized with the input data stream, thus imposing time constraints on the program execution.

* Determinism - Feeding a program with the same sequence of data inputs originates the same sequence of data outputs
* Predictability - Feeding a program with the same input sequence will generate the same output sequencce with a **known delay** or within a **known time window**

A **task** is an infinite sequence of activations, each executing a set of operations. Tasks can be:

* Periodic - each instance is activated after T time has passed
* Sporadic - there is a minimum time T between consecutive activations, but there can be a time T1, where T1 >= T
* Aperiodic - Can only be characterized stochastically

Tasks have several requirements:

* Temporal - time constraints on the finishing instants 
* Precedence - establish a certain order of execution among tasks
* Resource usage - need for using shared resources

**Preemption** 
When a task can be **temporarily suspended** for the execution of another one with **higher priority**, it is **preemptable**.
When a system makes use of the preemptability of tasks it is called preemptive.
A set of tasks admits full preemption when all of its tasks can be preempted in any point of their execution.

Tasks can be triggered by:
* **Time** (the execution of tasks is triggered by a control signal based on the progression of time (ex.: a periodic interrupt))
  * Used in automatic control, where there is a common time base. The CPU utilization is constant even when there are no variations in the system state. It has a **well defined worst-case situation**
* **Events** (the execution of tasks is triggered by an asynchronous control signal generated upon a change in the system state (ex.: an external interrupt))
  * Used in monitoring sporadic conditions in the system state. The CPU utilization is variable according the frequency of event ocurrence. It has a **poorly defined worst-case situation**
