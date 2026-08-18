#include "CenterBox.h"
#include "ConsoleExtensions.h"

CenterBox::CenterBox(SRef<Console> console) {
	//assumes scene 3
	con = console;
	full_box_loops = 4;
	half_box_loops = 8;
	extra_half_loop = false;
}

CenterBox::CenterBox(SRef<Console> console, int full_box, int half_box, bool extra_half) {
	con = console;
	full_box_loops = full_box;
	half_box_loops = half_box;
	extra_half_loop = extra_half;
}

void CenterBox::ChangeFullBox(char to_char, ConsoleTypes::Color colour) {
	std::string str(8, to_char);
	if (to_char == '@') 	 { str = "@@@@@@@@"; }
	else if (to_char == '#') { str = "########"; }
	else					 { str = "........"; }
	WriteBox(str, colour, true, true);
}

void CenterBox::ChangeLeftHalf(char to_char, ConsoleTypes::Color colour) {
	std::string str(4, to_char);
	if (to_char == '@') 	 { str = "@@@@"; }
	else if (to_char == '#') { str = "####"; }
	else					 { str = "...."; }
	WriteBox(str, colour, true, false);
}

void CenterBox::ChangeRightHalf(char to_char, ConsoleTypes::Color colour) {
	std::string str(4, to_char);
	if (to_char == '@') 	 { str = "@@@@"; }
	else if (to_char == '#') { str = "####"; }
	else					 { str = "...."; }
	WriteBox(str, colour, false, true);
}

void CenterBox::WriteBox(std::string str, ConsoleTypes::Color colour, bool left, bool right) {

	//potential optimization: write these to a local buffer, copy it into the scene, then write to the console from the scene
	int x = (right and not left)? 40 : 36;
	for (int i = 0; i < 4; i++) {
		con->Write(x, 11 + i, str, colour); 
	} 
	if (left) { left_state = str[0]; }
	if (right) { right_state = str[0]; }
}

void CenterBox::GetNextState() {
	if (full_box_loops > 0) {
		if (increment % 2 == 1) {
			if (left_state == '@') { 
				ChangeFullBox('.', ConsoleTypes::Color::F_YELLOW_BRIGHT); }
			else { ChangeFullBox('.', ConsoleTypes::Color::F_GREEN); }
		}
		else if (increment < 12) { ChangeFullBox('@', ConsoleTypes::Color::F_YELLOW_BRIGHT); }
		else { ChangeFullBox('#', ConsoleTypes::Color::F_GREEN); }
		if (increment == 19) { full_box_loops--; }
	}
	else if (half_box_loops > 0) {
		if (transition_frame) { 
			ChangeFullBox('@', ConsoleTypes::Color::F_YELLOW_BRIGHT); 
			transition_frame = false; }
		if (increment % 2 == 1) {
			if (left_state == '@') { 
				ChangeLeftHalf('.', ConsoleTypes::Color::F_YELLOW_BRIGHT); }
			else { ChangeRightHalf('.', ConsoleTypes::Color::F_GREEN); }
		}
		else if (increment < 12) { ChangeLeftHalf('@', ConsoleTypes::Color::F_YELLOW_BRIGHT); }
		else { ChangeRightHalf('#', ConsoleTypes::Color::F_GREEN); }
		if (increment == 19) { half_box_loops--; }
	}
	else if (extra_half_loop) {
		if (increment % 2 == 1) { ChangeLeftHalf('.', ConsoleTypes::Color::F_YELLOW_BRIGHT); }
		else if (increment < 12) { ChangeLeftHalf('@', ConsoleTypes::Color::F_YELLOW_BRIGHT); }
	}
	else { ChangeLeftHalf('@', ConsoleTypes::Color::F_YELLOW_BRIGHT);	}

	increment++;
	increment %= 20;
}
