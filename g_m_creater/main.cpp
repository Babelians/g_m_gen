#include <iostream>
#include <vector>
#include "console_art.h"
#include "g_m_gen.h"

int main()
{
	vector<int> pc = {
		ACCOUSTIC_PIANO, FLUTE, VIOLIN, CELLO, HARPSICHORD, DISTORTION_GUITAR,DISTORTION_GUITAR,
		ACCOUSTIC_GUITAR_STEEL, ELECTRIC_BASS_FINGER, DRUMS
	};
	cout << "================\n"
		<< "-----GM-Gen-----\n"
		<< "================\n";
	cout << art;
	char file_name[500];
	cout << "Input your MIDI file pass. Also you can drag and drop the file to input the pass.\n";
	cin >> file_name;

	g_m_gen gg;
	if (!gg.create_g_m(file_name, pc))
	{
		cout << "failed!";
		return -1;
	}
	else
	{
		cout << "successfully finished.";
		return 0;
	}
}