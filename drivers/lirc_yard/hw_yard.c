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
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include "hardware.h"
#include "ir_remote.h"
#include "lircd.h"
#include "hw_yard.h"
#include "yarddef.h"


/* Defines */
#define YARDSRV_SOCK_PATH	"/tmp/yardsrv_sock"
#define SRVCMD_IRREG		0xEE
#define SRVCMD_IRUNREG		0xDD
#define SRVRES_SUCCESS		0x00
#define SRVRES_ERROR		0xFF
#define IRCODE_NUM_BYTES	6
#define IRCODE_NUM_BITS		( IRCODE_NUM_BYTES * 8 )


/* Prototypes */
static int yard_sendSrvCmd(unsigned char cmd);


/* Variables */
static struct timeval start,end,last;
static ir_code code;

struct hardware hw_yard =
{
	YARDSRV_SOCK_PATH,		/* default device */
	-1,						/* fd */
	LIRC_CAN_REC_LIRCCODE,	/* features */
	0,						/* send_mode */
	LIRC_MODE_LIRCCODE,		/* rec_mode */
 	IRCODE_NUM_BITS,		/* code_length */
	yard_init,				/* init_func */
	NULL,					/* config_func */
	yard_deinit,			/* deinit_func */
	NULL,					/* send_func */
	yard_rec,				/* rec_func */
	yard_decode,			/* decode_func */
	NULL,					/* ioctl_func */
	NULL,					/* readdata */
	"yard"
};


/* Functions */
static int yard_sendSrvCmd(unsigned char cmd)
{
	unsigned char res;
	int byteCnt;
	
	
	// Error check
	if (hw.fd < 0)
	{
		return 0;
	}
	
	// Send command to yard server
	byteCnt = write(hw.fd, &cmd, 1);
	if (byteCnt < 0)
	{
		return 0;
	}
	
	// Wait for server response
	byteCnt = read(hw.fd, &res, 1);
	if (byteCnt < 0)
	{
		return 0;
	}
	
	switch (res)
	{
		case SRVRES_SUCCESS:
		{
			return 1;
			break;
		}
		case SRVRES_ERROR:
		default:
		{
			return 0;
			break;
		}
	}
}

int yard_decode(struct ir_remote *remote,
		 ir_code *prep,ir_code *codep,ir_code *postp,
		 int *repeat_flagp,
		 lirc_t *min_remaining_gapp,
		 lirc_t *max_remaining_gapp)
{
	if(!map_code(remote, prep, codep, postp,
		0, 0, IRCODE_NUM_BITS, code, 0, 0))
	{
		return 0;
	}

	map_gap(remote, &start, &last, 0, repeat_flagp,
		min_remaining_gapp, max_remaining_gapp);

	return 1;
}

int yard_init(void)
{
	struct sockaddr_un srvAddr;
	int srvAddrLen;
	
	
	// Establish connection to YARD server
	bzero( (char *)&srvAddr, sizeof(srvAddr));
	srvAddr.sun_family = AF_UNIX;
	strcpy(srvAddr.sun_path, YARDSRV_SOCK_PATH);
	srvAddrLen = strlen(srvAddr.sun_path) + sizeof(srvAddr.sun_family);
	
	hw.fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (hw.fd < 0)
	{
		logprintf(LOG_ERR, "yard: Can't create socket !");
		return 0;
	}
	if (connect(hw.fd, (struct sockaddr *)&srvAddr, srvAddrLen) < 0)
	{
		logprintf(LOG_ERR, "yard: Can't connect to yardsrv !");
		return 0;
	}
	
	// Register IR code notification
	if ( !yard_sendSrvCmd(SRVCMD_IRREG) )
	{
		logprintf(LOG_ERR, "yard: Can't register IR code notification !");
		return 0;
	}
	
	return 1;
}

int yard_deinit(void)
{
	// Unregister IR code notification
	yard_sendSrvCmd(SRVCMD_IRUNREG);
	
	// Close socket
	close(hw.fd);
	hw.fd = -1;
	return 1;
}

char *yard_rec(struct ir_remote *remotes)
{
	YARD_IRCODE yardIrCode;
	char *m;
	int i, byteCnt;
	
	
	// Error check
	if (hw.fd < 0)
	{
		return 0;
	}
	
	last = end;
	gettimeofday(&start, NULL);
	
	// Receive IR code from YARD server
	byteCnt = read(hw.fd, (unsigned char *)&yardIrCode, sizeof(YARD_IRCODE) );
	if ( byteCnt < sizeof(YARD_IRCODE) )
	{
		logprintf(LOG_ERR, "yard: Expected %d bytes - only received %d bytes !", sizeof(YARD_IRCODE), byteCnt);
		return NULL;
	}
		
	gettimeofday(&end, NULL);

	// Extract IR code bytes
	code = 0;
	for(i = 0; i < IRCODE_NUM_BYTES; i++)
	{
		code <<= 8;
		code |= yardIrCode.abIrData[i];
	}
	LOGPRINTF(1, "Receive IR-Code: %llx", (unsigned long long)code);

	m = decode_all(remotes);
	return(m);
}
