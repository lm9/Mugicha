#include "collision_checker.h"

bool is_collision(SquarePolygonBase * _a, SquarePolygonBase * _b)
{
	auto a = _a->get_vertexes();
	auto b = _b->get_vertexes();

	return
		a[0].x <= b[1].x
		&& a[1].x >= b[0].x
		&& a[0].y <= b[2].y
		&& a[2].y >= b[0].y
		? true : false;

}

bool is_collision(SquarePolygonBase * _a, ScalableObject * _b)
{
	auto a = _a->get_vertexes();
	auto b = _b->get_vertexes();

	return
		a[0].x <= b[1].x
		&& a[1].x >= b[0].x
		&& a[0].y <= b[2].y
		&& a[2].y >= b[0].y
		? true : false;
}