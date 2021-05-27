﻿#include "scaler.h"

using namespace RollingBall;

RollingBall::Scaler::Scaler(pixel m_px_rate)
{
	px_rate(m_px_rate);
	_fix_point_physical.x = 0;
	_fix_point_physical.y = 0;
	_fix_point_pixel.x = 0;
	_fix_point_pixel.y = 0;
}

void Scaler::px_rate(pixel px)
{
	_px_rate = px;
}

pixel RollingBall::Scaler::px_rate()
{
	return _px_rate;
}

pixel Scaler::px(cm_val cm)
{
	return int(cm * (cm_val)_px_rate + 0.5); //반올림을 위함임
}

cm_val Scaler::cm(pixel px)
{
	return (cm_val)px / _px_rate;
}

void RollingBall::Scaler::fix_point_physical(PhysicalVector& point)
{
	_fix_point_physical = point;
}

PhysicalVector RollingBall::Scaler::fix_point_physical()
{
	return _fix_point_physical;
}

void RollingBall::Scaler::fix_point_pixel(PixelCoord& point)
{
	_fix_point_pixel = point;
}

PixelCoord RollingBall::Scaler::fix_point_pixel()
{
	return _fix_point_pixel;
}

PhysicalVector RollingBall::Scaler::transform(PixelCoord& point)
{
	PhysicalVector result;

	result.x = _fix_point_physical.x + px(point.x - _fix_point_pixel.x);
	result.y = _fix_point_physical.y - px(point.y - _fix_point_pixel.y);

	return result;
}

PixelCoord RollingBall::Scaler::transform(PhysicalVector& point)
{
	PixelCoord result;

	result.x = _fix_point_pixel.x + px(point.x - _fix_point_physical.x);
	result.y = _fix_point_pixel.y - px(point.y - _fix_point_physical.y);

	return result;
}
