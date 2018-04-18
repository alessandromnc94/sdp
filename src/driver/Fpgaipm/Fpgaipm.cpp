/*
 * Fpgaipm.cpp
 *
 *  Created on: 18 apr 2018
 *      Author: Alessandro
 */

#include "Fpgaipm.h"

sem_t Fpgaipm::semaphore;
bool Fpgaipm::initialized = false;
CORE_ID Fpgaipm::currentCore;
CTRL_WORD Fpgaipm::currentCtrlWord;
OPCODE Fpgaipm::currentOpcode;

Fpgaipm::Fpgaipm() {
}

int Fpgaipm::init() {
	if (!initialized) {
		initialized = true;
		currentCore = 0;
		currentCtrlWord = 0;
		currentOpcode = 0;

		// TODO: configurare GPIOsssss come input_z

		return sem_init(&semaphore, 0, 1);
	}
	return -1;
}

int Fpgaipm::writeRow0(CORE_ID coreId, OPCODE opcode, CTRL_WORD ctrlWord) {
	// TODO: set GPIOs come output
	int ret = writeData(0,
			((CORE_ID_MASK & coreId) << CORE_ID_OFFSET)
					| ((OPCODE_MASK & opcode) << OPCODE_OFFSET)
					| ((CTRL_WORD_MASK & ctrlWord)
							<< CTRL_WORD_OFFSET));

	// TODO: setta GPIO come input
	return ret;
}
int Fpgaipm::writeData(uint8_t address, FPGA_DATA data) {
// TODO: inserire comando di scrittura
	return 0;
}

bool Fpgaipm::checkCoreId(CORE_ID coreId) {
	return coreId == currentCore;
}

int Fpgaipm::fpga_openTransaction(CORE_ID coreId, OPCODE opcode,
		bool interrupt) {
	int trywait = sem_trywait(&semaphore);
	if (trywait == 0) {
		/*
		 * TODO: verificare che il buffer non è lockato (GPIO interrupt alto in input) da un ipcore.
		 * se il buffer è libero, lockalo.
		 */
		currentCore = coreId;
		currentOpcode = opcode;
		currentCtrlWord = interrupt ? CTRL_INTERRUPT_EN : 0;
		writeRow0(currentCore, currentOpcode, currentCtrlWord);
	}
	return trywait;
}

int Fpgaipm::fpga_closeTransaction(CORE_ID coreId) {
	if (checkCoreId(coreId)) {
		// TODO: verificare stato POST
		int post = sem_post(&semaphore);
		CTRL_WORD ctrlWord = currentCtrlWord;
		ctrlWord &= ~CTRL_START_TRANSACTION;
		writeRow0(currentCore, (OPCODE) 0, ctrlWord);
		currentCore = 0;
		currentCtrlWord = 0;
		currentOpcode = 0;
		return post;
	}
	return -1;
}

int Fpgaipm::fpga_writeData(CORE_ID coreId, uint8_t n, FPGA_DATA *data) {
	if (checkCoreId(coreId)) {
		if (n < BUFFER_SIZE && n > 0) {
			// TODO: setta GPIOs come output (tutti)
			FPGA_DATA row0 = ((CORE_ID_MASK & coreId) << CORE_ID_OFFSET)
					| ((OPCODE_MASK & currentOpcode) << OPCODE_OFFSET)
					| ((CTRL_WORD_MASK & (currentCtrlWord | CTRL_START_WRITE))
							<< CTRL_WORD_OFFSET);
			writeData(0, row0);
			for (uint8_t i = 0; i < n; i++)
				writeData(i + 1, data[i]);

			row0 = ((CORE_ID_MASK & coreId) << CORE_ID_OFFSET)
					| ((OPCODE_MASK & currentOpcode) << OPCODE_OFFSET)
					| ((CTRL_WORD_MASK & (currentCtrlWord & ~CTRL_START_WRITE))
							<< CTRL_WORD_OFFSET);
			writeData(0, row0);
			row0 = ((CORE_ID_MASK & coreId) << CORE_ID_OFFSET)
					| ((OPCODE_MASK & currentOpcode) << OPCODE_OFFSET)
					| ((CTRL_WORD_MASK & (currentCtrlWord & ~CTRL_START_WRITE))
							<< CTRL_WORD_OFFSET);
			writeData(0, row0);
			// TODO: setta GPIO come input (tutti)
			return 0;
		}
		return -1;
	}
	return -1;
}

//int Fpgaipm::fpga_readDataVar(CORE_ID coreId, uint8_t * n, FPGA_DATA *data) {
//	if (checkCoreId(coreId)) {
//		if (n < BUFFER_SIZE && n > 0) {
//			FPGA_DATA row0 = ((CORE_ID_MASK & coreId) << CORE_ID_OFFSET)
//					| ((OPCODE_MASK & currentOpcode) << OPCODE_OFFSET)
//					| ((CTRL_WORD_MASK & (currentCtrlWord | CTRL_START_WRITE))
//							<< CTRL_WORK_OFFSET);
//			writeData(0, row0);
//			for (uint8_t i = 0; i < n; i++)
//				writeData(i + 1, data[i]);
//
//			row0 = ((CORE_ID_MASK & coreId) << CORE_ID_OFFSET)
//					| ((OPCODE_MASK & currentOpcode) << OPCODE_OFFSET)
//					| ((CTRL_WORD_MASK & (currentCtrlWord | ~CTRL_START_WRITE))
//							<< CTRL_WORK_OFFSET);
//			writeData(0, row0);
//			return 0;
//		}
//		return -1;
//	}
//	return -1;
//}

int Fpgaipm::fpga_readData(CORE_ID coreId, uint8_t n, FPGA_DATA *data) {
	if (checkCoreId(coreId)) {
		if (n < BUFFER_SIZE && n > 0) {
			FPGA_DATA row0 = ((CORE_ID_MASK & coreId) << CORE_ID_OFFSET)
					| ((OPCODE_MASK & currentOpcode) << OPCODE_OFFSET)
					| ((CTRL_WORD_MASK & (currentCtrlWord | CTRL_START_WRITE))
							<< CTRL_WORD_OFFSET);

			// TODO: set GPIO come output (tutti)
			writeData(0, row0);
			// TODO: set GPIO come input (dati) & output (address)
			for (uint8_t i = 0; i < n; i++)
				readData(i + 1, &data[i]);

			row0 = ((CORE_ID_MASK & coreId) << CORE_ID_OFFSET)
					| ((OPCODE_MASK & currentOpcode) << OPCODE_OFFSET)
					| ((CTRL_WORD_MASK & (currentCtrlWord & ~CTRL_START_WRITE))
							<< CTRL_WORD_OFFSET);
			// TODO: set GPIO come output (tutti)
			writeData(0, row0);
			// TODO: set GPIO come input (tutti)
			return 0;
		}
		return -1;
	}
	return -1;
}

int Fpgaipm::fpga_checkInterrupt(CORE_ID coreId) {
	if (checkCoreId(coreId)) {
		// TODO: setta GPIOs come input (dati) & output (address)
		// read row 0
		// TODO: setta GPIOs come input (tutti)
	}
	return -1;
}
