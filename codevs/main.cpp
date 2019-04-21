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
#define SPEED_MODE

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

const int MAX_TURN = 8;

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

//1 2
//3 4
//[4][3][2][1]

class State
{
public:
	ull map[WIDTH]; //上___下
	int ojama;
	int skill;

	array<byte, MAX_TURN> pos_history;
	array<byte, MAX_TURN> rot_history;
	double score;
	bool operator < (const State& obj) const
	{
		return (score < obj.score);
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


	int Put(int block, int pos, int rot)
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

		ull check = -1;
		if (!DropLine(pos, l, &check)) return -1;
		if (!DropLine(pos + 1, r, &check)) return -1;

		int chain = Submit(check);
		if (chain >= 1)
		{
			skill += SKILL_GAIN;
			if (skill > SKILL_MAX) skill = SKILL_MAX;
		}
		return chain;
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

	int GetDropY(int x)
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

	int Submit(ull first_check, int *erase_cnt = NULL)
	{
		int chein = 0;

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
			(*next) |= (y << (x * 4));
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
					return false;

				p *= 4;
				map[x] |= ((ull)OJAMA << p);
			}
		}

		return true;
	}

	int GetChainCount(int *erase_cnt = NULL, int *max_drop_x = NULL)
	{
		int chain_cnt = 0;

		for (int x = 0; x < WIDTH; x++)
		{
			int drop_y = GetDropY(x);
			if (drop_y >= HEIGHT)
			{
				continue;
			}

			ull check = -1;
			check &= ~((ull)mask4 << (x * 4));
			check |= (drop_y << (x * 4));

			for (int n = 0; n <= 9; n++)
			{
				State state = *this;
				state.DropBlock(x, drop_y, n);

				int erase = 0;
				int chain = state.Submit(check, &erase);
				if (chain_cnt < chain)
				{
					chain_cnt = chain;
					if (erase_cnt != NULL)
					{
						*max_drop_x = x;
						*erase_cnt = erase;
					}
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
		else if (max_y >= HEIGHT - 3)
		{
			return -100;
		}
		else if (max_y >= HEIGHT - 4)
		{
			return -10;
		}
		else if (max_y >= HEIGHT - 5)
		{
			return -1;
		}
		return 0;
	}

	double GetScore()
	{
		double score = 0.0;

		int block_cnt = 0;
		for (int x=0; x<WIDTH; x++)
		{
			for (int y = 0; y < HEIGHT; y++)
			{
				if (Get(x, y) > 0)
				{
					block_cnt++;
				}
			}
		}

		int erase = 0;
		int max_x = 0;
		int chain = GetChainCount(&erase, &max_x);

		score += chain * 10;
		//score += ((abs((WIDTH / 2.0) - max_x)) * 0.000001);

		score += block_cnt * 0.1;
		if (chain > 0)
		{
			score -= erase / (double)chain * 0.01;
		}

		score += GetYPenalty();

		score += ((xorshift32() % 1000) / 1000.0) * 0.0001;

		return score;
	}

	double GetSkillScore()
	{
		double score = 0.0;

		score += skill * 10;
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
							if (Get(dx, dy) > 0 && !flg[dy * WIDTH + dx])
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
State _prev_state;

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
	if (_turn == -1) exit(1);

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
}


int GetRealChain(State& state, int turn, int *max_pos = NULL, int *max_rot = NULL)
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
				if (max_pos != NULL)
				{
					*max_pos = pos;
					*max_rot = rot;
				}
			}
		}
	}
	return max_chain;
}


int main()
{
	cout << "y_kawano" << endl;

	Init();

	memset(_prev_state.map, -1, sizeof(_prev_state.map));

	while (true)
	{
		Input();
		time_point<system_clock> start_time = system_clock::now();

		const int NEED_CHAIN = 12;
		const int KILL_CHAIN = 15;
		const int SAFE_CHAIN = 7;
		const int NEED_SKILL = 50;

		//param
		int max_chain = 0;
		int max_pos = 0;
		int max_rot = 0;

		int ally_chain = _infos[0].state.GetChainCount();
		max_chain = GetRealChain(_infos[0].state, _turn, &max_pos, &max_rot);
		ally_chain = MAX(ally_chain, max_chain);
		int ally_skill = _infos[0].state.GetSkillOjama();

		int enemy_chain = _infos[1].state.GetChainCount();
		int real_enemy_chain = GetRealChain(_infos[1].state, _turn);
		enemy_chain = MAX(enemy_chain, real_enemy_chain);
		int enemy_skill = _infos[1].state.GetSkillOjama();

		bool skill_mode = false;

		//skill
		if (ally_skill >= NEED_SKILL && CHAIN_OJAMA_TABLE[ally_chain] < ally_skill && ally_chain <= SAFE_CHAIN)
		{
			skill_mode = true;
		}
		if (skill_mode && _infos[0].state.skill >= SKILL_COST)
		{
			cerr << "SKILL" << endl;
			cout << "S" << endl;
			continue;
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

		//print ifo
		if (skill_mode)
			cerr << "*";
		else if (enemy_chain >= SAFE_CHAIN)
			cerr << "+";
		cerr << "t:" << _turn << "  m:" << max_chain << " a_:" << _infos[0].state.skill << " e_:" << _infos[1].state.skill << endl;
		cerr << "ac:" << ally_chain << " ec:" << enemy_chain << " as:" << ally_skill << " es:" << enemy_skill << endl;

		//fire
		if ((_infos[1].state.skill >= SKILL_COST - SKILL_GAIN * 2 && max_chain >= 3 && enemy_skill >= MAX(CHAIN_OJAMA_TABLE[ally_chain], ally_skill)))
		{
			cerr << "SKILL_BOGAI" << endl;
			cout << max_pos << " " << max_rot << endl;
			continue;
		}

		int target_chain;
		if (enemy_chain < SAFE_CHAIN && _infos[0].state.ojama < 10)
		{
			target_chain = KILL_CHAIN;
			if (max_chain >= KILL_CHAIN)
			{
				cerr << "KILL_CHAIN" << endl;
				cout << max_pos << " " << max_rot << endl;
				continue;
			}
		}
		else
		{
			target_chain = NEED_CHAIN;
			if (max_chain >= NEED_CHAIN)
			{
				cerr << "ATTACK_CHAIN" << endl;
				cout << max_pos << " " << max_rot << endl;
				continue;
			}
		}


		//探索
		priority_queue<State> q[MAX_TURN + 1];
		q[0].push(_infos[0].state);

		//history
		bool is_match = true;
		for (int x = 0; x < WIDTH; x++)
		{
			for (int y = 0; y < HEIGHT; y++)
			{
				if (_infos[0].state.Get(x, y) != _prev_state.Get(x, y))
				{
					is_match = false;
					break;
				}
			}
			if (!is_match) break;
		}
		if (is_match && !skill_mode)
		{
			State history_state = _infos[0].state;
			for (int t = 0; t < MAX_TURN - 1; t++)
			{
				int chain = history_state.Put(_blocks[(_turn - 1) + (t + 1)], _prev_state.pos_history[t + 1], _prev_state.rot_history[t + 1]);
				history_state.score += history_state.GetScore();
				if (chain >= target_chain)
				{
					history_state.score += (MAX_TURN - t) * 1000;
				}
				history_state.pos_history[t] = _prev_state.pos_history[t + 1];
				history_state.rot_history[t] = _prev_state.rot_history[t + 1];
			}
			for (int pos = 0; pos < WIDTH - 1; pos++)
			{
				for (int rot = 0; rot < 4; rot++)
				{
					State clone = history_state;
					clone.pos_history[MAX_TURN - 1] = pos;
					clone.rot_history[MAX_TURN - 1] = rot;
					int chain = clone.Put(_blocks[_turn + (MAX_TURN - 1)], pos, rot);
					clone.score += clone.GetScore();
					if (chain >= target_chain)
					{
						clone.score += 1 * 1000;
					}
					q[MAX_TURN].push(clone);
				}
			}
			//cerr << "prev_score:" << q[MAX_TURN].top().score << endl;
		}

		//loop
		int loop = 0;
		while (true)
		{
			ll ms = duration_cast<milliseconds>(system_clock::now() - start_time).count();
			if (_turn <= 10)
			{
#ifdef SPEED_MODE
				if (ms >= 1000) break;
#else
				if (ms >= 10000 - (_turn * 1000)) break;
#endif
			}
			else if (_infos[0].time < 10000)
			{
				if (ms >= 100) break;
			}
			else
			{
				if (ms >= 1000) break;
			}

			for (int t = 0; t < MAX_TURN; t++)
			{
				if (q[t].size() == 0) continue;

				State state = q[t].top();
				q[t].pop();

				for (int pos = 0; pos < WIDTH - 1; pos++)
				{
					for (int rot = 0; rot < 4; rot++)
					{
						State clone = state;

						clone.pos_history[t] = pos;
						clone.rot_history[t] = rot;

						int chain = clone.Put(_blocks[_turn + t], pos, rot);
						loop++;

						if (skill_mode)
						{
							clone.score += clone.GetSkillScore();
							if (clone.skill >= SKILL_COST)
							{
								clone.score += (MAX_TURN - t) * 1000;
							}
							q[t + 1].push(clone);
						}
						else
						{
							if (chain >= 0)
							{
								clone.score += clone.GetScore();
								if (chain >= target_chain)
								{
									clone.score += (MAX_TURN - t) * 1000;
								}
								q[t + 1].push(clone);
							}
						}

						if (clone.ojama >= 10)
						{
							clone.Ojama();
							clone.ojama -= 10;;
						}
					}
				}
			}
		}

		if (q[MAX_TURN].size() == 0)
		{
			cout << "0 0" << endl;
		}
		else
		{
			State best = q[MAX_TURN].top();

			//cerr << "loop:" << loop << endl;
			cerr << "score:" << fixed << setprecision(2) << best.score << endl;
			cout << (int)best.pos_history[0] << " " << (int)best.rot_history[0] << endl;

			_prev_state = _infos[0].state;
			_prev_state.Put(_blocks[_turn], best.pos_history[0], best.rot_history[0]);
			_prev_state.pos_history = best.pos_history;
			_prev_state.rot_history = best.rot_history;
		}
	}
}
