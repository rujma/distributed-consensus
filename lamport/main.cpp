#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <random>

/* 
    A single processor p starts with an arbitrary value alpha(p). Execution of the system proceeds
    in synchronous rounds: at each round, the following two steps occur:
    (1) First, each non-faulty processor writes values ("sends messages") derived from its state into all
    of its out channels, while each faulty processor writes arbitrary values into all of its out channels.
    (2) Second, each processor reads the values from all of its in-channels. After some specified nmumber, k,
    of rounds, each processor p outputs a vector of values, one for each processor p.
    Each processor can then make decisions about the values received by the other replicas
*/

/* Useful types */
typedef std::string Path;
const char ONE = '1';
const char ZERO = '0';
const char UNKNOWN = '?';
const char FAULTY = 'X';

/* A Node is something like a pass-through between different rounds of the protocol */
/* Input is set on the first round, output is set on the second round */
struct Node
{
    Node(char input = FAULTY, char output = FAULTY)
    {
        input_value = input;
        output_value = output;
    };
    char input_value;
    char output_value;
};

/* Messaging behaviour for the processes */
/* This class is used to identify who is a general and who is a lieutenant */
class Properties
{
    public:
        /* */
        Properties(int source, int m, int n, bool debug = false) : mSource(source), mM(m), mN(n), mDebug(debug)
        {}

        /* Get the value return by a given process during a messaging round */

        /* Return a default value */
        char getDefault()
        {
            return ONE;
        }

        /* Identify the fault processes by ID */
        bool isFaulty(int process)
        {
            //return std::any_of(faultyProcessors.begin(), faultyProcessors.end(), [process](int elem)
            //{
            //    return process == elem;
            //});
        }
        
        const int mSource;    /* Process ID for the source process */     
        const int mM;         /* Number of messaging rounds */      
        const size_t mN;      /* Number of processes */
        const bool mDebug;
};

const int N         = 4;        /* Number of processes */
const int M         = 1;        /* Number of rounds which is the number of faults to be mitigated (or faulty processors) */
const int GENERAL   = 3;        /* Processor ID that acts like the general */
const bool DEBUG    = false;    /* Debug paths */


std::vector<int> faultyProcessors;


int main()
{
    /* Generate processes*/
    /* Generate which processor ID will be faulty */
    for (int i = 0; i < M; i++)
    {
        faultyProcessors.push_back((rand() % (N - 1)));
    }

    for (unsigned int i = 0; i < faultyProcessors.size(); i++)
        std::cout << faultyProcessors.at(i) << std::endl;


    return 0;
}