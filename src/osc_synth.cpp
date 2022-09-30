// OSCSynth Class
// In this class all oscillators and filter obejcts are created
// Midi handling and OSC handling happens here
// Callback function of the jack client is loaded with output buffers here as well

#include "osc_synth.h"

#include <aixlog.hpp>

OSCSynth::OSCSynth() : JackCpp::AudioIO("OSCSynth", 0,1)
{
	reserveInPorts(2);
	reserveOutPorts(2);

	// delivers the sample rate
	fs = getSampleRate();
	// delivers the buffer size
	nframes = getBufferSize();
	// default gain
	gain_ = 1.0;

	ring_buffer_out_ = new JackCpp::RingBuffer<float>(nframes*8, true);

	LOG(INFO) << "fs: " << fs << " Hz.\n";
	LOG(INFO) << "buffer size: " << nframes << " samples.\n";

	// Osclillator Containers for all available playable notes are created here
	//osci = new Oscicontainer *[7];

	// osci[0] = new Oscicontainer(fs);
	// osci[1] = new Oscicontainer(fs);
	// osci[2] = new Oscicontainer(fs);
	// osci[3] = new Oscicontainer(fs);
	// osci[4] = new Oscicontainer(fs);
	// osci[5] = new Oscicontainer(fs);
	// osci[6] = new Oscicontainer(fs);

	osci.insert(std::make_pair(0, new Oscicontainer(fs)));
	osci.insert(std::make_pair(1, new Oscicontainer(fs)));
	osci.insert(std::make_pair(2, new Oscicontainer(fs)));
	osci.insert(std::make_pair(3, new Oscicontainer(fs)));
	osci.insert(std::make_pair(4, new Oscicontainer(fs)));
	osci.insert(std::make_pair(5, new Oscicontainer(fs)));
	osci.insert(std::make_pair(6, new Oscicontainer(fs)));

	// osc manager is created
	osc = new OscMan("50000");
	// midi manager is created
	midi = new MidiMan();

	// the filter object is created
	filter = new Biquad(0, 0.01, 0.2, 1.0);
	// creates the lfo oscillator
	lfo = new Oscicontainer(fs, 0, 1);
	// disortion object is created
	distortion = new Distortion();


	filterStatus = false;

	midi->flushProcessedMessages();

	t_tracking = 0;
	counter = 0;

	//vector to store the played Midi Value - the position within the vector corresponds to the oscillator number
	Noten = {-1, -1, -1, -1, -1, -1, -1} ;
	//vector containing the unused oscillators
	freeOsci = { 6, 5, 4, 3, 2, 1, 0};
	//vector to track the time of played notes - the position within the vector corresponds to the oscillator number
	timetracker = { -1, -1, -1, -1, -1, -1, -1};
    
    //max number of oscillators
	maxAnzahl_Osci = Noten.size();

	valOld = 0.0;
	typeOld = "";
	pathOld = "";

}

OSCSynth::~OSCSynth()
{
	ring_buffer_out_->~RingBuffer();
}


/// Audio Callback Function:
/// - the output buffers are filled here
int
OSCSynth::audioCallback(jack_nframes_t nframes,
						// A vector of pointers to each input port.
						audioBufVector inBufs,
						// A vector of pointers to each output port.
						audioBufVector outBufs)
{
	// Do nothing with input buffer
	(void)inBufs;
	// Read the ring buffer and write to the output buffer
	ring_buffer_out_->read(outBufs[0], nframes);

 	///return 0 on success        
    return 0;
}


// The Midi Handler receives messages from the midi manager
// all note on and note off handling happens here
void OSCSynth::midiHandler()
{
      	/// process midi messages
        
        // In RT Midi defined values for note-on and note-off are being sent:
        // val1 : 144 -> note on, 128 -> note off
        //val2 : mote pitch from 0 bis 127 - > note pitch is being sent at both note-on and note-off
        //val3 : velocity - when note on, value is between 0 and 126, note off: 127
        
		midiMessage info = midi->get_rtmidi();
	
		auto val1 = info.byte1;
		auto val2 = info.byte2;
		auto val3 = info.byte3;
		auto delta_time = info.stamp; // get time information

      	//accumulate time
		t_tracking = t_tracking + delta_time; 
       

		size_t osci_nummer = 0;
		size_t position = 0;
		
		///////////////////
		// note-on procedure
		///////////////////
        if(val1 == 144)
		{

            //if all oscillators are being used, kill oldest
            if(counter==7) {
				auto min = timetracker[0];
				size_t index = 0;
				//find oldest value, i.e. smallest time value
				for(size_t i = 1; i < timetracker.size(); i++)
				{
					if(timetracker[i] < min)
					{
						min = timetracker[i];
						index = i;
					}
				}

				//kill oldest oscillator

				osci[index]->setReleaseNoteState(2);
				osci[index]->setADSRState(4);
				
				//delete value in note vector
				Noten[index] = -1;

				//return free osci to according vector
				freeOsci.push_back(index);
				  
				//delete time entry
				timetracker[index] = -1;
				   
				counter--;
			}

			//formula to calculate the frequency from midi note value
			auto f0 = std::pow(2.0, ((double)val2-69.0) / 12.0) * 440.0;
				  
			//find a free oscillator
			osci_nummer = freeOsci.back();
			
				  
			//make used osci unavailable
			freeOsci.pop_back();
			
			osci[osci_nummer]->frequency(f0);
				  
				 
			//hand amplitude and ADSR data to oscillator
			osci[osci_nummer]->setReleaseNoteState(1);
			osci[osci_nummer]->setADSRState(1);

			//n set amplitude and phase
			osci[osci_nummer]->amplitude(val3/126);
			osci[osci_nummer]->phase(0);
				  
			//safe the played midi value
			Noten[osci_nummer] = val2;
				  
			//safe the time instnace
			timetracker[osci_nummer] = t_tracking;
			
				  
			counter++;
        }
        
        ///////////////////
		// note-off procedure (val1 = 128)
		///////////////////    
        if(val1 == 128 )
		{

        	//find the oscillator that plays the frequency refering to the note-off order
            position = find(Noten.begin(), Noten.end(), val2) - Noten.begin();
            
            
            if(position < maxAnzahl_Osci) {	// security measure to prevent stack dump
            	// enter into release mode
              	osci[position]->setReleaseNoteState(2);
              	osci[position]->setADSRState(4);
              	
              
              	//delete array in notes vector
              	Noten[position]= -1;
              	//return freed oscillator
              	freeOsci.insert(freeOsci.begin(),position);
              
                //delete time instance
               	timetracker[position]= -1;
               
               	counter--;
            }  
 
        }
            
#ifdef __OSCSYNTH_DEBUG__
	if (val1 >= 0)
		LOG(DEBUG) << "Free: " << freeOsci.size() << "\tTime: " << t_tracking << "\tCounter: " << counter << "\n";
#endif // __OSCSYNTH_DEBUG__
}


// OSC Handler receives messegas from OSC manager and hands them over to Setters
void OSCSynth::oscHandler() {

  	auto val = 0.0;

  	// determine the osc message type and use the according getter method 
  	// (intiger = i, float= f, string =s)
	auto type = osc->getLastType();
	
	if (type != "empty")
	{
	  	if (type== "f")
			val =  osc->getLastDouble();
	  	else if (type== "i")
			val =  osc->getLastInt();
	  	else if  (type== "s")
			val =  osc->getLastChar();

	  	auto path = osc->getLastPath();

		typeOld = type;
		pathOld = path;
		valOld = val;

#ifdef __OSCSYNTH_DEBUG__
	// display osc messages
	LOG(DEBUG) << "/Val:" << val << "/Path:" << path << "/Type:" << type << "\n";
#endif // __OSCSYNTH_DEBUG__

		////////////////////////////////////////////////////////
		// this section sends osc messages to according setters
		///////////////////////////////////////////////////////
		if (path.compare("/SineAmpl") == 0)
			setAllSineAmpl(val);
		else if (path.compare("/SawAmpl") == 0)
			setAllSawAmpl(val);
		else if (path.compare("/SquareAmpl") == 0)
			setAllSquareAmpl(val);
		else if (path.compare("/NoiseAmpl") == 0)
			setAllNoiseAmpl(val);
	    else if (path.compare("/LFO_Q") == 0)
	      	filter->SetQ(val);
		else if (path.compare("/Filter_Type") == 0)
	      	filter->SetType((filterType)std::round(val));
		else if (path.compare("/Filter_Gain") == 0)
	      	filter->SetPeakGain(val);
		else if (path.compare("/LFO_Freq") == 0)
	    	lfo->frequency(val);
	    else if (path.compare("/LFO_Type") == 0)
	      	lfo->setLFOtype((int)val);
		else if (path.compare("/Distortion_Gain") == 0)
	      	SetGain(val);// Using general gain for the distortion fader.. //distortion->setGain((int)val);
		else if (path.compare("/Filter_Status") == 0)
			filterStatus = (int)val;
		else if (path.compare("/ADSR_Status") == 0)
			setAllADSRStatus(val);
		else if (path.compare("/ADSR_Sustain_Level") == 0)
			setAllADSRSustainLevel(val);
		else if (path.compare("/ADSR_Attack_Time") == 0)
	    	setAllADSRAttackTime(val);
		else if (path.compare("/ADSR_Release_Time") == 0)
			setAllADSRReleaseTime(val);
		else if (path.compare("/ADSR_Decay_Time") == 0)
			setAllADSRDecayTime(val);
		else if (path.compare("/Preset") == 0)
			presets((int)std::round(val));
	}
		
	usleep(500);
}


// These functions help to make changes to all created note (osci)-objects

void OSCSynth::setAllSineAmpl(double val) {

	osci[0]->setSineAmpl(val);
	osci[1]->setSineAmpl(val);
	osci[2]->setSineAmpl(val);
	osci[3]->setSineAmpl(val);
	osci[4]->setSineAmpl(val);
	osci[5]->setSineAmpl(val);
	osci[6]->setSineAmpl(val);
}

void OSCSynth::setAllSawAmpl(double val){

	osci[0]->setSawAmpl(val);
	osci[1]->setSawAmpl(val);
	osci[2]->setSawAmpl(val);
	osci[3]->setSawAmpl(val);
	osci[4]->setSawAmpl(val);
	osci[5]->setSawAmpl(val);
	osci[6]->setSawAmpl(val);
}

void OSCSynth::setAllSquareAmpl(double val) {

	osci[0]->setSquareAmpl(val);
	osci[1]->setSquareAmpl(val);
	osci[2]->setSquareAmpl(val);
	osci[3]->setSquareAmpl(val);
	osci[4]->setSquareAmpl(val);
	osci[5]->setSquareAmpl(val);
	osci[6]->setSquareAmpl(val);
}

void OSCSynth::setAllNoiseAmpl(double val) {

	val=val*0.5;
	osci[0]->setNoiseAmpl(val);
	osci[1]->setNoiseAmpl(val);
	osci[2]->setNoiseAmpl(val);
	osci[3]->setNoiseAmpl(val);
	osci[4]->setNoiseAmpl(val);
	osci[5]->setNoiseAmpl(val);
	osci[6]->setNoiseAmpl(val);
}

void OSCSynth::setAllADSRStatus(int val) {

	if (val == 1) {
		osci[0]->setADSRStatus(true);
		osci[1]->setADSRStatus(true);
		osci[2]->setADSRStatus(true);
		osci[3]->setADSRStatus(true);
		osci[4]->setADSRStatus(true);
		osci[5]->setADSRStatus(true);
		osci[6]->setADSRStatus(true);
	}
	else {
		osci[0]->setADSRStatus(false);
		osci[1]->setADSRStatus(false);
		osci[2]->setADSRStatus(false);
		osci[3]->setADSRStatus(false);
		osci[4]->setADSRStatus(false);
		osci[5]->setADSRStatus(false);
		osci[6]->setADSRStatus(false);
	}
}

void OSCSynth::setAllADSRSustainLevel(double val) {

	osci[0]->setADSRSustainLevel(val);
	osci[1]->setADSRSustainLevel(val);
	osci[2]->setADSRSustainLevel(val);
	osci[3]->setADSRSustainLevel(val);
	osci[4]->setADSRSustainLevel(val);
	osci[5]->setADSRSustainLevel(val);
	osci[6]->setADSRSustainLevel(val);
}

void OSCSynth::setAllADSRAttackTime(double val) {

	osci[0]->setADSRAttackTime(val);
	osci[1]->setADSRAttackTime(val);
	osci[2]->setADSRAttackTime(val);
	osci[3]->setADSRAttackTime(val);
	osci[4]->setADSRAttackTime(val);
	osci[5]->setADSRAttackTime(val);
	osci[6]->setADSRAttackTime(val);
}

void OSCSynth::setAllADSRReleaseTime(double val) {

	osci[0]->setADSRReleaseTime(val);
	osci[1]->setADSRReleaseTime(val);
	osci[2]->setADSRReleaseTime(val);
	osci[3]->setADSRReleaseTime(val);
	osci[4]->setADSRReleaseTime(val);
	osci[5]->setADSRReleaseTime(val);
	osci[6]->setADSRReleaseTime(val);
}


void OSCSynth::setAllADSRDecayTime(double val) {

	osci[0]->setADSRDecayTime(val);
	osci[1]->setADSRDecayTime(val);
	osci[2]->setADSRDecayTime(val);
	osci[3]->setADSRDecayTime(val);
	osci[4]->setADSRDecayTime(val);
	osci[5]->setADSRDecayTime(val);
	osci[6]->setADSRDecayTime(val);
}



//function that processes and scales the lfo-signal
void OSCSynth::lfoHandler() {

	//limits LFO Signal to certain step size
	double lfo_step = 0.001;

	//smallest value for Cutoff Frequency
	double fc_min=200.0/fs;

	//highest value for Cutoff Frequency
	double fc_max=20000.0/fs;

	//Get current LFO 
	double lfo_value=lfo->getCurrentAmpl(); 


	//scale from -1 to 1 in a way that the signal can oscillate between fc_max and fc_min
	lfo_value=((fc_max-fc_min)/(1-(-1))) * (lfo_value -(-1)) + fc_min;

	 
	//no negative values - dirty workaround just in case amplitude should be higher than 1 for some reason
	if (lfo_value<0) lfo_value=0;

	//limits step size
	if (lfo_value > (lfo_oldValue + lfo_step) || lfo_value < (lfo_oldValue - lfo_step) ) { 
      
		//change Cutoff of Filter when lfo_value changes
    	filter->SetFc(lfo_value);


      	lfo_oldValue=lfo_value;
      
	 }

}


void OSCSynth::presets(int preset) {

	switch(preset) {

		case wobble:

			//oscillator settings
			setAllSineAmpl(1);
			setAllSquareAmpl(1);
			setAllSawAmpl(1);
			setAllNoiseAmpl(0);

			//ADSR Settings
			setAllADSRStatus(0);
			/*setAllADSRAttackTime(1);
			setAllADSRDecayTime(1);
			setAllADSRReleaseTime(1);
			setAllADSRSustainLevel(1);*/
			
			//Biquad settings
			filterStatus = true;
			filter->SetType(filterType::LOWSHELF);
			//filter->setQ(1);
			filter->SetPeakGain(100);

			//lfo settings
			lfo->setLFOtype(0);
			lfo->frequency(2);

			//gain (distortion) settings
			distortion->setGain(50);


		break;

		case dreamy:

			//oscillator settings
			setAllSineAmpl(1);
			setAllSquareAmpl(1);
			setAllSawAmpl(0);
			setAllNoiseAmpl(0);

			//ADSR Settings
			setAllADSRStatus(1);
			setAllADSRAttackTime(20);
			setAllADSRDecayTime(80);
			setAllADSRSustainLevel(50);
			setAllADSRReleaseTime(50);

			
			//Biquad settings
			filterStatus = true;
			filter->SetType(filterType::BANDPASS);
			filter->SetQ(0.01);
			//filter->setPeakGain(100);

			//lfo settings
			lfo->setLFOtype(0);
			lfo->frequency(2);

			//gain (distortion) settings
			distortion->setGain(2);


		break;


		case nice_pulse:

			//oscillator settings
			setAllSineAmpl(1);
			setAllSquareAmpl(1);
			setAllSawAmpl(1);
			setAllNoiseAmpl(0);

			//ADSR Settings
			setAllADSRStatus(1);
			setAllADSRAttackTime(30);
			setAllADSRDecayTime(30);
			setAllADSRSustainLevel(50);
			setAllADSRReleaseTime(50);

			
			//Biquad settings
			filterStatus = true;
			filter->SetType(filterType::HIGHSHELF);
			//filter->setQ(0.01);
			filter->SetPeakGain(15);

			//lfo settings
			lfo->setLFOtype(2);
			lfo->frequency(5);

			//gain (distortion) settings
			distortion->setGain(5);


		break;


		case in_the_night:

			//oscillator settings
			setAllSineAmpl(0);
			setAllSquareAmpl(1);
			setAllSawAmpl(1);
			setAllNoiseAmpl(0);

			//ADSR Settings
			setAllADSRStatus(1);
			setAllADSRAttackTime(3);
			setAllADSRDecayTime(15);
			setAllADSRSustainLevel(70);
			setAllADSRReleaseTime(30);

			
			//Biquad settings
			filterStatus = true;
			filter->SetType(filterType::LOWPASS);
			filter->SetQ(0.5);
			//filter->setPeakGain(15);

			//lfo settings
			lfo->setLFOtype(1);
			lfo->frequency(3);

			//gain (distortion) settings
			distortion->setGain(5);


		break;


		case high_hat:

			//oscillator settings
			setAllSineAmpl(1);
			setAllSquareAmpl(1);
			setAllSawAmpl(1);
			setAllNoiseAmpl(1);

			//ADSR Settings
			setAllADSRStatus(0);
			/*setAllADSRAttackTime(3);
			setAllADSRDecayTime(15);
			setAllADSRSustainLevel(70);
			setAllADSRReleaseTime(30);*/

			
			//Biquad settings
			filterStatus = true;
			filter->SetType(filterType::HIGHPASS);
			filter->SetQ(0.09);
			//filter->setPeakGain(15);

			//lfo settings
			lfo->setLFOtype(1);
			lfo->frequency(8);

			//gain (distortion) settings
			distortion->setGain(30);


		break;

	}
}

void
OSCSynth::process()
{
	for(size_t i = 0; i < 1; i++)
	{
		size_t dataSize = ring_buffer_out_->getWriteSpace();
		std::vector<float> data;
    	for(size_t frameCNT = 0; frameCNT  < dataSize; frameCNT++)
		{
			auto sample = 		osci[0]->getNextSample() +
								osci[1]->getNextSample() +
								osci[2]->getNextSample() +
								osci[3]->getNextSample() +
								osci[4]->getNextSample() +
								osci[5]->getNextSample() +
								osci[6]->getNextSample();

			sample = sample / 7.0 * gain_;
			
			if(filterStatus)
				sample = filter->Process(sample);
		
			// Commented out, because it makes to much artifacts
			//sample = distortion->process(sample); 

			// rotate lfo oscillator to next step
			lfo->getNextSample();

			data.push_back((float)sample);
		}
		ring_buffer_out_->write(data.data(), dataSize);
    }
}