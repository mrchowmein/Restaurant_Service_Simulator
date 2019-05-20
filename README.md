# Restaurant Service Simulator
An algorithmic approach to how restaurants could serve their customers.

Problem:
In the minds of customers of a restaurant, we feel that it is only fair that we recieve service in the order we arrived at a restaurant. Essentially, first come first service.  However, is this the most optimal solution for the restaurant? Is this even the most opitimal solution for the customer? Can we solve this with an algorithm or two?  Drawing inspiration from various operating system scheduling algorithms, we can see this is a scheduling problem. We can approach this problem by modeling and simulating different scheduling algorithms to see which performs in a manner that benefits both the customer and the restaurant. By simulating different algorithms, a restaurant owner can decide which method of serving their customer would be the best.

Defintions:
Throughput: (number of customers service)/time
Turn around time: (time customer finishes - time customer arrives at restaurant)
Wait time: time customer spends waiting for a waiter ie ready to order, waiting for food etc.

Goals:
-For the restaurant, ideally, service should ultimately maxmimize the number of customers serviced. Thus, increase throughput, reduce turnaround time.
-For the customer, ideally, have a sense of fairness while minimizing wait time. Reduce wait time, and turn around time.
Thus, we need to look at throughput, turn around times and wait times.

Solution:
To represent the actions of the customers arriving, ordering etc, we use discrete event simulation to simulate the events of each customer. The algorithms we will be looking at will be First come first serve (FIFO), Last come last serve (LCLS), Shortest service time first, Round Robin, multilevel Priority, and multilevel Priority with Preemption. We will track throughput, turnaround time and waittime for each customer and the overall average times.  For preemptive algorithms, we will use a time quantum. The time quantum will represent the maximum time a server will service a customer before moving on to another customer.  In the real world, this translate to the service bringing you a dish or two at a time vs a non preemptive algorith, the server might bring a customers dishes.

Each time a customer needs to do something, an event is created.  The following will be the 5 valid customer event states: arrival, service, customerAct, preempt and done.  The Arrivial event is when the customer is seated. Service event is when the waiter is servicing a customer (ordering, serving food, refilling water, processing the bill). CustomerAct is the time spent engaging with the menu, food or the bill after the waiter service. Preemption represents a server stopping the service of one customer to serve another. The last state is done.
