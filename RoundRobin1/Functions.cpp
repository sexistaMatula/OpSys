//
//  Functions.cpp
//  RoundRobin
//
//  Created by alexis matuk on 10/11/15.
//  Copyright (c) 2015 alexis matuk. All rights reserved.
//

#include "Functions.h"
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

bool inQueue(Process* proc, std::deque<Process*> _queue)
{
    for(auto i : _queue)
    {
        if( *i == *proc)
            return true;
    }
    return false;
}

void checkProcessToQueue(int procPos, std::vector<Process*> processes, Process * copy,std::deque<Process*> & RequestQueue, float timeElapsed, bool & checkedAllProcs)
{
    for(int i = procPos+1; i < processes.size(); i++)
    {
        if(procPos == processes.size() - 1)
        {
            if(!inQueue(copy, RequestQueue) && copy->getTimeLeft() > 0)
                RequestQueue.push_back(processes[i-1]);
            break;
        }
        else if(processes[i]->getArrivalTime() <= timeElapsed && processes[i]->getTimeLeft() > 0)
        {
            if(!inQueue(processes[i], RequestQueue))
                RequestQueue.push_back(processes[i]);
            if( i == processes.size() - 1 )
                checkedAllProcs = true;
        }
        else
        {
            break;
        }
    }
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


void terminateProgram(std::vector<Log*> & log, std::vector<Process*> & processes, std::vector<Process*> & result)
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
    for(auto  i : log)
    {
        std::cout << "Time: " << i->currentTime << std::endl;
        if(i->queueStates.empty())
            std::cout << "Request queue empty";
        else
        {
            std::cout << "Queue - ";
            for( auto j : i->queueStates)
            {
                std::cout << *j;
            }
        }
        std::cout << std::endl;
    }
    for(auto i : processes)
        delete i;
    for(auto i : result)
        delete i;
    for(auto  i : log)
        delete i;
}

void roundRobin(std::deque<Process*> & RequestQueue, std::vector<Process*> & processes, std::vector<Log*> & log, std::vector<Process*> & result, float quantum)
{
    Process* proc;
    float timeElapsed = processes[0]->getArrivalTime();
    int procPos = 0;
    bool checkedAllProcs = false;
    while (!RequestQueue.empty())
    {
        proc = RequestQueue.front();
        RequestQueue.pop_front();
        Process* copy = new Process(proc);
        procPos = getProcPos(proc, processes);
        if(copy->getTimeLeft() <= quantum)
        {
            float startingTime = timeElapsed;
            timeElapsed += copy->getTimeLeft();
            updateWaitingTime(processes, copy, procPos, startingTime);
            copy->setStartingTime(startingTime);
            copy->setEndingTime(startingTime + copy->getTimeLeft());
            copy->setTimeLeft(0);
            result.push_back(copy);
            if(!checkedAllProcs)
                checkProcessToQueue(procPos, processes, copy, RequestQueue, timeElapsed, checkedAllProcs);
            Log* l = new Log();
            std::deque<Process*>* queueCopy = new std::deque<Process*>(RequestQueue);
            l->queueStates = *queueCopy;
            l->currentTime = timeElapsed;
            log.push_back(l);
        }
        else
        {
            float startingTime = timeElapsed;
            if(copy->getTimeLeft() - quantum < 0)
                copy->setTimeLeft(0);
            else
                copy->setTimeLeft(copy->getTimeLeft() - quantum);
            updateWaitingTime(processes, copy, procPos, startingTime);
            copy->setStartingTime(startingTime);
            copy->setEndingTime(timeElapsed+quantum);
            result.push_back(copy);
            timeElapsed+=quantum;
            if(!checkedAllProcs)
                checkProcessToQueue(procPos, processes, copy, RequestQueue, timeElapsed, checkedAllProcs);
            if(copy->getTimeLeft() > 0)
                RequestQueue.push_back(copy);
            Log* l = new Log();
            std::deque<Process*>* queueCopy = new std::deque<Process*>(RequestQueue);
            l->queueStates = *queueCopy;
            l->currentTime = timeElapsed;
            log.push_back(l);
        }
    }
}