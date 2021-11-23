// SPDX-License-Identifier: Apache-2.0
/*
 * Copyright (C) 2021 Gwenhael Goavec-Merou <gwenhael.goavec-merou@trabucayre.com>
 */

#include <iostream>
#include <vector>

#include "display.hpp"
#include "spiInterface.hpp"
#include "spiFlash.hpp"

/* spiFlash generic acces */
bool SPIInterface::protect_flash(uint32_t len, bool verbose)
{
	bool ret = true;
	printInfo("protect_flash");

	/* move device to spi access */
	if (!prepare_flash_access())
		return false;

	/* spi flash access */
	try {
		SPIFlash flash(this, false, verbose);
		flash.reset();
		/* configure flash protection */
		flash.enable_protection(len);
	} catch (std::exception &e) {
		printError(e.what());
		ret = false;
	}

	/* reload bitstream */
	return ret && post_flash_access();
}

bool SPIInterface::unprotect_flash(bool verbose)
{
	bool ret = true;

	printInfo("unprotect_flash");

	/* move device to spi access */
	if (!prepare_flash_access())
		return false;
	/* spi flash access */
	try {
		SPIFlash flash(this, false, verbose);
		flash.reset();
		/* configure flash protection */
		flash.disable_protection();
	} catch (std::exception &e) {
		printError(e.what());
		ret = false;
	}

	/* reload bitstream */
	return ret && post_flash_access();
}

bool SPIInterface::write(uint32_t offset, uint8_t *data, uint32_t len,
		bool verify, bool unprotect_flash, bool verbose)
{
	verbose = true;
	printInfo("write generic");
	bool ret = true;
	if (!prepare_flash_access())
		return false;

	/* test SPI */
	try {
		SPIFlash flash(this, unprotect_flash, verbose);
		flash.read_status_reg();
		if (flash.erase_and_prog(offset, data, len) == -1)
			ret = false;
		if (verify && ret)
			ret = flash.verify(offset, data, len);
	} catch (std::exception &e) {
		printError(e.what());
		ret = false;
	}

	bool ret2 = post_flash_access();
	return ret && ret2;
}
