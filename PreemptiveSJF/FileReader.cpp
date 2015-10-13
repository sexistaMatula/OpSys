//
//  FileReader.cpp
//  RoundRobin
//
//  Created by alexis matuk on 10/10/15.
//  Copyright (c) 2015 alexis matuk. All rights reserved.
//

#include "FileReader.h"

FileReader::~FileReader()
{
    closeFile();
}

/*
 Método para lectura de un archivo
 
 @params _route es el nombre del archivo con el formato de archivo especificado
 
 El archivo debe estar en el mismo directorio relativo al .cpp
 */
void FileReader::openFile()
{
    try
    {
        if (std::ifstream("./"+route))
        {
            std::cout << "File found... opening " << std::endl;
            file.open("./"+route);
        } else
        {
            std::cout << "File not found... creating new one" << std::endl;
            file.open("./"+route);
        }
        if(file.fail())
            throw 1;
    }
    catch (int e)
    {
        std::cerr << "Error reading data... Exception " << e << " caught" << std::endl;
    }
}

/*
 Método para cerrar el archivo abierto
 */
void FileReader:: closeFile()
{
    std::cout << std::endl;
    if(file.is_open())
    {
        file.close();
        std::cout << "File closed successfully" << std::endl;
    }
    else
    {
        std::cout << "There's no file opened" << std::endl;
    }
}

/*
 Método para leer los procesos del archivo file y guardarlos en un vector de procesos
 
 @params _processes es una referencia a un vector vacío de apuntadores a Process
 
 */
void FileReader::readProcesses(std::vector<Process*> & _processes)
{
    std::cout << "Reading Processes..." << std::endl;
    int lineCounter = 1;
    while(!file.eof())
    {
        if(lineCounter == 1)
        {
            file.ignore(1,'\n');
        }
        else
        {
            std::string proc;
            float arrTime, burst;
            file >> proc;
            file >> arrTime;
            file >> burst;
            Process* pr = new Process(proc, arrTime, burst);
            _processes.push_back(pr);
        }
        lineCounter++;
    }
}
