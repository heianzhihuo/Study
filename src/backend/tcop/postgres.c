
/*-------------------------------------------------------------------------
 *
 * postgres.c--
 *    POSTGRES C Backend Interface
 *
 * Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *    $Header: /usr/local/cvsroot/postgres95/src/backend/tcop/postgres.c,v 1.28 1997/01/27 22:37:52 scrappy Exp $
 *
 * NOTES
 *    this is the "main" module of the postgres backend and
 *    hence the main module of the "traffic cop".
 *
 *-------------------------------------------------------------------------
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <setjmp.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/param.h>          /* for MAXHOSTNAMELEN on most */
#ifndef MAXHOSTNAMELEN
#include <netdb.h>              /* for MAXHOSTNAMELEN on some */
#endif
#include <errno.h>
#ifdef aix
#include <sys/select.h>
#endif /* aix */


#include "postgres.h"
#include "miscadmin.h"
#include "catalog/catname.h"
#include "access/xact.h"

#include "lib/dllist.h"

#include "parser/catalog_utils.h"
#include "parser/parse_query.h"     /* for MakeTimeRange() */
#include "commands/async.h"
#include "tcop/tcopprot.h"          /* where declarations for this file go */
#include "optimizer/planner.h"

#include "tcop/tcopprot.h"
#include "tcop/tcopdebug.h"

#include "executor/execdebug.h"
#include "executor/executor.h"
#include "nodes/relation.h"

#include "optimizer/cost.h"
#include "optimizer/planner.h"
#if 0
#include "optimizer/xfunc.h"
#endif
#include "optimizer/prep.h"
#include "nodes/plannodes.h"

#include "storage/bufmgr.h"
//#include "fmgr.h"
#include "utils/palloc.h"
#include "utils/rel.h"

#include "nodes/pg_list.h"
#include "tcop/dest.h"
#include "nodes/memnodes.h"
#include "utils/mcxt.h"
#include "tcop/pquery.h"
#include "tcop/utility.h"
#include "tcop/fastpath.h"

#include "libpq/libpq.h"
#include "libpq/pqsignal.h"
#include "rewrite/rewriteHandler.h" /* for QueryRewrite() */



/* ----------------
 *      global variables
 * ----------------
 */
static bool     DebugPrintQuery = false;
static bool     DebugPrintPlan = false;
static bool     DebugPrintParse = false;
static bool     DebugPrintRewrittenParsetree = false;
/*static bool   EnableRewrite = true; , never changes why have it*/
CommandDest whereToSendOutput;

extern int      lockingOff;
extern int      NBuffers;

int     fsyncOff = 0;

int     dontExecute = 0;
static int      ShowStats;
static bool     IsEmptyQuery = false;
