#pragma once

/*
 * Generates a new hint from the tuple (hints,index), where:
 * - hints is the alphabet
 * - index is a position in the cartesian product formed with the alphabet
 */
char *make_hint(const char *hints, int index);
