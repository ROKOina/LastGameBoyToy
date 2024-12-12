#pragma once

#include <Windows.h>

#define		TGA_IMGTYPE_INDEX		(0x01)	// 無圧縮:インデックスカラー
#define		TGA_IMGTYPE_RGBA		(0x02)	// 無圧縮:RGB/RGBA
#define		TGA_IMGTYPE_GRAY		(0x03)	// 無圧縮:グレースケール
#define		TGA_IMGTYPE_RLE_INDEX	(0x09)	// RLE圧縮:インデックスカラー
#define		TGA_IMGTYPE_RLE_RGBA	(0x0A)	// RLE圧縮:RGB/RGBA
#define		TGA_IMGTYPE_RLE_GRAY	(0x0B)	// RLE圧縮:グレースケール

#define		TGA_ATTR_RIGHT2LEFT		(0x10)	// イメージは右から左に格納
#define		TGA_ATTR_UP2DOWN		(0x20)	// イメージは上から下に格納

struct TargaHeader
{
	UINT8		idLength;			//  0:1 ヘッダのに続くID領域サイズ
	UINT8		colorMapType;		//  1:1 CLUT有無(0/1)
	UINT8		imageType;			//  2:1 形式 (1=Indx, 2=RGB, 他もあるけど略)
	UINT8		colorMapStart[2];	//  3:2 CLUTインデックスの始点
	UINT8		colorMapLength[2];	//  5:2 CLUTインデックス数
	UINT8		colorMapDepth;		//  7:1 CLUTのbit数
	UINT16		offsetX;			//  8:2 画像始点座標X
	UINT16		offsetY;			// 10:2 画像始点座標Y
	UINT16		width;				// 12:2 画像幅
	UINT16		height;				// 14:2 画像高さ
	UINT8		pixelDepth;			// 16:1 ピクセルのbit数
	UINT8		imageDescriptor;	// 17:1 アトリビュート(略)0x00は左→右,下→上
};

