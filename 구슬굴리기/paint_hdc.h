﻿#ifndef __paint_hdc_h__
#define __paint_hdc_h__
#include <Windows.h>
#include <vector>
#include "paint_hbitmap.h"

using std::vector;

namespace RollingBall
{
	class Paint_hDC
	{
	public:
		//////////////////
		//	inner class
		//////////////////
		class _window
		{
		public:
			//////////////////
			//	inner class
			//////////////////
			class _mode
			{
				//window를 어떤 방식으로 얻을지 설정함
				//BeginPaint 또는 GetDC으로 얻을 수 있음
			private:
				BOOL flag_isGetDC;

			public:
				//////////////////////////////
				//	window.mode interface
				//////////////////////////////
				_mode() { flag_isGetDC = FALSE; }
				void set_BeginPaint();
				void set_GetDC();
				BOOL isGetDC();
				BOOL isBeginPaint();

			}mode;

		private:
			HDC m_window;
			PAINTSTRUCT m_ps;
			void init();

		public:
			//////////////////////////
			//	window interface
			//////////////////////////
			_window() {
				init();
			}
			void set(HWND hwnd);
			void release(const HWND& hwnd);
			BOOL isSet();
			operator const HDC& ();

		}window;
		class _mem
		{
		public:
			//////////////////
			//	inner class
			//////////////////
			class _windowBuffer
			{
			private:
				HDC m_windowBuffer;
				void init();

			public:
				//////////////////////////////////
				//	mem.windowBuffer interface
				//////////////////////////////////
				_windowBuffer() { init(); }
				void set(const HDC& window);			
				void release();
				BOOL isSet();
				void select(Paint_hBitmap& hBitmap);
				void restore(Paint_hBitmap& hBitmap);
				operator const HDC& ();

			} windowBuffer;
			class _res
			{
			private:
				vector<HDC> m_res;
				BOOL flag_isSet;
				void init();

			public:
				//////////////////////////
				//	mem.res interface
				//////////////////////////
				_res() { init(); }
				void set(const HDC& mem_windowBuffer);
				void release();
				void resize(const size_t& newSize);
				BOOL isSet();
				void select(Paint_hBitmap& hBitmap);
				void restore(Paint_hBitmap& hBitmap);
				const HDC& operator()(int idx);
			} res;

		public:
			//////////////////////
			//	mem interface
			//////////////////////
			void set(const HDC& window, const HDC& mem_windowBuffer);
			void release();
		} mem;

	public:
		//////////////////
		//	interface
		//////////////////
		void resize_res_vector(const size_t& newSize);
	};
}

#endif