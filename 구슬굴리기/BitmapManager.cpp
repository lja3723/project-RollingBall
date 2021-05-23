﻿#include "BitmapManager.h"
#include "FileManager.h"
#include "Debugger.h"
using namespace RollingBall;



/*****************************************
*
*	static variables initialization
* 
*****************************************/
int BitmapManager::bitmap_file_count = 0;
HINSTANCE BitmapManager::hInstance = NULL;
vector<BitmapManager::BitmapManagerObject> BitmapManager::object_info 
	= vector<BitmapManager::BitmapManagerObject>();
vector<HBITMAP> BitmapManager::hBitmap = vector<HBITMAP>();



/*****************************************
*
*			private functions
* 
*****************************************/
BOOL BitmapManager::init_object_info(HWND m_hwnd)
{
	LPCTSTR filename = _T("..\\res\\bmp\\object_info.txt");
	FileManager file;

	if (!file.open(filename, _T("r")))
	{
		TCHAR errmsg[256];
		_stprintf_s(errmsg, 256, _T("%s 파일을 열 수 없습니다."), filename);
		MessageBox(m_hwnd, errmsg, _T("오류"), MB_OK);
		return FALSE;
	}

	//파일을 한 줄씩 읽어 object에 저장한다.
	TCHAR line[128];
	BOOL isObjectReading = FALSE;
	int objidx = -1;
	while (file.readLine(line, sizeof(line)))
	{
		//공백 라인이거나 주석일 경우 패스
		if (_tcslen(line) == 0) continue;
		if (line[0] == _T('#')) continue;

		//오브젝트 정보를 읽는 중이 아닐 때...
		if (!isObjectReading) {

			//오브젝트 개수를 설정함
			if (_tcscmp(line, _T("object_count=")) == 0) {
				file.readLine(line, sizeof(line));
				object_info.resize(_ttoi(line));
			}

			//오브젝트 정보를 읽기 시작함
			if (_tcscmp(line, _T("<object>")) == 0) {
				objidx++;
				isObjectReading = TRUE;
				continue;
			}
		}

		//오브젝트 정보를 읽는 중일 때..
		if (isObjectReading)
		{
			//오브젝트의 이름을 저장
			if (_tcscmp(line, _T("name=")) == 0) {
				file.readLine(line, sizeof(line));
				object_info[objidx].name = line;
				continue;
			}

			//오브젝트 텍스쳐 개수 설정
			if (_tcscmp(line, _T("texture_count=")) == 0) {
				file.readLine(line, sizeof(line));
				object_info[objidx].texture.name.resize(_ttoi(line));
				continue;
			}

			//오브젝트 텍스쳐 이름 저장
			if (_tcscmp(line, _T("texture_name=")) == 0) {
				for (int i = 0; i < object_info[objidx].texture.name.size(); i++) {
					file.readLine(line, sizeof(line));
					object_info[objidx].texture.name[i] = line;
				}
				continue;
			}

			//오브젝트 텍스쳐 크기 값 개수 설정
			if (_tcscmp(line, _T("texture_size_count=")) == 0) {
				file.readLine(line, sizeof(line));
				object_info[objidx].texture.value.resize(_ttoi(line));
				continue;
			}

			//오브젝트 텍스쳐 크기 값 저장
			if (_tcscmp(line, _T("texture_size_value=")) == 0) {
				for (int i = 0; i < object_info[objidx].texture.value.size(); i++) {
					file.readLine(line, sizeof(line));
					object_info[objidx].texture.value[i] = _ttoi(line);
				}
				continue;
			}

			//오브젝트의 마스크 여부 저장
			if (_tcscmp(line, _T("has_mask=")) == 0) {
				file.readLine(line, sizeof(line));
				if (_tcscmp(line, _T("TRUE")) == 0)
					object_info[objidx].has_mask = TRUE;
				if (_tcscmp(line, _T("FALSE")) == 0)
					object_info[objidx].has_mask = FALSE;
				continue;
			}
		}

		//오브젝트 정보를 모두 읽었음
		if (_tcscmp(line, _T("</object>")) == 0) {
			isObjectReading = FALSE;
			continue;
		}

		//파일이 종료됨
		if (_tcscmp(line, _T("</object_info.txt>")) == 0)
			break;
	}

	//파일을 닫는다.
	file.close();
	return TRUE;
}
void BitmapManager::init_bitmap_file_count()
{
	if (!isInitObjectInfo()) return;

	bitmap_file_count = 0;
	for (int obj = 0; obj < object_info.size(); obj++)
		bitmap_file_count += get_object_file_count(obj);
			
}
void BitmapManager::init_hBitmap()
{
	if (!isInitBitmapFileCount()) return;
	if (isInitHBitmap())
		delete_hBitmap();

	for (int fileidx = 0; fileidx < bitmap_file_count; fileidx++)
		hBitmap.push_back(LoadBitmap(hInstance, MAKEINTRESOURCE(BMPFILEMACRO[fileidx])));

	//old code below
	//비트맵 로드
	/*
	oldvar_floor = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_FLOOR_WOOD1_256));
	for (int size = 0; size < old_BallSizeCount; size++)
	{
		oldvar_ball[size] = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BALL_IRON1_032 + 2 * size));
		oldvar_ball_mask[size] = LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BALL_IRON1_032M + 2 * size));
	}
	*/
}
void BitmapManager::delete_hBitmap()
{
	if (!isInitHBitmap()) return;
	for (int fileidx = 0; fileidx < bitmap_file_count; fileidx++)
		DeleteObject(hBitmap[fileidx]);
	
	hBitmap.clear();

	//old code below
	/*
	DeleteObject(oldvar_floor);
	for (int size = 0; size < old_BallSizeCount; size++)
	{
		DeleteObject(oldvar_ball[size]);
		DeleteObject(oldvar_ball_mask[size]);
	}
	*/
}
void BitmapManager::init_curselidx()
{
	set_cur_sel();
}
int BitmapManager::get_object_file_count(int objidx)
{
	if (!(0 <= objidx && objidx < object_info.size())) objidx = 0;
	return (int)object_info[objidx].texture.name.size()
		* (int)object_info[objidx].texture.value.size()
		* (object_info[objidx].has_mask ? 2 : 1);
}

BOOL BitmapManager::isInitObjectInfo()
{
	return object_info.size() != 0;
}
BOOL BitmapManager::isInitBitmapFileCount()
{
	return bitmap_file_count != 0;
}
BOOL BitmapManager::isInitHBitmap()
{
	return hBitmap.size() != 0;
}

void BitmapManager::arrange_idx(int& objidx, int& textureidx, int& sizeidx, BOOL& mask)
{
	if (!(0 <= objidx && objidx < object_info.size())) objidx = 0;
	if (!(0 <= textureidx && textureidx < object_info[objidx].texture.name.size())) textureidx = 0;
	if (!(0 <= sizeidx && sizeidx < object_info[objidx].texture.value.size())) sizeidx = 0;
	if (object_info[objidx].has_mask == FALSE && mask == TRUE) mask = FALSE;
} 



/*****************************************
*
*			public functions
*
*****************************************/
BOOL BitmapManager::init(HINSTANCE m_hInstance, HWND m_hwnd)
{
	if (isInit()) return TRUE;
	if (!init_object_info(m_hwnd)) return FALSE;

#ifdef __Debugger_h__
	for (int i = 0; i < object_info.size(); i++)
	{
		tstring name, value;
		for (int j = 0; j < object_info[i].texture.name.size(); j++)
			name += object_info[i].texture.name[j], name += _T(" ");
		for (int j = 0; j < object_info[i].texture.value.size(); j++)
			value += std::to_wstring(object_info[i].texture.value[j]), value += _T(" ");

		debuggerMessage(
			"object index: %d\n"
			"object name: %s\n\n"

			"texture count: %d\n"
			"texture name: %s\n\n"

			"texture value count: %d\n"
			"values: %s\n\n"

			"has mask: %s",
			i, object_info[i].name.c_str(),
			(int)object_info[i].texture.name.size(), name.c_str(),
			(int)object_info[i].texture.value.size(), value.c_str(),
			(object_info[i].has_mask ? _T("TRUE") : _T("FALSE"))
		);
	}
#endif

	hInstance = m_hInstance;
	hwnd = m_hwnd;
	init_bitmap_file_count();
	init_hBitmap();
	init_curselidx();

	return TRUE;
}
BOOL BitmapManager::isInit()
{
	return hInstance != NULL;
}
BitmapManager::~BitmapManager()
{
	if (isInit())
	{
		hInstance = NULL;
		delete_hBitmap();
	}
}

HBITMAP BitmapManager::get(int index)
{
	if (isInitHBitmap()) return hBitmap[index];
	else return NULL;
}
HBITMAP BitmapManager::create_hDC_compatible(HDC hdc, RECT& rt)
{
	return CreateCompatibleBitmap(hdc, rt.right, rt.bottom);
}
void BitmapManager::delete_hDC_compatible(HBITMAP hDCcompatibleBitmap)
{
	DeleteObject(hDCcompatibleBitmap);
}
int BitmapManager::get_curr_sel_idx()
{
	return index(
		curselidx.object, curselidx.texture, 
		curselidx.texture_size, curselidx.mask
	);
}
int BitmapManager::get_curr_object_idx()
{
	return curselidx.object;
}
LPCTSTR BitmapManager::get_curr_object_name()
{
	return object_info[curselidx.object].name.c_str();
}

int BitmapManager::get_curr_texture_idx()
{
	return curselidx.texture;
}
LPCTSTR BitmapManager::get_curr_texture_name()
{
	return object_info[curselidx.object].texture.name[curselidx.texture].c_str();
}

int BitmapManager::get_curr_texture_size_idx()
{
	return curselidx.texture_size;
}
int BitmapManager::get_curr_texture_size()
{
	return object_info[curselidx.object].texture.value[curselidx.texture_size];
}

BOOL BitmapManager::get_curr_object_has_mask()
{
	return object_info[curselidx.object].has_mask;
}

int BitmapManager::get_bitmap_file_count()
{
	return bitmap_file_count;
}




int BitmapManager::index(int objidx, int textureidx, int sizeidx, BOOL m_mask)
{
	int idx = 0;

	arrange_idx(objidx, textureidx, sizeidx, m_mask);

	//object에 따른 idx 탐색
	for (int curobjidx = 0; curobjidx < objidx; curobjidx++) idx += get_object_file_count(curobjidx);

	//texture에 따른 idx 탐색
	idx += (get_object_file_count(objidx) / (int)object_info[objidx].texture.name.size()) * textureidx;

	//mask 유무에 따른 idx 탐색
	if (m_mask == TRUE) idx += (int)object_info[objidx].texture.value.size();

	//size에 따른 idx 탐색
	idx += sizeidx;

	//구한 idx가 범위에 벗어나면 0으로 초기화
	if (!(0 <= idx && idx < bitmap_file_count)) idx = 0;

	return idx;
}
int BitmapManager::index(LPCTSTR m_obj, LPCTSTR m_texture, int m_size, BOOL m_mask)
{
	set_cur_sel(m_obj, m_texture, m_size, m_mask);
	return index(object(m_obj), texture(m_texture), size(m_size), m_mask);
}

int BitmapManager::object(LPCTSTR m_obj)
{
	for (int i = 0; i < object_info.size(); i++) 
		if (_tcscmp(m_obj, object_info[i].name.c_str()) == 0)
			return i;

	return 0;
}
LPCTSTR BitmapManager::object(int m_objidx)
{
	return object_info[m_objidx].name.c_str();
}

int BitmapManager::texture(LPCTSTR m_texture)
{
	for (int i = 0; i < object_info[curselidx.object].texture.name.size(); i++)
		if (_tcscmp(m_texture, object_info[curselidx.object].texture.name[i].c_str()) == 0)
			return i;

	return 0;
}
LPCTSTR BitmapManager::texture(int m_textureidx)
{
	return object_info[curselidx.object].texture.name[m_textureidx].c_str();
}

int BitmapManager::size(int m_size)
{
	for (int i = 0; i < object_info[curselidx.object].texture.value.size(); i++)
		if (m_size == object_info[curselidx.object].texture.value[i])
			return i;

	return 0;
}
int BitmapManager::idx_to_size(int sizeidx)
{
	return object_info[curselidx.object].texture.value[sizeidx];
}

void BitmapManager::set_cur_sel(int objidx, int textureidx, int sizeidx, BOOL m_mask)
{
	curselidx.object = objidx;
	curselidx.texture = textureidx;
	curselidx.texture_size = sizeidx;
	curselidx.mask = m_mask;
	arrange_idx(curselidx.object, curselidx.texture, curselidx.texture_size, curselidx.mask);
}
void BitmapManager::set_cur_sel(LPCTSTR m_obj, LPCTSTR m_texture, int m_size, BOOL m_mask)
{
	int objidx = object(m_obj);
	curselidx.object = objidx;
	int textureidx = texture(m_texture);
	curselidx.texture = textureidx;
	int sizeidx = size(m_size);
	set_cur_sel(objidx, textureidx, sizeidx, m_mask);
}
HBITMAP BitmapManager::operator[](int index)
{
	return get(index);
}

