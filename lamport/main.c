#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

/* 
    Each processor p starts with an arbitrary value alpha(p). Execution of the system proceeds
    in synchronous rounds: at each round, the following two steps occur:
    (1) First, each non-faulty processor writes values ("sends messages") derived from its state into all
    of its out channels, while each faulty processor writes arbitrary values into all of its out channels.
    (2) Second, each processor reads the values from all of its in-channels. After some specified nmumber, k,
    of rounds, each processor p outputs a vector of values, one for each processor p.
    Each processor can then make decisions about the values received by the other replicas
*/

int **_rvalues = NULL;
int R;
int N;


void sig_handler(int signum){
    for(int i = 0; i < R; i++)
        free(_rvalues[i]);
    free(_rvalues);
    fprintf(stderr, "sig kill");
}

int main(int argc, char** argv)
{
    /* Inputs R and K, whcih are the number of replicas and the number of rounds */
    R = atoi(argv[1]);
    N = atoi(argv[2]);

    // Register signal handler
    signal(SIGINT,sig_handler); 

    /* Intializes random number generator */
    time_t t;
    srand((unsigned) time(&t));

    /* Now we create a matrix with for the values of the replicas on each round */
    _rvalues = (int **)malloc(R * sizeof(int*));
    for(int i = 0; i < R; i++)
    {
        _rvalues[i] = (int *)malloc(N * sizeof(int));
    }

    /* Fill the array */
    for(int i = 0; i < R; i++) 
    {
        for(int j = 0; j < N; j++)
        {
            _rvalues[i][j] = rand() % 50;
        }
    }

    /* Start the execution loop */
    while(1)
    {

    }

    for(int i = 0; i < R; i++)
        free(_rvalues[i]);
    free(_rvalues);

}