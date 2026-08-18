#ifndef CENTERBOX
#define CENTERBOX
#include "Console.h"


class CenterBox {

	/*
	blinking patterns: 
	
	scene 3        
	le ri le ri    
	@. @. #. #. x4
	@. @@ .. #. x1
	@. .. .. #. x7
	@. .. (1 frame)

	scene 8
	le ri le ri
	@. @. #. #. x12
	@. @@ .. #. x1
	@. .. .. #. x2
	@. .. (x1 but cut off by the song ending)
	
	each loop is 2.666 seconds, which is also 8 song beats
	@ is always light yellow and blinks slower (12 times over 2.00 seconds, switches every 0.1666 seconds)
	# is always green		 and blinks faster ( 8 times over 0.66 seconds, switches every 0.0833 seconds)
	. is always the same colour as what was last there
	box is 8x4
	box top left is (36, 12) with (0, 0) as the top left character on screen

	the changes made during the transition loops are the same as the half box loops so i'm going to lump them together
	*/

	int full_box_loops, half_box_loops, increment = 0;
	bool extra_half_loop, transition_frame = true;
	char left_state = '@', right_state = '@';
	SRef<Console> con;

	void ChangeFullBox(char to_char, ConsoleTypes::Color colour);
	void ChangeLeftHalf(char to_char, ConsoleTypes::Color colour);
	void ChangeRightHalf(char to_char, ConsoleTypes::Color colour);
	void WriteBox(std::string str, ConsoleTypes::Color colour, bool left, bool right);

	public: 
	void GetNextState();
	CenterBox(SRef<Console> console);
	CenterBox(SRef<Console> console, int full_box, int half_box, bool extra_half);

};

#endif