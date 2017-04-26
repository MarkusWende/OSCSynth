// Romasynth Class
// In this class all oscillators and filter obejcts are created
// Midi handling and OSC handling happens here
// Callback function of the jack client is loaded with output buffers here as well

#include "romasynthi.h"


RoMaSynthi::RoMaSynthi() : JackCpp::AudioIO("RoMaSynthi", 0,1) {
	reserveInPorts(2);
	reserveOutPorts(2);

	// delivers the sample rate
	fs = getSampleRate();
	// delivers the buffer size
	nframes = getBufferSize();

	std::cout << "fs: " << fs << " Hz";
	std::cout << " || buffer size: " << nframes << " samples" << endl;

	// Osclillator Containers for all available playable notes are created here
	osci = new Oscicontainer *[7];

	osci[0] = new Oscicontainer(fs);
	osci[1] = new Oscicontainer(fs);
	osci[2] = new Oscicontainer(fs);
	osci[3] = new Oscicontainer(fs);
	osci[4] = new Oscicontainer(fs);
	osci[5] = new Oscicontainer(fs);
	osci[6] = new Oscicontainer(fs);

	// osc manager is created
	osc = new OscMan(50000);
	// midi manager is created
	midi = new MidiMan();

	// the filter object is created
	filter= new Biquad(0, 0.3, 0.2, 1.0);
	// creates the lfo oscillator
	lfo= new Oscicontainer(0, 1);
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



    /// Audio Callback Function:
    /// - the output buffers are filled here
int RoMaSynthi::audioCallback(jack_nframes_t nframes,
                              // A vector of pointers to each input port.
                              audioBufVector inBufs,
                              // A vector of pointers to each output port.
                              audioBufVector outBufs) {

        /// LOOP over all output buffers
    for(unsigned int i = 0; i < 1; i++) {

    	for(int frameCNT = 0; frameCNT  < nframes; frameCNT++) {
            
               
                
                outBufs[0][frameCNT] = (osci[0]->getNextSample() +
										osci[1]->getNextSample() + 
										osci[2]->getNextSample() +
										osci[3]->getNextSample() +
										osci[4]->getNextSample() +
										osci[5]->getNextSample() +
										osci[6]->getNextSample()) / 7;
                if(filterStatus) {
                	//hand over to filter
                	outBufs[0][frameCNT] = filter->process(outBufs[0][frameCNT]); 
                }

                //hand over to distortion
                outBufs[0][frameCNT] = distortion->process(outBufs[0][frameCNT]); 
				
				// rotate lfo oscillator to next step
				lfo->getNextSample();
		}
    }

        
 	///return 0 on success        
    return 0;
}


// The Midi Handler receives messages from the midi manager
// all note on and note off handling happens here
void RoMaSynthi::midiHandler() {


      	/// process midi messages
        
        // In RT Midi defined values for note-on and note-off are being sent:
        // val1 : 144 -> note on, 128 -> note off
        //val2 : mote pitch from 0 bis 127 - > note pitch is being sent at both note-on and note-off
        //val3 : velocity - when note on, value is between 0 and 126, note off: 127
        
		midiMessage info = midi->get_rtmidi();
	
		int val1 = info.byte1;
		int val2 = info.byte2;
		double val3 = info.byte3;
		double delta_time = info.stamp; // get time information

      	//accumulate time
		t_tracking = t_tracking + delta_time; 
       

		int osci_nummer = 0;
		int position = 0;
		
		///////////////////
		// note-on procedure
		///////////////////
        if(val1==144) {

            //if all oscillators are being used, kill oldest
            if(counter==7) {
				int min = timetracker[0];
				int index = 0;
				//find oldest value, i.e. smallest time value
				for(int i=1; i<timetracker.size(); i++) {
					if(timetracker[i]<min) {
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
			double f0 = std::pow(double (2), (val2-double(69))/double(12))*440;
				  
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
        
        if(val1==128 ) {

        	//find the oscillator that plays the frequency refering to the note-off order
            position = find(Noten.begin(), Noten.end(), val2) - Noten.begin();
            
            
            if(position<maxAnzahl_Osci) {	// security measure to prevent stack dump
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
            
         /*
         //for debugging
        if (val1>=0) {
         std::cout << "Frei: " << freeOsci.size() << " Zeit: " << t_tracking << "Counter: " << counter << endl;
       }*/
}


// OSC Handler receives messegas from OSC manager and hands them over to Setters
void RoMaSynthi::oscHandler() {

  	double val;

  	// determine the osc message type and use the according getter method 
  	// (intiger = i, float= f, string =s)
	string type = osc->getLastType();
	
	if (type != "empty")
	{
	  	if (type== "f") val =  osc->getLastDouble();

	  	else if (type== "i") val =  osc->getLastInt();

	  	else if  (type== "s") val =  osc->getLastChar();

	  	string path = osc->getLastPath();

		typeOld = type;
		pathOld = path;
		valOld = val;

		// display osc messages
		//cout<<val<<path<<"Type:"<<type<<endl;

		////////////////////////////////////////////////////////
		// this section sends osc messages to according setters
		///////////////////////////////////////////////////////
		if (path.compare("/SineAmpl") == 0) {
			osci[0]->setSineAmpl(val);
			osci[1]->setSineAmpl(val);
			osci[2]->setSineAmpl(val);
			osci[3]->setSineAmpl(val);
			osci[4]->setSineAmpl(val);
			osci[5]->setSineAmpl(val);
			osci[6]->setSineAmpl(val);
			//cout << "OSC01 Ampl: " << val	<< endl;
		}
		
		if (path.compare("/SawAmpl") == 0) {
			//cout << "OSC02 Ampl: " << val	<< endl;
			osci[0]->setSawAmpl(val);
			osci[1]->setSawAmpl(val);
			osci[2]->setSawAmpl(val);
			osci[3]->setSawAmpl(val);
			osci[4]->setSawAmpl(val);
			osci[5]->setSawAmpl(val);
			osci[6]->setSawAmpl(val);
		}
		if (path.compare("/SquareAmpl") == 0) {
			//cout << "SqureAmpl: " << val << endl;
			osci[0]->setSquareAmpl(val);
			osci[1]->setSquareAmpl(val);
			osci[2]->setSquareAmpl(val);
			osci[3]->setSquareAmpl(val);
			osci[4]->setSquareAmpl(val);
			osci[5]->setSquareAmpl(val);
			osci[6]->setSquareAmpl(val);
		}
		if (path.compare("/NoiseAmpl") == 0) {

			osci[0]->setNoiseAmpl(val);
			osci[1]->setNoiseAmpl(val);
			osci[2]->setNoiseAmpl(val);
			osci[3]->setNoiseAmpl(val);
			osci[4]->setNoiseAmpl(val);
			osci[5]->setNoiseAmpl(val);
			osci[6]->setNoiseAmpl(val);
		}
		
	    if (path.compare("/LFO_Q") == 0) {
	      	filter->setQ(val);
		}
			
		if (path.compare("/Filter_Type") == 0) {
	      	filter->setType((int)val);
		}
			
		if (path.compare("/Filter_Gain") == 0) {
	      	filter->setPeakGain(val);
		}
			
		if (path.compare("/LFO_Freq") == 0) {
	    	lfo->frequency(val);
	    }

	    if (path.compare("/LFO_Type") == 0) {
	      	lfo->setLFOtype((int)val);
		}

		if (path.compare("/Distortion_Gain") == 0) {
	      	distortion->setGain((int)val);
		}

		if (path.compare("/Filter_Status") == 0) {
			if ((int)val == 1) filterStatus = true;
			else filterStatus = false;
		}

		if (path.compare("/ADSR_Status") == 0) {
			if ((int)val == 1) {
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

		if (path.compare("/ADSR_Sustain_Level") == 0) {
	    	osci[0]->setADSRSustainLevel(val);
			osci[1]->setADSRSustainLevel(val);
			osci[2]->setADSRSustainLevel(val);
			osci[3]->setADSRSustainLevel(val);
			osci[4]->setADSRSustainLevel(val);
			osci[5]->setADSRSustainLevel(val);
			osci[6]->setADSRSustainLevel(val);
		}

		if (path.compare("/ADSR_Attack_Time") == 0) {
	    	osci[0]->setADSRAttackTime(val);
			osci[1]->setADSRAttackTime(val);
			osci[2]->setADSRAttackTime(val);
			osci[3]->setADSRAttackTime(val);
			osci[4]->setADSRAttackTime(val);
			osci[5]->setADSRAttackTime(val);
			osci[6]->setADSRAttackTime(val);
		}

		if (path.compare("/ADSR_Release_Time") == 0) {
	    	osci[0]->setADSRReleaseTime(val);
			osci[1]->setADSRReleaseTime(val);
			osci[2]->setADSRReleaseTime(val);
			osci[3]->setADSRReleaseTime(val);
			osci[4]->setADSRReleaseTime(val);
			osci[5]->setADSRReleaseTime(val);
			osci[6]->setADSRReleaseTime(val);
		}

		if (path.compare("/ADSR_Decay_Time") == 0) {
	    	osci[0]->setADSRDecayTime(val);
			osci[1]->setADSRDecayTime(val);
			osci[2]->setADSRDecayTime(val);
			osci[3]->setADSRDecayTime(val);
			osci[4]->setADSRDecayTime(val);
			osci[5]->setADSRDecayTime(val);
			osci[6]->setADSRDecayTime(val);
		}

		if (path.compare("/Dist_On") == 0) {
	      	distortion_on=(int)val;
		}


	}
	
	
	usleep(500);
}



void RoMaSynthi::lfoHandler() {

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
    	filter->setFc(lfo_value);


      	lfo_oldValue=lfo_value;
      
	 }

}

  