#include "selector.h"

// ソレにしか使わないので
Selector::Selector(LPDIRECT3DTEXTURE9 _tex, D3DXVECTOR2 *_camera) 
{
	w = 100;
	h = 100;
	tex = _tex;
	u = 0.0f;
	v = 0.0f;
	uw = 1.0f;
	vh = 1.0f;
	layer = 0;
	camera = _camera;
	init();
}

Selector::~Selector()
{
}

void Selector::update()
{
	unless(status) return;
	// selectionからxを導く
	x = selection * 200;
}

void Selector::init()
{
	x = 200;
	y = 600;
	drawing = false;
	status = false;
	selection = 1;
}

void Selector::left()
{
	if (selection < 2) return;
	selection -= 1;
}

void Selector::right()
{
	if (selection > 2) return;
	selection += 1;
}

char Selector::get_selection()
{
	return selection;
}
