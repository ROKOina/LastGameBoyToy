#pragma once

#include <Windows.h>

#define		TGA_IMGTYPE_INDEX		(0x01)	// �����k:�C���f�b�N�X�J���[
#define		TGA_IMGTYPE_RGBA		(0x02)	// �����k:RGB/RGBA
#define		TGA_IMGTYPE_GRAY		(0x03)	// �����k:�O���[�X�P�[��
#define		TGA_IMGTYPE_RLE_INDEX	(0x09)	// RLE���k:�C���f�b�N�X�J���[
#define		TGA_IMGTYPE_RLE_RGBA	(0x0A)	// RLE���k:RGB/RGBA
#define		TGA_IMGTYPE_RLE_GRAY	(0x0B)	// RLE���k:�O���[�X�P�[��

#define		TGA_ATTR_RIGHT2LEFT		(0x10)	// �C���[�W�͉E���獶�Ɋi�[
#define		TGA_ATTR_UP2DOWN		(0x20)	// �C���[�W�͏ォ�牺�Ɋi�[

struct TargaHeader
{
	UINT8		idLength;			//  0:1 �w�b�_�̂ɑ���ID�̈�T�C�Y
	UINT8		colorMapType;		//  1:1 CLUT�L��(0/1)
	UINT8		imageType;			//  2:1 �`�� (1=Indx, 2=RGB, �������邯�Ǘ�)
	UINT8		colorMapStart[2];	//  3:2 CLUT�C���f�b�N�X�̎n�_
	UINT8		colorMapLength[2];	//  5:2 CLUT�C���f�b�N�X��
	UINT8		colorMapDepth;		//  7:1 CLUT��bit��
	UINT16		offsetX;			//  8:2 �摜�n�_���WX
	UINT16		offsetY;			// 10:2 �摜�n�_���WY
	UINT16		width;				// 12:2 �摜��
	UINT16		height;				// 14:2 �摜����
	UINT8		pixelDepth;			// 16:1 �s�N�Z����bit��
	UINT8		imageDescriptor;	// 17:1 �A�g���r���[�g(��)0x00�͍����E,������
};

