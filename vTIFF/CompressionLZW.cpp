#include "CompressionLZW.h"
using namespace std;
CompressionLZW::CompressionLZW() : current(0), bitsCount(0)
{
	dict = new vector<byte> [4096]();
	used = new bool[4096]();
	dicIndex = 0;
	while (dicIndex < 256)
	{
		byte x = (byte)dicIndex;
		dict[dicIndex].push_back(x);
		used[dicIndex++] = true;
	}
	dicIndex = 258;
}

CompressionLZW::~CompressionLZW()
{
	if (dict != nullptr)
		delete[] dict;
}
void CompressionLZW::Decode(byte* _input, int _startPos, int _readLength, byte* _output)
{
	input = _input;
	output = _output;
	startPos = _startPos;
	bitsCount = _readLength * 8;
	

	//dict = new byte*[4096];
	ResetPara();
	int Code;
	int OldCode = 256;
	while ((Code = GetNextCode()) != EoiCode)
	{
		if (Code == ClearCode)
		{
			InitializeTable();
			Code = GetNextCode();
			if (Code == EoiCode)
			{
				break;
			}
			WriteResult(dict[Code]);
		}
		else
		{
			if (IsInDic(Code))
			{
				WriteResult(dict[Code]);
				//vector<byte> newCode = dict[OldCode];
				//newCode.push_back(dict[Code][0]);
				//字典扩充
				//dict[dicIndex++].assign(newCode.begin(),newCode.end());
				dict[dicIndex] = dict[OldCode];
				dict[dicIndex].push_back(dict[Code][0]);
				used[dicIndex] = true;
				dicIndex++;
				OldCode = Code;
			}
			else
			{
				/*vector<byte> newCode = dict[OldCode];
				newCode.push_back(dict[OldCode][0]);
				WriteResult(newCode);
				dict[dicIndex++] = newCode;
				OldCode = Code;*/
				dict[dicIndex] = dict[OldCode];
				dict[dicIndex].push_back(dict[OldCode][0]);
				used[dicIndex] = true;
				WriteResult(dict[dicIndex]);
				dicIndex++;
				OldCode = Code;
			}
		}
		OldCode = Code;
	}
}

int CompressionLZW::GetStep()
{
	int res = 12;
	res += ((dicIndex - 2047) >> 31);
	res += ((dicIndex - 1023) >> 31);
	res += ((dicIndex - 511) >> 31);
	return res;
}
int CompressionLZW::GetNextCode()
{
	int tmp = 0;
	int step = GetStep();
	if (current + step > bitsCount)
		return EoiCode;
	for (int i = 0; i < step; i++)
	{
		int x = current + i;
		int bit = GetBit(x) << (step - 1 - i);
		tmp += bit;
	}
	current += step;
	//一开始读9个bit
	//读到510的时候，下一个开始读10bit
	//读到1022的时候，下一个开始读11bit
	//读到2046的时候，下一个开始读11bit
	return tmp;
}
int CompressionLZW::GetBit(int x)
{
	int byteIndex = x / 8; //该bit在第几个byte里
	int bitIndex = 7 - x + byteIndex * 8;//该bit是这个byte的第几位
	byte b = input[startPos + byteIndex];//找到所在byte
	return (b >> bitIndex) & 1;//找到所在bit
}
void CompressionLZW::InitializeTable()
{
	//if (Dic != nullptr)//是删除快还是清空快？
	//	delete[] Dic;
	//Dic = new std::string[4096];
	/*if (dict != nullptr)
		delete[] dict;
	dict = new std::string[4096];*/

	/*
	dicIndex = 258;
	while (dicIndex < 4096)
	{
		dict[dicIndex++].clear();
	}
	//memcpy(&dict[258], nullString , 3838);
	dicIndex = 258;*/

	
	dicIndex = 258;
	while (dicIndex < 4096)
	{
		used[dicIndex++]=false;
	}
	//memcpy(&dict[258], nullString , 3838);
	dicIndex = 258;
}
void CompressionLZW::WriteResult(vector<byte> _str)
{
	for (int i = 0; i < _str.size(); i++)
		output[resIndex++] = _str[i];
}
bool CompressionLZW::IsInDic(int code)
{
	return used[code];
}