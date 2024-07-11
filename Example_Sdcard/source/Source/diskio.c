/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "Include/ff.h"			/* Obtains integer types */
#include "Include/diskio.h"		/* Declarations of disk functions */
#include "Include/sd.h"
#include "fsl_debug_console.h"

/* Definitions of physical drive number for each drive */
#define DEV_RAM		0	/* Example: Map Ramdisk to physical drive 0 */
#define DEV_MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define DEV_USB		2	/* Example: Map USB MSD to physical drive 2 */

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(BYTE pdrv /* Physical drive nmuber to identify the drive */
) {
	DSTATUS stat = STA_NOINIT;
	int result;

	if (pdrv)
		return STA_NOINIT; /* Supports only single drive */
	return stat;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(BYTE pdrv /* Physical drive nmuber to identify the drive */
) {
	DSTATUS stat;
	int result;

	if (pdrv != 0)
		return STA_NOINIT; /* Supports only single drive */
//	if (Stat & STA_NODISK)
//		return Stat; /* No card in the socket */

	/*< Incializacion de la tarjeta sd >*/
	result = sd_init();

	if (result == SD_OK)
		PRINTF("Sd card init success\r\n");
	else if (result == SD_GENERAL_ERROR)
		PRINTF("Error: during init sd card\r\n");

	return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(BYTE pdrv, /* Physical drive nmuber to identify the drive */
BYTE *buff, /* Data buffer to store read data */
LBA_t sector, /* Start sector in LBA */
UINT count /* Number of sectors to read */
) {
	DRESULT res;
	int result;

	if (pdrv || !count)
		return RES_PARERR;
//	if (Stat & STA_NOINIT)
//		return RES_NOTRDY;

	if (count == 1) { /* Lectura de un bloque simple */
		result = sd_read_single_block(sector, buff);
		count = 0;
	} else { /* Lectura de multpiles bloques */
		result = sd_read_multiple_block(sector, buff, &count);
	}

	return count ? RES_ERROR : RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if !FF_FS_READONLY

DRESULT disk_write(BYTE pdrv, /* Physical drive nmuber to identify the drive */
const BYTE *buff, /* Data to be written */
LBA_t sector, /* Start sector in LBA */
UINT count /* Number of sectors to write */
) {
	DRESULT res = STA_NOINIT;
	int result;

	if (pdrv || !count)
		return RES_PARERR;
//	if (res & STA_NOINIT)
//		return RES_NOTRDY;
//	if (res & STA_PROTECT)
//		return RES_WRPRT;

	if (count == 1) {
		result = sd_write_single_block(sector, buff);
		count = 0;
	} else {
		result = sd_multiple_block(sector, buff, &count);
	}

	return count ? RES_ERROR : RES_OK;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL

DRESULT disk_ioctl(BYTE pdrv, /* Physical drive nmuber (0..) */
BYTE cmd, /* Control code */
void *buff /* Buffer to send/receive control data */
) {
	DRESULT res;
	int result;

	switch (pdrv) {
	case DEV_RAM:

		// Process of the command for the RAM drive

		return res;

	case DEV_MMC:

		// Process of the command for the MMC/SD card

		return res;

	case DEV_USB:

		// Process of the command the USB drive

		return res;
	}

	return RES_PARERR;
}

#endif
