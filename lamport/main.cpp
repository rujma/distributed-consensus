#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>

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






int main(int argc, char** argv)
{
    return 0;
}