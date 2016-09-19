#include "all.h"

char *make_hint(const char *hints, int index) {
    if (!hints || index < 0)
        return NULL;

    size_t length_hints = strlen(hints);

    // Minimum upper bound of how many permutations with repetions
    // can be produced in order to represent the hint
    int possible_indexes = 0;

    // Number of characters used in the hint
    size_t length_hint = 1;

    for (size_t i = 1; i <= length_hints; i++) {
        length_hint = i;
        possible_indexes += ipow(length_hints, length_hint);
        if (possible_indexes > index)
            break;
    }

    // No hint possible
    if (possible_indexes <= index) {
		return NULL;
	}

    /*
     * Taken from https://stackoverflow.com/questions/9944915
     * Example for hints_length = 3:
     *
     * c(i) = [A[i_a], B[i_b], C[i_c]], where:
     *
     * i_a = i/(size(B)*size(C)) 
     * i_b = (i/size(C)) mod size(B)
     * i_c = i mod size(C)
     *
     * Note:
     * - All sizes are equal to hints_length
     * - The mod operation is also applied to the first index
     */
    else {
        // Each character of the resulting hint is stored in an array
        // with the minimum hint size
        char *hint = (char*) calloc(length_hint + 1, sizeof(char));

        // Relative offset for calculating sub-index position
        int offset = 0;

        /* 
         * Normalize index:
         * It should be a value within range [0, n],
         * where n is the maximum number of permutations with repetitons
         * that can be constructed from the minimum hint size
         */
        index -= (possible_indexes - ipow(length_hints, length_hint));

        for (int i = (length_hint - 1);
                i >= 0;
                i--, offset++) {
            hint[i] = hints[
                    (index / ipow(length_hints, offset)) % length_hints];
        } 
        
        return hint;
	}
}
