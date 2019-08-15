#include <iostream>
#include <SFML/Graphics.hpp>
#include <array>
#include <random>
#include <chrono>


using namespace std;
using namespace sf;

class Button : public Sprite
{
private:
	int shown;
	int value;

public:
	Button() : shown{ 0 }, value{ 0 }{}

	void set_value(int v) { value = v; }
	void set_shown(int s) { shown = s; }

	int get_value() { return value; }
	int get_shown() { return shown; }
};


class Controller
{
private:
	RenderWindow w;
	Event event;
	Clock t_render;

	RectangleShape header;

	Font font;
	Text begginer;
	Text intermediary;

	Button smile;
	vector<vector<Button>> grid;
	Texture t;
	IntRect t_r[40];

	bool m_clicked;
	bool f_clicked;
	bool alive;

	double tile_size;
	double t_size;
	double border;
	double top;

	int size;
	int n_bombs;

	Button b_counter[3];
	Button b_timer[3];

	bool timer_run;
	Clock timer;

	Sprite txt_background[2];

	int counter;
	int left;
	
public:
	Controller() : alive{ true }, m_clicked{ false }, f_clicked{ false }, timer_run{false}, tile_size{ 32 }, t_size{ 16 }, border{ 10 }, top{ 20 }, size{ 8 }, n_bombs{ 5 }
	{
		left = size * size - n_bombs;

		header.setPosition(0, 0);
		header.setSize(Vector2f(border * 2 + tile_size * size, top));
		header.setFillColor(Color::White);

		font.loadFromFile("font.ttf");
		begginer.setFont(font);
		intermediary.setFont(font);

		begginer.setPosition(-1, -2);
		begginer.setCharacterSize(16);
		begginer.setString("| Begginer |");
		begginer.setFillColor(Color::Black);
		begginer.setOutlineColor(Color(225, 225, 225, 255));
		
		intermediary.setPosition(70, -2);
		intermediary.setCharacterSize(16);
		intermediary.setString("| intermediary |");
		intermediary.setFillColor(Color::Black);
		intermediary.setOutlineColor(Color(225, 225, 225, 255));

		t.loadFromFile("t.bmp");
		set_texture_rext();

		set_grid();
		grid_scale();

		smile.setTexture(t);
		smile.setTextureRect(t_r[18]);
		smile.setScale(2,2);

		for (int i = 0; i < 2; i++)
		{
			txt_background[i].setTexture(t);
			txt_background[i].setTextureRect(t_r[34]);
			txt_background[i].setScale(2, 2);
		}
		for (int i = 0; i < 3; i++)
		{
			b_counter[i].setTexture(t);
			b_counter[i].setTextureRect(t_r[23]);
			b_counter[i].setScale(2, 2);
			

			b_timer[i].setTexture(t);
			b_timer[i].setTextureRect(t_r[23]);
			b_timer[i].setScale(2, 2);

		}

		set_positions();

		update_txt();

		w.create(VideoMode(border * 2 + tile_size * size, tile_size* size + border * 3 + (double)smile.getScale().y*26 + top) , "Minesweeper",Style::Titlebar | Style::Close);
	}

	void run()
	{
		while (w.isOpen())
		{
			loop();
		}
	}
	void loop() 
	{
		
		while (w.pollEvent(event))
		{
			if (event.type == Event::Closed)w.close();
		}

		input();

		if (t_render.getElapsedTime().asSeconds() > 0.016667)
		{
			t_render.restart();
			render();
		}
	}
	
	void render() 
	{
		w.clear(Color(198,198,198,255));

		w.draw(smile);
		for (auto&& row : grid)
		{
			for (auto& e : row)
			{
				w.draw(e);
			}
		}

		w.draw(header);
		w.draw(begginer);
		w.draw(intermediary);

		w.draw(txt_background[0]);
		w.draw(txt_background[1]);

		for (int i = 0; i < 3; i++)
		{
			w.draw(b_counter[i]);
			w.draw(b_timer[i]);
		}

		w.display();
	}

	void input() 
	{

		Vector2f v = Vector2f(Mouse::getPosition(w).x, Mouse::getPosition(w).y);

		if (Mouse::isButtonPressed(Mouse::Left))
		{
			if (!m_clicked)
			{
				m_clicked = true;

				if (smile.getGlobalBounds().contains(v))
				{
					//cout << v.x << " " << v.y << " " << smile.getGlobalBounds().left << " " << smile.getGlobalBounds().top << " " << smile.getGlobalBounds().width << " " << smile.getGlobalBounds().height << endl;
					smile.setTextureRect(t_r[22]);
					restart();
				}
				else if (begginer.getGlobalBounds().contains(v))
				{
					change_difficulty(8, 5);
				}
				else if (intermediary.getGlobalBounds().contains(v))
				{
					change_difficulty(25, 99);
				}
				else if(alive)
				{
					smile.setTextureRect(t_r[19]);
					Vector2i vv = grid_contains(v);
					if (vv.x != -1)
					{
						timer_run = true;
						timer.restart();
						click(vv.x, vv.y);
					}
				}

				won();
			}
		}
		else
		{
			if(alive) smile.setTextureRect(t_r[18]);
			m_clicked = false;
		}

		if (Mouse::isButtonPressed(Mouse::Right))
		{
			if (!f_clicked && alive)
			{
				f_clicked = true;

				Vector2i vv = grid_contains(v);
				if (vv.x != -1)
				{
					flag(vv.x, vv.y);
					flag_around(vv.x, vv.y);
					won();
				}


			}
		}
		else f_clicked = false;

		update_txt();
	}

	void set_grid()
	{
		grid.clear();
		counter = n_bombs;

		grid.resize(size);
		for (auto&& row : grid)
			row.resize(size,Button());

		//cout << grid[0][0].get_value();

		default_random_engine e;
		e.seed(chrono::system_clock::now().time_since_epoch().count());
		uniform_int_distribution<int> y(0, size - 1);

		for (int j = 0; j < n_bombs; j++)
		{
			grid[y(e)][y(e)].set_value(14);
		}

		for (int j = 0; j < size; j++)
			for (int i = 0; i < size; i++)
			{
				if (grid[j][i].get_value() == 14)
				{
					for (int k = j - 1; k < j + 2; k++)
						for (int h = i - 1; h < i + 2; h++)
						{
							if (k >= 0 && k < size && h>=0 && h < size)
							{
								if (grid[k][h].get_value() != 14)
									grid[k][h].set_value(grid[k][h].get_value() + 1);
							}
						}
				}

				grid[j][i].setTexture(t);
			}

		for (auto&& row : grid)
			for (auto& e : row)
			{
				e.setTextureRect(t_r[9]);
			}
	}
	void set_grid_position(Vector2f v)
	{
		for (int j = 0; j < size; j++)
			for (int i = 0; i < size; i++)
			{
				grid[j][i].setPosition(Vector2f(i * tile_size + v.x, j * tile_size + v.y));
			}
	}
	Vector2i grid_contains(Vector2f v)
	{
		for (int j = 0; j < size; j++)
			for (int i = 0; i < size; i++)
			{
				if (grid[j][i].getGlobalBounds().contains(v))return Vector2i(i, j);
			}

		return Vector2i(-1, -1);
	}
	void grid_scale()
	{
		for (int j = 0; j < size; j++)
			for (int i = 0; i < size; i++)
			{
				grid[j][i].scale(tile_size / t_size, tile_size / t_size);
			}
	}

	void update_txt()
	{
		if (timer_run)
		{
			int n1,n2,n3 = timer.getElapsedTime().asSeconds();
			
			if (n3 > 999)
			{
				timer_run = false;
				return;
			}

			n1 = n3%10;
			n2 = n3 % 100 / 10;
			n3 = n3 / 100;



			b_timer[0].setTextureRect(t_r[23 + n3]);
			b_timer[1].setTextureRect(t_r[23 + n2]);
			b_timer[2].setTextureRect(t_r[23 + n1]);

		}

		if (counter < 99 && counter > -99)
		{
			int aux = counter;

			if (counter < 0)
			{
				b_counter[0].setTextureRect(t_r[33]);
				aux *= -1;
			}
			else b_counter[0].setTextureRect(t_r[23]);

			b_counter[1].setTextureRect(t_r[23 + aux / 10]);
			b_counter[2].setTextureRect(t_r[23 + aux % 10]);

			//cout << counter<<" "<<aux << " " << aux / 10 << " " << aux % 10 << endl;
		}
		
	}

	void set_texture_rext()
	{
		for (int i = 0; i < 9; i++)
		{
			t_r[i] = IntRect(i * 16, 0, 16, 16);
		}
		for (int i = 9; i < 18; i++)
		{
			t_r[i] = IntRect((i-9) * 16, 16, 16, 16);
		}
		
		t_r[18] = IntRect(0, 23+32, 26, 26);
		t_r[19] = IntRect(26 + 1, 23+32, 26, 26);
		t_r[20] = IntRect(26*2 + 2, 23+32, 26, 26);
		t_r[21] = IntRect(26*3 + 3, 23+32, 26, 26);
		t_r[22] = IntRect(26*4 + 4, 23+32, 26, 26);

		for(int i=0;i<11;i++)
			t_r[23 + i] = IntRect(11 * i + (i), 33, 11, 21);

		t_r[34] = IntRect(28, 16*2 + 26 + 21 + 3, 41, 25);

	}
	void set_positions()
	{
		float middle = (tile_size * size)/2.0;

		float smile_size = (double)smile.getScale().y * 26;

		smile.setPosition(Vector2f(border + middle - smile_size/2, border + top));
		set_grid_position(Vector2f(border, (border * 2 + smile_size) +top));

		cout << middle / 2 <<" "<< middle << " " << middle*1.5<< " " << txt_background[0].getGlobalBounds().width / 2 << endl;
		
		txt_background[0].setPosition(border,
			smile.getGlobalBounds().top);

		txt_background[1].setPosition(middle*2 + border - txt_background[1].getGlobalBounds().width,
			smile.getGlobalBounds().top);

		b_counter[0].setPosition(5.5 + border, top + border + 4);
		b_counter[1].setPosition(8 + border + b_counter[0].getGlobalBounds().width, top + border + 4);
		b_counter[2].setPosition(12 + border + (double)b_counter[0].getGlobalBounds().width*2, top + border + 4);

		b_timer[0].setPosition(5.5 + txt_background[1].getPosition().x, top + border + 4);
		b_timer[1].setPosition(8 + txt_background[1].getPosition().x + b_timer[0].getGlobalBounds().width, top + border + 4);
		b_timer[2].setPosition(12 + txt_background[1].getPosition().x + (double)b_timer[0].getGlobalBounds().width * 2, top + border + 4);

	}

	void flag(int x, int y) 
	{
		if (grid[y][x].get_shown() == 0)
		{
			grid[y][x].set_shown(2);
			grid[y][x].setTextureRect(t_r[12]);
			counter--;
		}
		else if (grid[y][x].get_shown() == 2)
		{
			grid[y][x].set_shown(0);
			grid[y][x].setTextureRect(t_r[9]);
			counter++;
		}
	}
	void click(int x, int y) 
	{
		if (x >= 0 && x < size && y >= 0 && y < size)
		{
			if (grid[y][x].get_shown() == 0)
			{
				left--;

				grid[y][x].set_shown(1);

				grid[y][x].setTextureRect(t_r[grid[y][x].get_value()]);

				if (grid[y][x].get_value() == 14) lost();
				else if (grid[y][x].get_value() == 0)
				{
					click(x - 1, y + 1); click(x, y + 1); click(x + 1, y + 1);
					click(x - 1, y);					  click(x + 1, y);
					click(x - 1, y - 1); click(x, y - 1); click(x + 1, y - 1);
				}

			}
		}
	}
	void flag_around(int x, int y)
	{
		if (x >= 0 && x < size && y >= 0 && y < size)
			if (grid[y][x].get_shown() == 1)
			{
				
				int aux = grid[y][x].get_value();
				//cout << aux;
				for (int j = y - 1; j < y + 2; j++)
				{
					for (int i = x - 1; i < x + 2; i++)
						if (i >= 0 && i < size && j >= 0 && j < size)
						{
							if (grid[j][i].get_shown() == 2)aux--;
							cout << grid[j][i].get_shown() << " ";
						}cout << endl;
				}

				if (!aux)
				{
					click(x - 1, y + 1); click(x, y + 1); click(x + 1, y + 1);
					click(x - 1, y);					  click(x + 1, y);
					click(x - 1, y - 1); click(x, y - 1); click(x + 1, y - 1);
				}
			}
	}

	void change_difficulty(int s, int b)
	{
		size = s;
		n_bombs = b;
		counter = b;

		timer.restart();
		update_txt();
		timer_run = false;

		header.setSize(Vector2f(border * 2 + tile_size * size, top));

		w.setSize(Vector2u(border * 2 + tile_size * size, tile_size * size + border * 3 + (double)smile.getScale().y * 26 + top));
		w.setView(View(FloatRect(0, 0, (border * 2 + tile_size * size) , (tile_size * size + border * 3 + (double)smile.getScale().y * 26 + top))));
		
		restart();
	}
	void restart() 
	{
		left = size * size - n_bombs;
		alive = true;
		timer_run = false;
		set_grid();
		grid_scale();


		set_positions();
	}
	void lost() 
	{
		alive = false;
		timer_run = false;

		smile.setTextureRect(t_r[20]);

		for (int j = 0; j < size; j++)
			for (int i = 0; i < size; i++)
			{
				if (grid[j][i].get_shown() == 0)
					if (grid[j][i].get_value() == 14)
						grid[j][i].setTextureRect(t_r[11]);

				if (grid[j][i].get_shown() == 2)
					if (grid[j][i].get_value() != 14)
						grid[j][i].setTextureRect(t_r[13]);
			}

	}
	void won() 
	{
		if (left == 0)
		{
			alive = false;
			timer_run = false;

			smile.setTextureRect(t_r[21]);

			for (int j = 0; j < size; j++)
				for (int i = 0; i < size; i++)
				{
					if (grid[j][i].get_shown() == 0)
						if (grid[j][i].get_value() == 14)
							grid[j][i].setTextureRect(t_r[12]);
				}

		}
	}

};

int main()
{
	Controller c;
	c.run();
}
