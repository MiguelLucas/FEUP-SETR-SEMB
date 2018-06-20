# Real Time Systems

Real time expresses the dependency of a computing system with respect to time as imposed by a physical or logical external process.
A real time system is able to keep the pace of a process and influence it in a desired way. The most important aspects of a real time system are 
* Execution time of the components
  * Code Structure
  * Operating System or kernel
  * DMA, caches, instructions pipeline
* Response time and regularity of generating periodic events
  * Multi-tasking
  * Acess to shared resources
  * Hardware interrupts

Requirements of Real time Systems are
* Functional (acquiring environment data, direct digital control and human machine interface)
* Temporal
* Dependability

Temporal requirements sets constraints that must be met in all instances

Real Time constraints can be:

<img align="right" src="Images/graf1.PNG" width="150">

* Soft - Time contraint according to which the associated value **keeps some utility** to the application even after a deadline D, despite a possible degradation of the quality of service
<br/>

<img align="right" src="Images/graf2.PNG" width="150">

* Firm - Time constraint according to which the associated value **loses any utility** to the application after a deadline D

<img align="right" src="Images/graf3.PNG" width="150">

* Hard - Time constraint that, when not met, may generate a **catastrophic failure**
