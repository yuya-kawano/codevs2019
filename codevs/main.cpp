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
inline unsigned long __builtin_clzll(unsigned long x) { unsigned long r; _BitScanReverse64(&r, x); return 63 - r; }
#else
inline unsigned long __builtin_clzll(unsigned long x) { return hidword(x) ? __builtin_clz(hidword(x)) : __builtin_clz(lodword(x)) + 32; }
#endif // _MSC_VER

inline unsigned int bsr(unsigned long v) { return 63 - __builtin_clzll(v); } // 最上位の1は下から数えて何ビットめか？


using namespace std;
using namespace chrono;

int _dx[] = { 1, 0, -1, 0, 1, 1, -1, -1 };
int _dy[] = { 0, 1, 0, -1, 1, -1, 1, -1 };

const int WIDTH = 10;
const int HEIGHT = 16;
const int BOTTOM = HEIGHT - 1;
const int BLOCK_NUM = 500;
const u64 mask4 = 0b1111;


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

	int Get(int x, int y)
	{
		return (map[x] >> (y * 4) & mask4);
	}

	void SetBit(int x, int y, int n)
	{
		int shift = (y * 4);
		map[x] &= (~(mask4 << shift));
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
		DropLine(pos, l, &check);
		DropLine(pos + 1, r, &check);
		return Submit(check);
	}

	int DropLine(int x, int block_line, ull*check)
	{
		if (block_line == 0) return 0;

		int ret = 0;

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

			*check |= (p << (x * 4));

			p *= 4;
			map[x] |= (block_line << p);
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
							int dx = x;
							int dy = y;
							int sum = Get(x, y);

							while (true)
							{
								dx += _dx[d];
								dy += _dy[d];

								if (!IsIn(dx, dy)) break;

								int n = Get(dx, dy);

								if (n == 0) break;

								sum += n;

								if (sum > 10) break;

								if (sum == 10)
								{
									int qx = dx;
									int qy = dy;
									while (true)
									{
										q[q_cnt].x = qx;
										q[q_cnt].y = qy;
										q_cnt++;

										int next_bottom = ((next >> (qx * 4)) & mask4);
										if (next_bottom > qy)
										{
											next_bottom = qy;
											next &= ~((ull)mask4 << (qx * 4));
											next |= (qy << (qx * 4));
										}

										if (qx == x && qy == y) break;

										qx -= _dx[d];
										qy -= _dy[d];
									}
									break;
								}
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
				map[x] &= (~(mask4 << shift));
				erase_bit[x] |= (mask4 << shift);
			}

			for (int x = 0; x < WIDTH; x++)
			{
				map[x] = _pext_u64(map[x], ~erase_bit[x]);
			}

			chein++;
			check = next;
		}

		return chein;
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

	void Drop()
	{
	}
};



void Init()
{
	string end;
	int _;

	for (int i = 0; i < BLOCK_NUM; i++)
	{
		cin >> _ >> _;
		cin >> _ >> _;
		cin >> end;
		ASSERT(end == "END");
	}
}


void Input()
{
	int turn;
	int time;
	int ojama;
	int skill;
	int score;
	int n;
	string end;

	cin >> turn;
	cin >> time;
	cin >> ojama;
	cin >> skill;
	cin >> score;
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			cin >> n;
		}
	}
	cin >> end;
	ASSERT(end == "END");

	cin >> time;
	cin >> ojama;
	cin >> skill;
	cin >> score;
	for (int y = 0; y < HEIGHT; y++)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			cin >> n;
		}
	}
	cin >> end;
	ASSERT(end == "END");
}


int main()
{
	cout << "y_kawano" << endl;

	Init();

	State state;
	memset(state.map, 0, sizeof(state.map));

	//int block1 = (1 | (2 << 4) | (3 << 8) | (4 << 12));
	//int block2 = (1 | (2 << 4) | (0 << 8) | (4 << 12));

	//1 2
	//3 4

	//1 2
	//  4

	//state.Put(block1, 0, 0);
	//state.Put(block2, 0, 0);

	//state.Put(block1, 2, 1);
	//state.Put(block2, 2, 1);

	//state.Put(block1, 4, 2);
	//state.Put(block2, 4, 2);

	//state.Put(block1, 6, 3);
	//state.Put(block2, 6, 3);

	state.SetBit(0, 0, 1);

	state.SetBit(1, 0, 3);
	state.SetBit(1, 1, 5);
	state.SetBit(1, 2, 7);
	state.SetBit(1, 3, 8);

	state.SetBit(2, 0, 3);
	state.SetBit(2, 1, 1);

	state.SetBit(3, 0, 1);

	int block = (5 | (5 << 4) | (0 << 8) | (9 << 12));
	state.Put(block, 0, 0);

	for (int y = HEIGHT - 1; y >= 0; y--)
	{
		for (int x = 0; x < WIDTH; x++)
		{
			cout << state.Get(x, y);
			cout << " ";
		}
		cout << endl;
	}

	/*while (true)
	{
		Input();
		cout << "2 0" << endl;
	}*/
}
