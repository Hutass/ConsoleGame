#pragma once

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													������� �����
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		X 0 0 0	0 0	0						0 / NULL - ������ ���������
//		���� ��������						1 - ��������� � ������� ����
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		0 X 0 0	0 0	0						1 - ������� �������
//											2 - �������� ��������� �������
//		��������� ��������					3 - ���������� �������
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		0 0 X X	X 0	0											000 - ����� ������
//											001 - �����			002 - ����			003 - ������
//											003 - ������ ������ 005 - ������ �����	006 - �����
//		���������� ������
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		0 0 0 0 0 X 0						0 - �������� �����������
//		���������� ������					1-9 - ����� ��������
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		0 0 0 0 0 0 X						1 - �������
//		����������� ��������				2 - ��������
//											3 - �������
//											4 - ��������
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define PROJ_DELETE  1100303
#define ENEM_DELETE  1100203
#define I_AM_ENEM_PERS 1100214
#define I_AM_ENEM_CHAR 1100224
#define I_AM_ENEM_BITER 1100234
#define I_AM_PROJ_STD 1100314
#define I_AM_PROJ_RND 1100314
#define RECT 1100014
#define CIRC 1100014

typedef struct
{
	int code;
	int values[4];
} message;
message msg = { NULL };
