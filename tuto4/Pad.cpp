// Pad.cpp

#include "Pad.h"

#define DUALSHOCKMODE ((u_long *)0x80010000)

Pad::Pad()
{
    // which index in the actuator_vals buffer for each actuator to use
    actuator_indices[0] = 0;
    actuator_indices[1] = 1;
    actuator_indices[2] = 0xFF;
    actuator_indices[3] = 0xFF;
    actuator_indices[4] = 0xFF;
    actuator_indices[5] = 0xFF;
    
    
    stored_mode = DUALSHOCKMODE;
    
    // if these aren't initialized to zero the pad will 
    // set the vibration motors (actuators) based on these garbage values
    // which will most likely be non zero, hence turned on
    actuator_vals[0] = 0;
    actuator_vals[1] = 0;
    
	// default to 'center' of 128
    analog_vals[0] = 128;
	analog_vals[1] = 128;
	analog_vals[2] = 128;
	analog_vals[3] = 128;
    
    buttons_state 	   = 0;
	buttons_state_prev = 0;
}

// static instances of controller data buffers
u_char Pad::pad_buffers[2][34];

// this needs to be called BEFORE graphics initialization it seems...
void Pad::init()
{
    ResetCallback();
    
    PadInitDirect( pad_buffers[0], pad_buffers[1] );
    PadStartCom();
}

void Pad::init_controller( int controller_num )
{
    int offs;
    int maxID;
    int set;

    // ports used by PadInfoMode use controller_num and 0x10 for some reason
    // instead of 0 or 1
    this->controller_num = controller_num ? 0x10 : 0x00;

    // ReInit Dual Shock if required
    printf("%d\n",*stored_mode);
    VSync(20);
    maxID = PadInfoMode(
        this->controller_num,
        InfoModeIdTable,
        -1
    );
    printf("Max:%d\n",maxID);
    VSync(5);

    for ( offs=0; offs < maxID; offs++ )
    {
        printf(
            "Entry:%d Searching for:%d, Mode:%d\n",
            offs,
            *stored_mode,
            PadInfoMode( this->controller_num, InfoModeIdTable, offs )
        );
    
        if ( PadInfoMode(
                this->controller_num,
                InfoModeIdTable,offs
            ) 
            == *stored_mode
        )
        {
            printf("Found Match:%d\n",offs);
            VSync(6);
            PadSetMainMode(
                this->controller_num,  // port number
                offs,                  // controller mode id table offset
                2                      // 2 means the selector button is locked
            );
            break;

        }
    }

    // Wait till it's stable
    set = 0;
    while ( !set )
    {
        // if the pad is ready
        if ( PadGetState(this->controller_num) == PadStateStable )
        {
            // if the pad has actuators (vibration motors)
            if ( PadInfoMode(    
                    this->controller_num,
                    InfoModeCurExID,    // current active controller mode id on a controller that has vibration
                    0
                )
            )
            {

                // sets the transmit buffer for the actuators
                PadSetAct(
                    this->controller_num,   // 0x00 for port 1, 0x10 for port 2
                    actuator_vals,          // pointer to the buffer to use
                    2                       // the size of the buffer
                );
                VSync(20);
                
                // sets actuator paramter details: the offset in the buffer set by PadSetAct
                // for each actuator. the buffer must be 6 bytes, but usually only the first 2
                // values are used. Which is why here, actuator_indices is {0,1,0xFF,0xFF,0xFF,0xFF}. The
                // 0 and 1 means the first actuator uses an offset of 0 (actuator_vals[0]) in the data buffer
                // and the second actuator uses an offset of 1 (actuator_vals[1]), while the remaining
                // values are unused
                while( PadSetActAlign(
                        this->controller_num,
                        actuator_indices
                    ) 
                    == 0
                )
                {
                    for(ctr=0;ctr<6;ctr++) {
                        VSync(0);
                    }
                }
            }
            set = 1;
        }
        // FindCTP1 - check for a controller that doesn't support vibration (no analog controller)
        if ( PadGetState(this->controller_num) == PadStateFindCTP1 )
        {
            printf("Found Normal Controller\n");
            set = 1;
        }
    }

    printf("Finished Dual Shock Setup\n");

} // end initShock()

void Pad::read()
{
    int     padState;
    u_char  set;
    int     currentPad;
	int     button_buffer;
    int     pad_index;
    
    // based on the controller number, the pad index is 0 or 1
    pad_index = controller_num ? 1 : 0;

    padState = PadGetState(controller_num);
	
	// default values to 0
	// but 'center' is 128 so set that
	analog_vals[0] = 128;
	analog_vals[1] = 128;
	analog_vals[2] = 128;
	analog_vals[3] = 128;
    
    // handle a disconnected controller
    if (padState == PadStateDiscon)
	{
        return;
    }
    
    // handle an initializing controller
    if ( padState == PadStateFindPad )
    {
        // Set actuators if Dual Shock or old vibrator
        set = 0;
        while ( !set )
        {
            // reinitialize the controller
            if ( PadGetState(controller_num) == PadStateStable )
            {
                if ( PadInfoMode(controller_num,InfoModeCurExID,0) )
                {
                    PadSetAct( controller_num, actuator_vals, 2 );
                    
                    while( PadSetActAlign(controller_num,actuator_indices) 
                        == 0
                    )
                    {
                        for ( ctr = 0; ctr < 6; ctr++ ) {
                            VSync(0);
                        }
                    }
                }
                set = 1;
            }
            if ( PadGetState(controller_num) == PadStateFindCTP1 )
            {
                printf("Found Normal Controller\n");
                set = 1;
            }


        }
        return;
    }

    // ignore received data when communication failed
    if ( *pad_buffers[pad_index] )
    {
        return;
    }
    
    // read the button data from the controller buffer
    button_buffer = ~((pad_buffers[pad_index][2]<<8) | 
                    pad_buffers[pad_index][3]);
	
	// from Orion's psxlib 
	buttons_state_prev = button_buffer & (button_buffer ^ buttons_state);
	buttons_state      = button_buffer;
    
    // get the analog controller values from the controller buffer
    currentPad = PadInfoMode( controller_num, InfoModeCurExID, 0 );
    
    // 7 seems to be the analog button light is on/active
    if ( currentPad == 7 )
    {
        analog_vals[0] = pad_buffers[pad_index][4];
        analog_vals[1] = pad_buffers[pad_index][5];
        analog_vals[2] = pad_buffers[pad_index][6];
        analog_vals[3] = pad_buffers[pad_index][7];
    }
    // set analog values to zero otherwise
    //  currentPad == 4 seems to mean there are analog sticks but the analog button light is off/inactive
    else if (currentPad == 4)
    {
        // default values to 0
		// but 'center' is 128 so set that
		analog_vals[0] = 128;
		analog_vals[1] = 128;
		analog_vals[2] = 128;
		analog_vals[3] = 128;
    }
}

bool Pad::is_held(u_int button)
{
	return buttons_state & button;
}

bool Pad::is_clicked(u_int button)
{
	return buttons_state_prev & button;
}

int Pad::get_left_analog_x()
{
	// convert from 0..255 to -128..127
	return analog_vals[2] - 128;
}

int Pad::get_left_analog_y()
{
	// convert from 0..255 to -128..127
	return analog_vals[3] - 128;
}

int Pad::get_right_analog_x()
{
	// convert from 0..255 to -128..127
	return analog_vals[0] - 128;
}

int Pad::get_right_analog_y()
{
	// convert from 0..255 to -128..127
	return analog_vals[1] - 128;
}



void Pad::set_actuator_val( u_char actuator, int val )
{
    actuator_vals[actuator] = val;
}

int  Pad::get_actuator_val( u_char actuator )
{
    return actuator_vals[actuator];
}



