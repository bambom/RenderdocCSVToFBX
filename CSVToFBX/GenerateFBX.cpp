#include <iostream>
#include <fbxsdk.h>
#include <cassert>
#include<string>

#include "FBXCommon.h"
#include "CSVLoader.h"
using namespace std;

//TODO 顶点色
enum Layer
{
	NORMAL,
	TANGENT,
	UV,
	UV1,
	UV2
};


bool CreateScene(FbxScene* pScene, char* pSampleFileName, const CSVGeometry& cSVGeometry);
FbxNode* CreateMesh(FbxScene* pScene, const char* pName,const  CSVGeometry& cSVGeometry);
void SetMeshDefaultPosition(FbxNode* pTriangle);

//创建场景
bool CreateScene(FbxScene* pScene, char* pSampleFileName, const CSVGeometry& cSVGeometry)
{
	FbxNode* lMesh = CreateMesh(pScene, "Mesh", cSVGeometry);

	SetMeshDefaultPosition(lMesh);

	// Build the node tree.
	FbxNode* lRootNode = pScene->GetRootNode();

	lRootNode->AddChild(lMesh);

	return true;
}

//设置场景中mesh的默认坐标
void SetMeshDefaultPosition(FbxNode* pTriangle)
{
	pTriangle->LclTranslation.Set(FbxVector4(0.0, 0.0, 0.0));
	pTriangle->LclRotation.Set(FbxVector4(0.0, 0.0, 0.0));
	pTriangle->LclScaling.Set(FbxVector4(1.0, 1.0, 1.0));
}

//最主要的方法，根据前面读取到的csv文件数据，vertex,normal,tangent,texcoord来生成fbx
FbxNode* CreateMesh(FbxScene* pScene, const char* pName, const CSVGeometry& cSVGeometry)
{
	FbxMesh* lMesh = FbxMesh::Create(pScene, pName);

	assert(cSVGeometry.vertices.size() > 0, "vertices is null");

	// Create control points.
	lMesh->InitControlPoints(cSVGeometry.vertices.size());
	FbxVector4* lControlPoints = lMesh->GetControlPoints();

	for (int i = 0; i < cSVGeometry.vertices.size(); i++)
	{
		lControlPoints[i] = cSVGeometry.vertices[i];
	}

	for (int i = 0; i < cSVGeometry.vertices.size() / 3; i++)
	{
		lMesh->BeginPolygon();

		lMesh->AddPolygon(3 * i + 0);
		lMesh->AddPolygon(3 * i + 1);
		lMesh->AddPolygon(3 * i + 2);

		lMesh->EndPolygon();
	}

	// 法向量
	FbxLayer* lNormalLayer = lMesh->GetLayer((int)Layer::NORMAL);
	if (lNormalLayer == NULL)
	{
		lMesh->CreateLayer();
		lNormalLayer = lMesh->GetLayer((int)Layer::NORMAL);
	}

	FbxLayerElementNormal* lLayerElementNormal = FbxLayerElementNormal::Create(lMesh, "");
	lLayerElementNormal->SetMappingMode(FbxLayerElement::eByControlPoint);

	// Set the reference mode of so that the n'th element of the normal array maps to the n'th
	// element of the control point array.
	lLayerElementNormal->SetReferenceMode(FbxLayerElement::eDirect);

	for (auto iter = cSVGeometry.normals.begin(); iter != cSVGeometry.normals.end(); iter++)
	{
		lLayerElementNormal->GetDirectArray().Add(*iter);
	}

	lNormalLayer->SetNormals(lLayerElementNormal);

	// 添加切线
	FbxLayer* lTangentLayer = lMesh->GetLayer((int)Layer::TANGENT);
	if (lTangentLayer == NULL)
	{
		lMesh->CreateLayer();
		lTangentLayer = lMesh->GetLayer((int)Layer::TANGENT);
	}

	FbxLayerElementTangent* lLayerElementTangent = FbxLayerElementTangent::Create(lMesh, "");
	lLayerElementTangent->SetMappingMode(FbxLayerElement::eByControlPoint);
	lLayerElementTangent->SetReferenceMode(FbxLayerElement::eDirect);

	for (auto iter = cSVGeometry.tangents.begin(); iter != cSVGeometry.tangents.end(); iter++)
	{
		lLayerElementTangent->GetDirectArray().Add(*iter);
	}

	lTangentLayer->SetTangents(lLayerElementTangent);

	// 添加uv
	FbxLayer* lUVLayer = lMesh->GetLayer((int)Layer::UV);
	if (lUVLayer == NULL)
	{
		lMesh->CreateLayer();
		lUVLayer = lMesh->GetLayer((int)Layer::UV);
	}

	FbxLayerElementUV* lLayerElementUV = FbxLayerElementUV::Create(lMesh, "TEXCOORD0");
	lLayerElementUV->SetMappingMode(FbxLayerElement::eByControlPoint);
	lLayerElementUV->SetReferenceMode(FbxLayerElement::eDirect);

	for (auto iter = cSVGeometry.texCoords.begin(); iter != cSVGeometry.texCoords.end(); iter++)
	{
		FbxVector2 data(iter->mData[0], iter->mData[1]);
		lLayerElementUV->GetDirectArray().Add(data);
	}

	lUVLayer->SetUVs(lLayerElementUV);

	// 添加uv1
	FbxLayer* lUV1Layer = lMesh->GetLayer((int)Layer::UV1);
	if (lUV1Layer == NULL)
	{
		lMesh->CreateLayer();
		lUV1Layer = lMesh->GetLayer((int)Layer::UV1);
	}

	FbxLayerElementUV* lLayerElementUV1 = FbxLayerElementUV::Create(lMesh, "TEXCOORD1");
	lLayerElementUV1->SetMappingMode(FbxLayerElement::eByControlPoint);
	lLayerElementUV1->SetReferenceMode(FbxLayerElement::eDirect);

	for (auto iter = cSVGeometry.texCoords1.begin(); iter != cSVGeometry.texCoords1.end(); iter++)
	{
		FbxVector2 data(iter->mData[0], iter->mData[1]);
		lLayerElementUV1->GetDirectArray().Add(data);
	}

	lUV1Layer->SetUVs(lLayerElementUV1);


	// TODO 添加uv2

	// TODO 添加顶点色



	FbxNode* lNode = FbxNode::Create(pScene, pName);
	lNode->SetNodeAttribute(lMesh);

	return lNode;
}

//根据用户输入的字符串参数初始化fbx，也是生成fbx模型的起始方法
int OutputFBX(const char* myOutPut, const CSVGeometry& cSVGeometry)
{
	string lmyOutput = myOutPut;
	size_t myNameSize = strlen(lmyOutput.c_str());

	if (strcmp(lmyOutput.c_str() + myNameSize - 4, ".fbx"))
	{
		//printHelp();
		return 0;
	}
	char* llmyOutput = const_cast<char *>(lmyOutput.c_str());

	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	char* lSampleFileName = llmyOutput;
	bool lResult;

	// Prepare the FBX SDK.
	InitializeSdkObjects(lSdkManager, lScene);

	// Create the scene.
	lResult = CreateScene(lScene, lSampleFileName, cSVGeometry);

	if (lResult == false)
	{
		FBXSDK_printf("\n\nAn error occurred while creating the scene...\n");
		DestroySdkObjects(lSdkManager, lResult);
		return 0;
	}

	// Save the scene.
	lResult = SaveScene(lSdkManager, lScene, lSampleFileName);

	if (lResult == false)
	{
		FBXSDK_printf("\n\nAn error occurred while saving the scene...\n");
		DestroySdkObjects(lSdkManager, lResult);
		return 0;
	}

	// Destroy all objects created by the FBX SDK.
	DestroySdkObjects(lSdkManager, lResult);

	return 0;
}

