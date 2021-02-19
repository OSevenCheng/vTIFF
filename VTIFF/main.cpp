#include<iostream>
#include"vTIFF.h"
#include "vTIFF.h"
using namespace std;
int main()
{
	//读取vTIFF文件
	vTIFF* tiff = new vTIFF("D:\\Projects\\Unity\\Streamlines\\Assets\\Streamlines\\Data\\1.tif");
	
	//获取第0页的数据。返回二维字节数组
	byte** layer0 = tiff->GetLayer(0);
	cout<< tiff->Height()<<endl;
	cout << tiff->Width() << endl;
	cout << tiff->Layer() << endl;
	cout << tiff->Pass() << endl;
	//获取某一层的某个像素的数据。
	//for (int x = 0; x < 1440; x++)
	//{
		float* color = tiff->GetPixel(720, 360);
		cout << color[0] << "	" << color[1] << "	" << color[2] << "	" << color[3] << endl;
	//}
	
	delete tiff;
	return 0;
}