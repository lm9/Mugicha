#pragma once
#include <map>
#include <vector>
#include <string>
#include "stage.h"
#include "selector.h"
#include "helpers.h"

// �Q�[���Ǘ��p�̃N���X
class Controller
{
public:
	// �V�[���Ǘ��p�̗񋓌^
	enum class Scene
	{
		Ready,
		Title,
		Select,
		Gaming,
		GameOver,
		GameClear,
		End,
	};
private:
	Scene scene;
	Stage::GameInfo game_info;
	time_point latest_draw;
	time_point latest_update;
	std::map<const char*, LPDIRECT3DTEXTURE9> textures;
	PolygonsVector polygons;
	Background *background; // �w�i�|���S��
	Selector *selector; // �Z���N�^�[�|���S��
	Stage *stage;
	D3DXVECTOR2 camera;
	void update();
	void draw();
	void switch_scene(Scene _scene);
	void init();
public:
	Controller();
	void exec();
	Controller::Scene get_scene();
};