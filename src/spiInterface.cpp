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
