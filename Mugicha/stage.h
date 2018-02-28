#pragma once

#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include "polygons_register.h"


using polygon_vec = std::vector<SquarePolygonBase*>;

class Stage {
public:
	enum class Status {
		Prep,
		LoadError,
		Ready,
		Playing,
		Pause,
		Clear,
		Failed,
		Retire,
		End
	};
	using GameInfo = struct _GameInfo
	{
		int score;
		int stage_number;
		enum Stage::Status status;
		Player::DeadReason dead_reason;
		_GameInfo() {}
		_GameInfo(int _score, enum Stage::Status _status, int _stage_number) : score(_score), status(_status), stage_number(_stage_number), dead_reason(Player::DeadReason::ALIVE){}
	};
private:
	// vars
	std::map<std::string, LPDIRECT3DTEXTURE9> textures;
	std::map<SquarePolygonBase::PolygonTypes, polygon_vec> polygons;
	StageBackground *background;
	Goal *goal;
	Player *player; // プレイヤの変数
	std::vector<Enemy*> enemies; // 敵の可変長配列
	Stage::GameInfo info; // 続行管理と結果
	std::chrono::system_clock::time_point latest_update; // 最終更新
	std::chrono::system_clock::time_point latest_draw; // 最終描画
	D3DXVECTOR2 camera;
	enum class Sign { ZERO,	PLUS, MINUS} zoom_sign; // 拡大状態か縮小状態かってアレです
	POLSIZE zoom_level_target; // どこまで拡縮するかというアレ
	POLSIZE zoom_level;
	POLSIZE map_size; // 背景のデカさになります

	// funcs
	void multi_texture_loader(std::map<std::string, const char *> _textures);
	void multi_texture_loader(const char *filepath);
	bool stagefile_loader(const char* filepath);
	void init();
	void update();
	void draw();

	template<typename _T>
	_T push_polygon_back(SquarePolygonBase::PolygonTypes type, _T polygon);
public:
	Stage(char _stage_select);
	~Stage();

	GameInfo exec();
};