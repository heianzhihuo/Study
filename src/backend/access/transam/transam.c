/*-------------------------------------------------------------------------
 *
 * transam.c--
 *    postgres transaction log/time interface routines
 *
 * Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *    $Header: /usr/local/cvsroot/postgres95/src/backend/access/transam/transam.c,v 1.8 1996/11/27 15:15:54 vadim Exp $
 *
 * NOTES
 *    This file contains the high level access-method interface to the
 *    transaction system.
 *	
 *-------------------------------------------------------------------------
 */

#include <postgres.h>

#include <access/transam.h>
#include <access/xact.h>
#include <access/heapam.h>
#include <utils/mcxt.h>
#include <catalog/catname.h>
#include <storage/spin.h>

/* ----------------
 *    global variables holding pointers to relations used
 *    by the transaction system.  These are initialized by
 *    InitializeTransactionLog().
 * ----------------
 */

Relation LogRelation	  = (Relation) NULL;
Relation TimeRelation	  = (Relation) NULL;
Relation VariableRelation = (Relation) NULL;

/* ----------------
 *    	global variables holding cached transaction id's and statuses.
 * ----------------
 */
TransactionId	cachedGetCommitTimeXid;
AbsoluteTime	cachedGetCommitTime;
TransactionId	cachedTestXid;
XidStatus	cachedTestXidStatus;

/* ----------------
 *	transaction system constants
 * ----------------
 */
/* ----------------------------------------------------------------
 *	transaction system constants
 *
 *	read the comments for GetNewTransactionId in order to
 *      understand the initial values for AmiTransactionId and
 *      FirstTransactionId. -cim 3/23/90
 * ----------------------------------------------------------------
 */
TransactionId NullTransactionId = (TransactionId) 0;

TransactionId AmiTransactionId = (TransactionId) 512;

TransactionId FirstTransactionId = (TransactionId) 514;

/* ----------------
 *	transaction recovery state variables
 *
 *	When the transaction system is initialized, we may
 *	need to do recovery checking.  This decision is decided
 *	by the postmaster or the user by supplying the backend
 *	with a special flag.  In general, we want to do recovery
 *	checking whenever we are running without a postmaster
 *	or when the number of backends running under the postmaster
 *	goes from zero to one. -cim 3/21/90
 * ----------------
 */
int RecoveryCheckingEnableState = 0;

/* ------------------
 *	spinlock for oid generation
 * -----------------
 */
extern int OidGenLockId;

/* ----------------
 *	globals that must be reset at abort
 * ----------------
 */
extern bool	BuildingBtree;


/* ----------------
 *	recovery checking accessors
 * ----------------
 */
int
RecoveryCheckingEnabled(void)
{    
    return RecoveryCheckingEnableState;
}

void
SetRecoveryCheckingEnabled(bool state)
{    
    RecoveryCheckingEnableState = (state == true);
}
