#ifndef _HashFunc_h_
#define _HashFunc_h_

#include "stdlib.h"
#include "memory.h"
#include "stdio.h"
#include "assert.h"
#include "string.h"
#include <iostream>

#define ID_BITS 18				//����һ�����Դ�26���׸裨����������ڴ��㹻�󣩡�
#define OFFSET_BITS 14			//Ĭ��ÿ�׸�С��8.7���ӡ�
#define MAX_SONG_NUM (2<<18)
#define MAX_SONG_LEN 256
#define HashKeyNum (1<<20)
#define ValuePerBlock (1<<6)	//ÿһ���ڴ�ռ��С��64*sizeof(size_t)�������ڴ��ϣͰ�еĶ��valueֵ�������ݡ�
#define BlockNum 4				//Blockֵ�����ڶ�̬���ݡ�
#define OverFlowThreshold 1<<28
using namespace std;

//Hash��key���ͣ�����iFluBuild
//(f1, f2_f1, t)
struct HashKeyInfo{
	size_t* start;
	size_t length;
	HashKeyInfo* next;	//ָ����������
};
//Hash��key���ͣ�����iFluSelect
struct HashKeyTable{
	size_t *start;
	size_t length;
};

class THash{
private:
public:
	size_t *pValueStart;
	size_t *pValueEnd;
	short **vote_table;
	size_t data_num;
	char **song_list;
	size_t song_num;
	HashKeyInfo *key_info;
	HashKeyTable *key_table;

	THash::THash();
	THash::~THash();
	void THash::ReBuildInit();
/************************************  Functions for build tracks.(iFlyBuild) ************************************************/
	//�˺������ڶ���iFlyBuild���������wav��Hash_Table.				//Finished.
	void THash::BuildInit();
	void THash::BuildUnInit();
	//�Ӹ��������¸�������											//Finished.
	void THash::AddSongList(const char *filename);
	//��Value���ڴ��������ݣ�����Key_table.						//Finished.
	void THash::InsertHash(size_t f1, size_t f2_f1, size_t t, size_t id, size_t offset);
	//��Hash�����ļ���ˢ������ˢ�����ڴ棬��������iFlySelect���˷��ڴ�ռ䣩//Finished
	void THash::Hash2File(const char* filename);

/************************************  Functions for select tracks.(iFlySelect) ************************************************/
	size_t* THash::GetHash(size_t f1, size_t f2_f1, size_t t);
	void THash::File2Hash(const char* filename);
	//Functions for vote and save the top voted id to QueryId.
	void THash::VoteInit();
	void THash::Vote(size_t f1, size_t f2_f1, size_t t, size_t offset);
	size_t THash::VoteResult(size_t &offset);
};
#endif // _HashFunc_h_
