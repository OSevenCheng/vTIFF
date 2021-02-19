#pragma once
#include<string>
#include<vector>
#include"vIFD.h"


class vTIFF
{
	bool ByteOrder;
	byte* p_data;//原始数据
	std::vector<vIFD*> p_IFDs;

public:
	vTIFF()
	{
		ByteOrder = false;
	}
	vTIFF(std::string path)
	{
		Load(path);
	}
	~vTIFF()//清空内存中存放的tiff图像，将图像读取到显存或拷贝后才可释放
	{
		for (int i = 0; i < p_IFDs.size(); i++)
		{
			if (p_IFDs[i] != nullptr)
				delete p_IFDs[i];
			p_IFDs.pop_back();
		}
		Unload();
	}
	void Load(std::string path);
	void Unload();
	byte* GetLayer(int i);
	float* GetPixel(int x, int y, int layer=0);
	int Width() {
		return p_IFDs.empty()?0:p_IFDs[0]->GetImageWidth();
	}
	int Height() { 
		return p_IFDs.empty() ? 0 : p_IFDs[0]->GetImageLength();
	}
	int Layer() {
		return p_IFDs.size();
	}
	int Pass() { 
		return p_IFDs.empty() ? 0 : p_IFDs[0]->GetPass();
	}
	vFormat Format()
	{
		return p_IFDs.empty() ? vFormat::VT_UNDEFINED : p_IFDs[0]->GetFormat();
	}
private:
	
	bool ReadFile(std::string path);
	int DecodeIFH();
	
	//friend vIFD;
	/*int GetIntII(char* pd, int startPos, int Length);
	int GetIntMM(char* pd, int startPos, int Length);*/
	int (*pGetInt[2])(byte* pd, int startPos, int Length);
};

