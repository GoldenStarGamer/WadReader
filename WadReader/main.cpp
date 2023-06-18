#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdexcept>

using namespace std;

enum WadType
{
	iwad,
	pwad
};

class WadHeader
{
public:
	WadType wadType;
	int lumpc;
	int dir;
	void headerRead(fstream& wadFile)
	{
		char x[4];
		unsigned int y;
		unsigned int z;
		wadFile.read(x, 4);
		if (!strncmp(x, "IWAD", 4))
		{
			wadType = iwad;
		}
		else if (!strncmp(x, "PWAD", 4))
		{
			wadType = pwad;
		}
		else
		{
			cout << "Invalid wad byte: " << hex << showbase << static_cast<int>(x[0]) << endl;
			throw runtime_error("not a valid wad");
		}
		wadFile.seekg(4);
		wadFile.read(reinterpret_cast<char*>(&y), 4);
		lumpc = y;
		wadFile.seekg(8);
		wadFile.read(reinterpret_cast<char*>(&z), 4);
		dir = z;
	}
};

class WadDir
{
public:
	unsigned int lumpPos;
	unsigned int size;
	char name[8];

	void dirRead(fstream& wadFile,int dirPos)
	{
		wadFile.seekg(dirPos);
		wadFile.read(reinterpret_cast<char*>(&lumpPos), 4);
		wadFile.seekg(wadFile.tellg() + streampos(4));
		wadFile.read(reinterpret_cast<char*>(&size), 4);
		wadFile.seekg(wadFile.tellg() + streampos(4));
		wadFile.read(name, 8);

	}
};

class Wad
{
public:
	Wad(string wn) : wadName(wn)
	{
		wadFile.open(wn, ios::in | ios::out | ios::binary);

		if (!wadFile.is_open())
		{
			throw runtime_error("file open failure");
		}
		headerInfo.headerRead(wadFile);
		cout << headerInfo.wadType << endl
			<< headerInfo.lumpc << endl
			<< hex << showbase << headerInfo.dir;

		dirs = new WadDir[headerInfo.lumpc];

		for (int i = 1; i <= headerInfo.lumpc; i++)
		{
			dirs[i - 1].dirRead(wadFile, headerInfo.dir + (16 * i));
			cout << dirs[i - 1].name << endl << dirs[i - 1].size << endl;
		}
	}
	~Wad()
	{
		wadFile.close();
		delete[] dirs;
	}


private:
	fstream wadFile;
	string wadName;
	WadHeader headerInfo;
	WadDir* dirs;
};

int main(int argc, char* argv[])
{
	try
	{
		if (argc == 2)
		{
		Wad wad(argv[1]);
		}
		else
		{
			throw runtime_error("Invalid number of arguments");
		}
	}
	catch (const std::exception& e)
	{
		cout << "Error: " << e.what();
		return 1;
	}
	return 0;
}