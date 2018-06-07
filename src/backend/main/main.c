#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "postgres.h"
#include "miscadmin.h"

#include "utils/rel.h"
#include "storage/smgr.h"


#include <storage/bufpage.h>
#include <storage/lmgr.h>
#include <access/heapam.h>
#include <storage/bufmgr.h>
#include <utils/memutils.h>
#include <utils/mcxt.h>
#include "utils/relcache.h"

#include "bitmap.h"

int init()
{
	char* DataBaseName = "Postgres";
	IPCKey      key;
	short port = 10000;
	extern GlobalMemory	CacheCxt;
	EnableMemoryContext(true);//
	DataDir = "/home/ubuntu/data/postgres";
	SetDatabaseName(DataBaseName);
	if (!CacheCxt)
		CacheCxt = CreateGlobalMemory("Cache");
	
	smgrinit();//init smgr
	
	
	key = SystemPortAddressCreateIPCKey((SystemPortAddress) port);
	CreateSharedMemoryAndSemaphores(key);
	InitBufferPool(key);
	InitBufTable();
	SetTransactionFlushEnabled(true);
	return 1;
}

/*创建一个创建一个Relation*/
Relation heap_create(char *relname)
{
	Relation rdesc;
	char *name = relname;
	int len;
	int smgr = DEFAULT_SMGR;
	int fd;

	len = sizeof(RelationData);
	rdesc = (Relation) palloc(len);//分配空间
	memset((char *)rdesc, 0,len);//初始化

	rdesc->rd_rel = (Form_pg_class)palloc(sizeof *rdesc->rd_rel);
	memset((char *)rdesc->rd_rel, 0,sizeof *rdesc->rd_rel);
	
	namestrcpy(&(rdesc->rd_rel->relname),relname);//relname

	rdesc->rd_nblocks = 0;//???
	rdesc->rd_id = 16993;//OID,从16384开始分配
	
	rdesc->rd_isnailed = false;
	rdesc->rd_islocal = false;
	rdesc->rd_istemp = false;//非临时表
	RelationSetReferenceCount(rdesc, 1);
	

	rdesc->rd_rel->relkind = RELKIND_UNCATALOGED;
    rdesc->rd_rel->relnatts = 0;//关系中元组属性个数
    rdesc->rd_rel->relsmgr = smgr;//存储管理器选择？
    
	rdesc->rd_tmpunlinked = TRUE;	/* change once table is created */
    rdesc->rd_fd = (File)smgropen(smgr, rdesc);
    rdesc->rd_tmpunlinked = FALSE;

    RelationInitLockInfo(rdesc);

	return rdesc;
}

/*向relation写入数据,blockIndex为-1则putAtEND
 *在blockIndex的页插入数据,插入到该页的空闲位置，该页有足够多的空闲空间
*/
void RelationPut(Relation relation,BlockNumber blockIndex,
char *data,int t_len)
{
	Buffer		buffer;
    Page		pageHeader;
    BlockNumber		numberOfBlocks;
    OffsetNumber	offnum;
    unsigned int	len;
	
	numberOfBlocks = RelationGetNumberOfBlocks(relation);
    Assert(blockIndex < numberOfBlocks);
	
	if(blockIndex==-1)
	{
		RelationPutAtEnd(relation,data,t_len);
		return;
	}
	
	buffer = ReadBuffer(relation, blockIndex);
	if (!BufferIsValid(buffer)) {
	elog(WARN, "RelationPutHeapTuple: no buffer for %ld in %s",
	     blockIndex, &relation->rd_rel->relname);
    }

	pageHeader = (Page)BufferGetPage(buffer);
    len = (unsigned)DOUBLEALIGN(t_len);	/* be conservative */
    Assert((int)len <= PageGetFreeSpace(pageHeader));//保证该页空间足够
	
	offnum = PageAddItem((Page)pageHeader, (Item)data,
			 t_len, InvalidOffsetNumber, LP_USED);

	WriteBuffer(buffer);
	
}

/*在最后追加数据,追加t_len个字节的数据*/
void
RelationPutAtEnd(Relation relation,char *data,int t_len)
{
	Buffer		buffer;
    Page		pageHeader;
    BlockNumber		lastblock;
    OffsetNumber	offnum;
	unsigned int	len;
	
	//put At End
	lastblock = RelationGetNumberOfBlocks(relation);
	printf("BB:%d\n",lastblock);
	if (lastblock == 0)
	{	
		//原先没有数据
		buffer = ReadBuffer(relation, lastblock);
		pageHeader = (Page)BufferGetPage(buffer);
		if (PageIsNew((PageHeader) pageHeader))
		{
			buffer = ReleaseAndReadBuffer(buffer, relation, P_NEW);
			pageHeader = (Page)BufferGetPage(buffer);
			PageInit(pageHeader, BufferGetPageSize(buffer), 0);
		}
	}
	else
		buffer = ReadBuffer(relation, lastblock - 1);
	
	pageHeader = (Page)BufferGetPage(buffer);
	len = (unsigned)DOUBLEALIGN(t_len);	// be conservative 
	if (len > PageGetFreeSpace(pageHeader))
	{
		//数据太大，获取新页
	    buffer = ReleaseAndReadBuffer(buffer, relation, P_NEW);
	    pageHeader = (Page)BufferGetPage(buffer);
	    PageInit(pageHeader, BufferGetPageSize(buffer), 0);
	    
	    if (len > PageGetFreeSpace(pageHeader))
			elog(WARN, "Tuple is too big: size %d", len);
	}
	offnum = PageAddItem((Page)pageHeader, (Item)data,
			 t_len, InvalidOffsetNumber, LP_USED);

	WriteBuffer(buffer);
}

/*从最后一页中，或得数据*/
Item *getAllItemsAtEnd(Relation relation,int *num)
{
	Buffer		buffer;
    Page		pageHeader;
	BlockNumber		lastblock;
	Item *items;

	lastblock = RelationGetNumberOfBlocks(relation);
	if(lastblock == 0)
	{
		printf("No Datas in this relation");
		items = NULL;
		*num = 0;
		return items;
	}
	buffer = ReadBuffer(relation, lastblock-1);
	pageHeader = (Page)BufferGetPage(buffer);
	*num =  PageGetMaxOffsetNumber((Page)pageHeader);
	items = (Item *)palloc(sizeof(Item)*(*num));

	for(int i=*num-1;i>=0;i--)
	{
		ItemId itemId = &(((PageHeader)pageHeader)->pd_linp[i]);
		items[i] = PageGetItem(pageHeader,itemId);
	}
	return items;
}

/*从指定页中获得数据*/
Item *getAllItems(Relation relation,BlockNumber blockIndex,int *num)
{	
	Buffer		buffer;
    Page		pageHeader;
	BlockNumber		numberOfBlocks;
	Item *items;
	
	numberOfBlocks = RelationGetNumberOfBlocks(relation);
    Assert(blockIndex < numberOfBlocks);
	if(blockIndex==-1)
	{
		return getAllItemsAtEnd(relation,num);
	}

	buffer = ReadBuffer(relation, blockIndex);
	if (!BufferIsValid(buffer)) {
	elog(WARN, "RelationPutHeapTuple: no buffer for %ld in %s",
	     blockIndex, &relation->rd_rel->relname);
    }

	pageHeader = (Page)BufferGetPage(buffer);
    *num =  PageGetMaxOffsetNumber((Page)pageHeader);
	items = (Item *)palloc(sizeof(Item)*(*num));
	
	for(int i=*num-1;i>=0;i--)
	{
		ItemId itemId = &(((PageHeader)pageHeader)->pd_linp[i]);
		items[i] = PageGetItem(pageHeader,itemId);
	}
	return items;

}

int
main(int argc, char *argv[])
{
	Relation rdesc;
	char *relname = "Hellos";
	int len;
	char *data = "Hello World in the Post gre";
	Item *items;
	init();	
	
	rdesc = heap_create(relname);
	//printf("fd:%d\n",RelationGetLRelId(rdesc));
	/*
	Buffer		buffer;
    Page		pageHeader;
    BlockNumber		numberOfBlocks;
    OffsetNumber	offnum;

	numberOfBlocks = RelationGetNumberOfBlocks(rdesc);
	printf("NB:%d\n",numberOfBlocks);
	printf("%d\n",RelationIsValid(rdesc));
	*/
	//len = strlen(relname);
	//RelationPutAtEnd(rdesc,relname,len);
	len = strlen(data);
	RelationPut(rdesc,0,data,len);
	items = getAllItems(rdesc,0,&len);
	for(int i=0;i<len;i++)
		printf("Items:%s\n",items[i]);
	FlushBufferPool(!TransactionFlushEnabled());
	printf("Hello World!\n");

	word_t w = 0xBFFFFFFF;
	word_t p = 32;
	Bitmap bmp1 = makeBitmap();
	appendBits(bmp1, 1, 3);
	appendBits(bmp1, 0, 24);
	appendBits(bmp1,1, 2);
	appendBits(bmp1,0, 79);
	appendBits(bmp1,1, 20);
	printBitmap(bmp1);
	printf("%u\n", bmp1->nbits);

	Bitmap bmp2 = makeBitmap();
	appendBits(bmp2, 1, 31);
	appendBits(bmp2, 1, 31);
	appendWord(bmp2, 0x7C0001E0);
	appendWord(bmp2, 0x3FE00000);
	appendBits(bmp2, 0, 2);
	appendBits(bmp2, 1, 2);
	printBitmap(bmp2);

	Bitmap bmp3 = c2_and(bmp1, bmp2);
	printBitmap(bmp3);
	Bitmap bmp4 = c2_or(bmp1, bmp2);
	printBitmap(bmp4);

	const word_t bitnum = 10000000;//10000000
	double density[10] = { 0.001, 0.00030, 0.001, 0.0035, 0.01, 0.02, 0.045, 0.1, 0.25, 0.5 };
	clock_t time1[10] = { 0 };
	clock_t time2[10] = { 0 };
	clock_t start, end,time;
	int i;

	for (i = 0; i < 10; i++)
	{
		randBitmap(bmp1,bitnum,density[i]);
		randBitmap(bmp2,bitnum,density[i]);

		bmp3 = makeBitmap();
		start = clock();
		bmp3 = c00_or(bmp1,bmp2);
		end = clock();
		time1[i] = end-start;


		compress(bmp1);
		bitmapSize(bmp1);
		compress(bmp2);
		bitmapSize(bmp2);

		bmp4 = makeBitmap();
		start = clock();
		bmp4 = c2_or(bmp1,bmp2);
		end = clock();
		time2[i] = end-start;
	}
	for (i = 0; i < 10; i++)
		printf("%ld\t", time1[i]);
	printf("\n");
	for (i = 0; i < 10; i++)
		printf("%ld\t", time2[i]);
	printf("\n%d",CLOCKS_PER_SEC);
}

