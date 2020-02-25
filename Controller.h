#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <array>
#include <random>
#include <chrono>
#include "Button.h"

using namespace std;
using namespace sf;

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
	Controller() : alive{ true }, m_clicked{ false }, f_clicked{ false }, timer_run{ false }, tile_size{ 32 }, t_size{ 16 }, border{ 10 }, top{ 20 }, size{ 8 }, n_bombs{ 5 }
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
		smile.setScale(2, 2);

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

		w.create(VideoMode(border * 2 + tile_size * size, tile_size* size + border * 3 + (double)smile.getScale().y * 26 + top), "Minesweeper", Style::Titlebar | Style::Close);
	}

	void run();
	void loop();
	void render();
	void input();

	void set_grid();
	void set_grid_position(Vector2f v);
	Vector2i grid_contains(Vector2f v);
	void grid_scale();
	void update_txt();
	void set_texture_rext();
	void set_positions();

	void flag(int x, int y);
	void click(int x, int y);
	void flag_around(int x, int y);

	void change_difficulty(int s, int b);
	void restart();
	void lost();
	void won();

};