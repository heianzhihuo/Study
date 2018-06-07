#include "bitmap.h"

/*对两个bitmap进行and*/
Bitmap bmpand(Bitmap bmpa, Bitmap bmpb)
{
	Bitmap tmp = makeBitmap();
	word_t nbitsa, nbitsb, nbits;
	nbitsa = bitmapSize(bmpa);
	nbitsb = bitmapSize(bmpb);
	if (nbitsa != nbitsb)
	{
		printf("对两个大小不同的位向量进行操作%u,%u\n", nbitsa, nbitsb);
	}
	if (nbitsa > nbitsb)
	{
		nbits = nbitsa;
		bitmapCopy(bmpb, tmp);
		adjustSize(tmp, nbitsa);
		return bmpand(bmpa, tmp);
	}
	else if (nbitsb > nbitsa)
	{
		nbits = nbitsa;
		bitmapCopy(bmpa, tmp);
		adjustSize(tmp, nbitsb);
		return bmpand(bmpb, tmp);
	}
	return c2_and(bmpa, bmpb);
}

/*对两个bitmap进行or*/
Bitmap bmpor(Bitmap bmpa, Bitmap bmpb)
{
	Bitmap tmp = makeBitmap();
	word_t nbitsa, nbitsb, nbits;
	nbitsa = bitmapSize(bmpa);
	nbitsb = bitmapSize(bmpb);
	if (nbitsa != nbitsb)
	{
		printf("对两个大小不同的位向量进行操作%u,%u\n", nbitsa, nbitsb);
	}
	if (nbitsa > nbitsb)
	{
		nbits = nbitsa;
		bitmapCopy(bmpb, tmp);
		adjustSize(tmp, nbitsa);
		return bmpor(bmpa, tmp);
	}
	else if (nbitsb > nbitsa)
	{
		nbits = nbitsa;
		bitmapCopy(bmpa, tmp);
		adjustSize(tmp, nbitsb);
		return bmpand(bmpb, tmp);
	}
	return c2_or(bmpa, bmpb);
}

/*对未压缩的bitmap进行and，生成未压缩的bitmap*/
Bitmap c00_and(Bitmap bmp1, Bitmap bmp2)
{
	Bitmap result = makeBitmap();
	word_t i, j, wi, wj;
	result->m_vec = palloc(bmp1->m_size*sizeof(word_t));
	memset(result->m_vec,0,bmp1->m_size*sizeof(word_t));
	for (i = 0; i < bmp1->m_size; i++)
		result->m_vec[i] = bmp1->m_vec[i] & bmp2->m_vec[i];
	result->active.nbits = bmp1->active.nbits;
	result->active.val = bmp1->active.val & bmp1->active.val;
	return result;
}

/*对未压缩的bitmap进行or，生成未压缩的bitmap*/
Bitmap c00_or(Bitmap bmp1, Bitmap bmp2)
{
	Bitmap result = makeBitmap();
	word_t i, j, wi, wj;
	result->m_vec = palloc(bmp1->m_size*sizeof(word_t));
	memset(result->m_vec,0,bmp1->m_size*sizeof(word_t));
	for (i = 0; i < bmp1->m_size; i++)
		result->m_vec[i] = bmp1->m_vec[i] | bmp2->m_vec[i];
	result->active.nbits = bmp1->active.nbits;
	result->active.val = bmp1->active.val | bmp1->active.val;
	return result;
}

/*对未压缩的bitmap进行and,生成压缩的bitmap*/
Bitmap c0_and(Bitmap bmpa, Bitmap bmpb)
{
	Bitmap result = makeBitmap();
	word_t i, j,wi,wj;
	for (i = 0; i < bmpa->m_size; i++)
	{
		wi = bmpa->m_vec[i];
		wj = bmpb->m_vec[i];
		appendWord(result, wi & wj);
	}
	if (bmpa->active.nbits)
	{
		result->active.val = bmpa->active.val & bmpb->active.val;
		result->active.nbits = bmpa->active.nbits;
	}
	return result;
}

/*对未压缩的bitmap进行or，生成压缩的bitmap*/
Bitmap c0_or(Bitmap bmpa, Bitmap bmpb)
{
	Bitmap result = makeBitmap();
	word_t i, j, wi, wj;
	for (i = 0; i < bmpa->m_size; i++)
	{
		wi = bmpa->m_vec[i];
		wj = bmpb->m_vec[i];
		appendWord(result, wi | wj);
	}
	if (bmpa->active.nbits)
	{
		result->active.val = bmpa->active.val | bmpb->active.val;
		result->active.nbits = bmpa->active.nbits;
	}
	return result;
}

/*对两个压缩的bmp进行and，需要对bmp进行对齐*/
Bitmap c2_and(Bitmap bmpa, Bitmap bmpb)
{
	Bitmap result = makeBitmap();
	Bitmap tmp = makeBitmap();
	word_t xi = 0, yi = 0;
	word_t x, y;
	word_t headx,heady;
	word_t countx = 0, county = 0;
	while (true)
	{
		if (countx == 0)
		{
			if (xi >= bmpa->m_size) break;
			x = bmpa->m_vec[xi];
			headx = x >> 30;
			countx = (~HEADER_1F)& x;
			xi++;
		}
		if (county == 0)
		{
			if (yi >= bmpb->m_size) break;
			y = bmpb->m_vec[yi];
			heady = y >> 30;
			county = (~HEADER_1F)& y;
			yi++;
		}
		if (headx < 2 && heady < 2)
		{
			//x,y都是L,则直接进行或，同时两个都无需保存
			appendWord(result, x & y);
			countx = 0;
			county = 0;
		}
		else if (headx > 1 && heady > 1)
		{
			//x,y都是fill，则x，y中有一个被耗尽,添加最小个数的0或者最小个数的1
			word_t count_min = countx > county ? county : countx;
			countx -= count_min;
			county -= count_min;
			if (headx == 3 && heady == 3)
				appendCounter(result, 1, count_min);//两个都是1-fill
			else
				appendCounter(result, 0, count_min);//两个有一个是0-fill
		}
		else if (headx > 1)
		{//x是fill
			countx--;//x继续使用
			county = 0;//y跳到下一个
			if (headx == 2)
				appendCounter(result, 0,1);//x是0-fill,添加一个0-fill
			else
				appendWord(result, y);//x是1-fill,添加y
		}
		else
		{//y是fill
			county--;//y继续使用
			countx = 0;//x跳到下一个
			if (heady == 2)
				appendCounter(result, 0, 1);//y是0-fill,添加一个0-fill
			else
				appendWord(result, x);//y是1-fill,添加x	
		}
	}
	if (xi != bmpa->m_size)
	{
		printf("ErrorX:%d\n", xi);
	}
	if (yi != bmpb->m_size)
	{
		printf("ErrorY:%d\n", yi);
	}
	if (bmpa->active.nbits)
	{//对活动位进行操作
		result->active.val = bmpa->active.val & bmpb->active.val;
		result->active.nbits = bmpa->active.nbits;
	}
	return result;
}

/*对两个压缩的bmp进行or，需要对bmp进行对齐*/
Bitmap c2_or(Bitmap bmpa, Bitmap bmpb)
{
	Bitmap result = makeBitmap();
	word_t xi = 0, yi = 0;
	word_t x, y;
	word_t headx, heady;
	word_t countx = 0, county = 0;
	while (true)
	{
		if (countx == 0)
		{
			if (xi >= bmpa->m_size) break;
			x = bmpa->m_vec[xi];
			headx = x >> 30;
			countx = (~HEADER_1F)& x;
			xi++;
		}
		if (county == 0)
		{
			if (yi >= bmpb->m_size) break;
			y = bmpb->m_vec[yi];
			heady = y >> 30;
			county = (~HEADER_1F)& y;
			yi++;
		}
		if (headx < 2 && heady < 2)
		{
			//x,y都是L,则直接进行或，同时两个都无需保存
			appendWord(result, x | y);
			countx = 0;
			county = 0;
		}
		else if (headx > 1 && heady > 1)
		{
			//x,y都是fill，则x，y中有一个被耗尽,添加最小个数的0或者最小个数的1
			word_t count_min = countx > county ? county : countx;
			countx -= count_min;
			county -= count_min;
			if (headx == 3 || heady == 3)
				appendCounter(result, 1, count_min);//两个有一个是1-fill
			else
				appendCounter(result, 0, count_min);//两个都是0-fill
		}
		else if (headx > 1)
		{//x是fill
			countx--;//x继续使用
			county = 0;//y跳到下一个
			if (headx == 3)
				appendCounter(result, 1, 1);//x是1-fill,添加一个1-fill
			else
				appendWord(result, y);//x是0-fill,添加y
		}
		else
		{//y是fill
			county--;//y继续使用
			countx = 0;//x跳到下一个
			if (heady == 3)
				appendCounter(result, 1, 1);//y是1-fill,添加一个1-fill
			else
				appendWord(result, x);//y是0-fill,添加x	
		}
	}
	if (xi != bmpa->m_size)
	{
		printf("ErrorX:%d\n", xi);
	}
	if (yi != bmpb->m_size)
	{
		printf("ErrorY:%d\n", yi);
	}
	if (bmpa->active.nbits)
	{
		result->active.val = bmpa->active.val | bmpb->active.val;
		result->active.nbits = bmpa->active.nbits;
	}
	return result;
}

/*统计位数*/
word_t do_cnt(Bitmap bmp)
{
	word_t cnt = 0,w;
	word_t i;
	for (i = 0; i < bmp->m_size; i++)
	{
		w = bmp->m_vec[i];
		if (w > ALLONES)
			cnt += (w & (~HEADER_1F))*MAXBITS;
		else
			cnt += MAXBITS;
	}
	return cnt + bmp->active.nbits;
}

/*计算bitmap中的有效位数*/
word_t bitmapSize(Bitmap bmp)
{
	word_t sz = bmp->nbits + bmp->active.nbits;
	word_t sr = do_cnt(bmp);
	if (sz != sr)
		printf("WARN:%d,%d\n", sz, sr);
	return sz<sr?sz:sr;
}

/*对bitmap进行WAH压缩*/
void compress(Bitmap bmp)
{
	word_t *tmp;//压缩时数据临时存放处
	tmp = (word_t *)palloc(sizeof(word_t)*bmp->m_size);
	word_t i,j;//i,j分别是当前数组的和新的数组正在处理的word的下标
	word_t w;//w是旧的word
	int headi, headj;//headi,当前正在处理的head,headj,上一次处理的
	for (i = 0,j = 0; i < bmp->m_size; i++)
	{
		w = bmp->m_vec[i];
		if (w == 0)
		{//全0
			headi = 2;
			w = HEADER_0F | 1;
		}
		else if (w == ALLONES)
		{
			//全1
			headi = 3;
			w = HEADER_1F | 1;
		} 
		else
			headi = w >> 30;
		if (j == 0)
		{
			tmp[j] = w;
			j++;
		}
		else
		{
			if (headi == headj && headj>1)
				//类型相同
				tmp[j-1] += w & (~HEADER_1F);
			else
			{
				tmp[j] = w;
				j++;
			}
		}
		headj = headi;
	}
	bmp->m_size = j;
	pfree(bmp->m_vec);
	bmp->m_vec = tmp;
}

/*对bitmap进行解压缩，不进行分组处理，解压后每个word 31位,最高位为0*/
void decompress(Bitmap bmp)
{
	word_t *tmp;//压缩时数据临时存放处
	tmp = (word_t *)palloc(sizeof(word_t)*(bmp->nbits/MAXBITS));
	word_t i, j, k, w, cnt;
	int head;
	for (i = 0,j=0; i < bmp->m_size; i++)
	{
		w = bmp->m_vec[i];
		head = w >> 30;
		cnt = (~HEADER_1F)&w;
		if (head > 1)
		{//fill
			if (head == 2)//0-fill
				for (k = 0; k < cnt; k++, j++)
					tmp[j] = 0;
			else//1-fill
				for (k = 0; k < cnt; k++, j++)
					tmp[j] = ALLONES;
		}
		else
		{//Literal
			tmp[j] = w;
			j++;
		}
	}
	bmp->m_size = j;
	pfree(bmp->m_vec);
	bmp->m_vec = tmp;
}

/*16进制输出bitmap*/
void printBitmap(Bitmap bmp)
{
	word_t i;
	for (i = 0; i < bmp->m_size; i++)
		printf("%08x ", bmp->m_vec[i]);
	if (bmp->active.nbits)
		printf("%08x", bmp->active.val);
	printf("\n");
}

/*将a copy给b*/
void bitmapCopy(Bitmap bmpa, Bitmap bmpb)
{
	if (bmpb == NULL)
		bmpb = makeBitmap();
	bmpb->nbits = bmpa->nbits;
	bmpb->active = bmpa->active;
	bmpb->m_size = bmpa->m_size;
	bmpb->isCompressed = bmpa->isCompressed;
	bmpb->m_vec = palloc(bmpa->m_size * sizeof(word_t));
	memcpy(bmpb->m_vec, bmpa->m_vec, bmpa->m_size * sizeof(word_t));
}

/*调整bitmap的有效位数，使位数达到nbits，通过在右侧添加0*/
void adjustSize(Bitmap bmp, word_t nbits)
{
	word_t sz = bmp->nbits + bmp->active.nbits;
	if (sz == nbits) return;
	if (sz < nbits)
		appendBits(bmp, 0, nbits - sz);
}

/*产生一个空的bitmap*/
Bitmap makeBitmap()
{
	Bitmap bmp = (Bitmap)palloc(sizeof(bitmap));
	bmp->active.nbits = 0;
	bmp->active.val = 0;
	bmp->m_vec = NULL;
	bmp->isCompressed = false;
	bmp->m_size = 0;
	bmp->nbits = 0;
	return bmp;
}

/*当active word为空时，添加一个counter为cnt的val-fill
 * 即，添加cnt个val-fill
 * val必须是0或者1
 */
void appendCounter(Bitmap bmp, int val, word_t cnt)
{
	word_t head = 2 + val;
	word_t w = (head << 30) | cnt;//生成WAH word
	if (isEmpty(bmp))
		pushBitmap(bmp, w);//直接添加
	else
	{
		word_t w1 = getBitmapTop(bmp);
		if (w1 >> 30 == head)
			setBitmapTop(bmp, w1 + cnt);//将top的counter加cnt
		else
			pushBitmap(bmp, w);//将w添加到存储空间
	}
	bmp->nbits += cnt * MAXBITS;//增加位数
}

/* 
 * 将满31位的active word添加到bitmap中,
 * 同时清空active word
 * active word是未压缩的
 */
void appendActive(Bitmap bmp)
{
	word_t w = bmp->active.val;
	word_t head;
	if (w == 0)
	{
		//全0
		head = 2;
		w = HEADER_0F | 1;
	}
	else if (w == ALLONES)
	{
		//全1
		head = 3;
		w = HEADER_1F | 1;
	}
	else
		head = 0;
	if (isEmpty(bmp))
		pushBitmap(bmp, w);//将w添加到存储空间
	else
	{
		word_t w1 = getBitmapTop(bmp);
		if (w1 >> 30 == head && head >1)//和top的word是相同类型的fill
			setBitmapTop(bmp, w1 + 1);//top的counter加一
		else pushBitmap(bmp, w);//将w添加到存储空间
	}
	bmp->nbits += MAXBITS;//增加存储位数
	bmp->active.nbits = 0;
	bmp->active.val = 0;
}

/*将一个bitmap添加到另一个bitmap上*/
void appendBitmap(Bitmap bmp, Bitmap bmpa)
{
	word_t ex_size = bitmapSize(bmp) + bitmapSize(bmpa);
	word_t i;
	for (i = 0; i < bmpa->m_size; i++)
		appendWord(bmp,bmpa->m_vec[i]);//将a中的word添加到bmp中
	if (bmpa->active.nbits)
	{//合并active word
		if (bmp->active.nbits + bmpa->active.nbits < MAXBITS)
		{
			//合并后不足31位
			bmp->active.val << bmpa->active.nbits;//空出位置
			bmp->active.val |= bmpa->active.val;
			bmp->active.nbits = bmp->active.nbits + bmpa->active.nbits;
		}
		else
		{
			//合并后超过31位
			word_t nb1 = bmp->active.nbits + bmpa->active.nbits - MAXBITS;//超出部分
			bmp->active.val << (MAXBITS - bmp->active.nbits);//腾出空位
			word_t w1 = bmpa->active.val >> nb1;//提取出b的相应位置
			bmp->active.val |= w1;//合并出31位
			appendActive(bmp);
			bmp->active.nbits = nb1;//
			if (nb1 > 0)
				bmp->active.val = ((1U << nb1) - 1) & bmpa->active.val;//剩余部分
		}
	}
	else
	{//简单的复制过去
		bmp->active.nbits = bmpa->active.nbits;
		bmp->active.val = bmpa->active.val;
	}
	if (ex_size != bitmapSize(bmp))
	{
		printf("Error");
	}
}

/*修改bitmap栈中最上面的word为w*/
void setBitmapTop(Bitmap bmp, word_t w)
{
	if (isEmpty(bmp)) return;
	bmp->m_vec[bmp->m_size - 1] = w;
}

/*获得bitmap栈中最后的word*/
word_t getBitmapTop(Bitmap bmp)
{
	if (isEmpty(bmp))
		return 0;
	return bmp->m_vec[bmp->m_size - 1];
}

/*bitmap是否为空*/
bool isEmpty(Bitmap bmp)
{
	return !bmp->m_size;
}

/*向bitmap的存储空间中添加一个word*/
void pushBitmap(Bitmap bmp, word_t w)
{
	word_t size = bmp->m_size;
	if(size==0)
		bmp->m_vec = (word_t *)palloc(sizeof(word_t));
	else
		bmp->m_vec = repalloc(bmp->m_vec,(size+1) * sizeof(word_t));
	bmp->m_vec[bmp->m_size] = w;
	bmp->m_size++;
}

/*向bitmap中添加一个word w，w最高位为1表示已压缩*/
void appendWord(Bitmap bmp, word_t w)
{
	word_t w1;
	int head = w >> 30;
	int b2 = head % 2;
	int nb1, nb2;//nb1，nb2,用于保存位数
	word_t cnt;
	if (bmp->active.nbits)
	{
		nb1 = bmp->active.nbits;
		nb2 = MAXBITS - nb1;
		bmp->active.val = bmp->active.val << nb2;
		if (head > 1)
		{
			//已压缩
			if (head == 3)
			{
				//如果是1-fill，active低位应该填1
				w1 = (1 << nb2) - 1;
				bmp->active.val |= w1;
			}
			appendActive(bmp);
			cnt = (w & (~HEADER_1F)) - 1;//counter
			if (cnt>0)
				appendCounter(bmp, b2, cnt);
			bmp->active.nbits = nb1;
			bmp->active.val = ((1 << nb1) - 1)*b2;
		}
		else
		{
			//未压缩
			w1 = (w >> nb1);
			bmp->active.val |= w1;
			appendActive(bmp);
			w1 = (1 << nb1) - 1;
			bmp->active.val = w&w1;//active取w中低nbits位
			bmp->active.nbits = nb1;//添加了31位，nbits不变，
		}
	}
	else
	{
		//没有active
		if (head > 1)
		{
			//压缩
			cnt = w & (~HEADER_1F);//counter
			if (cnt>0)
				appendCounter(bmp, b2, cnt);
		}
		else
		{
			//未压缩
			bmp->active.val = w;
			appendActive(bmp);
		}
	}
}

/*
 * 向bitmap中添加n个0或者1
 * val必须是1或者0
 */
void appendBits(Bitmap bmp, word_t val, word_t n)
{
	if (n == 0) return;
	if (bmp->active.nbits>0)
	{
		//有active，先向active中添加，再
		word_t tmp = MAXBITS - bmp->active.nbits;
		if (tmp>n) tmp = n;
		bmp->active.nbits += tmp;
		bmp->active.val <<= tmp;
		n -= tmp;
		if (val == 1)
			bmp->active.val |= (1U << tmp) - 1;
		if (bmp->active.nbits == MAXBITS)
			appendActive(bmp);
	}
	if (n >= MAXBITS)
	{
		//剩余的超过31位
		word_t cnt = n / MAXBITS;
		appendCounter(bmp, val, cnt);
		n -= cnt * MAXBITS;//按31位一组添加后，剩余的
	}
	if (n>0)
	{
		//剩余不满31位的
		bmp->active.nbits = n;
		bmp->active.val = val*((1U << n) - 1);
	}
}

/*向bitmap中添加一位val，val必须是0或1*/
void appendOneBit(Bitmap bmp, word_t val)
{
	word_t w = bmp->active.val;
	bmp->active.val = (bmp->active.val << 1) | val;
	bmp->active.nbits++;
	if (bmp->active.nbits == MAXBITS)
		appendActive(bmp);
}

/*将未压缩的bmp的第nbit位设为1,nbit从0开始数*/
void setBitBitmap(Bitmap bmp, word_t nbit)
{
	if (!bmp->isCompressed && nbit<bitmapSize(bmp))
	{
		word_t nw = nbit / MAXBITS;
		int bit = nbit%MAXBITS;//在32位的组中，除最高位从左往右第bit位
		if (nw < bmp->m_size)
			bmp->m_vec[nw] |= (1 << (MAXBITS - bit - 1));
		else
			bmp->active.val |= (1 << (bmp->active.nbits - bit - 1));
	}
}

/*将未压缩的bmp的第nbit位设为0,nbit从0开始计数*/
void setBitZBitmap(Bitmap bmp, word_t nbit)
{
	if (!bmp->isCompressed && nbit<bitmapSize(bmp))
	{
		word_t nw = nbit / MAXBITS;
		int bit = nbit%MAXBITS;//在32位的组中，除最高位从左往右第bit位
		word_t w = 1 << (31 - bit);
		if (nw < bmp->m_size)
			bmp->m_vec[nw] &= ~(1 << (31 - bit));
		else
			bmp->active.val &= ~(1 << (bmp->active.nbits - bit - 1));
	}
}

/*生成随机的位图,总位数为nb,位密集度为density*/
void randBitmap(Bitmap bmp, word_t nb, double density)
{
	if (nb > 0 && density < 1)
	{
		srand(time(0));
		word_t nc = nb*density;//1的位数
		word_t nw = nb / MAXBITS;//需要的字节数
		word_t i=0,cnt = 0;
		bmp->m_vec = palloc(sizeof(word_t)*nw);
		memset(bmp->m_vec,0,sizeof(word_t)*nw);
		bmp->nbits = nw*MAXBITS;
		bmp->m_size = nw;
		bmp->active.nbits = nb%MAXBITS;
		bmp->active.val = 0;
		bmp->isCompressed = false;
		while (cnt<nc)
		{
			if (rand() < RAND_MAX*density)
			{
				setBitBitmap(bmp, i);
				cnt++;
			}
			i++;
			if (i >= nb)//知道生成足够多的1
				i=i^i;
		}
	}
}
