#include <iostream>
#include <vector>
#include "g_m_gen.h"

void main()
{
	vector<int> pc = {
		FLUTE, VIOLIN, ACCOUSTIC_PIANO, OBOE, ELECTRIC_BASS_FINGER, OVERDRIVEN_GUITAR, 1, OVERDRIVEN_GUITAR, -1
	};
	
	char file_name[500];
	cout << "MIDIファイルのパスを入力してください。\n";
	cin >> file_name;

	g_m_gen gg;
	if (!gg.create_g_m(file_name, pc))
	{
		cout << "failed";
	}
	else
	{
		cout << "正常に終了しました。";
	}
}