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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_CONFIG_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_CONFIG_H_

/**
 * @file smsdk_config.h
 * @brief Contains macros for configuring basic extension information.
 */

/* Basic information exposed publicly */
#define SMEXT_CONF_NAME			"Server Query Cache"
#define SMEXT_CONF_DESCRIPTION		"Responds to A2S_INFO queries"
#define SMEXT_CONF_VERSION		"1.7.1.0"
#define SMEXT_CONF_AUTHOR		"Recon + Afronanny"
#define SMEXT_CONF_URL			"http://microbits.info/"
#define SMEXT_CONF_LOGTAG		"QCACHE"
#define SMEXT_CONF_LICENSE		"GPL"
#define SMEXT_CONF_DATESTRING	__DATE__

/** 
 * @brief Exposes plugin's main interface.
 */
#define SMEXT_LINK(name) SDKExtension *g_pExtensionIface = name;

/**
 * @brief Sets whether or not this plugin required Metamod.
 * NOTE: Uncomment to enable, comment to disable.
 */
#define SMEXT_CONF_METAMOD		

/** Enable interfaces you want to use here by uncommenting lines */
//#define SMEXT_ENABLE_FORWARDSYS
//#define SMEXT_ENABLE_HANDLESYS
//#define SMEXT_ENABLE_PLAYERHELPERS
//#define SMEXT_ENABLE_DBMANAGER
//#define SMEXT_ENABLE_GAMECONF
//#define SMEXT_ENABLE_MEMUTILS
//#define SMEXT_ENABLE_GAMEHELPERS
//#define SMEXT_ENABLE_TIMERSYS
//#define SMEXT_ENABLE_THREADER
//#define SMEXT_ENABLE_LIBSYS
//#define SMEXT_ENABLE_MENUS
//#define SMEXT_ENABLE_ADTFACTORY
//#define SMEXT_ENABLE_PLUGINSYS
//#define SMEXT_ENABLE_ADMINSYS
//#define SMEXT_ENABLE_TEXTPARSERS
//#define SMEXT_ENABLE_USERMSGS
//#define SMEXT_ENABLE_TRANSLATOR
//#define SMEXT_ENABLE_NINVOKE

#endif // _INCLUDE_SOURCEMOD_EXTENSION_CONFIG_H_
