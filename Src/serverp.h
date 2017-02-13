/* Header file for program Producer/Consumer Assignment No. 1 ECE 650 */

#ifndef SERVER_BY_T
#define SERVER_BY_T

/* Running times limits in seconds */
#define MIN_RUN_TIME 10
#define MAX_RUN_TIME (300)

/* Buffer size in the number of messages */
#define MIN_BUFF_SIZE 1
#define MAX_BUFF_SIZE 10
/* Message size is specified in bytes */
#define MAX_MSG_SIZE  256

/* Producer and consumer limits */
#define MIN_PRODUCERS 1
#define MAX_PRODUCERS 8

#define MIN_CONSUMERS 1
#define MAX_CONSUMERS 8

/* P_t_params: Probability distribution for the random time P_t that the producers
   must wait between request production.

   Values are in microseconds, limits are 50 milliseconds to 200 milliseconds. The user must specify $\lambda$ 
   for the Poisson process that models the mean interarrival times of requests. 
 */

#define P_t_PARAMS_MIN 50000UL
#define P_t_PARAMS_MAX 200000UL


/* R_s parms: Probability distribution of the request size.
   Values in bytes  in the range [1..256]. The user must specify the $p$ parameter for the 
   uniform distribution, n is fixed to 255. */
#define R_s_PARAMS_MIN 4
#define R_s_PARAMS_MAX (MAX_MSG_SIZE-1)

/* CTx parms: Probability distribution of the time for consumers. Since the system is considered 
   to be M/M/c/K the values for C_t1 and C_t2 should be drawn from the exponential distribution
   with mean $\mu$. This is the mean time required to serve a request, it is the second parameter
   in an M/M/c/K model. */
/* CT1 No IO involved shortest is 10 millisecond longest is 100 milliseconds */
#define C_t1_PARAMS_MIN 10000UL
#define C_t1_PARAMS_MAX 100000UL

/* TC2 IO involved shortest is 10 millisecond longest is 200 millisecond */
#define C_t2_PARAMS_MIN 10000UL
#define C_t2_PARAMS_MAX 200000UL

/* Probability of NOT requiring IO if the ramdon choosen value for any request 
   is p(P_i) then the request computing time is calculated from CT1 and 
   CT2 otherwhise. P_i is specified by the user and 0 < P_i < 1.

   P(P_i) is taken from the Bernoulli distribution, P_i is specified by the 
   user.*/
#define P_i_PARAMS_MAX 1

/* Queue parameters */
#define QUEUE_NAME "/ReinierTorresLabrada_Assignment2" 
#define QUEUE_PERMISSIONS 0660 
#define QUEUE_MAX_MESSAGES 10 


#define WAIT_ON_BARRIER 1
#define RUN_NORMALLY    2
#define STOP_EXECUTION  3

#endif


