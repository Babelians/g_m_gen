#include "g_m_gen.h"

g_m_gen::g_m_gen()
{

}

g_m_gen::~g_m_gen()
{

}

bool g_m_gen::create_g_m(char* file_path, vector<int32_t>program_changes)
{
	read_midi(file_path);

	for (int32_t i = 0; i < sm.tracks.size(); i++)
	{
		int32_t conductor_track = 0;
		if (i != conductor_track)
		{
			insert_program_change(sm.tracks[i], program_changes[i - 1]);
		}
	}

	const int max_fn = 500;
	char* file_name;
	file_name = get_file_name(file_path, max_fn);

	char* ofname = add_file_name(file_name, "_GM");

	write_midi(ofname);

	return true;
}

void g_m_gen::read_midi(char* file_path)
{
	ifstream fin(file_path, ios::in | ios::binary);

	// メタデータ
	fin.read(sm.mthd, 4);
	read(fin, sm.header_size, len(sm.header_size));
	read(fin, sm.format, len(sm.format));
	read(fin, sm.track_size, len(sm.track_size)); cout << dec << "track size :" << hex_to_decimal(sm.track_size, len(sm.track_size)) - 1 << "\n";
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

void g_m_gen::write_midi(char* file_path)
{
	ofstream fout(file_path, ios::out | ios::binary);
	if (!fout) {
		cout << "failed to create gm\n";
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