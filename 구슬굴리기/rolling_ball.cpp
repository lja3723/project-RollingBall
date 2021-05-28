﻿#include "rolling_ball.h"
#include "debugger.h"

using namespace RollingBall;

BOOL RollingBallClass::init(HINSTANCE m_hInstance, HWND m_hwnd)
{
	winAPI.hInstance = m_hInstance;
	winAPI.hwnd = m_hwnd;

	if (!ObjectBitmapInfoVector::Load(winAPI.hwnd, _T("res\\bmp\\object_bmp_info.txt")))
		return FALSE;

	if (!paint.init(winAPI.hInstance, winAPI.hwnd)) return FALSE;

	Ball _ball;
	_ball.physical.pos(0, 1);
	_ball.physical.accel(0.015, 0.015);
	ball.push_back(_ball);

	//memset(&physics, 0, sizeof(physics));
	return TRUE;
}

void RollingBallClass::update_window()
{
	paint.begin();

	Background background;
	background.physical.size = 2;

	paint(background);
	paint(ball[0]);

	paint.end();
}

void RollingBallClass::update_state()
{
	if (!ball.size()) return;

	controller.update_ballPos(winAPI.hwnd, ball[0]);
}

void RollingBallClass::send_windowEvent(UINT m_iMsg, WPARAM m_wParam, LPARAM m_lParam)
{
	controller.translate_windowEvent(m_iMsg, m_wParam, m_lParam);
	paint.translate_windowEvent(m_iMsg, m_wParam, m_lParam);
}
