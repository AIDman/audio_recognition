// iFlyBuild.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "iFlyBuild.h"
#include "hash.h"
#include "fingerprint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

THash myhash;
TExtractFeature fingerprint;
CFileFind finder;

int Initialize(){
	int nRetCode = 0;
	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO:  ���Ĵ�������Է���������Ҫ
			_tprintf(_T("����:  MFC ��ʼ��ʧ��\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO:  �ڴ˴�ΪӦ�ó������Ϊ��д���롣
			myhash.BuildInit();
		}
	}
	else
	{
		// TODO:  ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����:  GetModuleHandle ʧ��\n"));
		nRetCode = 1;
	}
	return 0;
}

int ProcessBuildRes(const char *szResDir, const char *szResPath){
	setlocale(LC_CTYPE, "");
	USES_CONVERSION;
	CString szRes = A2T(szResDir);
	BOOL bWorking = finder.FindFile(szRes);
	int index = 0;
	do{
		bWorking = finder.FindNextFile();
		CString FileName = finder.GetFileName();
		if (FileName == L"." || FileName == L"..")continue;

		wprintf(L"The File Name is %s.\n", FileName);
		myhash.AddSongList(T2A(FileName));
		fingerprint.ExtractPair(T2A(finder.GetFilePath()), myhash, 0, (float)0.98, index, 0);
		fingerprint.UnInit();
		index++;		
	}while (bWorking);

	myhash.Hash2File(szResPath);
	printf("%d.\n", index);
	system("pause");
	return 0;
}

int unInitialize(){
	myhash.BuildUnInit();
	return 0;
}