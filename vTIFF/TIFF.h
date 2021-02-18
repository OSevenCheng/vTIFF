#pragma once
#include<string>
#include<vector>
#include"IFD.h"


class TIFF
{
	bool ByteOrder;
	byte* p_data;
	std::vector<IFD*> p_IFDs;

public:
	TIFF(std::string path)
	{
		Init(path);
	}
	~TIFF()
	{
		if(p_data !=nullptr)
		   delete[] p_data;
		for (int i = 0; i < p_IFDs.size(); i++)
		{
			delete p_IFDs[i];
			p_IFDs.pop_back();
		}
	}
	void Init(std::string path);
	void ReadFile(std::string path);
	int DecodeIFH();
	byte** GetLayer(int i);
	float* GetPixel(int x, int y, int layer=0);
	//friend IFD;
	/*int GetIntII(char* pd, int startPos, int Length);
	int GetIntMM(char* pd, int startPos, int Length);*/
	int (*pGetInt[2])(byte* pd, int startPos, int Length);
};

