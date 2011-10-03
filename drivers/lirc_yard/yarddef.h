/***************************************************************************
 *   Copyright (C) 2009 by M. Feser   *
 *      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef _YARDDEF_H__
#define _YARDDEF_H__


/*** DEFINES ***************************************************************/
// Yard multi byte commands
#define YCMD_SETTIME		0x01
#define YCMD_SETWAKEUPTIME	0x02
#define YCMD_SETUPIRTRANS	0x04
#define YCMD_SETIRCMD		0x05
#define YCMD_SENDIR			0x06
#define YCMD_READI2C		0x07
#define YCMD_WRITEI2C		0x08

// Yard single byte commands
#define YCMD_GETTIME		0x39
#define YCMD_GETWAKEUPTIME	0x3A 
#define YCMD_GETBOOTREASON	0x3B 
#define YCMD_STARTIRSCAN	0x3C 
#define YCMD_STOPIRSCAN		0x3D 
#define YCMD_GETFWVERSION	0x3E 

// Yard responses
#define YRC_ERROR			0x00
#define YRC_IRCODE			0x03
#define YRC_READI2C			0x07
#define YRC_GETTIME			0x39
#define YRC_GETWAKEUPTIME	0x3A
#define YRC_GETBOOTREASON	0x3B
#define YRC_STARTIRSCAN		0x3C
#define YRC_STOPIRSCAN		0x2C
#define YRC_READUSERPORT 	0x3D
#define YRC_GETFWVERSION	0x3E

// KeyLcd single byte commands
#define YICMD_SETTIME		0x01
#define YICMD_RESETPIC		0x02
#define YICMD_GETBOOTREASON	0x03
#define YICMD_READADVAL		0x10
#define YICMD_LCDRLDCHARSET	0x20
#define YICMD_LCDPRINTCHAR	0x21
#define YICMD_LCDCLEAR		0x22

// KeyLcd multi byte commands
#define YICMD_WRITEEEPROM	0x80
#define YICMD_READEEPROM	0x81
#define YICMD_SETBACKLMODE	0x90
#define YICMD_SETBACKLDUTY	0x91
#define YICMD_SETBACKLUPDT	0x92
#define YICMD_LCDPRINTSTR	0xA0
#define YICMD_LCDGOTOXY		0xA1
#define YICMD_LCDFILLCHAR	0xA2
#define YICMD_LCDWRITECGRAM	0xA3
#define YICMD_LCDWRITECTRL	0xA4
#define YICMD_LCDWRITERAW	0xA5
#define YICMD_LCDWRITERAWEX	0xA6


/*** TYPEDEFINITIONS *******************************************************/
typedef unsigned char BYTE;

typedef enum 
{
	YBR_UNKNOWN	= 0,
 YBR_RETURN	= 1,
 YBR_TIMER	= 2,
 YBR_REMOTE	= 3,
 YBR_TIMER2	= 4
} YARD_BOOTREASON;
	
typedef union
{
	struct
	{
		BYTE bCode;
		BYTE bLen;
		BYTE abData[32];
	} tMB;
	
	struct
	{
		BYTE bCode;
		BYTE bChSum;
	} tSB;
	
	struct
	{
		BYTE bCode;
		BYTE bLen;
		BYTE bI2cAddr;
		BYTE bI2cCode;
		BYTE abI2cData[28];
	} tIMB;
	
	struct
	{
		BYTE bCode;
		BYTE bLen;
		BYTE bI2cAddr;
		BYTE bI2cCode;
		BYTE bChSum;
	} tISB;
} YARD_CMD;

typedef struct 
{
	BYTE bCode;
	BYTE bLen;
	BYTE bPrcol;
	BYTE abIrData[6];
	BYTE bChsum;
} YARD_IRCODE;
	
typedef union
{
	struct 
	{
		BYTE bCode;
		BYTE bLen;
		BYTE abData[30];
	} tRaw;

	struct 
	{
		BYTE bCode;
		BYTE bLen;
		BYTE bVers;
		BYTE bChsum;
	} tVersion;

	struct
	{
		BYTE bCode;
		BYTE bLen;
		BYTE bReas;
		BYTE bChsum;
	} tPower;

	struct 
	{
		BYTE bCode;
		BYTE bLen;
		BYTE bState;
		BYTE bChsum;
	} tUserPort;

	struct 
	{
		BYTE bCode;
		BYTE bLen;
		BYTE abTime[4];    // Seconds since 1.1.2005
		BYTE bChsum;
	} tTime;

	struct 
	{
		BYTE bCode;
		BYTE bLen;
		BYTE bStatus;
		BYTE bChsum;
	} tError;
} YARD_RES;


#endif