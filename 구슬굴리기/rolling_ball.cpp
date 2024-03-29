﻿#include "rolling_ball.h"
#include "debugger.h"
using namespace RollingBall;

void RollingBall::RollingBallClass::paint_BallSwitchArrow()
{
	static auto tracing = ballSwitch;
	const static int fRate = 50;
	static int frameCount = 0;
	//볼스위치가 변화하면...
	if (tracing != ballSwitch)
		frameCount = fRate;

	//fRate동안 볼스위치 화살표를 보여줌
	if (frameCount) {
		frameCount--;
		auto pos = scaler.transform(ball[ballSwitch].physical.pos);
		auto radius = scaler.px(ball[ballSwitch].physical.size);
		paint.text(_T("↑"), pos.x - 8, pos.y + radius / 2 + 5);
	}

	//ballSwitch를 트레이싱함
	tracing = ballSwitch;
}

void RollingBallClass::init_scaler(int px_rate)
{
	//프로그램 화면 정중앙과 물리좌표 (0, 0)이 일치하도록 초기화함
	scaler.px_rate(px_rate);

	RECT windowRect;
	GetClientRect(winAPI.hwnd, &windowRect);
	scaler.set_fix_point(PixelCoord(windowRect.right / 2, windowRect.bottom / 2));
	scaler.set_fix_point(PhysicalVector(0, 0));
}

void RollingBallClass::update_window()
{
	paint.setModeBeginPaint();
	paint.begin();

	//배경 그리기
	Background background;
	background.physical.size = 25;
	paint(background);

	//공 정보 그리기
	for (int i = 0; i < ball.size(); i++)
		paint.info(ball[i], i * 20);

	//방향키 뷰어 그리기
	POINT keyViewer = { 300, 120 };
	int d_keyViewer = 18;

	KeyboardEvent e;
	if (e.isKeyPressed('S'))
		paint.text(_T("S"), keyViewer.x, keyViewer.y);
	if (e.isKeyPressed('W'))
		paint.text(_T("W"), keyViewer.x, keyViewer.y - d_keyViewer);
	if (e.isKeyPressed('A'))
		paint.text(_T("A"), keyViewer.x - d_keyViewer, keyViewer.y);
	if (e.isKeyPressed('D'))
		paint.text(_T("D"), keyViewer.x + d_keyViewer, keyViewer.y);

	//기타 정보 그리기
	TCHAR buff[256];
	_stprintf_s(buff, 256, _T("ballSwitch:%d"), ballSwitch);
	paint.text(buff, 300, 30);
	_stprintf_s(buff, 256, _T("ball count:%d"), (int)ball.size());
	paint.text(buff, 300, 50);
	PhysicalVector cen = scaler.get_fix_point_physical();
	_stprintf_s(buff, 256, _T("center position:(%3.2f, %3.2f)"), cen.x, cen.y);
	paint.text(buff, 300, 70);
	paint.text(buffdrag[0], 300, 90);
	paint.text(buffdrag[1], 300, 110);
	paint.text(buffdrag[2], 300, 130);

	//공 그리기
	for (int i = 0; i < ball.size(); i++)
		paint(ball[i]);

	//볼스위치 표현하기
	paint_BallSwitchArrow();

	paint.end();
}
void RollingBallClass::update_state()
{
	PhysicalVector posPrev = ball[ballSwitch].physical.pos;
	for (int i = 0; i < ball.size(); i++) {
		controller.force_to(ball[ballSwitch], 0.015);
		controller.update_ballPos(ball[i]);
	}
	PhysicalVector& posNow = ball[ballSwitch].physical.pos;
}
void RollingBallClass::trace_dragging(MouseEvent& e)
{
	if (e.isLButtonDown())
	{
		static PixelCoord prevPos;
		if (e.eventType.isMouseMove())
		{
			PixelCoord curPos(e.pos());
			PhysicalVector diff = scaler.transform(curPos) - scaler.transform(prevPos);
			dragging_action(diff, e);
			prevPos = curPos;
		}
		else
			prevPos = e.pos();
	}
}
void RollingBall::RollingBallClass::dragging_action(PhysicalVector& diff, MouseEvent& e)
{
	if (!ball_move(diff, e))
		map_move(diff);
}

void RollingBall::RollingBallClass::map_move(PhysicalVector& diff)
{
	scaler.set_fix_point(scaler.get_fix_point_physical() - diff);
}
void RollingBall::RollingBallClass::map_scale(MouseEvent& e)
{	
	//휠조작으로 맵 확대율 변경하기
	double zoom_rate = 1.1;
	if (e.eventType.isMouseWheelUp())
		if (scaler.px_rate() < 500)
			scaler.px_rate(scaler.px_rate() * zoom_rate);
	if (e.eventType.isMouseWheelDown())
		if (scaler.px_rate() > 5)
			scaler.px_rate(scaler.px_rate() / zoom_rate);
}

void RollingBall::RollingBallClass::ball_add(MouseEvent& e)
{
	if (e.eventType.isLButtonDoubleClick())
	{
		Ball b;
		b.physical.pos(scaler.transform(e.pos()));
		ball.push_back(b);
	}
}
void RollingBall::RollingBallClass::ball_select(MouseEvent& e)
{
	if (e.eventType.isLButtonDown())
	{
		PhysicalVector pos = scaler.transform(e.pos());
		for (int i = 0; i < ball.size(); i++)
			if (ball[i].isPosIncluded(pos))
			{
				ball[ballSwitch].physical.accel(0, 0);
				ballSwitch = i;
				return;
			}
	}
}
BOOL RollingBall::RollingBallClass::ball_move(PhysicalVector& diff, MouseEvent& e)
{
	
	for (int i = 0; i < ball.size(); i++)
	{
		int r = (int)ball.size() - i - 1;
		if (ball[r].isPosIncluded(scaler.transform(e.pos())))
		{
			ball[r].physical.pos += diff;
			return TRUE;
		}
	}
	return FALSE;
}

void RollingBallClass::set_timer(UINT frame_update_interval)
{
	if (isInitTimer) kill_timer();
	SetTimer(winAPI.hwnd, 1, frame_update_interval, NULL);
	isInitTimer = TRUE;
}
void RollingBallClass::kill_timer()
{
	if (!isInitTimer) return;
	KillTimer(winAPI.hwnd, 1);
	isInitTimer = FALSE;
}


/////////////////////////////////
//	event processing
/////////////////////////////////
void RollingBallClass::event_keyboard(KeyboardEvent e) {}
void RollingBallClass::event_mouse(MouseEvent e)
{
	//POINT dif = { e.pos().x - e.prevPos().x, e.pos().y - e.prevPos().y };
	_stprintf_s(buffdrag[1], 256, _T("(%d, %d)"), e.pos().x, e.pos().y);
	//_stprintf_s(buffdrag[1], 256, _T("(%d, %d)"), dif.x, dif.y);
	_stprintf_s(buffdrag[2], 256, _T("(%d, %d)"), e.prevPos().x, e.prevPos().y);
	//trace_dragging(e);
	if (e.isLButtonDragging())
	{
		_stprintf_s(buffdrag[0], 256, _T("dragging"));
		PhysicalVector diff = scaler.transform(e.pos()) - scaler.transform(e.prevPos());
		//_stprintf_s(buffdrag[1], 256, _T("(%lf, %lf)"), diff.x, diff.y);
		dragging_action(diff, e);
	}
	else
	{
		_stprintf_s(buffdrag[0], 256, _T(""));
		//_stprintf_s(buffdrag[1], 256, _T(""));
		//_stprintf_s(buffdrag[2], 256, _T(""));
	}

	
	map_scale(e);
	ball_add(e);
	ball_select(e);
}
void RollingBallClass::event_else(Event e)
{
	switch (e.winMsg.iMsg())
	{
	case WM_PAINT:
		update_window();
		break;
	case WM_TIMER:
		update_state();
		InvalidateRgn(winAPI.hwnd, NULL, FALSE);
		break;
	}
}


///////////////////////////////////
//	public interface
///////////////////////////////////
RollingBallClass::RollingBallClass()
{
	winAPI = { NULL, NULL };
	isInitTimer = FALSE;
	ballSwitch = 0;
}
RollingBallClass::~RollingBallClass()
{
	if (isInitTimer)
		kill_timer();
}
BOOL RollingBallClass::init(HINSTANCE m_hInstance, HWND m_hwnd, UINT frame_update_interval)
{
	winAPI.hInstance = m_hInstance;
	winAPI.hwnd = m_hwnd;
	set_timer(frame_update_interval);
	init_scaler(DEFAULT_PX_RATE);

	if (!ObjectBitmapInfoVector::Load(winAPI.hwnd, _T("res\\bmp\\object_bmp_info.txt")))
		return FALSE;

	if (!paint.init(winAPI.hInstance, winAPI.hwnd, &scaler)) return FALSE;

	Ball _ball;
	_ball.physical.size = 2;
	ball.push_back(_ball);

	return TRUE;
}
void RollingBallClass::set_frame_update_interval(UINT millisecond)
{
	set_timer(millisecond);
}
