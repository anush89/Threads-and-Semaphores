Threads-and-Semaphores
======================

Running instructions
====================

Compile and run PO_SIM.c

More Information about the project
==================================

Post Office Simulation

A Post Office is simulated by using threads to model customer and employee behavior. 

This project is similar to the “barbershop” example in the textbook. The following rules apply:

Customer:

1) 50 customers visit the Post Office (1 thread per customer up to 50), all created initially.

2) Only 10 customers can be inside the Post Office at a time.

3) Each customer upon creation is randomly assigned one of the following tasks:

a) buy stamps

b) mail a letter

c) mail a package

4) Times for each task are defined in the task table.

Postal Worker:

1) 3 created initially, one thread each.

2) Serves next customer in line.

3) Service time varies depending on customer task.

Scales:

1) Used by the postal worker when mailing a package.

2) There is only one, which can only be used one at a time. 

3) The scales are not a thread. They are just a resource the postal worker threads use. 

Other rules:

1) A thread should sleep 1 second in the program for each 60 seconds listed in the table. 

2) All mutual exclusion and coordination must be achieved with semaphores. 

3) A thread may not use sleeping as a means of coordination. 

4) Busy waiting (polling) is not allowed. 

5) Mutual exclusion should be kept to a minimum to allow the most concurrency.

6) Each thread should print when it is created and when it is joined.

7) Each thread should only print its own activities. The customer threads prints customer actions and 

the postal worker threads prints postal worker actions.

Semaphores used
===============
sem_t inside_post_office=10; //Allows only 10 threads inside post office initially.

sem_t cust_ready=0; //Signals when customers are all created.

sem_t mutex0=1; //Mutual exclusion for accessing random generator function.

sem_t mutex1=1; //Mutual exclusion for accessing a global variable while creating customers.

sem_t mutex2=1; //Mutual exclusion while assigning postal workers to customers.

sem_t mutex4=1; //Mutual exclusion for saving customer id's as postal workers finish their work.

sem_t mutex5=1; //Mutual exclusion for fetching the next customer waiting to be released.

sem_t worker_ready=0; //Semaphore to indicate that postal workers are ready to serve the customers.

sem_t request_worker=0; //Semaphore asking postal worker to start the assigned work.

sem_t scale_in_use=1; //Semaphore to control the use of scale.

sem_t finished_request=0; //Semaphore to signal that the worker has finished the work for a customer.

sem_t all_cust_created=0; //Semaphore to signal after all customer threads are created.

sem_t workers_created=0; //Semaphore to signal that workers are created.

Output:
=======

1) Each step of each task of each thread should be printed to the screen with identifying numbers so it 

is clear which threads are involved. 

2) Thread output sample. The wording in your output should exactly match the sample.

Simulating Post Office with 50 customers and 3 postal workers

Customer 0 created

Customer 0 enters post office

...

Customer 9 created

Customer 9 enters post office

Customer 10 created

Customer 11 created

...

Customer 49 created

Postal worker 0 created

Postal worker 0 serving customer 0

Postal worker 2 created

Postal worker 2 serving customer 1

Postal worker 1 created

Postal worker 1 serving customer 2

Customer 0 asks postal worker 0 to buy stamps

Customer 2 asks postal worker 1 to mail a package

Customer 1 asks postal worker 2 to mail a package

Scales in use by postal worker 1

Postal worker 0 finished serving customer 0

Customer 0 finished buying stamps

Postal worker 0 serving customer 3

Customer 3 asks postal worker 0 to mail a letter

Customer 0 leaves post office

Customer 10 enters post office

Joined customer 0

Scales released by postal worker 1

Postal worker 1 finished serving customer 2

Customer 5 asks postal worker 1 to mail a package

Scales in use by postal worker 2

Customer 2 finished mailing a package

Postal worker 1 serving customer 5

Customer 2 leaves post office

Postal worker 0 finished serving customer 3

Customer 3 finished mailing a letter

Postal worker 0 serving customer 4

Customer 4 asks postal worker 0 to mail a letter

Customer 3 leaves post office

Scales released by postal worker 2

Postal worker 2 finished serving customer 1

Scales in use by postal worker 1

Customer 1 finished mailing a package

Customer 1 leaves post office

Joined customer 1

Joined customer 2

Joined customer 3

...