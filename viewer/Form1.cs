using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace viewer
{
	public partial class Form1 : Form
	{
		const int WIDTH = 10;
		const int HEIGHT = 16;
		const string FILENAME = @"C:\project\codevs2019\codevs\dump.txt";
		
		int _chain = 0;
		int _index = 0;
		int _step = 0;


		class Log
		{
			public List<int[,]> Maps = new List<int[,]>();

			public Log(int[,] map_)
			{
				Maps = new List<int[,]>();

				var map = map_.Clone() as int[,];
				Maps.Add(map.Clone() as int[,]);
				
				int[] _dx = new int[] { 1, 0, -1, 0, 1, 1, -1, -1 };
				int[] _dy = new int[] { 0, 1, 0, -1, 1, -1, 1, -1 };

				while (true)
				{
					bool end = true;

					bool[,] flg = new bool[WIDTH, HEIGHT];
					for (int x = 0; x < WIDTH; x++)
					{
						for (int y = 0; y < HEIGHT; y++)
						{
							int a = map[x, y];

							if (a > 0)
							{
								for (int d = 0; d < 8; d++)
								{
									int dx = x + _dx[d];
									int dy = y + _dy[d];
									if (dx < 0 || dx >= WIDTH || dy < 0 || dy >= HEIGHT) continue;
									int b = map[dx, dy];
									if (a + b == 10)
									{
										flg[x, y] = true;
										end = false;
									}
								}
							}
						}
					}

					if (end) break;

					for (int x = 0; x < WIDTH; x++)
					{
						for (int y = 0; y < HEIGHT; y++)
						{
							if (flg[x, y])
							{
								map[x, y] = 0;
							}
						}
					}

					while (true)
					{
						bool drop_end = true;
						for (int x = 0; x < WIDTH; x++)
						{
							for (int y = 1; y < HEIGHT; y++)
							{
								if (map[x, y] > 0 && map[x, y - 1] == 0)
								{
									map[x, y - 1] = map[x, y];
									map[x, y] = 0;
									drop_end = false;
								}
							}
						}
						if (drop_end) break;
					}

					Maps.Add(map.Clone() as int[,]);
				}
			}
		}

		Dictionary<int, List<Log>> _dic = new Dictionary<int, List<Log>>();

		public Form1()
		{
			InitializeComponent();

			pictureBox.Image = new Bitmap(pictureBox.Width, pictureBox.Height);
		}

		private void Form1_Load(object sender, EventArgs e)
		{
			StreamReader sr = new StreamReader(FILENAME);

			int min_chain = int.MaxValue;

			while (!sr.EndOfStream)
			{
				int chain = int.Parse(sr.ReadLine());

				_dic[chain] = new List<Log>();

				int count = int.Parse(sr.ReadLine());
				for (int i = 0; i < count; i++)
				{
					min_chain = Math.Min(min_chain, chain);

					int[,] map = new int[WIDTH, HEIGHT];

					for (int y = 0; y < HEIGHT; y++)
					{
						var str = sr.ReadLine();
						var split = str.Split(' ');
						for (int x = 0; x < WIDTH; x++)
						{
							map[x, y] = int.Parse(split[x]);
						}
					}

					_dic[chain].Add(new Log(map));
				}

				listBoxChain.Items.Add(chain.ToString().PadLeft(2) + " : " + _dic[chain].Count);
			}

			sr.Close();

			_chain = min_chain;
			_index = 0;
			_step = 0;
			Draw();
		}

		private void ListBoxChain_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (listBoxChain.SelectedItem == null) return;

			listBoxList.Items.Clear();

			string str = listBoxChain.SelectedItem as string;
			int n = int.Parse(str.Split(':')[0]);
			for (int i = 0; i < _dic[n].Count; i++)
			{
				listBoxList.Items.Add(i);
			}
		}

		private void ListBoxList_SelectedIndexChanged(object sender, EventArgs e)
		{
			string str = listBoxChain.SelectedItem as string;
			int chain = int.Parse(str.Split(':')[0]);
			int index = (int)listBoxList.SelectedItem;

			_chain = chain;
			_index = index;
			_step = 0;

			Draw();
		}

		void Draw()
		{
			Graphics g = Graphics.FromImage(pictureBox.Image);
			g.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAliasGridFit;

			int[,] map = _dic[_chain][_index].Maps[_step];

			const int OFFSET = 10;
			const int SIZE = 30;

			g.Clear(Color.White);

			Font font = new Font("Consolas", 20);
			for (int y = HEIGHT - 1; y >= 0; y--)
			{
				for (int x = 0; x < WIDTH; x++)
				{
					if (map[x, y] > 0)
					{
						Brush brush = Brushes.White;
						switch (map[x, y])
						{
							case 1: brush = Brushes.OrangeRed; break;
							case 2: brush = Brushes.DodgerBlue; break;
							case 3: brush = Brushes.ForestGreen; break;
							case 4: brush = Brushes.Orange; break;
							case 5: brush = Brushes.Yellow; break;
							case 6: brush = Brushes.Orange; break;
							case 7: brush = Brushes.ForestGreen; break;
							case 8: brush = Brushes.DodgerBlue; break;
							case 9: brush = Brushes.OrangeRed; break;
							case 11: brush = Brushes.DimGray; break;
						}

						int draw_y = HEIGHT - y - 1;
						g.FillRectangle(brush, x * SIZE + OFFSET, draw_y * SIZE + OFFSET, SIZE, SIZE);
						g.DrawString(map[x, y].ToString(), font, Brushes.Black, x * SIZE + OFFSET, draw_y * SIZE + OFFSET);
					}
				}
			}

			for (int y = 0; y <= HEIGHT; y++)
			{
				g.DrawLine(Pens.DimGray, OFFSET, OFFSET + y * SIZE, OFFSET + WIDTH * SIZE, OFFSET + y * SIZE);
			}
			for (int x = 0; x <= WIDTH; x++)
			{
				g.DrawLine(Pens.DimGray, OFFSET + x * SIZE, OFFSET, OFFSET + x * SIZE, OFFSET + HEIGHT * SIZE);
			}

			pictureBox.Refresh();
		}

		private void ButtonPrev_Click(object sender, EventArgs e)
		{
			_step--;
			_step = Math.Max(0, _step);
			Draw();
		}

		private void ButtonNext_Click(object sender, EventArgs e)
		{
			_step++;
			_step = Math.Min(_dic[_chain][_index].Maps.Count - 1, _step);
			Draw();
		}
	}
}
