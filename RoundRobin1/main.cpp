//
//  main.cpp
//  RoundRobin
//
//  Created by alexis matuk on 10/10/15.
//  Copyright (c) 2015 alexis matuk. All rights reserved.
//

#include "Functions.h"

bool myfunction (Process* i,Process* j) { return (*i<*j); }

int main(int argc, const char * argv[]) {
    std::deque<Process*> RequestQueue;
    std::vector<Process*> processes;
    std::vector<Log*> log;
    std::vector<Process*> result;
    FileReader reader("roundrobin1.txt");
    reader.openFile();
    reader.readProcesses(processes);
    float quantum = reader.readQuantum();
    // Falta checar que las entradas estén en órden ascendente dependiendo de su arrival time
    std::sort(processes.begin(), processes.end(), myfunction);
    RequestQueue.push_back(processes[0]);
    roundRobin(RequestQueue, processes, log, result, quantum);
    terminateProgram(log, processes, result);
    return 0;
}
