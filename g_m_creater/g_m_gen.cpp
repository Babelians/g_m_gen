#include "g_m_gen.h"

bool g_m_gen::create_g_m(char file_name[], vector<int32_t>program_changes)
{
	read_midi(file_name);

	for (int32_t i = 0; i < sm.tracks.size(); i++)
	{
		insert_program_change(sm.tracks[i], program_changes[i]);
	}

	char ofname[200] = "aaaaa.mid";
	write_midi(ofname);

	return true;
}

void g_m_gen::read_midi(char* file_name)
{
	ifstream fin(file_name, ios::in | ios::binary);
	if (!fin) {
		cout << "failed to read midi" << "\n";
	}
	else
	{
		this->file_name = file_name;
	}

	// メタデータ
	fin.read(sm.mthd, 4);
	read(fin, sm.header_size, len(sm.header_size));
	read(fin, sm.format, len(sm.format));
	read(fin, sm.track_size, len(sm.track_size));
	read(fin, sm.bpm, len(sm.bpm));
	
	// トラック数をリサイズ
	int32_t track_size = hex_to_decimal(sm.track_size, len(sm.track_size));
	sm.tracks.resize(track_size);

	
	// トラックデータの読み取り
	for (int32_t i = 0; i < track_size; i++)
	{
		fin.read(sm.tracks[i].mtrk, 4);
		read(fin, sm.tracks[i].size, 4);

		int32_t data_size = hex_to_decimal(sm.tracks[i].size, len(sm.tracks[i].size));
		sm.tracks[i].data.resize(data_size);
		read(fin, sm.tracks[i].data.data(), data_size);
	}

	fin.close();
}

void g_m_gen::write_midi(char* file_name)
{
	ofstream fout(file_name, ios::out | ios::binary);
	if (!fout) {
		return;
	}
	fout.write(sm.mthd, 4);
	write(fout, sm.header_size, 4);
	write(fout, sm.format, 2);
	write(fout, sm.track_size, 2);
	write(fout, sm.bpm, 2);

	int32_t track_size = hex_to_decimal(sm.track_size, len(sm.track_size));

	for (int32_t i = 0; i < track_size; i++)
	{
		fout.write(sm.tracks[i].mtrk, 4);
		write(fout, sm.tracks[i].size, 4);
		write(fout, sm.tracks[i].data.data(), sm.tracks[i].data.size());
	}

	fout.close();
}