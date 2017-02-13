#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <assert.h>


/* POSIX queue */ 
#include <mqueue.h>

/* Thread includes */
#include <pthread.h>
#include <semaphore.h> 

/* Project includes */
#include "serverp.h"
//#include "init.h"

#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


/***************************************  Function declarations  ****************************************************/
/* Check the arguments are complete and within specified range */
void test_input(int argc, char* argv[]);

/* Clean up resources on the presence of forking error */
void clean_up_fork_failed(int pcount_total);

void produce(int pnum);
void consume(int pnum);

void signal_hdlr_main (int signo, siginfo_t *info, void *extra);
void signal_hdlr_pc (int signo, siginfo_t *info, void *extra);

pid_t spawn_process(char ptype, int pnum);
/*-------------------------------------------------------------------------------------------------------------------*/

/***************************************  Global variable declarations  **********************************************/
unsigned int RTime, BNum, PNum, CNum, R_s;
unsigned long int P_t, C_t1, C_t2; /* Program params */
double P_i;
unsigned int proc_trans = 0;

const gsl_rng_type * T;
gsl_rng * r;

mqd_t qd_queue; /* queue descriptor */ 
struct mq_attr attr; /* queue attributes */


unsigned int state;


/*----------------  Timing utilities  declarations ---------------*/
unsigned long int utimedif(struct timespec s1, struct timespec f1);
/* Macro to get timestamps */
#define GET_SYS_TICK(ts) clock_gettime(CLOCK_REALTIME, &(ts));
/*-------------------------------------------------------------------------------------------------------------------*/


/**************************************** MAIN  ********************************************/
/* OPERATION: This function executes spawns a new process. If the process is the child process it never returns 
              and calls the child function.  If fork() fails or the process is the parent process it returns 
	      the PID of the child process.
*/

int main (int argc, char* argv[])
{
pid_t process_hdlr;
int producer_status, pcount, pcount_total=0;
struct sigaction action_pc, action_main;
union sigval sig_value;
pid_t Prod[MAX_PRODUCERS];
pid_t Cons[MAX_CONSUMERS];

state = WAIT_ON_BARRIER;/* Default state for every process */


/* Registering the signal to synchronize with the main process */
action_main.sa_flags = SA_SIGINFO; 
action_main.sa_sigaction = &signal_hdlr_main;

if (sigaction(SIGUSR2, &action_main, NULL) == -1) 
  { 
    perror("SIGUSR2: Cannot register the signal");
    _exit(1);
  }

/* Registering the  signal to synchronize with the producers and consumers */
action_pc.sa_flags = SA_SIGINFO; 
action_pc.sa_sigaction = &signal_hdlr_pc;

if (sigaction(SIGUSR1, &action_pc, NULL) == -1)
  { 
    perror("SIGUSR1: Cannot register the signal");
    _exit(1);
  }

/* create a generator chosen by the environment variable GSL_RNG_TYPE */
gsl_rng_env_setup();
T = gsl_rng_default;
r = gsl_rng_alloc (T);


test_input(argc, argv);

printf ("the main program process ID is %d\n", (int) getpid ());

/*Allocate queue*/
      attr.mq_flags = 0; 
      attr.mq_maxmsg = BNum; 
      attr.mq_msgsize = MAX_MSG_SIZE; 
      attr.mq_curmsgs = 0;
      
      if ((qd_queue = mq_open (QUEUE_NAME, O_WRONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) 
	{ perror ("Server: mq_open (server)"); exit (1); }
/* Seed random number generator */
srand(time(NULL));

/* Create Producers  */
for (pcount = 0; pcount < PNum; pcount++)
  {
    process_hdlr = spawn_process('p', pcount);

    if(process_hdlr < 0)
     clean_up_fork_failed(pcount_total);

    Prod[pcount] = process_hdlr;
    pcount_total++;
  }

/* Create Consumers */
for (pcount = 0; pcount < CNum; pcount++)
  {
    process_hdlr = spawn_process('c', pcount);

    if(process_hdlr < 0)
     clean_up_fork_failed(pcount_total);

    Cons[pcount] = process_hdlr;
    pcount_total++;
  }

/* Run the producers */
 sig_value.sival_int = RUN_NORMALLY;
 for(pcount = 0; pcount < PNum; pcount++)
   sigqueue(Prod[pcount],SIGUSR1, sig_value);

/* Run the consumers */
 sig_value.sival_int = RUN_NORMALLY;
 for(pcount = 0; pcount < CNum; pcount++)
   sigqueue(Cons[pcount],SIGUSR1, sig_value);   


 int timelapse, total_trans= 0;
 struct timeval tvs, tve;
 gettimeofday(&tvs,NULL);
 while(RTime > 0)
   {
     
     pause();
     gettimeofday(&tve,NULL);

     timelapse = (tve.tv_sec - tvs.tv_sec);
     if( timelapse >= 10 )
       {
	 RTime -= 10;
	 total_trans = total_trans + proc_trans;
	 fprintf(stderr, "Transactions processed: Past ten seconds: %6d, Total: %6d: Remaining time: %6d \n",proc_trans, total_trans, RTime);
	 proc_trans = 0;
	 timelapse = 0;
	 tvs = tve;
       } 

     
   }

/* STOP all the producers */
 sig_value.sival_int = STOP_EXECUTION;
 for(pcount = 0; pcount < PNum; pcount++)
   sigqueue(Prod[pcount],SIGUSR1, sig_value);

/* STOP all the consumers */
 sig_value.sival_int = STOP_EXECUTION;
 for(pcount = 0; pcount < CNum; pcount++)
   sigqueue(Cons[pcount],SIGUSR1, sig_value);

for (pcount = 0; pcount < (PNum+CNum); pcount++)
  {
    process_hdlr = wait(&producer_status);
    if (WIFEXITED (producer_status))
      printf ("the child process %d exited normally, with exit code %d\n",  process_hdlr, WEXITSTATUS (producer_status));
  }


  /*We are done. Close and delete the queue*/
  if (mq_close (qd_queue) == -1) 
     perror ("Client: mq_close"); 

  if (mq_unlink (QUEUE_NAME) == -1) 
     perror ("Client: mq_unlink");

return 0;
}/*----------------------------------------------------------------------------------------------------------------------------------------------*/

/******************************************************** SIGNAL HANDLER MAIN ********************************************************************/
/* OPERATION: This function is the signal handler for synchronization between the main process and the child processes.
   It will change the status acording to the value received.
*/
void signal_hdlr_main (int signo, siginfo_t *info, void *extra)
{
  /* just count the requests */
  ++proc_trans;
  
}/*----------------------------------------------------------------------------------------------------------------------------------------------*/


/***************************************************** SIGNAL HANDLER PROD/CONS ******************************************************************/
/* OPERATION: This function is the signal handler for synchronization between the main process and the child processes.
   It will change the status acording to the value received.
*/
void signal_hdlr_pc (int signo, siginfo_t *info, void *extra)
{
  /* State changes to whatever the main process sends */
  state = info->si_value.sival_int;
  fprintf(stderr, "Singal received PID: %d\n",getpid());
}/*----------------------------------------------------------------------------------------------------------------------------------------------*/

/***************************************************************** Producer **********************************************************************/
/* INPUT: Producer Number
   OUTPUT: None
   OPERATION: This function executes a producer job according to specification on assignment 2
*/

void produce(int pnum)
{
  unsigned long int NextReq, BlockTime; 
  unsigned int MsgLen;
  struct timespec timeout;
  char sbuf[MAX_MSG_SIZE]; 
 
  struct timespec ts_bsend, ts_asend; /*timestamps before and after traying to send a message*/
  const char base_str[] = "A string to be used as a base message. It is just a container for a very long string with up to 256 characters. Part of it will be sent from the producer to the consumer depending on the calculated length bu using the Binomial distribution.**************";


  /* Seed the generator based on the producer number. This will ensure that each producer will
     have its own random sequence.*/
  gsl_rng_set(r, (pnum+1)*rand());

  /* Open the queue */
  fprintf(stderr, "Producer openning queue, %d\n", getpid());
  if ((qd_queue = mq_open (QUEUE_NAME, O_WRONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) 
    { perror ("Server: mq_open (server)"); exit (1); }
  
  while(state == WAIT_ON_BARRIER); /* Wait for signal to start */

  while(state == RUN_NORMALLY) /* Start producing requests */
    {

      memset(sbuf, '\0', sizeof sbuf); /* Clear the buffer */
      NextReq = gsl_ran_poisson (r,P_t); /* Calculate the time to the next request */
      MsgLen = gsl_ran_flat(r,R_s,MAX_MSG_SIZE-1); /* Calculate response size using the Uniform distribution */
      strncpy(sbuf,base_str,MsgLen); /* Copy characters to sending buffer */

      
      GET_SYS_TICK(ts_bsend);
      
      clock_gettime(CLOCK_REALTIME, &timeout);
      ++timeout.tv_sec;
	  
      if (mq_timedsend (qd_queue, sbuf, MsgLen+1, 1, &timeout) == -1)
		{ perror ("Producer: Rejecting current request");}
	   //	   fprintf(stderr, "S: %d \n", sbuf);
	   
      GET_SYS_TICK(ts_asend);      
      BlockTime = utimedif(ts_bsend, ts_asend); /* Compute time between after and before sending the message */
      BlockTime = (BlockTime > 20) ? BlockTime : 0;  /* If ellapsed time is small, discard it */

      //   fprintf(stderr, "P, %d, %6ld, %6ld, %ld.%ld \n", pnum, BlockTime, NextReq, ts_asend.tv_sec, ts_asend.tv_nsec);
      
      usleep(NextReq);
    }
  

exit(0);
}/*--------------------------------------------------------------------------------------------------------------------------------------------*/

/************************************************************** Consumer ***********************************************************************/
/* INPUT: Consumer Number
   OUTPUT: None
   OPERATION: This function executes a consumer job according to specification on assignment 2
*/

void consume(int pnum)
{
  char rbuf[MAX_MSG_SIZE];
  struct timespec timeout, ts_breceive, ts_areceive;
  int IdleTime;
  unsigned long int ProcTime;
  union sigval sig_value;
  sig_value.sival_int=0;
  /* Seed the generator based on the consumer number. This will ensure that each consumer will
     have its own random sequence.*/
  gsl_rng_set(r, (pnum+1)*rand());

  //fprintf(stderr, "Consumer openning queue, %d\n", getpid());
  if ((qd_queue = mq_open (QUEUE_NAME, O_RDONLY)) == -1) 
    { perror ("Client: mq_open (server)"); exit (1); }
  
	// fprintf(stderr,"Consumer is  waiting on barrier \n");
  while(state == WAIT_ON_BARRIER);
  //fprintf(stderr,"Consumer leave barrier \n");
  


  while(state == RUN_NORMALLY)
    {
      GET_SYS_TICK(ts_breceive);

      GET_SYS_TICK(timeout);
      ++timeout.tv_sec;

      /* Use timedreceive to prevent hanging. */
      if ( mq_timedreceive (qd_queue, rbuf, MAX_MSG_SIZE, NULL, &timeout) == -1 ) 
	if( state == RUN_NORMALLY ) /*The syscall was interrupted while waiting on an empty queue and the system is still running */
	  perror ("Client: mq_receive");  

      GET_SYS_TICK(ts_areceive);      
      IdleTime = utimedif(ts_breceive, ts_areceive);
      IdleTime = (IdleTime > 20) ? IdleTime : 0; 
      sigqueue(getppid(),SIGUSR2,sig_value);

      /* Calculate if the request will require IO. */
      if ( gsl_ran_bernoulli(r, P_i) ) 
	ProcTime = gsl_ran_poisson (r,C_t1); /* No IO required */
      else
	ProcTime = gsl_ran_poisson (r,C_t2); /* IO is required */


      //  fprintf(stderr, "C, %d, %6ld, %6ld, %ld.%ld \n", pnum, IdleTime, ProcTime, ts_areceive.tv_sec, ts_areceive.tv_nsec);
      
      usleep(ProcTime);
    }

exit(0);
}/*-------------------------------------------------------------------------------------------------------------------*/


/**************************************** Spawn a new Producer or consumer ********************************************/
/* INPUTS: char: Process type, use 'p' for producer, 'c' for consumers
           int:  Process number. Use this to number the process, so messages and other statistics can be collected 
	         without using PIDs.
   OUTPUT: pid_t: Process identifier when fork() fails to create a process or the process is the parent process 
   OPERATION: This function executes spawns a new process. If the process is the child process it never returns 
              and calls the child function.  If fork() fails or the process is the parent process it returns 
	      the PID of the child process.
*/

pid_t spawn_process(char ptype, int pnum)
{
pid_t child_pid;

child_pid = fork ();

/* The process is the parent process or the child couldn't be created*/
 if (child_pid != 0) 
  return child_pid;

 /* Running child only code*/
 if (ptype == 'p')
   produce(pnum); /* Process created succesfully run producer */
 else
   consume(pnum);

return 0;
}/*----------------------------------------------------------------------------------------------------------------------------------------------*/

/****************************************** Process and check input parameters  ******************************************************************/
/* INPUT: Program arguments
   OUTPUT: None
   OPERATION: This function checks all the input parameters and aborts program execution if any parameter is 
   missing, wrong type or out of range.
*/

void test_input(int argc, char* argv[])
{
  char *Str;
  if(argc != 10) 
    {
      printf("Incorrect number of input arguments.\n * %d arguments provided.\n * Please provide 9 arguments. \n", argc); 
      abort();
    }

  /* Process and check run time limits */
  RTime = strtol(argv[1], &Str, 10);
  if (argv[1] == Str)
    {
      printf("ERROR: Number expected for Run time. \n");
      abort();
    }
  if((RTime < MIN_RUN_TIME) || (RTime > MAX_RUN_TIME))
    {
      printf("Run time out of limits: %d \n * Run time must be in the range [%d ... %d] seconds.\n", RTime, MIN_RUN_TIME, MAX_RUN_TIME);
      abort();
    }


  /* Process and check Buffer size limits */
  BNum = strtol(argv[2], &Str, 10);
  if (argv[2] == Str)
    {
      printf("ERROR: Number expected for Buffer Size. \n");
      abort();
    }
  if(BNum < MIN_BUFF_SIZE || BNum > MAX_BUFF_SIZE)
    {
      printf("Buffer size (number of messages) out of limits. \n * Buffer size must be in the range [%d ... %d].\n", MIN_BUFF_SIZE, MAX_BUFF_SIZE);
      abort();
    }


  /* Process and check Number of Producers limits */
  PNum = strtol(argv[3], &Str, 10);
  if (argv[3] == Str) /* Argument is not a number */
    {
      printf("ERROR: Number expected for PRODUCERS. \n");
      abort();
    }
  if(PNum < MIN_PRODUCERS || PNum > MAX_PRODUCERS)
    {
      printf("Wrong number of producers. \n * Number of producers must be in the range [%d ... %d].\n", MIN_PRODUCERS, MAX_PRODUCERS);
      abort();
    }

  /* Process and check Number of Consumers limits */
  CNum = strtol(argv[4], &Str, 10);
  if (argv[4] == Str) /* Argument is not a number */
    {
      printf("ERROR: Number expected for CONSUMERS. \n");
      abort();
    }
  if(CNum < MIN_CONSUMERS || CNum > MAX_CONSUMERS)
    {
      printf("Wrong number of consumers. \n * Number of consumers must be in the range [%d ... %d].\n", MIN_CONSUMERS, MAX_CONSUMERS);
      abort();
    }


  /* Process and check P_t param limits */
  P_t = strtol(argv[5], &Str, 10);
  if (argv[5] == Str) /* Argument is not a number */
    {
      printf("ERROR: Number expected for P_t param. \n");
      abort();
    }
  if(P_t < P_t_PARAMS_MIN || P_t > P_t_PARAMS_MAX)
    {
      printf("Wrong P_t parameters. \n * P_t must be in the range:  [%ld ... %ld].\n", P_t_PARAMS_MIN, P_t_PARAMS_MAX);
      abort();
    }


  /* Process and check R_s param limits */
  R_s = strtol(argv[6], &Str, 10);
  if (argv[6] == Str) /* Argument is not a number */
    {
      printf("ERROR: Number expected for R_s param. \n");
      abort();
    }
  if(R_s < R_s_PARAMS_MIN || R_s > R_s_PARAMS_MAX)
    {
      printf("Wrong R_s parameter. \n * R_s must be in the range:  (%d ... %d].\n", R_s_PARAMS_MIN, R_s_PARAMS_MAX);
      abort();
    }

  /* Process and check C_t1 param limits */
  C_t1 = strtol(argv[7], &Str, 10);
  if (argv[7] == Str) /* Argument is not a number */
    {
      printf("ERROR: Number expected for C_t1 param. \n");
      abort();
    }
  if(C_t1 < C_t1_PARAMS_MIN || C_t1 > C_t1_PARAMS_MAX)
    {
      printf("Wrong C_t1 parameter. \n * C_t1 must be in the range:  [%ld ... %ld].\n", C_t1_PARAMS_MIN, C_t1_PARAMS_MAX);
      abort();
    }

  /* Process and check C_t2 param limits */
  C_t2 = strtol(argv[8], &Str, 10);
  if (argv[8] == Str) /* Argument is not a number */
    {
      printf("ERROR: Number expected for C_t2 param. \n");
      abort();
    }
  if(C_t2 < C_t2_PARAMS_MIN || C_t2 > C_t2_PARAMS_MAX)
    {
      printf("Wrong C_t2 parameter. \n * C_t2 must be in the range:  [%ld ... %ld].\n", C_t2_PARAMS_MIN, C_t2_PARAMS_MAX);
      abort();
    }

  /* Process and check P_i param limits */
  P_i = strtod(argv[9], &Str);
  if (argv[9] == Str) /* Argument is not a number */
    {
      printf("ERROR: Number expected for P_i param. \n");
      abort();
    }
  if(P_i <= 0 || P_i > P_i_PARAMS_MAX)
    {
      printf("Wrong P_i parameter. \n * P_i must be in the range:  (0 ... %d].\n", P_i_PARAMS_MAX);
      abort();
    }
} /*---------------------------------------------------------------------------------------------------*/

/***************************************************** Clean UP ******************************************************/
/* INPUT: int: Total spawned process
   OUTPUT: None
   OPERATION: This function will clean up all allocated memory and will kill any created child process. For the system 
              to work properly all the producers and cosumers specified on the program arguments must be created before
	      production/consumption can start. If during the process of creating all the processes fork() fails or
	      memory cannot be allocated for the buffer or syncronization objetcs, it is requiered to celan up all
	      currently allocated resources ane terminate the program safely.
*/

/* Forking has failed clean all process and return with exit code -1 */
void clean_up_fork_failed(int pcount_total)
{
  pid_t process_hdlr;
  int i, producer_status;
  for (i = 0; i < pcount_total; i++)
    {
      process_hdlr = wait(&producer_status);
      if (WIFEXITED (producer_status))
	printf ("the child process %d exited normally, with exit code %d\n",  process_hdlr, WEXITSTATUS (producer_status));
    }
  exit(-1);
}/*-------------------------------------------------------------------------------------------------------------------*/


/**************************************** Compute time difference  ****************************************************/
/* INPUT: int: Total spawned process
   OUTPUT: None
   OPERATION: This function will clean up all allocated memory and will kill any created child process. For the system 
              to work properly all the producers and cosumers specified on the program arguments must be created before
	      production/consumption can start. If during the process of creating all the processes fork() fails or
	      memory cannot be allocated for the buffer or syncronization objetcs, it is requiered to celan up all
	      currently allocated resources ane terminate the program safely.
*/
unsigned long int utimedif(struct timespec s1, struct timespec f1)
{
  unsigned long usecs;

  if(s1.tv_sec == f1.tv_sec) /*within same second */
    usecs = (f1.tv_nsec - s1.tv_nsec)/1000L;
  else /*  second overflow */
    {
    usecs = ((999999999L - s1.tv_nsec) + f1.tv_nsec)/1000L;
    if ((f1.tv_sec - s1.tv_sec) > 1) // More than a second
      usecs = usecs + (f1.tv_sec-s1.tv_sec)*1000000L;
    }
  return usecs;

}/*-----------------------------------------------------------------------------------------------------------------*/
