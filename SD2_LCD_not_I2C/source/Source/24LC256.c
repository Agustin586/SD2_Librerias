/*
 * 24LC256.c
 *
 *  Created on: 7 jul. 2024
 *      Author: Zuliani, Agustin
 */

#include "Include/24LC256.h"

#define USE_FREERTOS

#define I2C_EEPROM	0xA0

typedef struct {
	uint8_t direccion;
	uint8_t dato;
} EEPROM_data_t;

EEPROM_data_t Datos;

#if (!defined(USE_FREERTOS) && !defined(USE_NOT_FREERTOS))
#error "Se debe definir USE_FREERTOS o USE_NOT_FREERTOS"
#endif

#if defined(USE_FREERTOS)
#include "FreeRTOS.h"
#include "fsl_i2c_freertos.h"
#include "Include/i2cRtos.h"

#define i2c_init	i2cRtos_init
#define i2c_xtransfer(master_transfer)	i2cRtos_xtransfer(master_transfer)

i2c_master_transfer_t masterXfer;
#elif defined(USE_NOT_FREERTOS)
#include "fsl_i2c.h"

#define I2C_BASE I2C0
#define I2C_CLK_SRC I2C0_CLK_SRC
#define I2C_BAUDRATE 100000U  // 100 kHz


#endif

////////////////////////////////////////////////////////////////////////////////

extern void EEPROM_init(void) {
	i2c_init();

	return;
}

////////////////////////////////////////////////////////////////////////////////

extern void Eeprom_write(uint16_t dir, uint8_t dato) {
	masterXfer.slaveAddress = I2C_EEPROM;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = dir;
	masterXfer.subaddressSize = 2;
	masterXfer.data = &dato;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;

#if defined(USE_FREERTOS)
	i2cRtos_xtransfer(masterXfer);
#elif defined(USE_NOT_FREERTOS)
	I2C_MasterTransferBlocking(I2C_BASE, &masterXfer);
#endif

	return;
}

////////////////////////////////////////////////////////////////////////////////

extern uint8_t Eeprom_read(uint16_t dir) {
	uint8_t rx;

	masterXfer.slaveAddress = I2C_EEPROM;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = dir;
	masterXfer.subaddressSize = 2;
	masterXfer.data = &rx;
	masterXfer.dataSize = 1;
	masterXfer.flags = kI2C_TransferDefaultFlag;

#if defined(USE_FREERTOS)
	i2cRtos_xtransfer(masterXfer);
#elif defined(USE_NOT_FREERTOS)
	I2C_MasterTransferBlocking(I2C_BASE, &masterXfer);
#endif

	return rx;
}

////////////////////////////////////////////////////////////////////////////////

extern _Bool User_Scan(unsigned char num_user) {
	_Bool user_exist = 0;
	unsigned char pos = 0;

	//Primero debe leer la cantidad de usuario
	cant_user = M_Read(0, 2);

	//Segundo debe verificar si el numero de usuario ya existe
	for (char i = 0; i < cant_user; i++) {
		if (i != 0)
			pos = pos + 15;
		else
			pos = 4;
		if (num_user == (M_Read(pos, 2)))
			user_exist = 1;

		if (user_exist)
			break;
	}

	return user_exist;      //Devuelve un 1 si existe, sino un 0
}

////////////////////////////////////////////////////////////////////////////////

extern unsigned int New_User_Pos(void) {
	unsigned char cant_user = 0;
	unsigned int Posicion = 0;

	cant_user = Read_User(0);

	if (cant_user != 0)
		Posicion = (cant_user * 15) + 4;
	else
		Posicion = 4;

	return Posicion;
}

////////////////////////////////////////////////////////////////////////////////

unsigned char Num_User(long long hash) {
	unsigned char pos_h = 0, num_user = 0;

	if (Read_Hash_ex(hash) == 1) {
		for (char i = 0; i < cant_user; i++) {
			if (i == 0)
				pos_h = 7;
			else
				pos_h = pos_h + 15;

			if ((hash) == (M_Read(pos_h, 10))) {
				num_user = (Read_User(pos_h - 3));
				break;
			}
		}
		return num_user;
	} else
		return 0;

}

////////////////////////////////////////////////////////////////////////////////

unsigned long Read_Hash(unsigned char user) {
	unsigned long M_Hash = 0;
	unsigned char hash = 0, pos_hash = 0;

	if (User_Scan(user) == 0)
		return 0;

	if (User_Scan(user) == 1) {
		//Con esto podemos obtener la posicion del hash
		pos_hash = Pos_Num_User(user) + 4;

		if (pos_hash != 0) {
			//Guarda el valor del hash
			M_Hash = M_Read(pos_hash, 10);
		} else
			M_Hash = 0;
	}

	return M_Hash;
}

////////////////////////////////////////////////////////////////////////////////

char Pos_Num_User(unsigned char num_user) {
	unsigned char cant_user = 0, pos = 0, pos_usuario = 0;

	//Primero escanea si el usuario existe
	if (User_Scan(num_user) == 1) {
		//Lee la cantidad de usuarios
		cant_user = M_Read(0, 2);

		//Usa esa cantidad para buscar la posicion en la se encuentra el numero de usuario
		for (char i = 0; i < cant_user; i++) {
			if (i == 0)
				pos = 3;
			else
				pos = pos + 15;
			if (num_user == M_Read(pos, 2)) {
				pos_usuario = pos;
				break;
			}
		}
	}

	if (User_Scan(num_user) == 0)
		return 0;

	return pos_usuario;
}

////////////////////////////////////////////////////////////////////////////////

_Bool Read_Hash_ex(long long cod_rfid) {
	_Bool OK_HASH = 0;
	unsigned char pos_hash = 0;

	cant_user = Read_User(0);

	//Con esto podemos obtener la posicion del hash
	for (char i = 0; i < cant_user; i++) {
		if (i == 0)
			pos_hash = 7;
		else
			pos_hash = pos_hash + 15;

		if (cod_rfid == M_Read(pos_hash, 10)) {
			OK_HASH = 1;
			break;
		} else
			OK_HASH = 0;
	}

	return OK_HASH;
}

////////////////////////////////////////////////////////////////////////////////

void Sum_Cant_Usuarios(void) {
	unsigned char cant_usuario = 0;

	cant_usuario = Read_User(0);

	cant_usuario = cant_usuario + 1;

	Write_User(0, cant_usuario);

	return;
}

////////////////////////////////////////////////////////////////////////////////

void Write_User(unsigned int dir, unsigned char usuario) {
	char buffer[5];
	unsigned char MD_1 = 0, MD_2 = 0;

	//Envio de informacion:

	//Transforma la direccion a 2 bytes
	MD_1 = (dir & 0xff00) >> 8; // Parte alta
	MD_2 = dir & 0x00ff;        // Parte baja

	i2c_start();                // Comienza la comunicacion

	i2c_write(0b10100000);      // Modo escritura

	if (dir > 0) {
		i2c_write(MD_1);     // Parte alta de la direccion de la memoria interna
		i2c_write(MD_2);            // Parte baja
	} else {
		i2c_write(0);
		i2c_write(0);
	}

	sprintf(buffer, "%02d", usuario);
	i2c_array(buffer);

	i2c_stop();

	return;
}

unsigned char Read_User(unsigned int dir) {
	unsigned char MD_1 = 0, MD_2 = 0;
	unsigned char MR_Dato = 0;

	//GIEH = 0;
	GIEL = 0;

	Read_Eeprom = 1;

	//Envio de informacion para leer:

	//Transforma la direccion a 2 bytes
	MD_1 = (dir & 0xff00) >> 8; // Parte alta
	MD_2 = dir & 0x00ff;        // Parte baja

	i2c_start();                // Comienza la comunicacion
	i2c_write(0b10100000);      // Modo escritura

	i2c_write(MD_1);         // Parte alta de la direccion de la memoria interna
	i2c_write(MD_2);            // Parte baja

	i2c_restart();
	i2c_write(0b10100001);      // Cambia a modo lectura de datos

	//Lectura de datos:
	MR_Dato = (i2c_read() - 48) * 10;
	i2c_ack();
	MR_Dato = MR_Dato + (i2c_read() - 48);
	i2c_nack();

	//Fin de la comunicacion
	i2c_stop();

	Read_Eeprom = 0;

	//GIEH = 1;
	GIEL = 1;

	return MR_Dato;
}
