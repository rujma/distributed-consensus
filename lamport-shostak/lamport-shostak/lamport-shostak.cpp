#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <algorithm>

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
const char DEFAULT = 'A';
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
    {
        /* Generate which processor ID will be faulty */
        srand(time(NULL));
        for (int i = 0; i < 2; i++)
        {
            faultyProcessors.push_back((rand() % (n)));
            std::cout << "Faulty: " << faultyProcessors.at(i) << std::endl;
        }
    }

    /* Get the value return by a given process during a messaging round */
    char GetValue(char value, int source, int destination)
    {
        if (isFaulty(source))
        {
            int random = rand() % 2;
            return rand == 0 ? ZERO : ONE;
        }
        else
        {
            return value;
        }
    }

    Node GetSourceValue()
    {
        return Node(ZERO, UNKNOWN);
    }

    /* Return a default value */
    char getDefault()
    {
        return DEFAULT;
    }

    /* Identify the fault processes by ID */
    bool isFaulty(int process)
    {
        return std::any_of(faultyProcessors.begin(), faultyProcessors.end(), [process](int elem)
        {
            return process == elem;
        });
    }


    const int mSource;    /* Process ID for the source process */
    const int mM;         /* Number of messaging rounds */
    const size_t mN;      /* Number of processes */
    const bool mDebug;
    std::vector<int> faultyProcessors;  /* List of faulty processors */
};

class Process
{
public:
    Process(int id) : mId(id)
    {
        if (mChildren.size() == 0)
        {
            generateChildren(mProperties.mM, mProperties.mN, std::vector<bool>(mProperties.mN, true));
        }

        if (mId == mProperties.mSource)
        {
            mNodes[""] = mProperties.GetSourceValue();
        }
    }

    bool IsFaulty()
    {
        return mProperties.isFaulty(mId);
    }

    bool IsSource()
    {
        return mProperties.mSource == mId;
    }

    /* Call SendMessages on each process for every round. */
    void SendMessages(int round, std::vector<Process>& processes)
    {
        for (size_t i = 0; i < mPathsRank[round][mId].size(); i++)
        {
            Path source_node_path = mPathsRank[round][mId][i];
            source_node_path = source_node_path.substr(0, source_node_path.size() - 1);
            Node source_node = mNodes[source_node_path];
            for (size_t j = 0; j < mProperties.mN; j++)
            {
                if (j != mProperties.mSource)   /* Send to everyone except the general */
                {
                    char value = mProperties.GetValue(source_node.input_value, mId, (int)j);
                    std::cout << "Sending from process " << mId
                        << " to " << static_cast<unsigned int>(j)
                        << ": {" << value << ", "
                        << mPathsRank[round][mId][i]
                        << ", " << UNKNOWN << "}"
                        << ", getting value from source_node " << source_node_path
                        << "\n";
                    processes[j].receiveMessage(mPathsRank[round][mId][i], Node(value, UNKNOWN));
                }
            }
        }
    }

    char Decide()
    {
        /* The source does not have to do anything */
        if (mId == mProperties.mSource)
        {
            return mNodes[""].input_value;
        }

        /* Go for the leaf values */
        for (size_t i = 0; i < mProperties.mN; i++)
        {
            for (size_t j = 0; j < mPathsRank[mProperties.mM][i].size(); j++)
            {
                const Path& path = mPathsRank[mProperties.mM][i][j];
                Node& node = mNodes[path];
                node.output_value = node.input_value;
                //std::cout << "Processor: " << i << " Path: " << path << " In : " << node.input_value << " Out : " << node.output_value << std::endl;
            }
        }

        for (int round = (int)mProperties.mM - 1; round >= 0; round--)
        {
            for (size_t i = 0; i < mProperties.mN; i++)
            {
                for (size_t j = 0; j < mPathsRank[round][i].size(); j++)
                {
                    const Path& path = mPathsRank[round][i][j];
                    Node& node = mNodes[path];
                    node.output_value = getMajority(path);
                }
            }
        }
        const Path& top_path = mPathsRank[0][mProperties.mSource].front();
        const Node& top_node = mNodes[top_path];
        return top_node.output_value;
    }

private:
    int mId;                        /* Process ID */
    std::map<Path, Node> mNodes;    /* Process tree */
    static Properties mProperties;
    static std::map<Path, std::vector<Path>> mChildren;
    static std::map<size_t, std::map<size_t, std::vector<Path>>> mPathsRank;

    /* Private functions */
    char getMajority(const Path& path)
    {

        std::map<char, size_t> outputValues;
        size_t n = mChildren[path].size();
        for (size_t i = 0; i < n; i++)
        {
            const Path& child = mChildren[path][i];
            const Node& node = mNodes[child];
            outputValues[node.output_value]++;
        }
        /* Perform majority voting */
        if (outputValues[ONE] > (n / 2))
            return ONE;
        if (outputValues[ZERO] > (n / 2))
            return ZERO;
        if (outputValues[ONE] == outputValues[ZERO] && outputValues[ONE] == (n / 2))
            return mProperties.getDefault();
        return UNKNOWN;
    }

    void receiveMessage(const Path& path, const Node& node)
    {
        mNodes[path] = node;
    }

    /* Generate the tree that will be composed by all processors */
    static void generateChildren(const size_t m, const size_t n, std::vector<bool> finished, int source = mProperties.mSource, Path current_path = "", size_t rank = 0)
    {
        /* Set the corresponding ID as finished */
        finished[source] = false;
        /* Add to its path */
        current_path += static_cast<char>(source + '0');
        mPathsRank[rank][source].push_back(current_path);
        /* Check rank smaller than number of rounds */
        if (rank < m)
        {
            for (int i = 0; i < static_cast<int>(finished.size()); i++)
            {
                if (finished[i] == true)
                {
                    generateChildren(m, n, finished, i, current_path, rank + 1);
                    mChildren[current_path].push_back(current_path + static_cast<char>(i + '0'));
                }
            }
        }
        std::cout << current_path << ", children = ";
        for (size_t j = 0; j < mChildren[current_path].size(); j++)
            std::cout << mChildren[current_path][j] << " ";
        std::cout << "\n";
    }
};

const int N = 7;            /* Number of processes */
const int M = 2;            /* Number of rounds which is the number of faults to be mitigated (or faulty processors) */
const int GENERAL = 3;      /* Processor ID that acts like the general */
const bool DEBUG = false;   /* Debug paths */


std::map<Path, std::vector<Path> > Process::mChildren;
std::map<size_t, std::map<size_t, std::vector<Path> > >  Process::mPathsRank;
Properties Process::mProperties = Properties(GENERAL, M, N, DEBUG);


int main()
{
    /* Generate processes*/
    std::vector<Process> processes;
    for (int i = 0; i < N; i++)
    {
        processes.push_back(Process(i));
    }

    std::cout << "Finished generating Children" << std::endl;

    for (int i = 0; i <= M; i++)
        for (int j = 0; j < N; j++)
            processes[j].SendMessages(i, processes);


    std::cout << "Finished sending messages" << std::endl;

    for (int j = 0; j < N; j++) 
    {
        if (processes[j].IsSource())
            std::cout << "Source ";
        std::cout << "Process " << j;
        if (processes[j].IsFaulty())
            std::cout << " (faulty)";
        std::cout << " decided on value " << processes[j].Decide() << std::endl;
    }




    return 0;
}