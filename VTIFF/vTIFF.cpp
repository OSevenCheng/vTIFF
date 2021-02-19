#include "vTIFF.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
using namespace std;

void vTIFF::Load(string path)
{
    pGetInt[1] = GetIntII;
    pGetInt[0] = GetIntMM;

    if (!ReadFile(path))
    {
        return;
    }
    int pIFD = DecodeIFH();
    while (pIFD != 0)
    {
		vIFD* p_ifd = new vIFD(p_data,ByteOrder, pGetInt[ByteOrder]);
        pIFD = p_ifd->Decode(pIFD);
        //p_ifd->PrintInfo();
        p_IFDs.push_back(p_ifd);
    }
}
void vTIFF::Unload()//只是把原始数据释放//不再需要解码数据（GetLayer）后即可释放掉了
{
    if (p_data != nullptr)
    {
        delete p_data;
        p_data = nullptr;
    }
        
}
int vTIFF::DecodeIFH()
{
    //string byteOrder = p_data[0];// +"" + (char)p_data[1];
    if (p_data[0] == 'I')
        ByteOrder = true;
    else if (p_data[0] == 'M')
        ByteOrder = false;
    else
        cout<<"The order value is not II or MM."<<endl;
    int Version = pGetInt[ByteOrder](p_data,2,2);

    if (Version != 42)
        cout << "Not vTIFF."<<endl;

    return pGetInt[ByteOrder](p_data, 4, 4);
}

bool vTIFF::ReadFile(std::string path)
{
	ifstream f;   // 读取图像
	f.open(path, ios::in | ios::binary);

	f.seekg(0, ios::end);   // 将文件指针移动到文件末尾
	int length = f.tellg();   // 返回文件指针的位置
    if (length == -1)
    {
        cout << "文件路径错误！" << length << endl;
        return false;
    }
	
	f.seekg(0, ios::beg);
	//根据图像数据长度分配内存buffer
    p_data = new byte[length];
	f.read((char*)p_data, length);

	f.close();
    return true;
}

byte* vTIFF::GetLayer(int i=0)
{
    p_IFDs[i]->DecodeStrips();
    return p_IFDs[i]->GetImageData();
}
float* vTIFF::GetPixel(int x, int y, int layer)
{
    return p_IFDs[layer]->GetPixel(x, y);
}