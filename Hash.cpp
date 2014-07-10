#include "hash.h"
#include "stdafx.h"

//�ⲿ����
inline size_t HashTableOffset(size_t f1, size_t f2_f1, size_t t){
	return (f1 * (1 << 12) + f2_f1 * (1 << 6) + t);
}
/*
size_t* ValueTableAddr(size_t* start, HashKeyInfo* pKey){
	return start + pKey->start + pKey->length;
}
*/
//�෽��
/************************************  Functions for build tracks.(iFlyBuild) ************************************************/
//�˺������ڶ���iFlyBuild���������wav��Hash_Table.				//Finished.
void THash::BuildInit(){
	//Initialize memory value.
	pValueStart = (size_t*)malloc(sizeof(size_t) * HashKeyNum * ValuePerBlock * BlockNum);
	if (pValueStart==NULL)
		printf("error");
	memset(pValueStart, 0, sizeof(size_t) * HashKeyNum * ValuePerBlock * BlockNum);
	//�����ڴ��У���ʼ���ѷ���Ľ�β��ַ��
	pValueEnd = pValueStart + sizeof(size_t) * ValuePerBlock;
	//��ʼ��HashKey��
	key_info = (HashKeyInfo*)malloc(sizeof(HashKeyInfo) * HashKeyNum);
	for (int i = 0; i < HashKeyNum; i++){
		key_info[i].next = nullptr;
		key_info[i].length = 0;
	}
}

void THash::BuildUnInit(){
	for (int i=0; i<HashKeyNum; i++){
		HashKeyInfo *p = key_info[i].next;
		HashKeyInfo *q;
		while(p){
			q = p;
			p = p->next;
			free(q);
		}
	}
	free(key_info);
	free(pValueStart);
}

//�Ӹ��������¸�������
void THash::AddSongList(const char *filename){
	strcpy_s(song_list[song_num], strlen(filename) + 1, filename);
	song_num++;
}

//��Value���ڴ��������ݣ�����Key_table.
void THash::InsertHash(size_t f1, size_t f2_f1, size_t t, size_t id, size_t offset){
	//�쳣����
	if (id > (1 << ID_BITS) - 1 || offset > (1 << OFFSET_BITS) - 1){
		printf("The id/time offset overflow.\n");
		return;
	}
	//��ǰkey�ĵ�ַ
	HashKeyInfo *pKey = &key_info[HashTableOffset(f1, f2_f1, t)];
	if (pKey->length%ValuePerBlock==0){
		/*��ԭ���������ڴ���������һ��*/
		HashKeyInfo * pNode = (HashKeyInfo*)malloc(sizeof(HashKeyInfo));
		/* �ڴ���� */
		if ((pValueEnd - pValueStart) > OverFlowThreshold){
			printf("Memory out.\n");
			return;
		}
		pNode->start = pValueEnd;
		pValueEnd += ValuePerBlock*sizeof(size_t);
		pNode->length = 0;
		pNode->next = pKey->next;
		pKey->next = pNode;
	}
	size_t* pValue = pKey->next->start + pKey->next->length;
	*pValue = (size_t)((id << OFFSET_BITS) + offset);
	pKey->next->length++;
	pKey->length++;
	/* For File Write */
	data_num++;
}

//��Hash�����ļ���ˢ������ˢ�����ڴ棬��������iFlySelect���˷��ڴ�ռ䣩
void THash::Hash2File(const char* filename){
	FILE *fp;
	fopen_s(&fp, filename, "wb");
	if (fp == NULL){
		printf("File open WRONG.\n");
	}
	//Write SongName
	fwrite(&song_num, sizeof(size_t), 1, fp);
	printf("��%d�׸�\n", song_num);
	for (size_t i = 0; i<song_num; i++)
		fwrite(song_list[i], sizeof(char), strlen(song_list[i]) + 1, fp);
	//Write hash table.
	fwrite(&data_num, sizeof(size_t), 1, fp);
	for (int i = 0; i<HashKeyNum; i++){
		HashKeyInfo *p = key_info[i].next;
		while (p){
			fwrite(p->start, sizeof(size_t), p->length, fp);
			p = p->next;
		}
	}
	size_t start_place = 0;
	for (int i = 0; i<HashKeyNum; i++){
		fwrite(&start_place, sizeof(size_t), 1, fp);
		fwrite(&key_info[i].length, sizeof(size_t), 1, fp);
		start_place += key_info[i].length;
	}
	fclose(fp);
}

/************************************  Functions for select tracks.(iFlySelect) ************************************************/

void THash::File2Hash(const char *filename){
	FILE *fp;
	fopen_s(&fp, filename, "rb");
	char *chp;
	fread(&song_num, sizeof(size_t), 1, fp);
	printf("��%d�׸�\n", song_num);
	for (size_t i = 0; i<song_num; i++){
		chp = song_list[i];
		do{
			fread(chp, sizeof(char), 1, fp);
		} while (*(chp++) != 0);
	}
	fread(&data_num, sizeof(size_t), 1, fp);
	pValueStart = (size_t*)malloc(sizeof(size_t)* data_num);
	fread(pValueStart, sizeof(size_t), data_num, fp);
	key_table = (HashKeyTable*)malloc(sizeof(HashKeyTable)*HashKeyNum);
	fread(key_table, sizeof(size_t)* 2, HashKeyNum, fp);
	for (int i = 0; i<HashKeyNum; i++)
		key_table[i].start = (size_t)key_table[i].start + pValueStart;
}

void THash::ReBuildInit(){
	vote_table = (short **)malloc(sizeof(short*)* song_num);
	for (size_t i = 0; i<song_num; i++){
		vote_table[i] = (short *)malloc(sizeof(short)* (1 << OFFSET_BITS));
		assert(vote_table[i] != NULL);
	}
}

void THash::VoteInit(){
	for (size_t i = 0; i<song_num; i++){
		memset(vote_table[i], 0, sizeof(short)* (1 << OFFSET_BITS));
	}
	return;
}

void THash::Vote(size_t f1, size_t f2_f1, size_t t, size_t offset){
	HashKeyTable *pKey = &key_table[HashTableOffset(f1, f2_f1, t)];
	size_t length = pKey->length;
	while (length){
		length--;
		size_t offset_value = (*(pKey->start + length) << ID_BITS) >> ID_BITS;
		if (offset_value < offset)
			continue;	//	ΪʧЧͶƱ�����������ͶƱ���Ϊ���
		vote_table[(*(pKey->start + length)) >> OFFSET_BITS][offset_value - offset]++;
	}
	return;
}

size_t THash::VoteResult(size_t &offset){
	size_t result = 0;
	short max = -1;
	for (size_t i = 0; i < song_num; i++){
		for (size_t j = 0; j < (1<<OFFSET_BITS); j++)
		if (vote_table[i][j] > max){
			max = vote_table[i][j];
			result = i;
			offset = j;
			//if (vote_table[i][j])
			//	printf("%d %d %d\n", i, j, vote_table[i][j]);
		}
	}
	return result;
}

THash::THash(){
	pValueStart = nullptr;
	pValueEnd = nullptr;
	vote_table = nullptr;
	data_num = 0;
	song_num = 0;
	key_info = nullptr;
	song_list = (char **)malloc(MAX_SONG_NUM*sizeof(char*));
	for (int i=0; i<MAX_SONG_NUM; i++)
		song_list[i] = (char *)malloc(MAX_SONG_LEN*sizeof(char));
}

THash::~THash(){
	for (int i=0; i<MAX_SONG_NUM; i++)
		free(song_list[i]);
	free(song_list);
}
