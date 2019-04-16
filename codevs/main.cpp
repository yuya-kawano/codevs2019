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
const int BLOCK_NUM = 500;


const int mask4 = 0b1111;

//1 2
//3 4
//[4][3][2][1]

class State
{
public:
	u64 map[WIDTH]; //上___下

	int Get(int x, int y)
	{
		return (map[x] >> ((HEIGHT - y - 1) << 2) & mask4);
	}

	void Put(int block, int pos, int rot)
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

		Set(pos, l);
		Set(pos + 1, r);
	}

	void Set(int pos, int line)
	{
		if (map[pos] == 0)
		{
			map[pos] |= line;
		}
		else
		{
			int p = bsr((ull)map[pos]);
			p /= 4;
			p += 1;
			p *= 4;
			map[pos] |= (line << p);
		}
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

	int block1 = (1 | (2 << 4) | (3 << 8) | (4 << 12));
	int block2 = (1 | (2 << 4) | (0 << 8) | (4 << 12));

	state.Put(block1, 0, 0);
	state.Put(block2, 0, 0);
	int n1 = state.Get(0, HEIGHT - 1);
	int n2 = state.Get(0, HEIGHT - 1 - 1);
	int n3 = state.Get(0, HEIGHT - 1 - 2);
	int n4 = state.Get(0, HEIGHT - 1 - 3);

	while (true)
	{
		Input();
		cout << "2 0" << endl;
	}
}
