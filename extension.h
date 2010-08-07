/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Query Cache Extension
 * Copyright (C) 2010 Recon. All rights reserved.
 *
 * This program contains code from the following programs (which are licensed
 *                                                         under the GPL):
 *    1. SrcdsQueryCache 
 *       (http://www.wantedgov.it/page/62-srcds-query-cache/) 
 *        by Beretta Claudio
 *    2. SRCDS Denial of Service Protect
 *       (http://forums.alliedmods.net/showthread.php?t=95312) by raydan
 *    3. Left 4 Downtown (http://forums.alliedmods.net/showthread.php?t=91132)
 *       by Downtown1
 *
 * ============================================================================= 
 * A huge thank you goes to CrimsonGT for all his help with this extension. 
 *
 * Thanks also to Luigi Auriemma (http://aluigi.org/),
 *  whose Winsock code from UDP proxy/pipe (http://aluigi.org/mytoolz.htm)
 *  helped me learn the Winsock API.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *****************************************************************************
 * This program is a SourceMod extension, and is effected by the following
 * exceptions from SourceMod's authors:
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

/**
 * @file extension.h
 * @brief QueryCache extension header.
 */

#include "smsdk_ext.h"

// Function prototypes
string VersionString();
void BuildStaticReplyInfo();
void BuildReplyInfo();
uint64 GetGSSteamId();

int RecvFromHook(int s, char *buf, int len, int flags, 
struct sockaddr *from, int *fromlen);
void EnableReceiveHook();
void DisableReceiveHook();

/**
 * @brief QueryCache extension class. 
 */
class QueryCache : public SDKExtension, public IConCommandBaseAccessor
{
public:

	/**
	 * @brief This is called after the initial loading sequence has been processed.
	 *
	 * @param error		Error message buffer.
	 * @param maxlength	Size of error message buffer.
	 * @param late		Whether or not the module was loaded after map load.
	 * @return			True to succeed loading, false to fail.
	 */
	virtual bool SDK_OnLoad(char *error, size_t maxlength, bool late);

	/**
	 * @brief This is called right before the extension is unloaded.
	 */
	virtual void SDK_OnUnload();

	/**
	 * @brief This is called once all known extensions have been loaded.
	 * Note: It is is a good idea to add natives here, if any are provided.
	 */
	virtual void SDK_OnAllLoaded();

	/**
	 * @brief Called when the pause state is changed.
	 */
	//virtual void SDK_OnPauseChange(bool paused);

	/**
	 * @brief this is called when Core wants to know if your extension is working.
	 *
	 * @param error		Error message buffer.
	 * @param maxlength	Size of error message buffer.
	 * @return			True if working, false otherwise.
	 */
	//virtual bool QueryRunning(char *error, size_t maxlength);

    // For convars
    bool RegisterConCommandBase(ConCommandBase *pVar);

public:
#if defined SMEXT_CONF_METAMOD
	/**
	 * @brief Called when Metamod is attached, before the extension version is called.
	 *
	 * @param error			Error buffer.
	 * @param maxlength		Maximum size of error buffer.
	 * @param late			Whether or not Metamod considers this a late load.
	 * @return				True to succeed, false to fail.
	 */
	virtual bool SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlength, bool late);

	/**
	 * @brief Called when Metamod is detaching, after the extension version is called.
	 * NOTE: By default this is blocked unless sent from SourceMod.
	 *
	 * @param error			Error buffer.
	 * @param maxlength		Maximum size of error buffer.
	 * @return				True to succeed, false to fail.
	 */
	//virtual bool SDK_OnMetamodUnload(char *error, size_t maxlength);

	/**
	 * @brief Called when Metamod's pause state is changing.
	 * NOTE: By default this is blocked unless sent from SourceMod.
	 *
	 * @param paused		Pause state being set.
	 * @param error			Error buffer.
	 * @param maxlength		Maximum size of error buffer.
	 * @return				True to succeed, false to fail.
	 */
	//virtual bool SDK_OnMetamodPauseChange(bool paused, char *error, size_t maxlength);
#endif
};

//Had trouble compiling with itoa on linux, just copied the function from windows headers
#ifndef itoa
char* itoa(int value, char* str, int radix) {      
	static char dig[] =	"0123456789" "abcdefghijklmnopqrstuvwxyz";
	int n = 0, neg = 0;

	unsigned int v;
	char* p, *q;
	char c;
	if (radix == 10 && value < 0) {
		value = -value;
		neg = 1;
	}
	v = value;
	do {
		str[n++] = dig[v%radix];
		v /= radix;
	} while (v);
	if (neg)
		str[n++] = '-';
	str[n] = '\0';
	for (p = str, q = p + (n-1); p < q; ++p, --q)
		c = *p, *p = *q, *q = c;
	return str;
}
#endif


#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
