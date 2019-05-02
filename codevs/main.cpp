#define _USE_MATH_DEFINES
#include <array>
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <cstring>
#include <cmath>
#include <stack>
#include <iomanip>
#include <tuple>
#include <functional>
#include <cfloat>
#include <fstream>
#include <cassert>
#include <chrono>
#include <map>
#include <unordered_set>
#include <cstdint>
#include <sstream>
#include <set>
#include <unordered_set>
#ifdef _MSC_VER
#include <intrin.h>
#endif

//#define PRINT_RENSA
//#define SPEED_MODE

int MAX(int a, int b) { return (((a) > (b)) ? (a) : (b)); }
int MIN(int a, int b) { return (((a) < (b)) ? (a) : (b)); }
double MAX(double a, double b) { return (((a) > (b)) ? (a) : (b)); }
double MIN(double a, double b) { return (((a) < (b)) ? (a) : (b)); }

#define ARY_SORT(a, size) sort((a), (a)+(size))
#define SORT(v) sort((v).begin(), (v).end())
#define RSORT(v) sort((v).rbegin(), (v).rend())

#define ASSERT_DEBUG
#ifdef ASSERT_DEBUG
#define ASSERT(cond) assert(cond)
#else
#define ASSERT(cond)
#endif

int xorshift32_s = 7;
unsigned int xorshift32()
{
	xorshift32_s ^= (xorshift32_s << 13);
	xorshift32_s ^= (xorshift32_s >> 17);
	xorshift32_s ^= (xorshift32_s << 15);
	return xorshift32_s;
}

typedef unsigned int uint;
typedef long long ll;
typedef unsigned long long ull;
typedef unsigned long u64;
typedef unsigned char byte;

#ifdef _MSC_VER
inline unsigned long __builtin_clzll(ull x) { unsigned long r; _BitScanReverse64(&r, x); return 63 - r; }
#endif // _MSC_VER

inline unsigned int bsr(ull v) { return 63 - __builtin_clzll(v); } // 最上位の1は下から数えて何ビット目か？


using namespace std;
using namespace chrono;

int _dx[] = { 1, 0, -1, 0, 1, 1, -1, -1 };
int _dy[] = { 0, 1, 0, -1, 1, -1, 1, -1 };

const int WIDTH = 10;
const int HEIGHT = 16;
const int BOTTOM = HEIGHT - 1;
const int BLOCK_NUM = 500;
const int OJAMA = 11;

const int SKILL_GAIN = 8;
const int SKILL_COST = 80;
const int SKILL_MAX = 100;

const u64 mask4 = 0b1111;

const int MAX_TURN = 15;

int CHAIN_OJAMA_TABLE[] = { 0,1,2,3,4,6,9,13,18,25,33,45,60,79,105,138,181,237,310,405,528,689,897,1168,1521,1979,2575,3350,4358,5667,7370,9583,12461,16202,21066,27389,35609,46295,60186,78245,101722,132242,171919,223498,290551,377720,491040,638356,829867, };

bool IsIn(int x, int y)
{
	return 0 <= x && x < WIDTH && 0 <= y && y < HEIGHT;
}


class Point
{
public:
	int x;
	int y;

	Point() { this->x = 0; this->y = 0; }
	Point(int x, int y) { this->x = x; this->y = y; }
};


ull rotl(ull n, int shift)
{
	//return _rotl64(n, shilft);
	return (n << shift) | (n >> (64 - shift));
}

//1 2
//3 4
//[4][3][2][1]

class State
{
public:
	ull map[WIDTH]; //上___下
	int ojama;
	int skill;

	int prev_drop_x;
	int erase_min_x;
	int erase_max_x;

	array<byte, MAX_TURN> pos_history;
	array<byte, MAX_TURN> rot_history;
	array<byte, MAX_TURN> chain_history;

	double score;
	bool operator < (const State& obj) const
	{
		return (score < obj.score);
	}

	ull GetHash()
	{
		ull a = rotl(map[1], 4);

		ull hash = 0;
		hash ^= map[0] * 1;
		hash ^= rotl(map[1], 30) * 11ull;
		hash ^= rotl(map[2], 5) * 13ull;
		hash ^= rotl(map[3], 35) * 17ull;
		hash ^= rotl(map[4], 10) * 19ull;
		hash ^= rotl(map[5], 40) * 23ull;
		hash ^= rotl(map[6], 15) * 29ull;
		hash ^= rotl(map[7], 45) * 31ull;
		hash ^= rotl(map[8], 20) * 37ull;
		hash ^= rotl(map[9], 50) * 39ull;
		return hash;
	}

	int Get(int x, int y)
	{
		return ((map[x] >> (y * 4)) & mask4);
	}

	void SetBit(int x, int y, int n)
	{
		int shift = (y * 4);
		map[x] &= (~((ull)mask4 << shift));
		map[x] |= ((ull)n << shift);
	}


	bool Drop(int block, int pos, int rot, ull* check)
	{
		ASSERT(0 <= rot && rot < 4);
		ASSERT(0 <= pos && pos < WIDTH - 1);

		int l = 0;
		int r = 0;

		switch (rot)
		{
		case 0:
			//1 2
			//3 4
			l = ((((block >> 0) & mask4) << 4) | ((block >> 8) & mask4));
			r = ((((block >> 4) & mask4) << 4) | ((block >> 12) & mask4));
			break;
		case 1:
			//3 1
			//4 2
			l = ((((block >> 8) & mask4) << 4) | ((block >> 12) & mask4));
			r = ((((block >> 0) & mask4) << 4) | ((block >> 4) & mask4));
			break;
		case 2:
			//4 3
			//2 1
			l = ((((block >> 12) & mask4) << 4) | ((block >> 4) & mask4));
			r = ((((block >> 8) & mask4) << 4) | ((block >> 0) & mask4));
			break;
		case 3:
			//2 4
			//1 3
			l = ((((block >> 4) & mask4) << 4) | ((block >> 0) & mask4));
			r = ((((block >> 12) & mask4) << 4) | ((block >> 8) & mask4));
			break;
		}

		if ((l & mask4) == 0) l >>= 4;
		if ((r & mask4) == 0) r >>= 4;

		*check = -1;
		if (!DropLine(pos, l, check)) return false;
		if (!DropLine(pos + 1, r, check)) return false;

		return true;
	}


	int Put(int block, int pos, int rot)
	{
		ull check;
		bool can_drop = Drop(block, pos, rot, &check);

		if (can_drop)
		{
			int chain = Submit(check);
			if (chain >= 1)
			{
				skill += SKILL_GAIN;
				if (skill > SKILL_MAX) skill = SKILL_MAX;
			}
			return chain;
		}
		else
		{
			return -1;
		}
	}

	bool DropLine(int x, int block_line, ull*check)
	{
		if (block_line == 0) return 0;

		*check &= ~((ull)mask4 << (x * 4ull));

		if (map[x] == 0)
		{
			map[x] |= block_line;
		}
		else
		{
			int p = bsr(map[x]);
			p /= 4;
			p += 1;

			if (block_line >= 16 && p >= HEIGHT - 1)
				return false;
			if (block_line < 16 && p >= HEIGHT)
				return false;

			*check |= ((ull)p << (x * 4ull));

			p *= 4;
			map[x] |= ((ull)block_line << p);
		}

		return true;
	}

	int GetHeight(int x)
	{
		if (map[x] == 0)
		{
			return 0;
		}
		else
		{
			int p = bsr(map[x]);
			p /= 4;
			p += 1;
			return p;
		}
	}

	void DropBlock(int x, int drop_y, int number)
	{
		if (drop_y == 0)
		{
			map[x] |= number;
		}
		else
		{
			map[x] |= ((ull)number << (drop_y * 4));
		}
	}

	ull pext(ull val, ull mask)
	{
		int shift = 0;
		ull ret = 0;
		for (int i = 0; i < 16; i++)
		{
			if ((mask >> (i * 4)) & 1)
			{
				ret |= (((val >> (i * 4)) & mask4) << shift);
				shift += 4;
			}
		}
		return ret;
	}

	//ull pext(ull val, ll mask)
	//{
	//	ull res = 0;
	//	for (ull bb = 1; mask; bb += bb) {
	//		if (val & mask & -mask)
	//			res |= bb;
	//		mask &= mask - 1;
	//	}
	//	return res;
	//}

	int Submit(ull first_check, int *erase_cnt = NULL, int *erase_min_x = NULL, int *erase_max_x = NULL)
	{
		int chein = 0;

		if (erase_min_x != NULL)
		{
			*erase_min_x = WIDTH;
			*erase_max_x = -1;
		}

		ull check = first_check;

		while (true)
		{
			ull next = -1;

			static Point q[WIDTH * HEIGHT];
			int q_cnt = 0;

			ull erase_bit[WIDTH] = {};

			for (int x = 0; x < WIDTH; x++)
			{
				int bottom = ((check >> (x * 4)) & mask4);
				if (bottom < mask4)
				{
					for (int y = bottom; y < HEIGHT; y++)
					{
						for (int d = 0; d < 8; d++)
						{
							int dx = x + _dx[d];
							int dy = y + _dy[d];
							if (!IsIn(dx, dy)) continue;

							int sum = Get(x, y) + Get(dx, dy);

							if (sum == 10)
							{
								SubmitSub(q, &q_cnt, &next, x, y);
								SubmitSub(q, &q_cnt, &next, dx, dy);
							}
						}
					}
				}
			}

			if (q_cnt == 0) break;

			for (int i = 0; i < q_cnt; i++)
			{
				int x = q[i].x;
				int y = q[i].y;

				int shift = (y * 4);

				if (erase_cnt != NULL)
				{
					if (map[x] & ((ull)mask4 << shift))
					{
						(*erase_cnt)++;

						if (erase_min_x != NULL)
						{
							(*erase_min_x) = MIN(x, *erase_min_x);
							(*erase_max_x) = MAX(x, *erase_max_x);
						}
					}
				}

				map[x] &= (~((ull)mask4 << shift));
				erase_bit[x] |= ((ull)mask4 << shift);
			}

#ifdef PRINT_RENSA
			Print();
#endif

			for (int x = 0; x < WIDTH; x++)
			{
				map[x] = pext(map[x], ~erase_bit[x]);
			}

#ifdef PRINT_RENSA
			Print();
#endif

			chein++;
			check = next;
		}

		return chein;
	}

	void SubmitSub(Point *q, int *q_cnt, ull *next, int x, int y)
	{
		q[*q_cnt].x = x;
		q[*q_cnt].y = y;
		(*q_cnt)++;
		int next_bottom = (((*next) >> (x * 4)) & mask4);
		if (next_bottom > y)
		{
			next_bottom = y;
			(*next) &= ~((ull)mask4 << (x * 4));
			(*next) |= ((ull)y << (x * 4));
		}
	}

	void Print()
	{
		for (int y = HEIGHT - 1; y >= 0; y--)
		{
			for (int x = 0; x < WIDTH; x++)
			{
				cout << Get(x, y);
				cout << " ";
			}
			cout << endl;
		}
		cout << endl;
	}

	bool Ojama()
	{
		bool is_end = false;
		for (int x = 0; x < WIDTH; x++)
		{
			if (map[x] == 0)
			{
				map[x] |= OJAMA;
			}
			else
			{
				int p = bsr(map[x]);
				p /= 4;
				p += 1;

				if (p >= HEIGHT)
				{
					is_end = true;
					continue;
				}

				p *= 4;
				map[x] |= ((ull)OJAMA << p);
			}
		}
		return is_end;
	}

	int GetChainCount(int drop_x_start, int drop_x_end, int *erase_cnt, int *max_drop_x, int *erase_min_x, int* erase_max_x)
	{
		int chain_cnt = 0;

		for (int x = drop_x_start; x <= drop_x_end; x++)
		{
			int drop_y = GetHeight(x);
			if (drop_y >= HEIGHT)
			{
				continue;
			}

			ull check = -1;
			check &= ~((ull)mask4 << (x * 4));
			check |= ((ull)drop_y << (x * 4));

			for (int n = 1; n <= 9; n++)
			{
				State state = *this;
				state.DropBlock(x, drop_y, n);

				int erase = 0;
				int min_x;
				int max_x;
				int chain = state.Submit(check, &erase, &min_x, &max_x);
				if (chain_cnt < chain)
				{
					chain_cnt = chain;

					*max_drop_x = x;
					*erase_cnt = erase;
					*erase_min_x = min_x;
					*erase_max_x = max_x;
				}
			}
		}

		return chain_cnt;
	}


	int GetMaxY()
	{
		int max_y = 0;
		for (int y = 0; y < HEIGHT; y++)
		{
			for (int x = 0; x < WIDTH; x++)
			{
				int n = Get(x, y);
				if (n > 0)
				{
					max_y = MAX(max_y, y);
				}
			}
		}
		return max_y;
	}

	int GetYPenalty()
	{
		int max_y = GetMaxY();
		if (max_y >= HEIGHT - 1)
		{
			return -10000;
		}
		else if (max_y >= HEIGHT - 2)
		{
			return -1000;
		}
		//else if (max_y >= HEIGHT - 3)
		//{
		//	return -100;
		//}
		//else if (max_y >= HEIGHT - 4)
		//{
		//	return -10;
		//}
		//else if (max_y >= HEIGHT - 5)
		//{
		//	return -1;
		//}
		return 0;
	}

	int GetDistanceFromHalf(int x)
	{
		//WIDTH = 10

		if (x <= 4)
		{
			return 4 - x;
		}
		else
		{
			return x - 5;
		}
	}

	double GetScore()
	{

		int drop_x;
		int score_chain;
		int erase_min_x;
		int erase_max_x;
		return GetScore(-1, &drop_x, &score_chain, &erase_min_x, &erase_max_x);
	}

	double GetScore(int drop_x, int *max_drop_x, int *score_chain, int *erase_min_x, int *erase_max_x)
	{
		double score = 0.0;

		//int block_cnt = 0;
		double block_score = 0;
		for (int x = 0; x < WIDTH; x++)
		{
			for (int y = 0; y < HEIGHT; y++)
			{
				if (Get(x, y) > 0)
				{
					//block_cnt++;
					block_score += 1.0 - GetDistanceFromHalf(x) * 0.001;
				}
			}
		}

		int drop_x_start;
		int drop_x_end;
		if (drop_x == -1)
		{
			drop_x_start = 0;
			drop_x_end = WIDTH - 1;
		}
		else
		{
			drop_x_start = drop_x - 1;
			drop_x_end = drop_x + 1;
			drop_x_start = MAX(0, drop_x_start);
			drop_x_end = MIN(WIDTH - 1, drop_x_end);
		}

		int erase = 0;
		int chain = GetChainCount(drop_x_start, drop_x_end, &erase, max_drop_x, erase_min_x, erase_max_x);
		if (chain <= 1)
		{
			*max_drop_x = -1;
			*erase_min_x = 0;
			*erase_max_x = WIDTH - 1;
		}
		else
		{
			//ASSERT(0 <= half_x && half_x <= 5);
			score -= GetDistanceFromHalf(*max_drop_x) * 0.001;
		}
		*score_chain = chain;

		score += chain * 100.0;

		//score += block_cnt * 0.1;
		score += block_score * 0.1;
		if (chain > 0)
		{
			score -= erase / (double)chain * 1.0;
		}

		score += GetYPenalty();

		return score;
	}

	double GetSkillScore()
	{
		double score = 0.0;

		score += skill * 10.0;
		score += GetSkillOjama() * 0.01;
		score += GetYPenalty();

		return score;
	}

	int GetSkillOjama()
	{
		int cnt = 0;

		bool flg[WIDTH * HEIGHT] = {};

		for (int x = 0; x < WIDTH; x++)
		{
			for (int y = 0; y < HEIGHT; y++)
			{
				if (Get(x, y) == 5)
				{
					if (!flg[y * WIDTH + x])
					{
						flg[y * WIDTH + x] = true;
						cnt++;
					}
					for (int d = 0; d < 8; d++)
					{
						int dx = x + _dx[d];
						int dy = y + _dy[d];
						if (IsIn(dx, dy))
						{
							if (Get(dx, dy) != OJAMA && Get(dx, dy) > 0 && !flg[dy * WIDTH + dx])
							{
								flg[dy * WIDTH + dx] = true;
								cnt++;
							}
						}
					}
				}
			}
		}

		return (int)(floor(25 * pow(2, cnt / 12.0)) / 2.0);
	}
};

class Info
{
public:
	int turn;
	int time;
	int score;

	State state;
};

int _blocks[BLOCK_NUM];
int _turn;
Info _infos[2];

void Init()
{
	string end;

	for (int i = 0; i < BLOCK_NUM; i++)
	{
		int b[4];
		cin >> b[0] >> b[1];
		cin >> b[2] >> b[3];
		cin >> end;

		_blocks[i] = 0;
		for (int j = 0; j < 4; j++)
		{
			_blocks[i] |= (b[j] << (j * 4));
		}

		ASSERT(end == "END");
	}
}


void Input()
{
	cin >> _turn;
	if (_turn == -1) std::exit(1);

	for (int p = 0; p < 2; p++)
	{
		Info& info = _infos[p];
		cin >> info.time;
		cin >> info.state.ojama;
		cin >> info.state.skill;
		cin >> info.score;

		memset(info.state.map, 0, sizeof(info.state.map));

		for (int y = 0; y < HEIGHT; y++)
		{
			for (int x = 0; x < WIDTH; x++)
			{
				ull n;
				cin >> n;
				info.state.map[x] |= (n << ((BOTTOM - y) * 4));
			}
		}

		string end;
		cin >> end;
		ASSERT(end == "END");
	}


	//ojama
	for (int p = 0; p < 2; p++)
	{
		if (_infos[p].state.ojama >= 10)
		{
			_infos[p].state.Ojama();
			_infos[p].state.ojama -= 10;;
		}
	}
}


int GetRealChain(State& state, int turn)
{
	int max_chain = 0;
	for (int pos = 0; pos < WIDTH - 1; pos++)
	{
		for (int rot = 0; rot < 4; rot++)
		{
			State clone = state;
			int chain = clone.Put(_blocks[_turn], pos, rot);
			if (max_chain < chain)
			{
				max_chain = chain;
			}
		}
	}
	return max_chain;
}

//#define HASH_TEST
//#define DUMP_TEST

State GetBestState(int time_limit, int target_chain, int *play_best_turn, int *out_best_chain)
{
#ifdef DUMP_TEST
	map<int, vector<State>> _dump;
	map<int, vector<int>> _dump_turn;
	map<int, vector<int>> _dump_pos;
	map<int, vector<int>> _dump_rot;
	map<int, vector<double>> _dump_score;
	map<int, vector<int>> _dump_drop_x;
	map<int, vector<int>> _dump_chain;
#endif

#ifdef HASH_TEST
	map<ull, State> _hash_test;
#endif
	unordered_set<ull> _hash[MAX_TURN];

	time_point<system_clock> start_time = system_clock::now();

	//探索
	priority_queue<State> q[MAX_TURN + 1];

	_infos[0].state.prev_drop_x = -1;
	_infos[0].state.erase_min_x = 0;
	_infos[0].state.erase_max_x = WIDTH - 1;

	q[0].push(_infos[0].state);

	State best_state[MAX_TURN];
	int best_chain[MAX_TURN] = {};
	double best_score[MAX_TURN] = {};

	State emergency_state;
	emergency_state.score = -DBL_MAX;

	int turn_limit = MAX_TURN;

	//loop
	int loop = 0;
	int skip = 0;
	while (true)
	{
		ll ms = duration_cast<milliseconds>(system_clock::now() - start_time).count();
		if (ms >= time_limit)
		{
			break;
		}

		bool has_update = false;
		for (int t = 0; t < turn_limit; t++)
		{
			if (q[t].size() == 0) continue;
			has_update = true;

			State state = q[t].top();
			q[t].pop();

			int drop_min_x = MAX(0, state.erase_min_x - 3);
			int drop_max_x = MIN(WIDTH - 2, state.erase_max_x + 2);

			if (_turn == 0 && t == 0)
			{
				drop_min_x = 3;
				drop_max_x = 5;
			}

			for (int pos = drop_min_x; pos <= drop_max_x; pos++)
			{
				for (int rot = 0; rot < 4; rot++)
				{
					State clone = state;

#ifdef HASH_TEST
					ull test = clone.GetHash();
					auto it = _hash_test.find(test);
					if (it != _hash_test.end())
					{
						State& hash_state = it->second;
						for (int x = 0; x < WIDTH; x++)
						{
							for (int y = 0; y < HEIGHT; y++)
							{
								if (clone.Get(x, y) != hash_state.Get(x, y))
								{
									ASSERT(false);
								}
							}
						}
					}
					else
					{
						_hash_test[test] = clone;
					}
#endif

					int chain = clone.Put(_blocks[_turn + t], pos, rot);
					loop++;

					clone.pos_history[t] = pos;
					clone.rot_history[t] = rot;
					clone.chain_history[t] = 0;

					ull hash = clone.GetHash();
					if (_hash[t].find(hash) != _hash[t].end())
					{
						skip++;
#ifndef HASH_TEST
						continue;
#endif
					}
					_hash[t].insert(hash);

					if (clone.ojama >= 10)
					{
						if (clone.Ojama())
						{
							continue;
						}
						clone.ojama -= 10;
					}

					if (chain >= target_chain)
					{
						turn_limit = t + 1;
					}

					if (chain >= 0)
					{
						int drop_x;
						int score_chain;
						int erase_min_x;
						int erase_max_x;

						clone.score = clone.GetScore(clone.prev_drop_x, &drop_x, &score_chain, &erase_min_x, &erase_max_x);
						clone.prev_drop_x = drop_x;
						clone.chain_history[t] = score_chain;
						clone.erase_min_x = erase_min_x;
						clone.erase_max_x = erase_max_x;

						if (best_chain[t] < chain || (best_chain[t] == chain && best_score[t] < clone.score))
						{
							best_state[t] = clone;
							best_chain[t] = chain;
							best_score[t] = clone.score;
						}

						if (drop_x < 0)
						{
							clone.erase_min_x = pos;
							clone.erase_max_x = pos + 1;
						}

						if (emergency_state.score < clone.score)
						{
							emergency_state = clone;
						}

#ifdef DUMP_TEST
						_dump[t].push_back(state);
						_dump_turn[t].push_back(t);
						_dump_pos[t].push_back(pos);
						_dump_rot[t].push_back(rot);
						_dump_score[t].push_back(clone.score);
						_dump_drop_x[t].push_back(drop_x);
						_dump_chain[t].push_back(chain);
#endif

						if (chain <= 1)
						{
							//if (t < 2 || clone.chain_history[t - 2] < score_chain)
							//{
							//	q[t + 1].push(clone);
							//}

							if (t >= 2 && clone.chain_history[t - 2] >= score_chain)
							{
								int sub = clone.chain_history[t - 2] - score_chain;
								clone.score -= 1000.0 * sub;
							}

							if (t + 1 == turn_limit)
							{
								q[MAX_TURN].push(clone);
							}
							else
							{
								q[t + 1].push(clone);
							}
						}
					}
				}
			}
		}
		if (!has_update)
		{
			break;
		}
	}

#ifdef DUMP_TEST
	stringstream ss;
	for (int i = 0; i < MAX_TURN; i++)
	{
		cerr << i << " : " << _dump[i].size() << endl;
		ss << i << endl;
		ss << _dump[i].size() << endl;

		for (int j = 0; j < _dump[i].size(); j++)
		{
			State& state = _dump[i][j];

			ull check;
			int turn = _dump_turn[i][j];
			state.Drop(_blocks[_turn + turn], _dump_pos[i][j], _dump_rot[i][j], &check);

			ss << turn << endl;
			ss << _dump_chain[i][j] << endl;
			ss << _dump_drop_x[i][j] << endl;
			ss << std::setprecision(10) << _dump_score[i][j] << endl;
			for (int y = 0; y < HEIGHT; y++)
			{
				for (int x = 0; x < WIDTH; x++)
				{
					ss << state.Get(x, y) << " ";
				}
				ss << endl;
			}
		}
	}
	ofstream ofs("C:/project/codevs2019/codevs/dump.txt");
	ofs << ss.str();
	ofs.close();
	exit(0);
#endif

	*play_best_turn = 0;

	//int best_turn = -1;
	//double best_chain_score = 0;
	//for (int t = 0; t < MAX_TURN; t++)
	//{
	//	if (best_chain[t] >= target_chain)
	//	{
	//		double score = best_chain[t] / (double)(t + 1);
	//		if (score > best_chain_score)
	//		{
	//			best_chain_score = score;
	//			best_turn = t;
	//		}
	//	}
	//}
	//if (best_turn >= 0)
	//{
	//	*play_best_turn = best_turn;
	//	*out_best_chain = best_chain[best_turn];
	//	return best_state[best_turn];
	//}

	for (int t = 0; t < MAX_TURN; t++)
	{
		if (best_chain[t] >= target_chain)
		{
			*play_best_turn = t;
			*out_best_chain = best_chain[t];
			return best_state[t];
		}
	}

	if (q[MAX_TURN].size() == 0)
	{
		*play_best_turn = 0;
		*out_best_chain = 0;

		if (emergency_state.score > -(DBL_MAX / 2.0))
		{
			return emergency_state;
		}
		else
		{
			_infos[0].state.score = 0.0;
			_infos[0].state.pos_history[0] = 0;
			_infos[0].state.rot_history[0] = 0;
			return _infos[0].state;
		}
	}
	else
	{
		*play_best_turn = 3;
		*out_best_chain = 0;
		return q[MAX_TURN].top();
	}
}

bool IsMatch(ull *map1, ull*map2)
{
	for (int x = 0; x < WIDTH; x++)
	{
		if (map1[x] != map2[x])
		{
			return false;
		}
	}
	return true;
}

void PrintMap(State& state)
{
	for (int y = HEIGHT - 1; y >= 0; y--)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			int n = state.Get(x, y);
			if (n == 11)
			{
				cerr << "*" << " ";
			}
			else
			{
				cerr << state.Get(x, y) << " ";
			}
		}
		cerr << endl;
	}
}

State AllSearch(State& org_state, int rest_turn, int* play_turn, int* play_chain)
{
	State best_state = org_state;
	int best_chain = 0;
	int best_turn = 0;
	double best_score = 0.0;

	queue<State> q;
	queue<int> q_turn;

	q.push(org_state);
	q_turn.push(0);

	while (q.size() > 0)
	{
		State state = q.front();
		q.pop();
		int turn = q_turn.front();
		q_turn.pop();

		for (int pos = 0; pos < WIDTH - 1; pos++)
		{
			for (int rot = 0; rot < 4; rot++)
			{
				State clone = state;

				int chain = clone.Put(_blocks[_turn + turn], pos, rot);

				clone.pos_history[turn] = pos;
				clone.rot_history[turn] = rot;

				if (clone.ojama >= 10)
				{
					if (clone.Ojama())
					{
						continue;
					}
					clone.ojama -= 10;
				}

				if (chain >= 0)
				{
					if (best_chain == chain && best_turn == turn)
					{
						double score = clone.GetScore();
						if (score > best_score)
						{
							best_state = clone;
							best_score = score;
						}
					}
					else if ((best_chain < chain) || (best_chain == chain && best_turn > turn))
					{
						best_chain = chain;
						best_state = clone;
						best_turn = turn;
						best_score = clone.GetScore();
					}

					if (turn + 1 < rest_turn)
					{
						q.push(clone);
						q_turn.push(turn + 1);
					}
				}
			}
		}
	}

	*play_chain = best_chain;
	*play_turn = best_turn;
	return best_state;
}

int GetChain(State& state)
{
	int erase_cnt;
	int max_drop_x;
	int erase_min_x;
	int erase_max_x;
	return state.GetChainCount(0, WIDTH - 1, &erase_cnt, &max_drop_x, &erase_min_x, &erase_max_x);
}

const int OPENING_TURN = 10;
const int SKILL_TYPE_THRESHOLD = 6;
const int DANGER_SKILL_DAMAGE = 30;
const int KILL_CHAIN = 15;
const int ATTACK_CHAIN = 12;
const int SKILL_BREAK_CHAIN = 3;

bool _enemy_is_skill_type = true;

int GetTargetChain()
{
	int target_chain = ATTACK_CHAIN;
	if (_turn < OPENING_TURN)
	{
		target_chain = ATTACK_CHAIN;
	}
	else if (_infos[1].state.ojama >= 30 || _enemy_is_skill_type)
	{
		target_chain = KILL_CHAIN;
	}

	int enemy_height = 0;
	for (int x = 0; x < WIDTH; x++)
	{
		int h = 0;
		for (int y = 0; y < HEIGHT; y++)
		{
			if (_infos[1].state.Get(x, y) == OJAMA)
			{
				h++;
			}
		}
		enemy_height = MAX(enemy_height, h);
	}
	int enemy_top_space = HEIGHT - enemy_height;
	for (int i = 8; i < target_chain; i++)
	{
		int ojama_h = (_infos[1].state.ojama + CHAIN_OJAMA_TABLE[i]) / 10;
		if (ojama_h > enemy_top_space + 1)
		{
			cerr << "=== update chain : " << i << " ===" << endl;
			target_chain = i;
			break;
		}
	}
	return target_chain;
}

State _play_state;
int _play_turn = -1;
int _play_turn_rest = -1;
int _play_chain = 0;

void NextPlayState(int time_limit, int target_chain)
{
	int ally_next_turn = 0;
	int ally_next_chain = 0;
	State ally_next = AllSearch(_infos[0].state, 1, &ally_next_turn, &ally_next_chain);

	int enemy_next_turn = 0;
	int enemy_next_chain = 0;
	State enemy_next = AllSearch(_infos[1].state, 1, &enemy_next_turn, &enemy_next_chain);

	int ally_nexnex_turn = 0;
	int ally_nexnex_chain = 0;
	State ally_nexnex = AllSearch(_infos[0].state, 2, &ally_nexnex_turn, &ally_nexnex_chain);

	int enemy_nexnex_turn = 0;
	int enemy_nexnex_chain = 0;
	State enemy_nexnex = AllSearch(_infos[1].state, 2, &enemy_nexnex_turn, &enemy_nexnex_chain);

	int ally_skill = _infos[0].state.GetSkillOjama();
	int enemy_skill = _infos[1].state.GetSkillOjama();

	//スキルにかかるターン
	//81 -> -1
	//80 -> 0
	//73 -> 1
	//72 -> 1
	//71 -> 2
	int enemy_skill_rest_turn;
	if (_infos[1].state.skill >= 80)
	{
		enemy_skill_rest_turn = 0;
	}
	else
	{
		enemy_skill_rest_turn = ((SKILL_COST - _infos[1].state.skill - 1) / SKILL_GAIN) + 1;
	}

	//スキルつぶし
	if (enemy_skill >= DANGER_SKILL_DAMAGE && enemy_skill_rest_turn <= 2)
	{
		if (enemy_skill_rest_turn == 2)
		{
			if (ally_nexnex_chain >= SKILL_BREAK_CHAIN)
			{
				cerr << "&&& skill break 2 &&&" << endl;
				_play_turn = 0;
				_play_turn_rest = ally_nexnex_turn;
				_play_chain = ally_nexnex_chain;
				_play_state = ally_nexnex;
				return;
			}
		}
		else
		{
			if (ally_next_chain >= SKILL_BREAK_CHAIN)
			{
				cerr << "&&& skill break 1 &&&" << endl;
				_play_turn = 0;
				_play_turn_rest = ally_next_turn;
				_play_chain = ally_next_chain;
				_play_state = ally_next;
				return;
			}
		}
	}


	//再計算
	if (_play_turn_rest < 0)
	{
		int best_play_turn;
		int best_chain;
		State state = GetBestState(time_limit, target_chain, &best_play_turn, &best_chain);

		_play_state = state;
		_play_turn = 0;
		_play_turn_rest = best_play_turn;
		_play_chain = best_chain;

		cerr << "*** ch:" << _play_chain << " pl:" << best_play_turn << " score:" << fixed << setprecision(4) << _play_state.score << endl;
		return;
	}

}

int main()
{
	cout << "y_kawano" << endl;
	Init();

	State work_state;
	memset(_play_state.map, -1, sizeof(_play_state.map));

	while (true)
	{
		Input();

		cerr << endl;

		int ally_chain = GetChain(_infos[0].state);
		int ally_real_chain = GetRealChain(_infos[0].state, _turn);
		int ally_skill = _infos[0].state.GetSkillOjama();
		int enemy_chain = GetChain(_infos[1].state);
		int enemy_real_chain = GetRealChain(_infos[1].state, _turn);
		int enemy_skill = _infos[1].state.GetSkillOjama();
		cerr << "t:" << _turn 
			<< " a_:" << _infos[0].state.skill << " e_:" << _infos[1].state.skill 
			<< " ao:" << _infos[0].state.ojama << " eo:" << _infos[1].state.ojama
			<< endl;

		if (enemy_chain >= SKILL_TYPE_THRESHOLD)
		{
			_enemy_is_skill_type = false;
		}

		//print
		if (_enemy_is_skill_type) cerr << "!";
		cerr << "pc:" << _play_chain << " re:" << _play_turn_rest << " as:" << ally_skill << " es:" << enemy_skill << endl;
		cerr << "ac:" << ally_chain << " ec:" << enemy_chain << " arc:" << ally_real_chain << " erc:" << enemy_real_chain << endl;

		//time_limit
		int time_limit = 10000;
		if (_turn == 0)
		{
			time_limit = 18000;
		}
		else if (_infos[0].time <= 100000)
		{
			time_limit = 5000;
		}

		//is_match
		if (_play_turn_rest >= 0 && !IsMatch(_infos[0].state.map, work_state.map))
		{
			cerr << "((( not match )))" << endl;
			_play_turn = -1;
			_play_turn_rest = -1;
		}

		//copy
		work_state = _infos[0].state;

		//target_chain
		int target_chain = GetTargetChain();

		//update_state
		NextPlayState(time_limit, target_chain);

		//end
		cout << (int)_play_state.pos_history[_play_turn] << " " << (int)_play_state.rot_history[_play_turn] << endl;
		work_state.Put(_blocks[_turn], _play_state.pos_history[_play_turn], _play_state.rot_history[_play_turn]);

		if (work_state.ojama >= 10)
		{
			work_state.Ojama();
			work_state.ojama -= 10;
		}

		_play_turn_rest--;
		_play_turn++;
	}
}
