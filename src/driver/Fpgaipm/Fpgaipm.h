/*
 * Fpgaipm.h
 *
 *  Created on: 18 apr 2018
 *      Author: Alessandro Monaco
 */

#ifndef FPGAIPM_H_
#define FPGAIPM_H_

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

#define CORE_ID_MASK 0b1111111
#define OPCODE_MASK 0b11111
#define CTRL_WORD_MASK 0b1111

#define GPIO_ADDR_BASE_ADDR 0x0
#define GPIO_DATA_BASE_ADDR 0x0

#define BUFFER_SIZE 64

#define CTRL_INTERRUPT_EN 0b1000
#define CTRL_ACK 0b0100
#define CTRL_START_TRANSACTION 0b0010
#define CTRL_START_WRITE 0b0001

#define CORE_ID_OFFSET 0
#define CTRL_WORD_OFFSET 7
#define OPCODE_OFFSET 11

typedef uint16_t FPGA_DATA;
typedef uint8_t CORE_ID;
typedef uint8_t OPCODE;
typedef uint8_t CTRL_WORD;

class Fpgaipm {

private:
	static sem_t semaphore;
	static bool initialized;
	static CORE_ID currentCore;
	static OPCODE currentOpcode;
	static CTRL_WORD currentCtrlWord;

	int writeRow0(CORE_ID coreId, OPCODE opcode, CTRL_WORD ctrlWord);
	int writeData(uint8_t address, FPGA_DATA data);
	int readData(uint8_t address, FPGA_DATA *data);
	bool checkCoreId(CORE_ID coreId);

public:
	Fpgaipm();

	int init();
	int fpga_openTransaction(CORE_ID coreId, OPCODE opcode, bool interrupt);
	int fpga_closeTransaction(CORE_ID coreId);
	int fpga_writeData(CORE_ID coreId, uint8_t n, FPGA_DATA *data);
	int fpga_readData(CORE_ID coreId, uint8_t n, FPGA_DATA *data);
	int fpga_checkInterrupt(CORE_ID coreId);

};

#endif /* FPGAIPM_H_ */
