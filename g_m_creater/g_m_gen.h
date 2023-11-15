#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iterator>

using namespace std;

#define len(x) sizeof(x) / sizeof(x[0])

template <typename T> int32_t length(T list)
{
	int32_t i = 0;
	while (list[i])
	{
		i++;
	}

	return i;
}

const int32_t DRUMS=-1,ACCOUSTIC_PIANO=0x00, HARPSICHORD=0x06,ACCOUSTIC_GUITAR_STEEL=0x1B, OVERDRIVEN_GUITAR=0x19, DISTORTION_GUITAR = 0x1E,
			  ELECTRIC_BASS_FINGER = 0x21,VIOLIN=0x28,VIOLA=0x29, CELLO=0x2A, OBOE=0x44, FLUTE=0x49;

class g_m_gen
{
public:
	g_m_gen();
	~g_m_gen();
	bool create_g_m(char* file_path, vector<int32_t>program_changes);

private:

	struct track
	{
		char mtrk[4];
		int32_t size[4];
		vector<int32_t>data;
	};

	struct standard_midi
	{
		char mthd[4];
		int32_t header_size[4];
		int32_t format[2];
		int32_t track_size[2];
		int32_t bpm[2];
		vector<track>tracks;
	};

	standard_midi sm;

	void read_midi(char* file_path);
	void write_midi(char* file_path);
	char* hoge; //このような何らかのポインタ変数を定義しないとバイナリファイルの各バイトにccccccがついて値がおかしくなる。

	bool read(ifstream& fin, int32_t* data, int32_t len)
	{
		for (int i = 0; i < len; i++)
		{
			fin.read(reinterpret_cast<char*>(&data[i]), 1);
		}

		return true;
	}

	bool write(ofstream& fout, int32_t* data, int32_t len)
	{
		for (int i = 0; i < len; i++)
		{
			fout.write(reinterpret_cast<char*>(&data[i]), 1);
		}

		return true;
	}

	void to_binary(int32_t n, vector<int32_t>&binary)
	{
		do {
			int32_t bin = n % 2;
			n /= 2;
			binary.insert(binary.begin(), bin);
		} while (1 <= n);
	}

	void update_track_size(track& track)
	{
		decimal_to_hex(track.data.size(), track.size, 4);
	}

	int32_t read_time(int32_t* data, int32_t& idx)
	{
		vector<int32_t>times;
		vector<int32_t>binary_times;

		bool still_time_part = true;
		while (still_time_part)
		{
			idx++;
			times.push_back(data[idx]);
			if (data[idx] < 128) //8bit目に1が立っていないとき
			{
				still_time_part = false;
			}
		}
		int32_t time = hex_to_decimal(times.data(), times.size());
		to_binary(time, binary_times); // 可変長2進数にする

		int32_t binary_power = 1;
		int32_t result = 0;
		for (int32_t i = 0; i < binary_times.size(); i++)
		{
			int32_t index = binary_times.size() - 1 - i;
			if (i % 8 != 7) // フラグビットを無視する
			{
				result += binary_power * binary_times[index];
				binary_power *= 2;
			}
		}

		return result;
	}

	void insert_program_change(track& track, int32_t sound_name)
	{
		int32_t i = -1;
		bool end_flug = false;
		while (!end_flug)
		{
			int32_t start_index = i + 1;
			int32_t time = read_time(track.data.data(), i);
			i++;
			if (track.data[i] == 0xFF) // メタイベント
			{
				i++;
				int32_t event_type = track.data[i];
				if (event_type == 0x2F)
				{
					end_flug = true; cout << "end point\n";
					break;
				}
				int32_t data_len = read_time(track.data.data(), i);
				i += data_len;
			}
			else
			{
				if (0x80 <= track.data[i] && track.data[i] <= 0xBF)
				{
					i += 2;
				}
				else if(0xC0 <= track.data[i] && track.data[i] <= 0xCF) //プログラムチェンジ
				{
					cout << "try to delete program change : " << hex << track.data[i+1] << "\n";
					track.data.erase(track.data.begin() + start_index, track.data.begin() + i + 2); // 既存のプログラムチェンジを削除
					update_track_size(track); cout << "deleted instrument\n";
					i -= 2;
					//break;
				}
				else if (0xD0 <= track.data[i] && track.data[i] <= 0xDF) //チャンネルプレッシャー
				{
					i++;
				}
				else if (0xE0 <= track.data[i] && track.data[i] <= 0xEF)
				{
					i += 2;
				}
				else
				{
					cout << "encounted unkown event\n";
				}
			}
		}

		int32_t pc[3] = {
			0x00, 0xC0, sound_name
		};

		if (sound_name < 0)// ドラムの時
		{
			pc[1] = 0xCA;
			pc[2] = 0x00;
		}

		for (int32_t i = 0; i < len(pc); i++)
		{
			int32_t index = len(pc) - 1 - i;
			track.data.insert(track.data.begin(), pc[index]);
		}

		update_track_size(track);
	}


	// リトルエンディアン
	int32_t hex_to_decimal(int32_t* data, int32_t len)
	{
		int32_t sum = 0;
		for (int32_t i = 0; i < len; i++)
		{
			int32_t index = len - 1 - i;
			if (data[index] < 0)cout << hex << data[index] << "\n";
			sum += power(16, 2 * i) * data[index];
		}

		if (sum < 0)cout << "minus\n";

		return sum;
	}

	void decimal_to_hex(int32_t dcml, int32_t* hx, int32_t len)
	{
		for (int32_t i = 0; i < len; i++)
		{
			int32_t index = len - 1 - i;
			int32_t n = dcml % (16 * 16);
			hx[index] = n;
			dcml /= (16 * 16);

			if (dcml <= 0) { return; };
		}
	}

	int32_t figures(int32_t n)
	{
		int32_t i = 0;
		while (1 <= n)
		{
			n /= 10;
			i++;
		}
		return i;
	}

	int32_t power(int32_t a, int32_t b)
	{
		if (b == 0) { return 1; }

		int32_t mul = 1;
		for (int32_t i = 0; i < b; i++)
		{
			mul *= a;
		}

		return mul;
	}

	char* get_file_name(char* path, int32_t len)
	{
		int32_t backslash_size = 0;
		int32_t file_name_len = 0;

		for (int32_t i = 0; i < len; i++)
		{
			int32_t idx = len - 1 - i;
			if (path[idx] == '\\')
			{
				file_name_len = i;
				break;
			}
		}

		char* file_name = new char[file_name_len];

		int32_t i = 0;
		int32_t path_idx = len - 1 - i;
		while (path[path_idx] != '\\')
		{   
			path_idx = len - 1 - i;
			int32_t fname_idx = file_name_len - 1 - i;
			file_name[fname_idx] = path[path_idx];
			i++;
		}

		return file_name;
	}

	char* add_file_name(char* file_name, const char* add)
	{
		int32_t file_name_len = 0;
		while (file_name[file_name_len])
		{
			file_name_len++;
		}

		int32_t add_len = 0;
		while (add[add_len])
		{
			add_len++;
		}
		int32_t ofname_len = file_name_len + add_len;
		char* ofname = new char[ofname_len];

		int32_t idx = file_name_len - 1;
		int32_t ofname_idx = ofname_len - 1;

		for (int32_t i = 0; i < file_name_len; i++)
		{
			ofname[ofname_idx] = file_name[idx];

			if (ofname[ofname_idx] == '.')
			{
				for (int32_t j = 0; j < add_len; j++)
				{
					int32_t jdx = add_len - 1 - j;
					ofname_idx--;
					ofname[ofname_idx] = add[jdx];
				}
			}
			idx--;
			ofname_idx--;
		}

		ofname[ofname_len] = NULL;

		return ofname;
	}
};
