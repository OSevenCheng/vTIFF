// testTIFF.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<iostream>
#include"vTIFF.h"

using namespace std;
#include<ctime>
clock_t startT, endT;
int main()
{
	//读取vTIFF文件
	Timer time("Total time");

	vTIFF* tiff = new vTIFF();
	tiff->Load("D:\\Projects\\Unity\\Streamlines\\Assets\\Streamlines\\Data\\screenshot.tiff");

	//vTIFF* tiff = new vTIFF("D:\\Projects\\Unity\\Streamlines\\Assets\\Streamlines\\Data\\1.tif");
	
	//获取第0页的数据。返回二维字节数组
	byte* layer0 = tiff->GetLayer(0);

	tiff->Unload();

	time.End();



	cout << tiff->Height() << endl;
	cout << tiff->Width() << endl;
	cout << tiff->Layer() << endl;
	cout << tiff->Pass() << endl;
	cout << tiff->Format() << endl;

	//获取某一层的某个像素的数据。
	//for (int x = 0; x < 1440; x++)
	//{
	float* color = tiff->GetPixel(1, 123);
	cout << color[0]*255 << "	" << color[1] * 255 << "	" << color[2] * 255 << endl;
	//cout << color[0] << "	" << color[1] << "	" << color[2] << "	" << color[3] << endl;
	//}

	delete tiff;
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
