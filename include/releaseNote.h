#ifndef releaseNote_h
#define releaseNote_h

class releaseNote {
private:
    // state of the releaseNote envelope, e.g. note_on, note_off ...
    int state;
    // output value, which the signal is multiplied with
    float output;
    
public:
    // constructor
	releaseNote(void);

	float process(void);
	void gate(int on);
    void reset(void);

    // getters
    float getOutput(void);
    int getState(void);

    // states of the envelope
    enum noteState {
        note_off = 0,
        note_on = 1,
        note_release = 2
    };
};

/* process() function
 * returns the output value which the signal is muliplied with
 * the calculation depends on the envelope state
 */
inline float releaseNote::process() {

    if(state == note_release) {
        output = output * 0.99;
        if (output <= 0.001) {
            output = 0.0;
            state = note_off;
        }
    } else if(state == note_on) {
        if (output == 0.0)
            output = 0.001;

        output = output + output * 0.01;
        if (output >= 0.99) {
            output = 1.0;
        }
    } else {
        output = 0.0;
        state = note_off;
    }

    return output;
}

#endif