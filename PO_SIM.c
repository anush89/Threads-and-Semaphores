/* Post office simulation using 50 customer threads and 3 postal worker threads.*/

#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

//Defining our simulation's customer, postal workers and number of people allowed inside post office at a time.
#define MAX_CUSTOMER 50
#define POSTAL_WORKER 3
#define INSIDE_POST_OFFICE 10

//Random number generating function. Definition at the end.
int randomNo();

//Declaring the semaphores.
sem_t max_customers;	
sem_t inside_post_office;	//Allows only 10 threads inside post office initially.
sem_t postal_worker;		
sem_t cust_ready;		//Signals when customers are all created.
sem_t mutex0;			//Mutual exclusion for accessing random generator function.
sem_t mutex1;			//Mutual exclusion for accessing a global variable while creating customers.
sem_t mutex2;			//Mutual exclusion while assigning postal workers to customers.
sem_t mutex3;
sem_t mutex4;           //Mutual exclusion for saving customer id's as postal workers finish their work.
sem_t mutex5;           //Mutual exclusion for fetching the next customer waiting to be released.
sem_t worker_ready;     //Semaphore to indicate that postal workers are ready to serve the customers.
sem_t request_worker;   //Sempahore asking postal worker to start the assigned work.
sem_t scale_in_use;     //Semaphore to control the use of scale.
sem_t finished_request; //Semaphore to signal that the worker has finished the work for a customer.
sem_t all_cust_created; //Semaphore to signla after all customer threads are created.
sem_t workers_created;  //semaphore to signal workers created

//Global variable for sharing between threads.
int count=0;                          //Customer creation counter.
int randomNumber[MAX_CUSTOMER];       //Random number for each customer
int customer[MAX_CUSTOMER];           //Each customer and his assigned worker.
int cust_count_worker_thread=0;       //Keeping track of the latest customer count in postal worker thread.
int loop_counter=0;                   //Postal worker while loop counter for running the loop 50 times each for one customer.
int order[MAX_CUSTOMER];              //Array maintains the customer order in which postal worker finish the work.
int order_counter=0;                  //Counter for the order[] array in postal worker threads.
int cust_order_counter=0;             //Counter for the order[] array in customer thread.
int cust_enter_order=0;		      //Ensures correct order of customer going to workers.

//Customer thread deals with the interactions of customers.
void *customerThread(void *arg)
{
    int *myNum=(int *)arg;
    int num=*myNum;
    free(arg);
    //Local customer number variable.
    int customer_no;
    if(sem_wait(&mutex1)==-1)
	{
        printf("Semaphore wait error\n");
        exit(1);
	}
    customer_no=count;      //current customer number from count
    count=count+1;          //increment count(from 0 to 50)
    printf("Customer %d created\n",customer_no);
    if(count>=MAX_CUSTOMER)
    {
    	if(sem_post(&all_cust_created)==-1)          //signal when count is 50. To make sure all customer threads are created first.
    	{
            printf("Semaphore post error\n");
            exit(1);
	    }
    }
    if(sem_post(&mutex1)==-1)
	{
        printf("Semaphore post error\n");
        exit(1);
	}
    if(sem_wait(&inside_post_office)==-1)              //only allows 10 people inside post office
	{
        printf("Semaphore wait error\n");
        exit(1);
	}
    printf("Customer %d enters post office\n",customer_no);
    if(sem_post(&cust_ready)==-1)
	{
        printf("Semaphore post error\n");
        exit(1);
	}    
    if(sem_wait(&worker_ready)==-1)
	{
        printf("Semaphore wait error\n");
        exit(1);
	}    
    if(sem_wait(&mutex0)==-1)
	{
        printf("Semaphore wait error\n");
        exit(1);
	}    
    customer_no=cust_enter_order;			               //next in line
    randomNumber[customer_no]=randomNo();                  //random number generated and stored for each customer
    cust_enter_order++;
    if(sem_post(&mutex0)==-1)
	{
        printf("Semaphore post error\n");
        exit(1);
	}
    if(randomNumber[customer_no]==0)                       //based on the stored random number ask the postal worker.
    {
        printf("Customer %d asks postal worker %d to buy stamps\n",customer_no,customer[customer_no]);
    }
    else if(randomNumber[customer_no]==1)
    {
        printf("Customer %d asks postal worker %d to mail a letter\n",customer_no,customer[customer_no]);
    }
    else if(randomNumber[customer_no]==2)
    {
        printf("Customer %d asks postal worker %d to mail a package\n",customer_no,customer[customer_no]);
    }
    if(sem_post(&request_worker)==-1)          //signal to start the work
	{
        printf("Semaphore post error\n");
        exit(1);
	}    
    if(sem_wait(&finished_request)==-1)        //wait until postal worker finishes
	{
        printf("Semaphore wait error\n");
        exit(1);
	}    
    if(sem_wait(&mutex5)==-1)
	{
        printf("Semaphore wait error\n");
        exit(1);
	}        
    customer_no=order[cust_order_counter];     //process the customers in order as they were finished by postal worker.
    cust_order_counter=cust_order_counter+1;
    if(sem_post(&mutex5)==-1)
	{
        printf("Semaphore post error\n");
        exit(1);
	}        
    if(randomNumber[customer_no]==0)
    {
        printf("Customer %d finished buying stamps\n",customer_no);
    }
    else if(randomNumber[customer_no]==1)
    {
        printf("Customer %d finished mailing a letter\n",customer_no);
    }
    else if(randomNumber[customer_no]==2)
    {
        printf("Customer %d finished mailing a package\n",customer_no);
    }
    printf("Customer %d leaves post office\n",customer_no);
    if(sem_post(&inside_post_office)==-1)                         //signal next person to enter the office
    {
        printf("Semaphore post error\n");
        exit(1);
    }
    return NULL;
}

void *postalWorkerThread(void *arg)
{
	int *myNum=(int *)arg;
	int num=*myNum;       //postal worker thread number is in num.
	free(arg);
	int cust_count;       //local variable for keeping the customer number.
	if(sem_wait(&cust_ready)==-1)          //wait for customer ready signal
	{
        printf("Semaphore wait error\n");
        exit(1);
	}
	//sem_wait(&mutex2);
	printf("Postal worker %d created\n",num);
	while(loop_counter<MAX_CUSTOMER)         //run for all customers
	{
        if(sem_wait(&mutex2)==-1)
	{
            printf("Semaphore wait error\n");
            exit(1);
	}            
        cust_count=cust_count_worker_thread;
        printf("Postal worker %d serving customer %d\n",num,cust_count);
        customer[cust_count]=num;	     //which postal worker is serving which customer is in this array.
        cust_count_worker_thread=cust_count_worker_thread+1;
        loop_counter=loop_counter+1;
        if (sem_post(&mutex2)==-1)
        {
            printf("Semaphore post error\n");
            exit(1);
        }
        if (sem_post(&worker_ready)==-1)
        {
            printf("Semaphore post error\n");
            exit(1);
        }
        if(sem_wait(&request_worker)==-1)           //wait for customer thread to request something from the worker
	    {
            printf("Semaphore wait error\n");
            exit(1);
	    }          
        if(randomNumber[cust_count]==0)
        {
            sleep(1);                  //60sec=1sec program time
            printf("Postal worker %d finished serving customer %d\n",num,cust_count);
            if(sem_wait(&mutex4)==-1)
	        {
                printf("Semaphore wait error\n");
                exit(1);
	        }            
            order[order_counter]=cust_count;  //save the order in which postal worker completes customer requests.
            order_counter=order_counter+1;
            if(sem_post(&mutex4)==-1)
	        {
                printf("Semaphore post error\n");
                exit(1);
	        }            
            if(sem_post(&finished_request)==-1)      //signal that the work is done.
	        {
                printf("Semaphore post error\n");
                exit(1);
	        }              
        }
        else if (randomNumber[cust_count]==1)
        {
            sleep(1.5);
            printf("Postal worker %d finished serving customer %d\n",num,cust_count);
            if(sem_wait(&mutex4)==-1)
	        {
                printf("Semaphore wait error\n");
                exit(1);
	        }
            order[order_counter]=cust_count;
            order_counter=order_counter+1;
            if(sem_post(&mutex4)==-1)
	        {
                printf("Semaphore post error\n");
                exit(1);
	        }   
            if(sem_post(&finished_request)==-1)      //signal that the work is done.
	        {
                printf("Semaphore post error\n");
                exit(1);
	        }
        }
        else if(randomNumber[cust_count]==2)
        {
            if(sem_wait(&scale_in_use)==-1)        //only allow one to use scales at a time
	        {
                printf("Semaphore wait error\n");
                exit(1);
	        }            
            printf("Scales in use by postal worker %d\n",num);
            sleep(2);
            printf("Scales released by postal worker %d\n",num);
            order[order_counter]=cust_count;
            order_counter=order_counter+1;
            printf("Postal worker %d finished serving customer %d\n",num,cust_count);
            if(sem_post(&scale_in_use)==-1)
	    {
                printf("Semaphore post error\n");
                exit(1);
	    }            
            if(sem_post(&finished_request)==-1)
	    {
                printf("Semaphore post error\n");
                exit(1);
	    }            
        }
	if(cust_count==0)
	{
	if(sem_post(&workers_created)==-1)
	{ 
            printf("Semaphore post error\n");
            exit(1);
	}
	}
	}
	return NULL;
}

void main()
{
    pthread_t custThreads[MAX_CUSTOMER];              //50 customer threads
    pthread_t postalWorker[POSTAL_WORKER];            //3 postal worker threads
    int thread_count;                                 //counter for loops
    int thread_status;                                //save thread status in case of error
    srandom(time(NULL));                              //seed for random function.
    printf("Simulating Post Office with %d customers and %d postal workers\n",MAX_CUSTOMER,POSTAL_WORKER);
    
   /* for(thread_count=0;thread_count<MAX_CUSTOMER;thread_count++)   //set initially the entries of arrya to -1.
    {
        randomNumber[thread_count]=-1;
    }*/
    
    //initializing the semaphores with correct inital value.
    if(sem_init(&workers_created,0,0)==-1 || sem_init(&mutex5,0,1)==-1 || sem_init(&mutex4,0,1)==-1 || sem_init(&mutex0,0,1)==-1 || sem_init(&all_cust_created,0,0)==-1 || sem_init(&scale_in_use,0,1)==-1 || sem_init(&finished_request,0,0)==-1 || sem_init(&request_worker,0,0)==-1 || sem_init(&max_customers,0,0)==-1 || sem_init(&worker_ready,0,0)==-1 || sem_init(&cust_ready,0,3)==-1 || sem_init(&inside_post_office,0,10)==-1 || sem_init(&postal_worker,0,3)==-1 || sem_init(&mutex1,0,1)==-1 || sem_init(&mutex2,0,1)==-1)
    {
    	printf("Error while initializing semaphore\n");
    	exit(1);
    }
    
    //customer thread creation
    for(thread_count=0;thread_count<MAX_CUSTOMER; thread_count++)
    {
    	int *myNum=(int*)malloc(sizeof(int));
    	*myNum=thread_count;
    	thread_status=pthread_create(&custThreads[thread_count],NULL,customerThread,(void*) myNum);
    	if(thread_status!=0)
    	{
    		printf("Error while creating customer threads\n");
    		exit(1);
    	}
    }
    
    //wait for all customer thread creation
    if(sem_wait(&all_cust_created)==-1)
    {
        printf("Semaphore wait error\n");
        exit(1);
    }    
    
    //creating postal workers
    for(thread_count=0;thread_count<POSTAL_WORKER;thread_count++)
    {
    	int *myNum=(int*)malloc(sizeof(int));
    	*myNum=thread_count;
    	thread_status=pthread_create(&postalWorker[thread_count],NULL,postalWorkerThread,(void*) myNum);
    	if(thread_status!=0)
    	{
    		printf("Error creating postal worker threads\n");
    		exit(1);
    	}
    }
    //wait for postal worker thread 
    if(sem_wait(&workers_created)==-1)
    {
    	printf("Semaphore wait error\n");
	exit(1);
    }
    //join customer threads
    for (thread_count = 0; thread_count < MAX_CUSTOMER; thread_count++) 
    {
        thread_status = pthread_join (custThreads[thread_count], NULL);
        printf("Joined customer %d\n",thread_count);
        if (thread_status != 0)
        {
            printf("Join thread\n");
            exit(1);
        }
    }
    
    //join postal workers
    for (thread_count = 0; thread_count < POSTAL_WORKER; thread_count++) 
    {
        thread_status = pthread_join (postalWorker[thread_count], NULL);
        printf("Joined postal worker %d\n",thread_count);
        if (thread_status != 0)
        {
            printf("Join thread\n");
            exit(1);
        }
    }
}

int randomNo()
{
    return (random()%3);
}
