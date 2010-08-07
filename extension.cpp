/**
* vim: set ts=4 :
* =============================================================================
* SourceMod Query Cache Extension
* Ported to OB and made cross-platform by Afronanny
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

#define REQ_PACKET "\xff\xff\xff\xff\x54\x53\x6f\x75\x72\x63\x65\x20\x45\x6e\x67\x69\x6e\x65\x20\x51\x75\x65\x72\x79\x00"
#define CONNECT_PACKET "\xff\xff\xff\xff\x71\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x00"

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <dlfcn.h>
#include <sys/socket.h>
#endif


#include <bitbuf.h>
#include <iserver.h>
#include <icommandline.h>
#include <convar.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <Steamworks.h>

#include <ISDKTools.h>

using namespace std;
using std::string;

#include "extension.h"
#define S_STRLEN 256

/* //If a MM:S port is done - this should be used for game folder name
// Windows path seperator
#ifdef _WIN32
#define PATH_SEP "\\"
#elif defined _LINUX
#define PATH_SEP "/"
#endif
*/

#define DEFAULT_GAME_VERSION "1.0.9.1"
#define SOCKET_ERROR            (-1)


// Default protocol version
#define DEFAULT_PROTO_VERSION 15

typedef HSteamPipe (*GetPipeFn)();
typedef HSteamUser (*GetUserFn)();
typedef void * (*GetHandleFn)(HSteamUser, HSteamPipe);
typedef uint64 (*GetCSteamIDFn)(void *);

GetPipeFn g_GameServerSteamPipe;
GetUserFn g_GameServerSteamUser;
GetHandleFn g_GameServerHandle;
GetCSteamIDFn g_GameServerSteamID;


/**
* @file extension.cpp
* @brief Implement extension code here.
*/

QueryCache g_QueryCache;		/**< Global singleton for extension's main interface */


SMEXT_LINK(&g_QueryCache);

IServer *g_pServer = NULL;
ISDKTools *g_pSDKTools = NULL;

bool g_recvfrom_hooked = false;
int (*g_real_recvfrom_ptr) (int , char *, int , int , struct sockaddr *, int *);

time_t g_a2s_time = 0;
char g_replyStore[1024];
bf_write g_replyPacket(g_replyStore, 1024);

char g_gameDir[S_STRLEN];
char g_gameDesc[S_STRLEN];
char g_gameVersion[S_STRLEN];
int g_appID;
int g_maxClients;
int g_protoVersion;
uint64 g_SteamId;
int g_iNumPackets;
bool QueryCache::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	#ifdef _WIN32
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		strncpy(error, "Could not init Winsock", maxlength);
		return false;
	}
	#endif
	return true;
}

bool QueryCache::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	// Get ICvar (code from Left4Downtown)
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);	
	/*ConVar_Register(0, this);*/
	
	// TODO: Error handling?
	return true;
}

void QueryCache::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(SDKTOOLS, g_pSDKTools);	
	g_pServer = g_pSDKTools->GetIServer();	
	BuildStaticReplyInfo();
	g_protoVersion = DEFAULT_PROTO_VERSION;
g_maxClients = g_pServer->GetMaxClients();	
	EnableReceiveHook();	
	
}

void QueryCache::SDK_OnUnload()
{
	DisableReceiveHook();

#ifdef _WIN32
	WSACleanup();
#endif
}

bool QueryCache::RegisterConCommandBase(ConCommandBase *pVar) 
{
	return META_REGCVAR(pVar); 
}

int RecvFromHook(int s, char *buf, int len, int flags, struct sockaddr *from, int *fromlen)
{	
	int ret = g_real_recvfrom_ptr(s, buf, len, flags, from, fromlen);
	if(ret > 5)
	{
		/* Connect challenge */
		if (strcmp(buf, CONNECT_PACKET) == 0)
		{
			if (time(NULL) - g_a2s_time <= 5)
			{
				if (g_iNumPackets <= 5)
				{
					g_iNumPackets++;
					return ret;
				}
			} else {
				g_iNumPackets = 0;
				return ret;
			}
#ifdef _WIN32
			flags = WSAETIMEDOUT;
#elif defined _LINUX
			flags = ETIMEDOUT;
#endif
			return SOCKET_ERROR;
		}

		/* A2S_INFO */
		if(strcmp(buf, REQ_PACKET) == 0)
		{			
			if (time(NULL) - g_a2s_time > 5)
			{
				BuildReplyInfo();						
			}
			sendto(s, (const char *)g_replyPacket.GetData(), g_replyPacket.GetNumBytesWritten(), 0, from, *fromlen);
#ifdef _WIN32
			flags = WSAETIMEDOUT;
#elif defined _LINUX
			flags = ETIMEDOUT;
#endif
			return SOCKET_ERROR;
		}
		
	}
	return ret;
}

void DisableReceiveHook()
{	
	if(g_recvfrom_hooked)
	{
		g_pVCR->Hook_recvfrom = g_real_recvfrom_ptr;
		g_recvfrom_hooked = false;		
	}	
}

void EnableReceiveHook()
{
	if(!g_recvfrom_hooked)
	{
		g_real_recvfrom_ptr = g_pVCR->Hook_recvfrom;
		g_pVCR->Hook_recvfrom = &RecvFromHook;
		g_recvfrom_hooked = true;		
	}	
}

void BuildStaticReplyInfo()
{
	/* //Use this method in a MM:S port, using SM for now
	engine->GetGameDir(&g_gameDir[0], S_STRLEN);
	
	// Strip everything except the
	// actual game dir
	string temp(g_gameDir);
	int pos = temp.find_last_of(PATH_SEP);	
	strncpy(&g_gameDir[0], 
	(temp.substr(pos + 1, temp.size() - pos)).c_str(), S_STRLEN);	
	*/

	strncpy(g_gameDir, g_pSM->GetGameFolderName(), S_STRLEN);
	
	if (strcmp(g_gameDir, "empires") == 0)
	{
		strcpy(&g_gameDesc[0], "Empires v2.24d");
	}
	else
	{
		strcpy(&g_gameDesc[0], gamedll->GetGameDescription());		
	}

	g_SteamId = GetGSSteamId();

	g_appID = engine->GetAppID();
	strncpy(&g_gameVersion[0],VersionString().c_str(), S_STRLEN);
}

void BuildReplyInfo()
{
	g_replyPacket.Reset();	

	int passByte = 00;
	const char *pGamePass = g_pServer->GetPassword();

	if(pGamePass)
	{
		passByte = 01;
	}

	g_replyPacket.WriteLong(-1);
	g_replyPacket.WriteByte(73);
	g_replyPacket.WriteByte(DEFAULT_PROTO_VERSION);
	g_replyPacket.WriteString(g_pServer->GetName());
	g_replyPacket.WriteString(g_pServer->GetMapName());
	g_replyPacket.WriteString(g_gameDir);
	g_replyPacket.WriteString(g_gameDesc);
	g_replyPacket.WriteShort(g_appID);
	g_replyPacket.WriteByte(g_pServer->GetNumClients());
	g_replyPacket.WriteByte(g_maxClients);
	g_replyPacket.WriteByte(g_pServer->GetNumFakeClients());
	g_replyPacket.WriteByte(100);
	#ifdef _WIN32
	g_replyPacket.WriteByte(119);
	#elif defined _LINUX
	g_replyPacket.WriteByte(108);
	#endif
	g_replyPacket.WriteByte(passByte);
	g_replyPacket.WriteByte(01);
	g_replyPacket.WriteString(VersionString().c_str());
	g_replyPacket.WriteByte(176);
	g_replyPacket.WriteShort(g_pServer->GetUDPPort());
	g_replyPacket.WriteLongLong(g_SteamId);

	//TODO: Get the server's actual tags, hardcoding tags is BAD
	g_replyPacket.WriteString("Tags,Tags,MORETAGS");
	g_a2s_time = time(NULL);
}

uint64 GetGSSteamId() {
#ifdef _LINUX	
	void *steamapihandle;
	void *steamclienthandle;
	
	steamapihandle = dlopen("libsteam_api.so", RTLD_LAZY);
	if (steamapihandle == NULL)
	{
		g_pSM->LogMessage(myself, dlerror());
		return 0;
	} else
	g_GameServerSteamUser = (GetUserFn)dlsym(steamapihandle, "SteamGameServer_GetHSteamUser");
	g_GameServerSteamPipe = (GetPipeFn)dlsym(steamapihandle, "SteamGameServer_GetHSteamPipe");
	dlclose(steamapihandle);
	
	
	steamclienthandle = dlopen("steamclient.so", RTLD_LAZY);
	if (steamclienthandle == NULL)
	{
		g_pSM->LogMessage(myself, dlerror());
		return 0;
	} else {
		g_GameServerHandle = (GetHandleFn)dlsym(steamclienthandle, "Steam_GetGSHandle");
		g_GameServerSteamID = (GetCSteamIDFn)dlsym(steamclienthandle, "Steam_GSGetSteamID");
		dlclose(steamclienthandle);
	}
#elif defined _WIN32
	g_GameServerSteamUser = (GetUserFn)GetProcAddress(GetModuleHandleA("steam_api.dll"), "SteamGameServer_GetHSteamUser");
	g_GameServerSteamPipe = (GetPipeFn)GetProcAddress(GetModuleHandleA("steam_api.dll"), "SteamGameServer_GetHSteamPipe");
	g_GameServerHandle = (GetHandleFn)GetProcAddress(GetModuleHandleA("steamclient.dll"), "Steam_GetGSHandle");
	g_GameServerSteamID = (GetCSteamIDFn)GetProcAddress(GetModuleHandleA("steamclient.dll"), "Steam_GSGetSteamID");
#endif
	CSteamID steamID = CSteamID(g_GameServerSteamID(g_GameServerHandle(g_GameServerSteamUser(), g_GameServerSteamPipe())));	
	return steamID.ConvertToUint64();
	
}
	
string VersionString() {

	FILE *pFile;
	char buff[512];
	g_pSM->BuildPath(Path_Game, buff, 512, "steam.inf");

	char line[128];
	string strline;
	pFile = fopen(buff, "r");
	if (pFile == NULL)
	{
		g_pSM->LogMessage(myself, "Error opening steam.inf");
		return NULL;
	}
	fgets(line, 128, pFile);
	strline = string(line);
	string version = strline.substr(13);
	fclose(pFile);
	return version;
}




