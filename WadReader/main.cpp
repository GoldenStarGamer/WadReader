#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdexcept>
#include <string>

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
			throw runtime_error("Invalid Wad");
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
	int size;
	char name[8];

	void dirRead(fstream& wadFile, int dirPos)
	{
		wadFile.seekg(dirPos);
		wadFile.read(reinterpret_cast<char*>(&lumpPos), 4);
		wadFile.read(reinterpret_cast<char*>(&size), 4);
		wadFile.read(name, 8);
	}

	void printName() const
	{
		for (int i = 0; i < 8; i++)
		{
			if (name[i] != '\0')
				cout << name[i];
			else
				break;
		}
		cout << endl;
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
			throw runtime_error("Unable to Open File");
		}
		headerInfo.headerRead(wadFile);
		cout << endl << "Type: ";
		switch (headerInfo.wadType)
		{
		case 0:
			cout << "IWAD";
			break;
		case 1:
			cout << "PWAD";
			break;
		default:
			cout << headerInfo.wadType;
			break;
		}
		cout << endl
			<< "Number of Lumps: " << headerInfo.lumpc << endl
			<< "Directory Location: " << hex << showbase << headerInfo.dir << endl << endl;

		dirs = new WadDir[headerInfo.lumpc];

		for (int i = 1; i < headerInfo.lumpc; i++)
		{
			dirs[i - 1].dirRead(wadFile, headerInfo.dir + (16 * i));
			cout << "Position: " << dirs[i - 1].lumpPos << endl << "Name: ";
			dirs[i - 1].printName();
			cout << "Size:" << dec << dirs[i - 1].size << endl << endl;
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
		string wadPath;

		if (argc == 2)
		{
			wadPath = argv[1];
		}
		else
		{
			cout << "Wad Location: ";
			getline(cin, wadPath);
			wadPath.erase(remove(wadPath.begin(), wadPath.end(), '"'), wadPath.end());
		}

		Wad wad(wadPath);
	}
	catch (const std::exception& e)
	{
		cout << "Error: " << e.what();
		return 1;
	}
	return 0;
}