#pragma once

#include "polygonsquare.h"

// 移動しない前提で作ってる
// 拡縮に伴う移動は行う
// よくわからん
class ScalableObject : public SquarePolygonBase
{
private:
	POLSIZE zoom_level; // 拡大倍率
	D3DXVECTOR2 scaling_base; // 拡大するための基準．コンストラクト時に決める
	char scaling_dir; // 拡大する向き 0 => 左上, 1 => 右上, 2 => 右下, 3 => 左下
	void generate_vertexes();
public:
	ScalableObject(float _x, float _y, float _w, float _h, LPDIRECT3DTEXTURE9 _tex, char _scaling_dir, int _layer, D3DXVECTOR2 *_camera, float _u = 0.0f, float _v = 0.0f, float _uw = 1.0f, float _vh = 1.0f);
	~ScalableObject();
	void update();                                                                                           
	void draw();
	bool is_drawing();
	void show();
	void hide();
	bool is_active();
	void enable();
	void disable();
	void change_texture(LPDIRECT3DTEXTURE9 _tex);
	D3DXVECTOR2 get_coords();
	POLSIZE get_size();
	void add_coord(float _x, float _y);
	void zoom(POLSIZE _zoom_level);
	VERTEX_2D *get_vertexes();
};