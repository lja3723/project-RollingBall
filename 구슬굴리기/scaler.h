﻿#pragma once
#ifndef __scaler_h__
#define __scaler_h__

#include "physics.h"
#include <Windows.h>

namespace RollingBall
{

	typedef int pixel;

	//픽셀 좌표계를 표현한다
	class PixelCoord {
	public:
		pixel x;
		pixel y;

		PixelCoord(pixel _x = 0, pixel _y = 0) { x = _x, y = _y; }

		PixelCoord(const POINT& p) : PixelCoord(p.x, p.y) {}

		PixelCoord& operator+=(const PixelCoord& p) { 
			x += p.x;
			y += p.y;
			return *this;
		}
		PixelCoord& operator-=(const PixelCoord& p) {
			x -= p.x;
			y -= p.y;
			return *this;
		}
		PixelCoord operator+(const PixelCoord& p) {
			PixelCoord result(x + p.x, y + p.y);
			return result;
		}
		PixelCoord operator-(const PixelCoord& p) {
			PixelCoord result(x - p.x, y - p.y);
			return result;
		}

		PixelCoord& operator()(pixel _x, pixel _y) { x = _x, y = _y; return *this; }
	};

	class Scaler 
	{
	private:
		double _px_rate;
		PhysicalVector _fix_point_physical;
		PixelCoord _fix_point_pixel;
	public:
		Scaler(double m_px_rate = 30);


		//1cm 당 몇 pixel을 차지하는 지 설정
		void px_rate(double px);

		//px_rate를 얻음
		double px_rate();

		//cm_val을 pixel으로 변환한다
		pixel px(cm_val cm);

		//pixel을 cm_val으로 변환한다
		cm_val cm(pixel px);

		//물리 좌표계의 고정점을 설정한다
		void set_fix_point(PhysicalVector& point);
		void set_fix_point(PhysicalVector&& point);

		//물리 좌표계의 고정점을 반환한다
		PhysicalVector& get_fix_point_physical();

		//픽셀 좌표계의 고정점을 설정한다
		void set_fix_point(PixelCoord& point);
		void set_fix_point(PixelCoord&& point);

		//픽셀 좌표계의 고정점을 반환한다
		PixelCoord& get_fix_point_pixel();

		//픽셀 좌표계를 물리 좌표계로 변환한다
		PhysicalVector transform(PixelCoord& point);
		PhysicalVector transform(PixelCoord&& point);

		//물리 좌표계를 픽셀 좌표계로 변환한다
		PixelCoord transform(PhysicalVector& point);
		PixelCoord transform(PhysicalVector&& point);
	};

}

#endif