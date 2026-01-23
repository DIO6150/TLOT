#pragma once

#include <stdint.h>

extern uint64_t component_counter;

template<class T>
uint64_t GenerateUID () {
	static uint64_t uid = ++component_counter;

	return (uid);
}