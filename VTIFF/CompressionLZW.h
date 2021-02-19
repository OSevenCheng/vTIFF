#pragma once
#include <vector>
#include "Utils.h"
#define EoiCode 257
#define ClearCode 256
class CompressionLZW
{
public:
	CompressionLZW();
	~CompressionLZW();
	void Decode(byte* input, int _startPos, int _readLength, byte* output);

private:
	byte* input;
	byte* output;
	//std::string* Dic;
	std::vector<byte>* dict;
	bool* used;
	int dicIndex;
	int current;
	int bitsCount;
	int startPos;
	int resIndex;
	void ResetPara()
	{
		dicIndex = 0;
		current = 0;
		resIndex = 0;
	}
	int GetNextCode();
	int GetStep();
	int GetBit(int x);
	
	void InitializeTable();
	void WriteResult(std::vector<byte> _str);
	bool IsInDic(int code);
};
//struct ByteNode
//{
//	ByteNode(byte _d) :data(_d),pNext(nullptr) {}
//	byte data;
//	ByteNode* pNext;
//};
//class ByteString
//{
//public:
//	ByteString(){}
//	ByteNode* head;//head的next才是第一个
//	ByteNode* tail;
//	void Add(ByteNode* _pNode)
//	{
//		tail->pNext = _pNode;
//		tail = _pNode;
//	}
//};
