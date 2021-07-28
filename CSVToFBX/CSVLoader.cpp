#include <istream>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cassert>
//#include<string>
//#include <cstdlib>
#include<iterator>
#include "CSVLoader.h"
using namespace std;

extern vector<char*> inputArgv;
extern int inputArgc;

void CSVLoader::AssignedOutputObj(const vector<vector<double> >& outputDataObj, CSVGeometry& objFile, vector<string>& inputPrefix)
{
	for (int j = 0; j < outputDataObj.at(0).size(); ++j)
	{
		objFile.vertices.emplace_back();
		objFile.color.emplace_back();
		objFile.texCoords.emplace_back();
		objFile.texCoords1.emplace_back();
		objFile.texCoords2.emplace_back();
		objFile.normals.emplace_back();
		objFile.tangents.emplace_back();

		FbxVector4& vertex = objFile.vertices.back();
		FbxVector4& color = objFile.color.back();
		FbxVector4& texCoord = objFile.texCoords.back();
		FbxVector4& texCoord1 = objFile.texCoords1.back();
		FbxVector4& texCoord2 = objFile.texCoords2.back();
		FbxVector4& normal = objFile.normals.back();
		FbxVector4& tangent = objFile.tangents.back();

		for (int i = 0; i < inputPrefix.size(); ++i)
		{ 
			if (inputPrefix.at(i) == "Pos.x")
				vertex[0] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Pos.y")
				vertex[1] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Pos.z")
				vertex[2] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Pos.w")
				vertex[3] = outputDataObj.at(i).at(j);

			else if (inputPrefix.at(i) == "Tex.x")
				texCoord[0] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Tex.y")
				texCoord[1] = outputDataObj.at(i).at(j);


			else if (inputPrefix.at(i) == "Te1.x")
				texCoord1[0] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Te1.y")
				texCoord1[1] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Te1.z")
				texCoord1[2] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Te1.w")
				texCoord1[3] = outputDataObj.at(i).at(j);

			else if (inputPrefix.at(i) == "Te2.x")
				texCoord2[0] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Te2.y")
				texCoord2[1] = outputDataObj.at(i).at(j);

			else if (inputPrefix.at(i) == "Nor.x")
				normal[0] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Nor.y")
				normal[1] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Nor.z")
				normal[2] = outputDataObj.at(i).at(j);
			// 顶点色
			else if (inputPrefix.at(i) == "Col.x")
				color[0] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Col.y")
				color[1] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Col.z")
				color[2] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Col.w")
				color[3] = outputDataObj.at(i).at(j);

			else if (inputPrefix.at(i) == "Tan.x")
				tangent[0] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Tan.y")
				tangent[1] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Tan.z")
				tangent[2] = outputDataObj.at(i).at(j);
			else if (inputPrefix.at(i) == "Tan.w")
				tangent[3] = outputDataObj.at(i).at(j);
		}
	}
}

//在prefix中找到input匹配的字符串，找到了就记录在prefix中出现的位置，并置tag为1
void CSVLoader::MatchSemanticsPrefix(vector<string>& input, vector<string>& prefix, vector<int>& tag,vector<vector<double> >& output)
{
	assert(!input.empty());
	assert(!prefix.empty());
	for (int i = 0; i < input.size(); ++i)
	{
		 vector<string>::iterator it = find(prefix.begin(), prefix.end(), input.at(i));
		 int pos = distance(begin(prefix), it);
		 tag.at(pos) = 1;
		 vector<double> tmp;
		 output.push_back(tmp);
	}
}

//把一长串buffer字符按分隔符拆分到values中
void CSVLoader::CleanAndSplitTitle(char buffer[], std::vector<std::string>& values)
{
	char* end = std::remove(buffer, buffer + strlen(buffer), ',');
	*end = 0;
	std::istringstream iss(buffer);
	values = std::vector<std::string>(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
}

void CSVLoader::CleanAndSplitData(char buffer[], std::vector<std::string>& values)
{
	std::replace(buffer, buffer + strlen(buffer), ',', ' ');
	std::istringstream iss(buffer);
	values = std::vector<std::string>(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
}


//把用户输入，拆分为语义部分sem和对应值部分，分别保存在数组中，例：pos.x(值)，POSITION.x(语义)
void const CSVLoader::GetSemanticName(const vector<char*>& src, vector<string> &sem, vector<string> &var)
{
	assert(src.empty() != true);
	for (int i = 0; i < src.size(); ++i)
	{
		string str1 = src.at(i);
		//zmk
		sem.emplace_back(str1.substr(6, strlen(str1.c_str()) - 6));
		var.emplace_back(str1.substr(0, 5));
	}
	//str1.substr(6, strlen(str1.c_str()) - 6)
}

CSVLoader::CSVLoader()
{
}

CSVGeometry& CSVLoader::Load(const char* strCSVPath)
{
	vector<int> semanticsTag;									//glsl中顶点坐标可能是四维的，这里给出最大数组的标记位，4*4*4*4
	// TODO 这里不好，改之
	semanticsTag.assign(28, -1);									//需要输出的最大语义数量的标签，全部初始化为-1，需要输出就置为1,最大3*pos、3*normal、4*tangent、2*uv、4*color
	vector<string> semanticsPrefix = {							//最多字符串的前缀
									   "Pos.x","Pos.y","Pos.z","Pos.w",
									   "Col.x","Col.y","Col.z","Col.w",
									   "Tex.x","Tex.y","Tex.z","Tex.w",
									   "Te1.x","Te1.y","Te1.z","Te1.w",
									   "Te2.x","Te2.y","Te2.z","Te2.w",
									   "Nor.x","Nor.y","Nor.z","Nor.w",
									   "Tan.x","Tan.y","Tan.z","Tan.w"

	};

	vector<string> inputSemantics;								//用户输入的语义 例:POSITION.x
	vector<string> inputPrefix;									//用户输入的前缀变量 例:Pos.x
	GetSemanticName(inputArgv, inputSemantics, inputPrefix);    //把用户输入的语义放在inputSemantics中,前缀放在inputPrefix中
	//在semanticsPrefix中找到prefix匹配的字符串，找到了就记录在semantics中出现的位置，并置semanticsTag为1
	vector<vector<double> > outputData;							//用outputData来保存语义数值部分
	MatchSemanticsPrefix(inputPrefix, semanticsPrefix, semanticsTag, outputData);

	//读取csv文件第一行，把语义存放在
	char buffer[2048] = {};
	ifstream fIn(strCSVPath);
	fIn.getline(buffer, sizeof(buffer));
	vector<string> CsvTittleName;
	CleanAndSplitTitle(buffer, CsvTittleName);//分离buffer中的内容，把语义名称对应在CsvTittleName中

	//写一个循环，在semanticsTag中值为1的就是需要读取的语义所在的列的值，
	//需要定义几个vector?我需要在csv中输出的索引
	while (!fIn.eof())
	{
		fIn.getline(buffer, sizeof(buffer));
		if (strlen(buffer) == 0)
			break;
		std::vector<std::string> values;
		CleanAndSplitData(buffer, values);      //分离buffer

		//使用三层循环保证用户输入的语义顺序是无序的
		for (int i = 0; i < semanticsTag.size(); ++i)
		{
			vector<vector<string> > semLists;
			if (semanticsTag.at(i) == 1)
			{
				string currentPrefix = semanticsPrefix.at(i);
				for (int j = 0; j < inputPrefix.size(); ++j)
				{
					if (currentPrefix == inputPrefix.at(j))
					{
						string currentSemantic = inputSemantics.at(j);
						for (int k = 0; k < CsvTittleName.size(); ++k)
						{
							if (CsvTittleName.at(k) == currentSemantic)
							{
								outputData.at(j).push_back(stod(values.at(k)));
							}
						}
					}
				}
			}
		}
	}
	fIn.close();

	//接下来赋值，把读取到的数值outputData赋值给output,根据semanticsTag中值为1的项赋值
	AssignedOutputObj(outputData, _cOutput, inputPrefix);

	return _cOutput;
}