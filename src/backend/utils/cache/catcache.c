/*-------------------------------------------------------------------------
 *
 * catcache.c--
 *    System catalog cache for tuples matching a key.
 *
 * Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *    $Header: /usr/local/cvsroot/postgres95/src/backend/utils/cache/catcache.c,v 1.6 1996/12/04 03:06:09 bryanh Exp $
 *
 * Notes:
 *	XXX This needs to use exception.h to handle recovery when
 *		an abort occurs during DisableCache.
 *	
 *-------------------------------------------------------------------------
 */
#include <string.h>
#include "postgres.h"
#include "access/heapam.h"
#include "access/genam.h"
#include "utils/builtins.h"
#include "utils/tqual.h"
#include "storage/bufpage.h"
#include "access/valid.h"
#include "miscadmin.h"
#include "utils/portal.h"
#include "utils/catcache.h"
//#include "fmgr.h"		/* for F_BOOLEQ, etc.  DANGER */
#include "utils/elog.h"
#include "utils/palloc.h"
#include "utils/mcxt.h"
#include "utils/rel.h"
#include "catalog/pg_type.h"	/* for OID of int28 type */
#include "lib/dllist.h"


/* ----------------
 *	variables, macros and other stuff
 *
 *  note CCSIZE allocates 51 buckets .. one was already allocated in
 *  the catcache structure.
 * ----------------
 */

#ifdef CACHEDEBUG
#define CACHE1_elog(a,b)		elog(a,b)
#define CACHE2_elog(a,b,c)		elog(a,b,c)
#define CACHE3_elog(a,b,c,d)		elog(a,b,c,d)
#define CACHE4_elog(a,b,c,d,e)		elog(a,b,c,d,e)
#define CACHE5_elog(a,b,c,d,e,f)	elog(a,b,c,d,e,f)
#define CACHE6_elog(a,b,c,d,e,f,g)	elog(a,b,c,d,e,f,g)
#else
#define CACHE1_elog(a,b)
#define CACHE2_elog(a,b,c)
#define CACHE3_elog(a,b,c,d)
#define CACHE4_elog(a,b,c,d,e)
#define CACHE5_elog(a,b,c,d,e,f)
#define CACHE6_elog(a,b,c,d,e,f,g)
#endif

CatCache	*Caches = NULL;
GlobalMemory	CacheCxt;























