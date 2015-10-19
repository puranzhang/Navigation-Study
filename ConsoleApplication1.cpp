// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ConsoleApplication1.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

//uncomment the line "vibNum = 8;" to make it end a connection

int _tmain(int argc, _TCHAR* argv[])
{

	printf("Welcome to the serial com app!\n");

	std::wstring comNum;
	if (argc != 2)
	{
		std::wcout << L"\nEnter the port (ex: COM4): ";
		std::wcin >> comNum;
	}
	else {
		std::wcout << L"\nPort name from command line ";
		comNum = argv[1];
	}
	std::wstring comPrefix = L"\\\\.\\";
	std::wstring comID = comPrefix + comNum;


	Serial* SP = new Serial(comID.c_str());

	if (SP->IsConnected())
		printf("We're connected");



	while (SP->IsConnected()){	// If file is not open.

		std::ifstream ifs("heading.txt", std::ifstream::in);

		if (ifs.is_open()){

			int array_size = 3;  // Should just be a 1 digit number. 
			char * heading = new char[array_size]; 
			int position = 0; //this will be used incremently to fill characters in the array 
			bool badData = false;

			/*checking whether file could be opened or not. If file does not exist or don't have read permissions, file
			stream could not be opened.*/
			//file opened successfully so we are here
			//cout << "File Opened successfully!!!. Reading data from file into array" << endl;
			//this loop run until end of file (eof) does not occur
			while (!ifs.eof() && position < array_size+1)
			{
				ifs.get(heading[position]); //reading one character from file to array
				position++;
			}

			
			//std::cout << heading[0];
			//std::cout << heading[1];
			//std::cout << heading[2];
			//std::cout << "\n";

			if (heading[0] < '0' || heading[0] > '9'){
				badData = true;
				std::cout << "bad data";
			}

			std::string str(heading);


			int value = atoi(str.c_str());
			//heading[position-1] = 'a';
			//heading[position] = '\n'; //placing character array terminating character

			ifs.close();

			int vibNum;

			if ((value>338) || (value <= 22)){
				vibNum = 0;
			}
			else if ((value > 22) && (value <= 68)){
				vibNum = 7;
			}
			else if ((value > 68) && (value <= 113)){
				vibNum = 6;
			}
			else if ((value > 113) && (value <= 158)){
				vibNum = 5;
			}
			else if ((value > 158) && (value <= 203)){
				vibNum = 4;
			}
			else if ((value > 203) && (value <= 248)){
				vibNum = 3;
			}
			else if ((value > 248) && (value <= 293)){
				vibNum = 2;
			}
			else if ((value > 293) && (value <= 338)){
				vibNum = 1;
			}


			array_size = 1;

			unsigned int arraySize = 1;

			//vibNum = 8;  // uncomment to stop an established connection.

			std::string vibNumString = std::to_string(vibNum);
			const char *cstrCst = vibNumString.c_str();
			char *cstr = new char[vibNumString.length() + 1];
			strcpy_s(cstr,2, cstrCst);

			if (badData!= true){

				SP->WriteData(cstr, arraySize);

			}

			delete[] cstr;
			delete[] heading;

			Sleep(75);
			
			//The following two lines makes the program quit
			ifs.close();
			exit(0);
		}
		else{
			Sleep(75);
		}
	}

	return 0;
}


Serial::Serial(LPCWSTR portName)
{
	//We're not yet connected
	this->connected = false;

	//Try to connect to the given port throuh CreateFile
	this->hSerial = CreateFile( portName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	//Check if the connection was successfull
	if (this->hSerial == INVALID_HANDLE_VALUE)
	{
		//If not success full display an Error
		if (GetLastError() == ERROR_FILE_NOT_FOUND){

			//Print Error if neccessary
			printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);

		}
		else
		{
			printf("ERROR!!!");
		}
	}
	else
	{
		//If connected we try to set the comm parameters
		DCB dcbSerialParams = { 0 };

		//Try to get the current
		if (!GetCommState(this->hSerial, &dcbSerialParams))
		{
			//If impossible, show an error
			printf("failed to get current serial parameters!");
		}
		else
		{
			//Define serial connection parameters for the arduino board
			dcbSerialParams.BaudRate = CBR_9600;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = NOPARITY;

			//Set the parameters and check for their proper application
			if (!SetCommState(hSerial, &dcbSerialParams))
			{
				printf("ALERT: Could not set Serial Port parameters");
			}
			else
			{
				//If everything went fine we're connected
				this->connected = true;
				//We wait 2s as the arduino board will be reseting
				Sleep(ARDUINO_WAIT_TIME);
			}
		}
	}

}

Serial::~Serial()
{
	//Check if we are connected before trying to disconnect
	if (this->connected)
	{
		//We're no longer connected
		this->connected = false;
		//Close the serial handler
		CloseHandle(this->hSerial);
	}
}

int Serial::ReadData(char *buffer, unsigned int nbChar)
{
	//Number of bytes we'll have read
	DWORD bytesRead;
	//Number of bytes we'll really ask to read
	unsigned int toRead;

	//Use the ClearCommError function to get status info on the Serial port
	ClearCommError(this->hSerial, &this->errors, &this->status);

	//Check if there is something to read
	if (this->status.cbInQue>0)
	{
		//If there is we check if there is enough data to read the required number
		//of characters, if not we'll read only the available characters to prevent
		//locking of the application.
		if (this->status.cbInQue>nbChar)
		{
			toRead = nbChar;
		}
		else
		{
			toRead = this->status.cbInQue;
		}

		//Try to read the require number of chars, and return the number of read bytes on success
		if (ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL) && bytesRead != 0)
		{
			return bytesRead;
		}

	}

	//If nothing has been read, or that an error was detected return -1
	return -1;

}


bool Serial::WriteData(char *buffer, unsigned int nbChar)
{
	DWORD bytesSend;

	//printf("sending: ");
	for (unsigned int j = 0; j < nbChar; j++)
	{
		//printf("%c \n", buffer[j]);
	}

	//Try to write the buffer on the Serial port
	if (!WriteFile(this->hSerial, (void *)buffer, nbChar, &bytesSend, 0))
	{
		//In case it don't work get comm error and return false
		ClearCommError(this->hSerial, &this->errors, &this->status);

		return false;
	}
	else
		return true;
}

bool Serial::IsConnected()
{
	//Simply return the connection status
	return this->connected;
}