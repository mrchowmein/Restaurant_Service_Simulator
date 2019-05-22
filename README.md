# Restaurant Service Simulator
An algorithmic approach to how restaurants could serve their customers.

Problem:
In the minds of customers of a restaurant, we feel that it is only fair that we receive service in the order we arrived at a restaurant. Essentially, first come first service method.  However, is this the most optimal solution for the restaurant? Is this even the most optimal solution for the customer? What if the waiter served their important customers first? or what if the waiter served customers that required less service more often?  How can we find out which order of serving our customers will yield the best results?  

Solution:
Drawing inspiration from queueing theory algorithms, we can address this problem by building a scheduling simulator that can simulate different queuing theory methods of serving customers. These queuing algorithms are commonly found in operating systems to elevators. By simulating different algorithms, a restaurant owner can decide which method of serving their customer would be the best. At at least figure out how each method differs.
The simulator will simulate the following algorithms: First come first serve, Last come last serve, Shortest service time first, Round robin, multilevel priority, and multilevel priority with preemption. 

Inputs:  
Each input file will represent a resturant's service session.  For the sake of simplicity, we will start at time zero. Each line of the input represents a customer, customer's arrival time, their estimated required waiter service time, base waiter service burst and base customer-activity burst.

Outputs:
We will print out each customer's statistics in a table. The print out format as follows:
<Seating Time> <Total Service Time> <Base Service Burst><Base Cust-Act burst> <Priority Level> | <Finish Time> <Turnaround Time> <Total Cust-Act Time> <Total Cust Wait Time>

We will also print out the summary for all the customers in the following format that includes the following metrics:
Throughput, Avg Turn around time, Avg Wait Time, Avg Customer Turnaround time, Avg Waiter Utilization Avg Customer Activity Utilization. 


How will the simulator work:
The simulator will be built around the concept of a discrete event simulator. Customer's activities such as being seated, waiting for server, etc will be treated as events. The scheduling algorithm will decide which customer to serve next and for how long. 
Each time a customer needs to do something, an event is created.  The following will be the 5 valid customer event states: seated, service, ready, preempt and cust-act.  The seated event is when the customer is seated. Service event is when the waiter is servicing a customer (ordering, serving food, refilling water, processing the bill). Cust-act is the time spent engaging with the menu, food or the bill after the waiter service. Preemption represents a server stopping the service of one customer to serve another. 

For preemptive algorithms, we will use a time quantum. The time quantum will represent the maximum time a server will service a customer before moving on to another customer.  In the real world, this translate to the waiter bringing a customer a dish or two at a time vs a nonpreemptive method where the waiter might bring all the customers dishes at once. 

The simulation will be built around the concepts cpu and io bursts of a process scheduler. As we mentioned, there is a total waiter service time required for the customer. This can be seen as an estimated amount of time the waiter is expected to serve the customer. Longer times suggest larger parties.  After each time a waiter serves the customer, the customer will go into the state of customer activity such as eating.  To calculate the time for each of these activities, we will generate a random service time (service burst) and random customer activity time (cust activity burst burst)using a random number generator and the base burst time. We will use a random number file so that our comparisons between algorithms are consistent. Once the total service time requirement is met and the final customer activity is complete, then the customer is in the done state.

The cycle of events: 
[[https://github.com/mrchowmein/Restaurant_Service_Simulator/blob/master/restsimcycle.png|alt=cycle]]

![alt text](https://github.com/mrchowmein/Restaurant_Service_Simulator/blob/master/restsimcycle.png)








Metrics we want to compare different algorithms:
Throughput: (number of customers service)/time
Turn around time: (time customer finishes - time customer arrives at the restaurant)
Wait Time: time customer spends waiting for a waiter ie ready to order, waiting for food etc.
Customer Turnaround time: Amount of time the customer spent at the restaurant. 
Waiter Utilization: The percentage of time the waiter was serving a customer during a shift.
Customer Activity Utilization: The percentage of time the customer was actually eating or order instead of waiting.
