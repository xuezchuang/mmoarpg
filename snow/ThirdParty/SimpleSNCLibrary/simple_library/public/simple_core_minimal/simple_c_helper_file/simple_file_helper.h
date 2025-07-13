#pragma once
//Copyright (C) RenZhai.2019.All Rights Reserved.
//���� ��լ
//��c�����ϸԴ�뽲����AboutCG ���ְ�������Ӳ��c���ԡ���Ƶ�������棺
//https://www.aboutcg.org/courseDetails/902/introduce
//ϣ��ѧϰ�������漼�� ������UE4��Ϸ, ���Է���������ַ��
//https://zhuanlan.zhihu.com/p/60117613
//
//bibi���Կ���������լϵ�н̳̽��ܣ�
//https://space.bilibili.com/29544409
//
//���˲�����վ
//http://renzhai.net
//
//���ڱ��װ�������ϸ���� :
//���ְ汾(��ϸ)��
//https://zhuanlan.zhihu.com/p/144558934
//��Ƶ�汾��
//https://www.bilibili.com/video/BV1x5411s7s3
#include "../simple_c_core/simple_core_minimal.h"

_CRT_BEGIN_C_HEADER
typedef struct
{
	int index;
	char paths[8196][1024];//MAX_PATH
}def_c_paths;

void init_def_c_paths(def_c_paths *c_paths);

int copy_file(char *Src, char *Dest);

void find_files(char const *in_path, def_c_paths *str,bool b_recursion);

bool create_file(char const *filename);
 
bool create_file_directory(char const *in_path);

bool get_file_buf(const char *path,char *buf);

bool add_file_buf(const char *path, char *buf);

bool add_new_file_buf(const char *path, char *buf);

unsigned int get_file_size_by_filename(const char *filename);

unsigned int get_file_size(FILE *file_handle);
_CRT_END_C_HEADER