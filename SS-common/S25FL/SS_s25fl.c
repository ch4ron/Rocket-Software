/*
 * SS_s25fl.c
 *
 *  Created on: Jan 19, 2020
 *      Author: Mikolaj Wielgus
 */

#include "SS_s25fl.h"

#define NRST_GPIO FLASH_RESET_GPIO_Port
#define NRST_PIN FLASH_RESET_Pin

#define TIMEOUT_ms 3000
#define ERASE_ALL_TIMEOUT_ms 500000

// Identification reading commands.

#define CMD_READ_ID_REMS 0x90
#define CMD_READ_ID 0x9F
#define CMD_READ_SIG 0xAB
#define CMD_READ_SFDP 0x5A

// Register access commands.

#define CMD_READ_STATUS_REG1 0x05
#define CMD_READ_STATUS_REG2 0x07
#define CMD_READ_CONFIG_REG 0x35

#define CMD_WRITE_REGISTERS 0x01
#define CMD_WRITE_DISABLE 0x04
#define CMD_WRITE_ENABLE 0x06

#define CMD_CLEAR_STATUS_REG1 0x30
#define CMD_ECC_READ 0x18

#define CMD_AUTOBOOT_REG_READ 0x14
#define CMD_AUTOBOOT_REG_WRITE 0x15

#define CMD_BANK_REG_READ 0x16
#define CMD_BANK_REG_WRITE 0x17
#define CMD_BANK_REG_ACCESS 0xB9
#define CMD_DATA_LEARNING_PATTERN_READ 0x41
#define CMD_PROGRAM_NV_DATA_LEARNING_REG 0x43
#define CMD_WRITE_VOLATILE_DATA_LEARNING_REG 0x4A

// Flash reading commands.

#define CMD_READ 0x03
#define CMD_4_READ 0x13

#define CMD_FAST_READ 0x0B
#define CMD_4_FAST_READ 0x0C
#define CMD_DDR_FAST_READ 0x0D
#define CMD_4_DDR_FAST_READ 0x0E

#define CMD_READ_DUAL_OUT 0x3B
#define CMD_4_READ_DUAL_OUT 0x3C
#define CMD_READ_QUAD_OUT 0x6B
#define CMD_4_READ_QUAD_OUT 0x6C

#define CMD_DUAL_IO_READ 0xBB
#define CMD_4_DUAL_IO_READ 0xBC
#define CMD_DDR_DUAL_IO_READ 0xBD
#define CMD_4_DDR_DUAL_IO_READ 0xBC

#define CMD_QUAD_IO_READ 0xEB
#define CMD_4_QUAD_IO_READ 0xEC
#define CMD_DDR_QUAD_IO_READ 0xED
#define CMD_4_DDR_QUAD_IO_READ 0xEE

// Flash programming commands.

#define CMD_PAGE_PROGRAM 0x02
#define CMD_4_PAGE_PROGRAM 0x12
#define CMD_QUAD_PAGE_PROGRAM 0x32
#define CMD_QUAD_PAGE_PROGRAM_ALT 0x38
#define CMD_4_QUAD_PAGE_PROGRAM 0x34

#define CMD_PROGRAM_SUSPEND 0x85
#define CMD_PROGRAM_RESUME 0x8A

// Flash erasing commands.

#define CMD_BULK_ERASE 0x60
#define CMD_BULK_ERASE_ALT 0xC7

#define CMD_SECTOR_ERASE 0xD8
#define CMD_4_SECTOR_ERASE 0xDC

#define CMD_ERASE_SUSPEND 0x75
#define CMD_ERASE_RESUME 0x7A

// OTP commands.

#define CMD_OTP_PROGRAM 0x42
#define CMD_OTP_READ 0x4B

// Register flags.

#define STATUS_REG1_SRWD 0x80
#define STATUS_REG1_P_ERR 0x40
#define STATUS_REG1_E_ERR 0x20
#define STATUS_REG1_BP2 0x10
#define STATUS_REG1_BP1 0x08
#define STATUS_REG1_BP0 0x04
#define STATUS_REG1_WEL 0x02
#define STATUS_REG1_WIP 0x01

#define CONFIG_REG_LC1 0x80
#define CONFIG_REG_LC0 0x40
#define CONFIG_REG_TBPROT 0x20
#define CONFIG_REG_BPNV 0x08
#define CONFIG_REG_QUAD 0x02
#define CONFIG_REG_FREEZE 0x01

#define STATUS_REG2_ES 0x02
#define STATUS_REG2_PS 0x01

static const QSPI_CommandTypeDef default_cmd = {
        .Address = 0,
        .AddressMode = QSPI_ADDRESS_NONE,
        .AddressSize = QSPI_ADDRESS_8_BITS,
        .AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE,
        .AlternateBytes = 0,
        .AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS,
        .DataMode = QSPI_DATA_NONE,
        .DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY,
        .DdrMode = QSPI_DDR_MODE_DISABLE,
        .DummyCycles = 0,
        .Instruction = 0x00,
        .InstructionMode = QSPI_INSTRUCTION_NONE,
        .NbData = 0,
        .SIOOMode = QSPI_SIOO_INST_EVERY_CMD
};

static S25flStatus cmd_write_regs(uint8_t status_reg1, uint8_t config_reg);

static S25flStatus cmd_read_status_reg1(uint8_t *val);
//static S25flStatus cmd_read_status_reg1_dma(uint8_t *val);

static S25flStatus cmd_read_status_reg2(uint8_t *val);
//static S25flStatus cmd_read_status_reg2_dma(uint8_t *val);

static S25flStatus cmd_read_config_reg(uint8_t *val);
static S25flStatus cmd_enable_write(void);

static S25flStatus cmd_write(QSPI_CommandTypeDef cmd, uint8_t *data);
static S25flStatus cmd_write_dma(QSPI_CommandTypeDef cmd, uint8_t *data);

static S25flStatus cmd_read(QSPI_CommandTypeDef cmd, uint8_t *data);
static S25flStatus cmd_read_dma(QSPI_CommandTypeDef cmd, uint8_t *data);

static S25flStatus wait_until_ready(uint32_t timeout_ms);
static QSPI_CommandTypeDef create_write_cmd(uint32_t addr, uint32_t size);
static QSPI_CommandTypeDef create_read_cmd(uint32_t addr, uint32_t size);
static S25flStatus translate_status_regs(uint8_t status_reg1, uint8_t status_reg2);
static S25flStatus translate_hal_status(HAL_StatusTypeDef hal_status);

extern QSPI_HandleTypeDef hqspi;
extern DMA_HandleTypeDef hdma_quadspi;
static volatile bool is_dma_read_ready = true;
static volatile bool is_dma_write_ready = true;

S25flStatus SS_s25fl_init(void) {
    HAL_GPIO_WritePin(NRST_GPIO, NRST_PIN, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(NRST_GPIO, NRST_PIN, GPIO_PIN_SET);
    HAL_Delay(1);

#ifdef S25FL_USE_QUAD
    return cmd_write_regs(0x00, CONFIG_REG_QUAD);
#else
    return cmd_write_regs(0x00, 0x00);
#endif
}

S25flStatus SS_s25fl_read_rems_id(uint16_t *id) {
    S25flStatus status = SS_s25fl_wait_until_ready();
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    QSPI_CommandTypeDef cmd = default_cmd;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_READ_ID_REMS;
    cmd.AddressMode = QSPI_ADDRESS_1_LINE;
    cmd.AddressSize = QSPI_ADDRESS_24_BITS;
    cmd.Address = 0x000000;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.NbData = 2;

    uint8_t data[2];
    status = cmd_read(cmd, data);
    if (status == S25FL_STATUS_OK) {
        *id = (data[0] << 8) | data[1];
    }

    return status;
}

S25flStatus SS_s25fl_wait_until_ready(void) {
    /*uint32_t tickstart = HAL_GetTick();
    S25flStatus status;

    while ((status = SS_s25fl_get_status()) == S25FL_STATUS_BUSY) {
        if (HAL_GetTick() - tickstart > TIMEOUT_ms) {
            return S25FL_STATUS_BUSY;
        }
    }

    return status;*/
    return wait_until_ready(TIMEOUT_ms);
}

/*S25flStatus SS_s25fl_wait_until_ready_dma(void)
{
	uint32_t tickstart = HAL_GetTick();
	S25flStatus status;

	while ((status = SS_s25fl_get_status_dma()) == S25FL_STATUS_BUSY) {
		if (HAL_GetTick() - tickstart > TIMEOUT_ms) {
			return S25FL_STATUS_BUSY;
		}
	}

	return status;
}*/

// Sometimes it is necessary to wait only for DMA.
S25flStatus SS_s25fl_wait_until_dma_ready(void) {
    uint32_t tickstart = HAL_GetTick();

    while (!SS_s25fl_get_is_dma_ready()) {
        if (HAL_GetTick() - tickstart > TIMEOUT_ms) {
            return S25FL_STATUS_BUSY;
        }
    }

    return S25FL_STATUS_OK;
}

S25flStatus SS_s25fl_erase_all(void) {
    S25flStatus status = SS_s25fl_wait_until_ready();
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    status = cmd_enable_write();
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    QSPI_CommandTypeDef cmd = default_cmd;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_BULK_ERASE;

    HAL_StatusTypeDef hal_status = HAL_QSPI_Command(&hqspi, &cmd, TIMEOUT_ms);
    if (hal_status != HAL_OK) {
        return translate_hal_status(hal_status);
    }

    // Normally, routines in this module do not wait until the device is ready.
    // This routine is an exception because bulk erase takes much longer time than other commands.
    // Note that the timeout is different than usual.
    return wait_until_ready(ERASE_ALL_TIMEOUT_ms);
}

S25flStatus SS_s25fl_erase_sector(uint32_t sector) {
    S25flStatus status = SS_s25fl_wait_until_ready();
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    status = cmd_enable_write();
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    QSPI_CommandTypeDef cmd = default_cmd;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_4_SECTOR_ERASE;
    cmd.AddressMode = QSPI_ADDRESS_1_LINE;
    cmd.AddressSize = QSPI_ADDRESS_32_BITS;
    cmd.Address = sector * S25FL_SECTOR_SIZE;

    HAL_StatusTypeDef hal_status = HAL_QSPI_Command(&hqspi, &cmd, TIMEOUT_ms);
    return translate_hal_status(hal_status);
}

// XXX: It should be forbidden to write to bytes directly over a continuous segment overlapping more than one page.

S25flStatus SS_s25fl_write_bytes(uint32_t addr, uint8_t *data, uint32_t size) {
    // Writing across more than one page will not work properly.
    //assert(addr+size >= (addr/PAGE_SIZE+1)*PAGE_SIZE);

    S25flStatus status = SS_s25fl_wait_until_ready();
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    QSPI_CommandTypeDef cmd = create_write_cmd(addr, size);
    return cmd_write(cmd, data);
}

S25flStatus SS_s25fl_write_bytes_dma(uint32_t addr, uint8_t *data, uint32_t size) {
    //S25flStatus status = SS_s25fl_wait_until_ready_dma();
    S25flStatus status = SS_s25fl_wait_until_ready();
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    QSPI_CommandTypeDef cmd = create_write_cmd(addr, size);
    return cmd_write_dma(cmd, data);
}

S25flStatus SS_s25fl_write_page(uint32_t page, uint8_t *data) {
    return SS_s25fl_write_bytes(page * S25FL_PAGE_SIZE, data, S25FL_PAGE_SIZE);
}

S25flStatus SS_s25fl_write_page_dma(uint32_t page, uint8_t *data) {
    return SS_s25fl_write_bytes_dma(page * S25FL_PAGE_SIZE, data, S25FL_PAGE_SIZE);
}

S25flStatus SS_s25fl_read_bytes(uint32_t addr, uint8_t *data, uint32_t size) {
    S25flStatus status = SS_s25fl_wait_until_ready();
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    QSPI_CommandTypeDef cmd = create_read_cmd(addr, size);
    return cmd_read(cmd, data);
}

S25flStatus SS_s25fl_read_bytes_dma(uint32_t addr, uint8_t *data, uint32_t size) {
    //S25flStatus status = SS_s25fl_wait_until_ready_dma();
    S25flStatus status = SS_s25fl_wait_until_ready();
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    QSPI_CommandTypeDef cmd = create_read_cmd(addr, size);
    return cmd_read_dma(cmd, data);
}

S25flStatus SS_s25fl_read_page(uint32_t page, uint8_t *data) {
    return SS_s25fl_read_bytes(page * S25FL_PAGE_SIZE, data, S25FL_PAGE_SIZE);
}

S25flStatus SS_s25fl_read_page_dma(uint32_t page, uint8_t *data) {
    return SS_s25fl_read_bytes_dma(page * S25FL_PAGE_SIZE, data, S25FL_PAGE_SIZE);
}

S25flStatus SS_s25fl_get_status(void) {
    uint8_t status_reg1, status_reg2;

    if (!SS_s25fl_get_is_dma_ready()) {
        return S25FL_STATUS_BUSY;
    }

    S25flStatus status = cmd_read_status_reg1(&status_reg1);
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    status = cmd_read_status_reg2(&status_reg2);
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    return translate_status_regs(status_reg1, status_reg2);
}

// Note that this function, unlike other functions in this module that use DMA,
// is blocking until the return value is obtained.
/*S25flStatus SS_s25fl_get_status_dma(void)
{
	uint8_t status_reg1, status_reg2;

	if (!SS_s25fl_get_is_dma_ready()) {
		return S25FL_STATUS_BUSY;
	}

	S25flStatus status = cmd_read_status_reg1_dma(&status_reg1);
	if (status != S25FL_STATUS_OK) {
		return status;
	}

	status = cmd_read_status_reg2_dma(&status_reg2);
	if (status != S25FL_STATUS_OK) {
		return status;
	}

	status = SS_s25fl_wait_until_dma_ready();
	if (status != S25FL_STATUS_OK) {
		return status;
	}

	return translate_status_regs(status_reg1, status_reg2);
}*/

bool SS_s25fl_get_is_dma_ready(void) {
    return is_dma_write_ready && is_dma_read_ready;
}

void SS_s25fl_txcplt_handler(void) {
    //if (HAL_DMA_GetState(&hdma_quadspi) == HAL_DMA_STATE_READY && HAL_QSPI_GetState(&hqspi) == HAL_QSPI_STATE_READY) {
    is_dma_write_ready = true;
    //}
}

void SS_s25fl_rxcplt_handler(void) {
    //if (HAL_DMA_GetState(&hdma_quadspi) == HAL_DMA_STATE_READY && HAL_QSPI_GetState(&hqspi) == HAL_QSPI_STATE_READY) {
    is_dma_read_ready = true;
    //}
}

#ifdef DEBUG
S25flStatus SS_s25fl_debug_read_regs(uint8_t *status_reg1, uint8_t *status_reg2, uint8_t *config_reg)
{
    S25flStatus status = cmd_read_status_reg1(status_reg1);
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    status = cmd_read_status_reg2(status_reg2);
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    return cmd_read_config_reg(config_reg);
}
#endif

void SS_s25fl_test(void) {
}

static S25flStatus cmd_write_regs(uint8_t status_reg1, uint8_t config_reg) {
    QSPI_CommandTypeDef cmd = default_cmd;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_WRITE_REGISTERS;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.NbData = 2;

    uint8_t data[2] = {status_reg1, config_reg};
    return cmd_write(cmd, data);
}

static S25flStatus cmd_read_status_reg1(uint8_t *val) {
    QSPI_CommandTypeDef cmd = default_cmd;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_READ_STATUS_REG1;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.NbData = 1;

    return cmd_read(cmd, val);
}

/*static S25flStatus cmd_read_status_reg1_dma(uint8_t *val)
{
	QSPI_CommandTypeDef cmd = default_cmd;
	cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	cmd.Instruction = CMD_READ_STATUS_REG1;
	cmd.DataMode = QSPI_DATA_1_LINE;
	cmd.NbData = 1;

	return cmd_read_dma(cmd, val);
}*/

static S25flStatus cmd_read_status_reg2(uint8_t *val) {
    QSPI_CommandTypeDef cmd = default_cmd;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_READ_STATUS_REG2;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.NbData = 1;

    return cmd_read(cmd, val);
}

/*static S25flStatus cmd_read_status_reg2_dma(uint8_t *val)
{
	QSPI_CommandTypeDef cmd = default_cmd;
	cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	cmd.Instruction = CMD_READ_STATUS_REG2;
	cmd.DataMode = QSPI_DATA_1_LINE;
	cmd.NbData = 1;

	return cmd_read_dma(cmd, val);
}*/

static S25flStatus cmd_read_config_reg(uint8_t *val) {
    QSPI_CommandTypeDef cmd = default_cmd;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_READ_CONFIG_REG;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.NbData = 1;

    return cmd_read(cmd, val);
}

static S25flStatus cmd_enable_write(void) {
    QSPI_CommandTypeDef cmd = default_cmd;
    cmd.Instruction = CMD_WRITE_ENABLE;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;

    HAL_StatusTypeDef status = HAL_QSPI_Command(&hqspi, &cmd, TIMEOUT_ms);
    return translate_hal_status(status);
}

static S25flStatus cmd_write(QSPI_CommandTypeDef cmd, uint8_t *data) {
    S25flStatus status = cmd_enable_write();
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    HAL_StatusTypeDef hal_status = HAL_QSPI_Command(&hqspi, &cmd, TIMEOUT_ms);
    if (hal_status != HAL_OK) {
        return translate_hal_status(hal_status);
    }

    hal_status = HAL_QSPI_Transmit(&hqspi, data, TIMEOUT_ms);
    return translate_hal_status(hal_status);
}

static S25flStatus cmd_write_dma(QSPI_CommandTypeDef cmd, uint8_t *data) {
    S25flStatus status = SS_s25fl_wait_until_dma_ready();
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    status = cmd_enable_write();
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    HAL_StatusTypeDef hal_status = HAL_QSPI_Command(&hqspi, &cmd, TIMEOUT_ms);
    if (hal_status != HAL_OK) {
        return translate_hal_status(hal_status);
    }

/*#ifdef DEBUG
	fprintf(stderr, "is_dma_write_ready = false;\r\n");
#endif*/
    is_dma_write_ready = false;
    hal_status = HAL_QSPI_Transmit_DMA(&hqspi, data);
    return translate_hal_status(hal_status);
}

static S25flStatus cmd_read(QSPI_CommandTypeDef cmd, uint8_t *data) {
    HAL_StatusTypeDef hal_status = HAL_QSPI_Command(&hqspi, &cmd, TIMEOUT_ms);
    if (hal_status != HAL_OK) {
        return translate_hal_status(hal_status);
    }

    hal_status = HAL_QSPI_Receive(&hqspi, data, TIMEOUT_ms);
    return translate_hal_status(hal_status);
}

static S25flStatus cmd_read_dma(QSPI_CommandTypeDef cmd, uint8_t *data) {
    S25flStatus status = SS_s25fl_wait_until_dma_ready();
    if (status != S25FL_STATUS_OK) {
        return status;
    }

    HAL_StatusTypeDef hal_status = HAL_QSPI_Command(&hqspi, &cmd, TIMEOUT_ms);
    if (hal_status != HAL_OK) {
        return translate_hal_status(hal_status);
    }

/*#ifdef DEBUG
	fprintf(stderr, "is_dma_read_ready = false;\r\n");
#endif*/
    is_dma_read_ready = false;
    hal_status = HAL_QSPI_Receive_DMA(&hqspi, data);
    return translate_hal_status(hal_status);
}

static S25flStatus wait_until_ready(uint32_t timeout_ms) {
    uint32_t tickstart = HAL_GetTick();
    S25flStatus status;

    while ((status = SS_s25fl_get_status()) == S25FL_STATUS_BUSY) {
        if (HAL_GetTick() - tickstart > timeout_ms) {
            return S25FL_STATUS_BUSY;
        }
    }

    return status;
}

static QSPI_CommandTypeDef create_write_cmd(uint32_t addr, uint32_t size) {
    QSPI_CommandTypeDef cmd = default_cmd;

#ifdef S25FL_USE_QUAD
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_4_QUAD_PAGE_PROGRAM;
    cmd.AddressMode = QSPI_ADDRESS_1_LINE;
    cmd.AddressSize = QSPI_ADDRESS_32_BITS;
    cmd.Address = addr;
    cmd.DataMode = QSPI_DATA_4_LINES;
    cmd.NbData = size;
#else
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_4_PAGE_PROGRAM;
    cmd.AddressMode = QSPI_ADDRESS_1_LINE;
    cmd.AddressSize = QSPI_ADDRESS_32_BITS;
    cmd.Address = addr;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.NbData = size;
#endif

    return cmd;
}

static QSPI_CommandTypeDef create_read_cmd(uint32_t addr, uint32_t size) {
    QSPI_CommandTypeDef cmd = default_cmd;

#ifdef S25FL_USE_QUAD
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_4_QUAD_IO_READ;
    cmd.AddressMode = QSPI_ADDRESS_4_LINES;
    cmd.AddressSize = QSPI_ADDRESS_32_BITS;
    cmd.Address = addr;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_4_LINES;
    cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
    cmd.AlternateBytes = 0x00;
    cmd.DummyCycles = 4;
    cmd.DataMode = QSPI_DATA_4_LINES;
    cmd.NbData = size;
#else
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = CMD_4_READ;
    cmd.AddressMode = QSPI_ADDRESS_1_LINE;
    cmd.AddressSize = QSPI_ADDRESS_32_BITS;
    cmd.Address = addr;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.NbData = size;
#endif

    return cmd;
}

static S25flStatus translate_status_regs(uint8_t status_reg1, uint8_t status_reg2) {
    if (status_reg1 & STATUS_REG1_P_ERR || status_reg1 & STATUS_REG1_E_ERR) {
        return S25FL_STATUS_ERR;
    }
    if (status_reg2 & STATUS_REG2_ES || status_reg2 & STATUS_REG2_PS) {
        return S25FL_STATUS_SUSPENDED;
    }
    if (status_reg1 & STATUS_REG1_WIP) {
        return S25FL_STATUS_BUSY;
    }

    return S25FL_STATUS_OK;
}

static S25flStatus translate_hal_status(HAL_StatusTypeDef hal_status) {
    switch (hal_status) {
        case HAL_OK: return S25FL_STATUS_OK;
        case HAL_BUSY: return S25FL_STATUS_BUSY;
        case HAL_ERROR:
        default: return S25FL_STATUS_ERR;
    }
}
