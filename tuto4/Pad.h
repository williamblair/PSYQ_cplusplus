// Pad.h

#ifndef PSX_PAD_INCLUDED
#define PSX_PAD_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <libetc.h>
#include <libpad.h>

// modified from Orion's psx library
#define PadUp           PADLup
#define PadDown	        PADLdown
#define PadLeft	        PADLleft
#define PadRight		PADLright
#define PadTriangle     PADRup
#define PadCross		PADRdown
#define PadSquare		PADRleft
#define PadCircle		PADRright
#define PadL1			PADL1
#define PadL2			PADL2
#define PadR1			PADR1
#define PadR2			PADR2
#define PadStart		PADstart
#define PadSelect		PADselect

class Pad
{
public:

	Pad();
	
	// Initialize psyq pad system
	// should be static and only called ONCE ever
	static void init();

    
    // Sets controller mode and detects its type
    // Should be called once for each controller being used
	void init_controller( int controller_num );
	

    
	// Update button and analog values, check if controller 
	// is disconnected or changed state
	void read();
	
	
	// Check if a button is held or clicked
	bool is_held(    u_int button );
	bool is_clicked( u_int button );
	
    
	// Get analog stick values
	int get_left_analog_x();
	int get_left_analog_y();
	int get_right_analog_x();
	int get_right_analog_y();

    
    // Set/Get vibration motors
    // actuator 0 - on or off, so can only take a value of 0 or 1
    // actuator 1 - value range from 0 to 255
    void set_actuator_val( u_char actuator, int val );
    int  get_actuator_val( u_char actuator );

private:

	static u_char pad_buffers[2][34]; // each pad uses 34 bytes of data
	
	// Dual Shock values
	u_char 	 actuator_vals[2];
	u_char	 actuator_indices[6];
	u_long 	*stored_mode;
	int 	 ctr;

    // controller 0 or 1?
    int controller_num;
	
	// analog stick values from -128 to 127 each
	int analog_vals[4];
	
	// all of the button states stored in a single value
	int buttons_state_prev;
	int buttons_state;
	

};

#endif // PAD_H_INCLUDED

