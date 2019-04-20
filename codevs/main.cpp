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
#include <immintrin.h>
#include <intrin.h>


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

#ifdef _MSC_VER
inline unsigned long __builtin_clzll(ull x) { unsigned long r; _BitScanReverse64(&r, x); return 63 - r; }
#else
inline unsigned long __builtin_clzll(ull x) { return hidword(x) ? __builtin_clz(hidword(x)) : __builtin_clz(lodword(x)) + 32; }
#endif // _MSC_VER

inline unsigned int bsr(ull v) { return 63 - __builtin_clzll(v); } // 最上位の1は下から数えて何ビットめか？


using namespace std;
using namespace chrono;

int _dx[] = { 1, 0, -1, 0, 1, 1, -1, -1 };
int _dy[] = { 0, 1, 0, -1, 1, -1, 1, -1 };

const int WIDTH = 10;
const int HEIGHT = 16;
const int BOTTOM = HEIGHT - 1;
const int BLOCK_NUM = 500;
const int OJAMA = 11;
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

//1 2
//3 4
//[4][3][2][1]

class State
{
public:
	ull map[WIDTH]; //上___下

	int firist_pos;
	int firist_rot;
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
		map[x] |= (n << shift);
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

		return Submit(check);
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

			*check |= (p << (x * 4));

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

	int Submit(ull first_check)
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
				map[x] &= (~((ull)mask4 << shift));
				erase_bit[x] |= (mask4 << shift);
			}

			//Print();

			for (int x = 0; x < WIDTH; x++)
			{
				map[x] = _pext_u64(map[x], ~erase_bit[x]);
			}

			//Print();

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

	double GetScore()
	{
		double score = 0.0;

		double max_chain_score = 0;

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

			State state = *this;
			for (int n = 0; n <= 9; n++)
			{
				state.DropBlock(x, drop_y, n);
				
				int chain = state.Submit(check);
				double chain_score = chain;
				chain_score += ((abs((WIDTH / 2.0) - x)) / 10.0);
				if (max_chain_score < chain_score)
				{
					max_chain_score = chain_score;
				}
			}
		}

		score += max_chain_score;

		//max_y
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

		if (max_y >= HEIGHT - 1)
		{
			score -= 10000;
		}
		else if (max_y >= HEIGHT - 2)
		{
			score -= 1000;
		}

		return score;
	}
};

class Info
{
public:
	int turn;
	int time;
	int ojama;
	int skill;
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
		cin >> info.ojama;
		cin >> info.skill;
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


int main()
{
	cout << "y_kawano" << endl;

	Init();

	while (true)
	{
		Input();
		time_point<system_clock> start_time = system_clock::now();

		const int NEED_CHAIN = 12;

		//ojama
		for (int p = 0; p < 2; p++)
		{
			if (_infos[p].ojama >= 10)
			{
				_infos[p].state.Ojama();
				_infos[p].ojama -= 10;;
			}
		}

		//連鎖発動
		int max_chain = 0;
		int max_pos = 0;
		int max_rot = 0;
		for (int pos = 0; pos < WIDTH - 1; pos++)
		{
			for (int rot = 0; rot < 4; rot++)
			{
				State clone = _infos[0].state;
				int chain = clone.Put(_blocks[_turn], pos, rot);
				//cout << "★★ " << chain << " ★★" << endl;
				if (max_chain < chain)
				{
					max_chain = chain;
					max_pos = pos;
					max_rot = rot;
				}
			}
		}
		cerr << "turn:" << _turn << "  max:" << max_chain << endl;
		if (max_chain >= NEED_CHAIN)
		{
			cout << max_pos << " " << max_rot << endl;
			continue;
		}

		if (_infos[0].ojama >= 10)
		{
			cerr << "SOUSAI" << endl;
			int prev_line_cnt = _infos[0].ojama / 10;
			int next_line_cnt = (_infos[0].ojama - CHAIN_OJAMA_TABLE[max_chain]) / 10;
			if (prev_line_cnt > next_line_cnt)
			{
				cout << max_pos << " " << max_rot << endl;
				continue;
			}
		}

		//探索
		priority_queue<State> q[MAX_TURN + 1];
		q[0].push(_infos[0].state);

		int loop = 0;
		while (true)
		{
			ll ms = duration_cast<milliseconds>(system_clock::now() - start_time).count();
			if (_turn <= 10)
			{
				if (ms >= 10000) break;
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

						if (t == 0)
						{
							clone.firist_pos = pos;
							clone.firist_rot = rot;
						}

						int chain = clone.Put(_blocks[_turn + t], pos, rot);
						loop++;
						if (chain >= 0)
						{
							clone.score = state.score;
							clone.score += clone.GetScore();
							if (chain >= NEED_CHAIN)
							{
								clone.score += (MAX_TURN - t) * 1000;
							}
							q[t + 1].push(clone);
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

			//skill
			if (_infos[0].skill >= 80 && best.score <= 1000)
			{
				int five_cnt = 0;
				for (int y = 0; y < HEIGHT; y++)
				{
					for (int x = 0; x < WIDTH; x++)
					{
						if (_infos[0].state.Get(x, y) == 5)
							five_cnt++;
					}
				}
				if (five_cnt >= 1)
				{
					cerr << "SKILL" << endl;
					cout << "S" << endl;
					continue;
				}
			}

			cerr << "loop:" << loop << endl;
			cerr << "score:" << best.score << endl;
			cout << best.firist_pos << " " << best.firist_rot << endl;

			_prev_state = _infos[0].state;
			_prev_state.Put(_blocks[_turn], best.firist_pos, best.firist_rot);
		}
	}
}
