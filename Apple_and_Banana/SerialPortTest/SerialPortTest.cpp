// main.cpp : Defines the entry point for the console application.  
//  

#include "stdafx.h"
#include "SerialPort.h"  
#include <iostream>  

int _tmain(int argc, _TCHAR* argv[])
{

	CSerialPort mySerialPort;
	if (!mySerialPort.InitPort(7))
	{
		std::cout << "initPort fail !" << std::endl;
	}
	else
	{
		std::cout << "initPort success !" << std::endl;
	}

	if (!mySerialPort.OpenListenThread())
	{
		std::cout << "OpenListenThread fail !" << std::endl;
	}
	else
	{
		std::cout << "OpenListenThread success !" << std::endl;
	}

	int temp;
	while (1)
	{
		std::cin >> temp;
		if (!mySerialPort.BuffSend(temp))
		{
			std::cout << "SerialPort Data Send fail !" << std::endl;
		}
		else
		{
			std::cout << "SerialPort Data Send success !" << std::endl;
		}
	}

	return 0;
}