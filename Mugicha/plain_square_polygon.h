#pragma once

#include "square_polygon_base.h"

/*
* PlainSquarePolygon
* SquarePolygonBaseの実装をやっている
*/

class PlainSquarePolygon : public SquarePolygonBase
{
protected:
	D3DXVECTOR2 & camera; // 描画中心座標 => 基本的にプレイヤ中心
	
	void generate_vertexes();
public:
	PlainSquarePolygon(float _x, float _y, float _w, float _h, LPDIRECT3DTEXTURE9 _tex, int _layer, D3DXVECTOR2 &_camera, float _u = 0.0f, float _v = 0.0f, float _uw = 1.0f, float _vh = 1.0f);
	~PlainSquarePolygon();
	
	void init();
	void update();
	void draw();
	bool is_drawing();
	void show();
	void hide();
	bool is_active();
	void enable();
	void disable();

	void on();
	void off();

	void change_texture(LPDIRECT3DTEXTURE9 _tex);

	SQUARE get_square();
	D3DXVECTOR2 get_coords();
	POLSIZE get_size();
	VERTEX_2D *get_vertexes();

	void add_coord(float _x, float _y);

	void zoom(float _zoom_level);
	
	PlainSquarePolygon* operator->();
};