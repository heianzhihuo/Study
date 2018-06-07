#pragma once
#ifndef BITMAP_H_INCLUDED
#define BITMAP_H_INCLUDED
#include <postgres.h>
#endif // BITMAP_H_INCLUDED


typedef uint32 word_t;

static const word_t WORD_SIZE = 32;
static const word_t MAXBITS = 31;
static const word_t HEADER_0F = 2U << 30;
static const word_t HEADER_1F = 3U << 30;
static const word_t ALLONES = ~(1 << 31);

typedef struct
{
	word_t val;//原始数据,保存在右侧
	word_t nbits;//位数，有效位数,最低位开始计数
}ActiveWord;//活动word，不满31位的word

/* bitmap 结构 */
typedef struct
{
	bool isCompressed;
	word_t nbits;//位数，二进制位的总数，这个nbits不包含活动word的nbits
	word_t *m_vec;//存储空间
	word_t m_size;//存储空间大小
	ActiveWord active;//活动word
}bitmap;
typedef bitmap *Bitmap;

/*在bitmap.c中实现*/

Bitmap makeBitmap();
bool isEmpty(Bitmap bmp);
void setBitmapTop(Bitmap bmp, word_t w);
word_t getBitmapTop(Bitmap bmp);
void pushBitmap(Bitmap bmp, word_t w);
void appendActive(Bitmap bmp);
void appendCounter(Bitmap bmp, int val, word_t cnt);
word_t do_cnt(Bitmap bmp);
word_t bitmapSize(Bitmap bmp);
void bitmapCopy(Bitmap bmpa, Bitmap bmpb);
void adjustSize(Bitmap bmp, word_t nbits);
void printBitmap(Bitmap bmp);

void appendWord(Bitmap bmp, word_t w);
void appendOneBit(Bitmap bmp, word_t val);
void appendBits(Bitmap bmp, word_t val, word_t n);
void appendBitmap(Bitmap bmp, Bitmap bmpa);


void compress(Bitmap bmp);
void decompress(Bitmap bmp);

Bitmap bmpand(Bitmap bmpa, Bitmap bmpb);
Bitmap bmpor(Bitmap bmpa, Bitmap bmpb);
Bitmap c2_or(Bitmap bmpa, Bitmap bmpb);
Bitmap c1_or(Bitmap bmpa, Bitmap bmpb);
Bitmap c0_or(Bitmap bmpa, Bitmap bmpb);
Bitmap c2_and(Bitmap bmpa, Bitmap bmpb);
Bitmap c1_and(Bitmap bmpa, Bitmap bmpb);
Bitmap c0_and(Bitmap bmpa, Bitmap bmpb);

Bitmap c00_and(Bitmap bmp1, Bitmap bmp2);
Bitmap c00_or(Bitmap bmp1, Bitmap bmp2);

void setBitBitmap(Bitmap bmp, word_t nbit);
void setBitZBitmap(Bitmap bmp, word_t nbit);
void randBitmap(Bitmap bmp, word_t nb, double density);

