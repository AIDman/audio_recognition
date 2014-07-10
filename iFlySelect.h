#ifndef _IFLY_RES_SELECT_H_
#define _IFLY_RES_SELECT_H_

#include <string>

#ifdef WIN32
#define __EXPORT __declspec(dllexport)
#else
#define __EXPORT
#endif /* WIN32 */

//+---------------------------------------------------------------------------+
//+ ��ʼ��
//+ ����Ҫ�����ݽṹ���г�ʼ������
//+---------------------------------------------------------------------------+
//+ szResPath			- ��Դ����·��
//+ return				- ������
//+---------------------------------------------------------------------------+
extern "C" __EXPORT int Initialize(const char *szResPath);
typedef int(*Proc_Initialize)(const char *szResPath);

//+---------------------------------------------------------------------------+
//+ ��Դ����
//+ ���ر���õ���Դ�������������Ƶ���м������������Ӧ�ĸ�����
//+---------------------------------------------------------------------------+
//+ szAudioPath		- ��������Ƶ·��
//+ szResultString		- ����������ʽ��{[����ˮ]}
//+ return				- ������
//+---------------------------------------------------------------------------+
extern "C" __EXPORT int ProcessSelelct(const char *szAudioPath, std::string &szResultString);
typedef int(*Proc_ProcessSelelct)(const char *szAudioPath, std::string &szResultString);

//+---------------------------------------------------------------------------+
//+ ���ʼ��
//+ ����Ҫ�����ݽṹ��������
//+---------------------------------------------------------------------------+
//+ return				- ������
//+---------------------------------------------------------------------------+
extern "C" __EXPORT int unInitialize();
typedef int(*Proc_unInitialize)();

#endif//_IFLY_RES_SELECT_H_
