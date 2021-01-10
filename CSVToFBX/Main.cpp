#include "CSVLoader.h"
#include "FBXCommon.h"
#include<iostream>
#include<string>
#include<assert.h>
#include<vector>
//#include"fbxsdk.h"
using namespace std;

//判断输入的参数个数是否合法
bool ArgcNumIsLegal(int arg)
{
	bool tag = true;
	//目前版本最多支持15个参数，最少4个参数
	//exe、input、output、3*pos、3*normal、4*tangent、2*uv
	if (arg < 4 || arg >17)
		tag = false;
	return tag;
}

//判断输入和输出的参数是否合法
bool InputAndOutputNameIsLegal(char* inputName, char* outputName)
{
	int tag = true;
	string str1 = inputName;
	string str2 = outputName;
	string str3 = str1.substr(6, strlen(str1.c_str()) - 6); //根据用户输入的字符串得到csv文件名
	string str4 = str2.substr(7, strlen(str2.c_str()) - 7); //根据用户输入的字符串得到fbx文件名
	if (str1.substr(0, 5) != "Input" || str1.substr(5, 1) != "=")
	{
		cout << "输入前缀必须为‘Input='" << endl;
		tag = false;
		return tag;
	}
	else if (strcmp(str3.c_str() + strlen(str3.c_str()) - 4, ".csv"))
	{
		cout << "请使用csv文件作为输入" << endl;
		tag = false;
		return tag;
	}
	if (str2.substr(0, 6) != "Output" || str2.substr(6, 1) != "=")
	{
		cout << "输入前缀必须为‘Output='" << endl;
		tag = false;
		return tag;
	}
	else if (strcmp(str4.c_str() + strlen(str4.c_str()) - 4, ".fbx"))
	{
		cout << "请使用fbx文件作为输出" << endl;
		tag = false;
		return tag;
	}
	return tag;
}

const string GetCsvFileName(char* &src)
{
	string csvName;
	string str1 = src;
	csvName = str1.substr(6, strlen(str1.c_str()) - 6);
	return csvName;
}
const string GetFbxFileName(char* &dest)
{
	string fbxName;
	string str2 = dest;
	fbxName = str2.substr(7, strlen(str2.c_str()) - 7);
	return fbxName;
}

int intputArgc;
//char* inputArgv[30];
vector<char*> inputArgv;
extern int OutputFBX(const char* myOutPut, const CSVGeometry& objFile);
int main(int argc, char* argv[])
{
	if (!ArgcNumIsLegal(argc))
	{
		cout << "输入的参数个数不合法:含exe在内最少4个，最多17个" << endl;
		exit(1);
	}
	if (!InputAndOutputNameIsLegal(argv[1], argv[2]))
	{
		cout << "输入和输出的参数名不合法" << endl;
		exit(1);
	}
	for (int i = 3; i < argc; ++i)
	{
		inputArgv.push_back(argv[i]);
	}
	intputArgc = argc - 3;

	//输入和输出参数无误，接下来就是读取csv文件了
	string csvFileName = GetCsvFileName(argv[1]);
	//根据用户的输入读取csv文件
	CSVLoader cCSVLoader;
	//一定要使用该方法将csv中数据获取到，否则fbx永远都是空的
	CSVGeometry& cSVGeometry = cCSVLoader.Load(csvFileName.c_str());

	string fbxFileName = GetFbxFileName(argv[2]);
	OutputFBX(fbxFileName.c_str(), cSVGeometry);

	return 0;
}