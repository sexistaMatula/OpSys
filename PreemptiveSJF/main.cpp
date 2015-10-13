//
//  main.cpp
//  RoundRobin
//
//  Created by alexis matuk on 10/10/15.
//  Copyright (c) 2015 alexis matuk. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <vector>
#include "FileReader.h"
#include "Process.h"
#include <deque>


int getProcPos(Process* proc, std::vector<Process*> processes)
{
    int pos = 0;
    for (int i = 0; i<processes.size(); i++)
    {
        if(*proc == *processes[i])
        {
            return pos;
        }
        pos++;
    }
    return pos;
}



void updateWaitingTime(std::vector<Process*> processes, Process* copy, int procPos, float startingTime)
{
    if(!processes[procPos]->getFirstSum())
    {
        processes[procPos]->addToWaitingTime(startingTime - copy->getArrivalTime());
        processes[procPos]->setFirstSum(true);
    }
    else
    {
        if(startingTime - copy->getEndingTime() < 0)
            processes[procPos]->addToWaitingTime(0);
        else
            processes[procPos]->addToWaitingTime(startingTime - copy->getEndingTime());
    }
}


void terminateProgram(std::vector<Process*> & processes, std::vector<Process*> & result)
{
    std::cout << std::endl;
    std::cout << "=====Processes====" << std::endl;
    for(auto i : result)
        std::cout << *i;
    std::cout << std::endl;
    std::cout << std::endl;
    float waitingTime = 0;
    std::cout << "=====Waiting Times====" << std::endl;
    for(auto i : processes)
    {
        waitingTime+=i->getWaitingTime();
        std::cout << i->getName() << " - " << i->getWaitingTime() << std::endl;
    }
    std::cout << "Average waiting time: " << (float)waitingTime/(size_t)processes.size() << std::endl;
    std::cout << std::endl;
    std::cout << "=====Queue States====" << std::endl;
    for(auto i : processes)
        delete i;
    for(auto i : result)
        delete i;
}

bool myfunction (Process* i,Process* j) { return (*i<*j); }
bool sortQueue(Process* i, Process* j)
{
    if(i->getTimeLeft() == j->getTimeLeft())
        return i->getArrivalTime() < j->getArrivalTime();
    return i->getTimeLeft() < j->getTimeLeft();
}

bool insideAvailableProcs(Process* proc, std::vector<Process*>availableProcs)
{
    std::vector<Process*>::iterator it;
    
    it = std::find (availableProcs.begin(), availableProcs.end(), proc);
    if (it != availableProcs.end())
        return true;
    return false;
}

void updateAvailableProcs(std::vector<Process*> processes, std::vector<Process*> & availableProcs, float timeElapsed, float startingTime, int procPos, Process* proc)
{
    for(int i=procPos; i<processes.size(); i++)
    {
        if(*processes[i] == *proc)
        {
            continue;
        }
        else
        {
            if(processes[i]->getArrivalTime() >= startingTime && processes[i]->getArrivalTime() <= timeElapsed && !insideAvailableProcs(processes[i], availableProcs))
            {
                availableProcs.push_back(processes[i]);
            }
        }
    }
}

void updateCurrentProcess(Process* & proc, Process* & checkProc, std::vector<Process*> availableProcs, std::vector<Process*> processes, bool & checkedAllProcs)
{
    //Si tienen el mismo tieme left que proc, dejas el proc
    std::vector<Process*> copyVector = availableProcs;
    std::sort(copyVector.begin(), copyVector.end(), sortQueue);
    int j = 0;
    while(j < availableProcs.size()-1)
    {
        if(copyVector[j]->getTimeLeft() != 0)
        {
            Process* newProc = copyVector[j];
            float procPos = getProcPos(newProc, availableProcs);
            proc = processes[procPos];
            break;
        }
        j++;
    }
    
    float procPos = getProcPos(proc, processes);
    int i = procPos+1;
    while(i < processes.size())
    {
        if(processes[i]->getArrivalTime() == proc->getArrivalTime())
        {
            i++;
            continue;
        }
        else
        {
            if(!insideAvailableProcs(processes[i], availableProcs))
            {
                checkProc = processes[i];
                break;
            }
        }
        i++;
    }
}

int main(int argc, const char * argv[]) {
    std::vector<Process*> availableProcs;
    std::vector<Process*> processes;
    std::vector<Process*> result;
    FileReader reader("SJF2.txt");
    reader.openFile();
    reader.readProcesses(processes);
    std::sort(processes.begin(), processes.end(), myfunction);
    Process* proc = processes[0];
    float timeElapsed = processes[0]->getArrivalTime();
    int procPos = 0;
    bool checkedAllProcs = false;
    Process* checkProc;
    float startingTime = 0.0;
    availableProcs.push_back(proc);
    
    int i = 0;
    while(processes[i]->getArrivalTime() == proc->getArrivalTime())
    {
        i++;
        checkProc = processes[i];
    }
    while(!checkedAllProcs)
    {
        float workingTime = checkProc->getArrivalTime() - timeElapsed;
        if(workingTime < proc->getTimeLeft())
        {
            procPos = getProcPos(proc, processes);
            startingTime = timeElapsed;
            timeElapsed+=workingTime;
            Process* copy = new Process(proc);
            availableProcs[procPos]->setTimeLeft(copy->getTimeLeft() - workingTime);
            processes[procPos]->setTimeLeft(copy->getTimeLeft() - workingTime);
            copy->setEndingTime(timeElapsed);
            copy->setStartingTime(startingTime);
            copy->setTimeLeft(copy->getTimeLeft() - workingTime);
            result.push_back(copy);
            procPos = getProcPos(proc, processes);
            updateAvailableProcs(processes, availableProcs, timeElapsed, startingTime, procPos, proc);
        }
        else
        {
            procPos = getProcPos(proc, processes);
            startingTime = timeElapsed;
            workingTime = proc->getTimeLeft();
            timeElapsed+=workingTime;
            Process* copy = new Process(proc);
            availableProcs[procPos]->setTimeLeft(copy->getTimeLeft() - workingTime);
            processes[procPos]->setTimeLeft(copy->getTimeLeft() - workingTime);
            copy->setEndingTime(timeElapsed);
            copy->setStartingTime(startingTime);
            copy->setTimeLeft(copy->getTimeLeft() - workingTime);
            result.push_back(copy);
            procPos = getProcPos(proc, processes);
            updateAvailableProcs(processes, availableProcs, timeElapsed, startingTime, procPos, proc);
        }
        updateCurrentProcess(proc, checkProc, availableProcs, processes, checkedAllProcs);
        if(availableProcs.size() == processes.size())
            checkedAllProcs = true;
    }
    std::sort(availableProcs.begin(), availableProcs.end(), sortQueue);
    for( int i = 0; i<availableProcs.size(); i++)
    {
        if(availableProcs[i]->getTimeLeft() == 0)
            continue;
        else
        {
            procPos = getProcPos(availableProcs[i], processes);
            startingTime = timeElapsed;
            timeElapsed+=availableProcs[i]->getTimeLeft();
            Process* copy = new Process(availableProcs[i]);
            availableProcs[i]->setTimeLeft(0.0);
            copy->setStartingTime(startingTime);
            copy->setEndingTime(timeElapsed);
            copy->setTimeLeft(0.0);
            result.push_back(copy);
        }
    }
    for(int i=0; i<result.size(); i++)
    {
        float procPos = getProcPos(result[i], processes);
        if(!processes[procPos]->getFirstSum())
        {
            processes[procPos]->addToWaitingTime(result[i]->getStartingTime() - result[i]->getArrivalTime());
            processes[procPos]->setEndingTime(result[i]->getEndingTime());
            processes[procPos]->setFirstSum(true);
        }
        else
        {
            if(result[i]->getStartingTime() - processes[procPos]->getEndingTime() < 0)
                processes[procPos]->addToWaitingTime(0);
            else
                processes[procPos]->addToWaitingTime(result[i]->getStartingTime() - processes[procPos]->getEndingTime());
        }
    }
    terminateProgram(processes, result);
    return 0;
}