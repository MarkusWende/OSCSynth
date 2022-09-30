#include "releaseNote.h"
#include <math.h>

/* constructor
 */
releaseNote::releaseNote(void) {
    reset();
}

/* set the state of the envelope
 */
void releaseNote::gate(int gate) {
    if (gate == 0)
        state = note_off;
    else if (gate == 1)
        state = note_on;
    else if (gate == 2)
        state = note_release;
}

/* get the current state of the envelope
 */
int releaseNote::getState() {
    return state;
}

/* reset the envelope with preset values
 */
void releaseNote::reset() {
    state = note_off;
    output = 0.0;
}

/* returns the output
 */
float releaseNote::getOutput() {
    return output;
}