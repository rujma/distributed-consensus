// lamport-shostak-rr.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <algorithm>
#include "SharedBus.h"

//
// Some useful global definitions
//
typedef std::string Path;
typedef std::pair<Path, int> Message;
const int N = 4;			/* Number of processes */
const int M = 1;			/* Number of rounds which is the number of faults to be mitigated (or faulty processors) */
const bool GENERAL = true;
const int myID = 2;

/* Map of the possible tree paths */
static std::map<Path, std::vector<Path> > pathTree_;
static std::map<size_t, std::map<size_t, std::vector<Path>>> pathTreeByRank_;
static std::map<Path, int> pathNodes;
SharedBus sb_;


int getSourceValue()
{
	return 1;
}


int getMajority(const std::string& path)
{
	return 1;
}

int decide()
{
	/* If you are the general, do not iterate through the tree */
	//return getSourceValue();

	/* Loop through all the nodes and perform majority voting */
	for (int round = (M); round >= 1; round--)
	{
		for (size_t i = 0; i < N; i++)
		{
			for (size_t j = 0; j < pathTreeByRank_[round][i].size(); j++)
			{
				const Path& path = pathTreeByRank_[round][i][j];
				std::cout << path << std::endl;
				//int decision = getMajority(path);
			}
		}
	}
	return 1;
}

void sendSourceMessages()
{
	/* On a real application we retrive a value from w/e */
	int sourceValue = getSourceValue();
	/* Loop through all lieutenants */
	for (int i = 1; i <= N; i++)
	{
		if (i != myID)
		{
			/* Create a message with a path */
			Path path = "";
			path += static_cast<char>(myID + '0');
			Message msg = Message(path, sourceValue);
			std::string msg_str = msg.first;
			msg_str += "," + std::to_string(msg.second);
			/* Send message to corresponding lieutenant */
			sb_.send(msg_str, i);
		}
	}
}

void receiveMessage()
{
	/* Receive a single message from the shared bus and put it on the correct path */
	std::string msg_str = sb_.receive();
	/* Parse into message */
	std::string path = msg_str.substr(0, msg_str.find(','));
	std::string value = msg_str.erase(0, msg_str.find(',') + sizeof(char));
	Message msg(path, std::stoi(value));
	/* Add to nodes: BEWARE OF FIRST MESSAGE */
	/*                              #TODO: DEAL WITH THE MESSAGE SENT FROM THE GENERAL                           */
	pathNodes[msg.first] = msg.second;
}

/* Generate the possible paths */
void generatePaths(const int n, const int m, std::vector<bool> ids, int source = 0, Path current_path = "", int rank = 0)
{
	ids[source] = true;
	current_path += static_cast<char>(source + '0');
	pathTreeByRank_[rank][source].push_back(current_path);
	if (rank < m)
	{
		for (int i = 0; i < static_cast<int>(ids.size()); i++)
		{
			if (ids[i] == false)
			{
				generatePaths(n, m, ids, i, current_path, rank + 1);
				pathTree_[current_path].push_back(current_path + static_cast<char>(i + '0'));
			}
		}
	}
	std::cout << current_path << ", children = ";
	for (size_t j = 0; j < pathTree_[current_path].size(); j++)
		std::cout << pathTree_[current_path][j] << " ";
	std::cout << "\n";
}

std::string dumpTree(Path path = "")
{
	bool root = false;
    std::stringstream s;
    if (path == "") {
        root = true;
        path = pathTreeByRank_[0][1].front();
        s << "digraph byz {\n"
            << "rankdir=LR;\n"
            << "nodesep=.0025;\n"
            << "node [fontsize=8,width=.005,height=.005,shape=plaintext];\n"
            << "edge [fontsize=8,arrowsize=0.25];\n";
    }
    for (size_t i = 0; i < pathTree_[path].size(); i++)
        s << dumpTree(pathTree_[path][i]);
    if (path.size() == 1)
        s << "General->";
    else {
        Path parent_path = path.substr(0, path.size() - 1);
        s  << "\"{" << parent_path << "}\"->";
    }
	s << "\"{" << path << "}\";\n";
    if (root)
        s << "}\n";
    return s.str();
}


int main()
{
	/* Generate paths for each possible general */
	for (int i = 0; i < N; i++)
	{
		generatePaths(N, M, std::vector<bool>(N, false), i);
		
	}

	/* As a general, I have to send messages to each replica */
	if (GENERAL == true)
	{
		sendSourceMessages();
		decide();
	}
	else    /* As a lieutenant I wait for messages */
	{

	}

	
	//std::cout << pathTreeByRank_[0][1].size() << std::endl;
	/*for (int r = 0; r <= M; r++)
	{
		for (int i = 0; i < pathTreeByRank_[r].size(); i++)
			//std::vector<Path> vec(pathTreeByRank_[0][1]);
		{
			//std::vector<Path> vec(pathTreeByRank_[0][0]);
			for (int j = 0; j < pathTreeByRank_[r][i].size(); j++)
				std::cout << pathTreeByRank_[r][i][j] << " ";
			
		}
		std::cout << std::endl;
	}*/


	return 0;
}

