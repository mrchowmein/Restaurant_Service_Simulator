# Restaurant Service Simulator
An algorithmic approach to how restaurants could serve their customers.

Problem:  
In the minds of customers of a restaurant, generally, people feel that it is only fair that they receive service in the order they were seated in or in the order we were ready to engage with the waiter at a restaurant. Essentially, first come first service order.  However, is this the most optimal solution for the restaurant? Is this even the most optimal solution for the customer? What if the waiter served their important customers first? or what if the waiter served customers that required less service first or more often?  How can we find out which order of serving our customers will yield the best results?  

Solution:  
Drawing inspiration from queueing theory algorithms, we can address this problem by building a service queue simulator that utilizes queueing theory algorithms. These queuing algorithms are commonly found in operating systems to elevators. By simulating different algorithms, a restaurant owner can decide which method of serving their customer would be the best. At at least figure out how each method differs.
The simulator will simulate the following algorithms: First come first serve, Last come last serve, Shortest service time first, Round robin, multilevel priority, and multilevel priority with preemption. Whenever a customer is ready to engage with the waiter, the customer will be added to the "service" or run queue.

Inputs:  
The input file will represent a restaurant's service session.  For the sake of simplicity, we will start at time zero. Each line of the input represents a customer, customer's arrival time, their estimated required waiter service time (in seconds), base waiter service burst and base customer-activity burst.

Outputs:  
We will print out each customer's statistics in a table. The console printout will include each customers input statistic and their corresponding finished statistics: 
Finish Time, Turnaround Time, Total Cust-Act Time, Total Cust Wait Time

We will also print out the summary for all the customers in the following format that includes the following metrics:
Throughput, Avg Turn around time, Avg Wait Time, Avg Customer Turnaround time, Avg Waiter Utilization Avg Customer Activity Utilization. 

How will the simulator work:  
The simulator will be built around the concept of a discrete event simulator.  Customer's activities such as being seated, ready for server, etc will be treated as events. There will be two queues. First, events will be added to the event queue. Secondly, once a customer is ready to receive service, he or she will be added to the service queue for the waiter to serve them. The scheduling algorithm will decide which customer the waiter will serve next and for how long.

How events work:  
Each time a customer needs to do something, an event is created.  The following will be the 5 valid customer event: seated, ready, service, preempt and cust-act.  The seated event is when the customer is seated. Ready event is when the customer is ready and waiting for the waiter to serve them. Service event is when the waiter is to service a customer (ordering, serving food, refilling water, processing the bill). Cust-act is the time spent engaging with the menu, food or the bill after the waiter service. Preemption represents a server stopping the service of one customer to serve another. 

For preemptive algorithms, we will use a time quantum. The time quantum will represent the maximum time a server will service a customer before moving on to another customer.  In the real world, this translate to the waiter bringing a customer a dish or two at a time vs a nonpreemptive method where the waiter might bring all the customers dishes at once. 

The simulation will be built around the concepts CPU and io bursts of a process scheduler. As we mentioned, there is a total waiter service time required for the customer. This can be seen as an estimated amount of time the waiter is expected to serve the customer. Longer times suggest larger parties.  After each time a waiter serves the customer, the customer will go into the state of customer activity such as eating.  To calculate the time for each of these activities, we will generate a random service time (service burst) and random customer activity time (cust activity  burst)using a random number generator and the base burst time. However, we will use a random number file so that our comparisons between algorithms are consistent. If the service time requirement is not met after each customer activity burst, then the customer goes back into the ready state.  This is to simulate a customer receiving a dish but not all the dishes. Thus, will still need service from the waiter for the next dish. Once the total service time requirement is met and the final customer activity is complete, then the customer is in the done state. The cycle on how this actually works is shown below in the diagram.

The cycle of events:  
![alt text](https://github.com/mrchowmein/Restaurant_Service_Simulator/blob/master/restsimcycle.png)

Metrics we want to compare different algorithms:
Throughput: Customers served per hour.
Turn around time: (time customer finishes - time customer arrives at the restaurant).
Wait Time: Time customer spent waiting for service to start once they are in the ready state to engage with a waiter.
Customer Turnaround time: Amount of time the customer spent at the restaurant. 
Waiter Utilization: The percentage of the total time the waiter was serving a customer.
Customer Activity Utilization: The percentage of the total time the customer was actually eating/ordering/bill.

To use the program: compile it with G++ with the -std=c++11 standard.  
To run, the executable takes the following arguments from commandline:  
./restSim -s[algorithm] [inputfile] [randomfile]

For example, to run first come first serve algorithm:  
./restSim -sF input1 rfile

For example, to run round robin with a quantum of 60, we just add the number 60 after the scheduler option:  
./restSim -sR60 input1 rfile

For example, to run priority with a quantum of 50 with 6 priority levels, we add a colon between the quantum and priority level:  
./restSim -sP50:6 input1 rfile

Sample input includes 10 customers arriving over the course of an hour.  
Using the sample imput, these are the results comparing the different algorithms. We see that the algorithm with the highest throughput is to Shortest Service Time First.
First Come First  
TotalTime: 5073  
Waiter Utilization: 34.81%  
Cust Activity Utilization: 97.65%  
Avg Cust Turnaround: 1429.36  
Avg Cust Waittime: 96.55  
Throughput: 7.806  

Shortest Service Time First  
TotalTime: 4623  
Waiter Utilization: 38.20%  
Cust Activity Utilization: 96.47%  
Avg Cust Turnaround: 1404.73  
Avg Cust Waittime: 67.73  
Throughput: 8.566  

Round Robin @ 45 seconds  
TotalTime: 5335  
Waiter Utilization: 33.10%  
Cust Activity Utilization: 98.18%  
Avg Cust Turnaround: 1596.45  
Avg Cust Waittime: 58.91  
Throughput: 7.423  

Priority with 45 second quantum  
TotalTime: 5255  
Waiter Utilization: 33.61%  
Cust Activity Utilization: 98.04%  
Avg Cust Turnaround: 1641.55  
Avg Cust Waittime: 46.82  
Throughput: 7.536  

Priority with Preemption with 45 second quantum   
TotalTime: 5420  
Waiter Utilization: 32.58%  
Cust Activity Utilization: 96.73%  
Avg Cust Turnaround: 1506.73  
Avg Cust Waittime: 86.73  
Throughput: 7.306  

