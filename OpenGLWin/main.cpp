#include<windows.h>
#include"glew.h"
#include<GL/GL.h>
#include<GL/GLU.h>
//#include<stdio.h>
//#include<string.h>//C
//#include<string>//C++
#include<iostream>
#include"vTIFF_GL.h"
// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

HDC			hDC = NULL;		// Private GDI Device Context
HGLRC		hRC = NULL;		// Permanent Rendering Context
HWND		hWnd = NULL;		// Holds Our Window Handle
HINSTANCE   hIns = NULL;
bool	keys[256];			// Array Used For The Keyboard Routine
bool	fullscreen = FALSE;	// Fullscreen Flag Set To Fullscreen Mode By Default

bool firstFrame = true;
bool lbudown = false, lbuup = true;
UINT g_nXCoor1 = 0, g_nXCoor2 = 0, g_nXCoor3 = 0; //The position of X 
UINT g_nYCoor1 = 0, g_nYCoor2 = 0, g_nYCoor3 = 0; //The position of Y


// camera
//Scamera mCameraNode(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

vTIFF_GL* img;
int ilayer = 0;
unsigned int VBO, VAO, EBO;
unsigned int texture1;
unsigned int* textures;
unsigned int shaderProgram;
const char* vertexShaderSource = 
"#version 430 core\n"
"layout(location =0) in vec3 aPos;\n"
"layout(location =1) in vec2 aTexCoord;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos,1.0);\n"
"	TexCoord = aTexCoord;"
"}\0";
const char* fragShaderSource = 
"#version 430 core\n"
"out vec4 fColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D ourTexture;\n"
"void main()\n"
"{\n"
//"   fColor = vec4(TexCoord,0.0,1.0);\n"
"   fColor = texture(ourTexture,TexCoord);\n"
"}\0";

void LoadTIFFLayer(int i)
{
	if (textures[i] == 0)
	{
		glGenTextures(1, &textures[i]);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		int w = img->Wid();
		int h = img->Hig();
		unsigned char* data = (unsigned char*)img->GetData(i);
		glTexImage2D(GL_TEXTURE_2D, 0, img->InFormat(), w, h, 0, img->ExFormat(), img->ExType(), data);
		delete data;
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, textures[i]);
	}
}
void InitScene()
{
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "vertexShader error : " << infoLog << std::endl;
	}

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragShaderSource, NULL);
	glCompileShader(fragmentShader);
	
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "vertexShader error : " << infoLog << std::endl;
	}
	
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "link error : " << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	float vertices[] = {
		0.5f,  0.5f, 0.0f,   1.0f,1.0f,//右上
		0.5f, -0.5f, 0.0f,   1.0f,0.0f,//右下
		-0.5f,-0.5f, 0.0f,   0.0f,0.0f,//左下
		-0.5f ,0.5f, 0.0f,   0.0f,1.0f //左上
	};
	unsigned int indices[] = {
		0,1,3,
		1,2,3
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);/**/

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	

	img = new vTIFF_GL();
	//img->LoadFile("D:\\Projects\\Unity\\Streamlines\\Assets\\Streamlines\\Data\\1.tif");
	img->LoadFile("D:\\Projects\\vTIFF\\hycom.tif");
	int w = img->Wid();
	int h = img->Hig();
	textures = new unsigned int[img->Lay()]();
	LoadTIFFLayer(ilayer);
	
}

GLvoid MoveCamera();

bool leftFlip = true;
bool rightFlip = true;
void Flip()
{
	if (keys[VK_LEFT] && leftFlip)
	{
		ilayer--;
		if (ilayer < 0)
			ilayer = 0;
		else
			LoadTIFFLayer(ilayer);
		leftFlip = false;
	}
	if (!keys[VK_LEFT])
	{
		leftFlip = true;
	}
	if (keys[VK_RIGHT] && rightFlip)
	{
		ilayer++;
		if (ilayer > img->Lay())
			ilayer = img->Lay();
		else
			LoadTIFFLayer(ilayer);
		rightFlip = false;
	}
	if (!keys[VK_RIGHT])
	{
		rightFlip = true;
	}
}
void DrawScene()
{
	Flip();
	MoveCamera();

	glClearColor(2.0 / 256.0, 6.0 / 256.0, 18.0 / 256.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	

	glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture1); 
	
	glUseProgram(shaderProgram);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);
}
void FiniScene()
{
	
}
GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height == 0)										// Prevent A Divide By Zero By
	{
		height = 1;										// Making Height Equal One
	}
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);						// Reset The Current Viewport	
}

GLvoid MoveCamera()
{
}

bool OnSpecialKeyDown(int key, int x, int y)  //按键按下
{
	switch (key)
	{
	case VK_LEFT:
		keys[VK_LEFT] = true;
		break;
	case VK_RIGHT:
		keys[VK_RIGHT] = true;
		break;
	case VK_UP:
		keys[VK_UP] = true;
		break;
	case VK_DOWN:
		keys[VK_DOWN] = true;
		break;
	case VK_SPACE:
		keys[VK_SPACE] = true;
		break;
	case VK_PRIOR:break;
	case VK_NEXT:break;
	case VK_HOME:break;
	case VK_END:break;
	case VK_INSERT:break;
	case VK_DELETE:break;
	case 'A':
		keys['A'] = true;
		break;
	case 'D':
		keys['D'] = true;
		break;
	case 'W':
		keys['W'] = true;
		break;
	case 'S':
		keys['S'] = true;
		break;
	case 'C':
		keys['C'] = true;
		break;
	case 'X':
		keys['X'] = true;
		break;
	default:
		return false;
	}
	return true;
}

bool OnSpecialKeyUp(int key, int x, int y)//按键松开
{
	switch (key)
	{
	case VK_LEFT:
		keys[VK_LEFT] = false;
		break;
	case VK_RIGHT:
		keys[VK_RIGHT] = false;
		break;
	case VK_UP:
		keys[VK_UP] = false;
		break;
	case VK_DOWN:
		keys[VK_DOWN] = false;
		break;
	case VK_PRIOR:break;
	case VK_SPACE:
		keys[VK_SPACE] = false;
		break;
	case VK_NEXT:break;
	case VK_HOME:break;
	case VK_END:break;
	case VK_INSERT:break;
	case VK_DELETE:break;
	case 'A':
		keys['A'] = false;
		break;
	case 'D':
		keys['D'] = false;
		break;
	case 'W':
		keys['W'] = false;
		break;
	case 'S':
		keys['S'] = false;
		break;
	case 'C':
		keys['C'] = false;
		break;
	case 'X':
		keys['X'] = false;
		break;
	default:
		return false;
	}
	return true;
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL, 0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}
	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL, L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		hRC = NULL;										// Set RC To NULL
	}
	if (hDC && !ReleaseDC(hWnd, hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL, L"Release Device Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hDC = NULL;										// Set DC To NULL
	}
	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL, L"Could Not Release hWnd.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		hWnd = NULL;										// Set hWnd To NULL
	}
}
LRESULT CALLBACK GLWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:							// Is A Key Being Held Down?
	{
		keys[wParam] = TRUE;					// If So, Mark It As TRUE

		int virtKey = (int)wParam;

		// Get cursor position client coordinates.
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hwnd, &point);
		int xPos = (int)point.x;
		int yPos = (int)point.y;

		OnSpecialKeyDown(virtKey, xPos, yPos);

		return 0;								// Jump Back
	}

	case WM_KEYUP:								// Has A Key Been Released?
	{
		keys[wParam] = FALSE;					// If So, Mark It As FALSE

		int virtKey = (int)wParam;

		// get the cursor position in client coordinates
		POINT point;
		GetCursorPos(&point);
		ScreenToClient(hwnd, &point);
		int xPos = (int)point.x;
		int yPos = (int)point.y;

		OnSpecialKeyUp(virtKey, xPos, yPos);

		return 0;								// Jump Back
	}

	case WM_SIZE:								// Resize The OpenGL Window
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
		return 0;								// Jump Back
	}
	case WM_LBUTTONDOWN:  //0x00A1#define WM_LBUTTONDOWN
	{
		lbudown = true;
		g_nXCoor1 = LOWORD(lParam);
		g_nYCoor1 = HIWORD(lParam);
		lbuup = false;
		break;
	}
	case WM_LBUTTONUP:  //0x0201#define WM_LBUTTONUP
	{
		lbuup = true;
		g_nXCoor2 = LOWORD(lParam);
		g_nYCoor2 = HIWORD(lParam);
		break;
	}
	case WM_MOUSEMOVE:  //WM_MOUSEMOVE
	{
		if (lbudown && (!lbuup))
		{
			g_nXCoor2 = LOWORD(lParam);
			g_nYCoor2 = HIWORD(lParam);
			float dx = (float(g_nXCoor2) - float(g_nXCoor1))*0.25*3.1415 / 600 * 5;  //计算X方向旋转的角度
			float dy = (float(g_nYCoor2) - float(g_nYCoor1))*0.25*3.1415 / 600 * 5;  //计算X方向旋转的角度

			lbuup = false;
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		int deltaZ = HIWORD(wParam);
	}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR IpCmdLine, int nShowCmd)
{
	WNDCLASSEX wndclass;
	wndclass.cbClsExtra = 0;
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = NULL;
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = NULL;
	wndclass.hIconSm = NULL;
	wndclass.hInstance = hInstance;
	wndclass.lpfnWndProc = GLWindowProc;
	wndclass.lpszClassName = L"GLWindow";//name
	wndclass.lpszMenuName = NULL;
	wndclass.style = CS_VREDRAW | CS_HREDRAW;
	ATOM atom = RegisterClassEx(&wndclass);
	if (!atom)
	{
		MessageBox(NULL, L"Notice", L"Error", MB_OK);
		return 0;
	}
	//Set the size of the window (without frame)
	RECT rect;
	rect.left = 0;
	rect.right = SCR_WIDTH;
	rect.top = 0;
	rect.bottom = SCR_HEIGHT;
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, NULL);
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	//Create Window
	hWnd = CreateWindowEx(NULL, L"GLWindow", L"OpenGL Window", WS_OVERLAPPEDWINDOW,
		100, 100, windowWidth, windowHeight,
		NULL, NULL, hInstance, NULL);
	hIns = hInstance;
	//Pixel format
	hDC = GetDC(hWnd);//device
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nVersion = 1;
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	int pixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormat, &pfd);
	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);

	glewInit();
	int main_version, sub_version, release_version;
	const char* version = (const char*)glGetString(GL_VERSION);
	/*sscanf(version, "%d.%d.%d", &main_version, &sub_version, &release_version);
	if (main_version < 2)
	{
		MessageBox(NULL, L"verson of opengl is not supported!", MB_OK, 0);
		return 0;
	}*/

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	std::string str = version;
	InitScene();
	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				KillGLWindow();
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);//调用GLWindowProc
		}
		DrawScene();
		//mScene.DrawScene();
		SwapBuffers(hDC);
	}
	FiniScene();
	return 0;
}