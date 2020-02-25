#pragma once

#include <SFML/Graphics.hpp>

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