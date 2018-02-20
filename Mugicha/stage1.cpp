#include "stage.h"

void Stage::init1()
{
	if (textures.empty())
	{
		multi_texture_loader({
			//			{ "__TAG__", "__FILE_PATH__" },
			{ "BACKGROUND", "./resources/textures/background.jpg" },
			{ "PLAYER", "./resources/textures/player.jpg" },
			{ "BLOCK", "./resources/textures/block.png" },
			{ "ORIGIN", "./resources/textures/origin.png" },
			{ "BLOCK2", "./resources/textures/block2.png" },
			{ "SAMPLE1", "./resources/textures/sample1.png" },
			});
	}

	std::map<enum PolygonTypes, polygon_vec>().swap(polygons); // すべて無にする　後のことは考えてない　すみません

	// 背景の登録
	background = push_polygon_back(BACKGROUND, new Background(textures["BACKGROUND"], &camera));
	background->enable();
	background->show();

	// プレイヤの登録
	player = push_polygon_back(PLAYER, new Player(textures["PLAYER"], &camera, polygons));
	player->enable();
	player->show();

	// 拡縮できるオブジェクトを登録
	polygons[SCALABLE_OBJECT].push_back(new ScalableObject(25, 25, 50, 50, textures["SAMPLE1"], 1, &camera));
	polygons[SCALABLE_OBJECT].back()->enable();
	polygons[SCALABLE_OBJECT].back()->show();

	polygons[SCALABLE_OBJECT].push_back(new ScalableObject(100, 25, 50, 50, textures["SAMPLE1"], 1, &camera));
	polygons[SCALABLE_OBJECT].back()->enable();
	polygons[SCALABLE_OBJECT].back()->show();

	// 原点
	polygons[PLAIN].push_back(new PlainSquarePolygon(0, 0, 20, 20, textures["ORIGIN"], 2, &camera));
	polygons[PLAIN].back()->enable();
	polygons[PLAIN].back()->show();

	// x軸ガイドライン
	polygons[PLAIN].push_back(new PlainSquarePolygon(0, 0, 100000, 10, textures["BLOCK2"], 3, &camera));
	polygons[PLAIN].back()->enable();
	polygons[PLAIN].back()->show();

	// y軸ガイドライン
	polygons[PLAIN].push_back(new PlainSquarePolygon(0, 0, 10, 100000, textures["BLOCK2"], 3, &camera));
	polygons[PLAIN].back()->enable();
	polygons[PLAIN].back()->show();

	zoom_level = { 1, 1 };

#ifdef _DEBUG
		printf("LOADED STAGE1\n");
#endif
}