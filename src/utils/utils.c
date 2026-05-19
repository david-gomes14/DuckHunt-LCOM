#include <lcom/lcf.h>

#include <stdint.h>
#include "utils.h"

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t tmp;

  if (sys_inb(port, &tmp) != 0) {
		return 1;
	}

	*value = (uint8_t) tmp;

	return 0;
}


int(util_get_LSB)(uint16_t val, uint8_t *lsb) {

  if (lsb == NULL) {
	return 1;
  }

  *lsb = 0xFF & val;
  return 0;
}


int(util_get_MSB)(uint16_t val, uint8_t *msb) {

  if (msb == NULL) {
	return 1;
  }
  
  *msb = (val >> 8) & 0xFF;
  return 0;
}


