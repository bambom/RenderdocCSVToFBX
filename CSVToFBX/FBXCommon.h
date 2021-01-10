#ifndef _FBXCOMMON_H
#define _FBXCOMMON_H
#include"fbxsdk.h"

void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
void DestroySdkObjects(FbxManager* pManager, bool pExitStatus);
bool SaveScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename, int pFileFormat = -1, bool pEmbedMedia = false);
bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);

#endif 
