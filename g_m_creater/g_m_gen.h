#pragma once

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

#define len(x) sizeof(x) / sizeof(x[0])

const int32_t ACCOUSTIC_PIANO=1,ELECTRIC_BASS_FINGER = 21,OVERDRIVEN_GUITAR=30,DISTORTION_GUITAR=31,VIOLIN=41,OBOE=69, FLUTE=74;

class g_m_gen
{
public:
	bool create_g_m(char file_name[], vector<int32_t>program_changes);

private:
	void read_midi(char* file_name);
	void write_midi(char* file_name);
	char* file_name;
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

	bool read(ifstream& fin, int32_t* data, int32_t len)
	{
		for (int i = 0; i < len; i++)
		{
			fin.read(reinterpret_cast<char*>(&data[i]), 1);
			//data[i] -= correction;
		}

		return true;
	}

	bool write(ofstream& fout, int32_t* data, int32_t len)
	{
		for (int i = 0; i < len; i++)
		{
			fout.write(reinterpret_cast<char*>(&data[i]), 1);
			//data[i] += correction;
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
					cout << "delete start\n";
					track.data.erase(track.data.begin() + start_index, track.data.begin() + i + 1); // 既存のプログラムチェンジを削除
					update_track_size(track); cout << "deleted instrument\n";
					i -= 2;
					break;
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
};
