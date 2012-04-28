/*
  Copyright (C) 2012 Birunthan Mohanathas <http://poiru.net>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <cmath>
#include "cad_sdk.h"
#include "Plugin.h"

const GUID ORDER_DEFAULT = { 0xBFC61179, 0x49AD, 0x4E95, { 0x8D, 0x60, 0xA2, 0x27, 0x06, 0x48, 0x55, 0x05 } }; // {BFC61179-49AD-4E95-8D60-A22706485505}
const GUID ORDER_REPEAT = { 0x4BF4B280, 0x0BB4, 0x4DD0, { 0x8E, 0x84, 0x37, 0xC3, 0x20, 0x9C, 0x3D, 0xA2 } }; // {4BF4B280-0BB4-4DD0-8E84-37C3209C3DA2}
const GUID ORDER_REPEAT_PLAYLIST = { 0x681CC6EA, 0x60AE, 0x4BF9, { 0x91, 0x3B, 0xBB, 0x5F, 0x4E, 0x86, 0x4F, 0x2A } }; // {681CC6EA-60AE-4BF9-913B-BB5F4E864F2A}
const GUID ORDER_REPEAT_TRACK = { 0x4BF4B280, 0x0BB4, 0x4DD0, { 0x8E, 0x84, 0x37, 0xC3, 0x20, 0x9C, 0x3D, 0xA2 } }; // {4BF4B280-0BB4-4DD0-8E84-37C3209C3DA2}
const GUID ORDER_RANDOM = { 0x611AF974, 0x4316, 0x43AC, { 0xAB, 0xEC, 0xC2, 0xEA, 0xA3, 0x5C, 0x3F, 0x9B } }; // {611AF974-4316-43AC-ABEC-C2EAA35C3F9B}
const GUID ORDER_SHUFFLE_TRACKS = { 0xC5CF4A57, 0x8C01, 0x480C, { 0xB3, 0x34, 0x36, 0x19, 0x64, 0x5A, 0xDA, 0x8B } }; // {C5CF4A57-8C01-480C-B334-3619645ADA8B}
const GUID ORDER_SHUFFLE_ALBUMS = { 0x499E0B08, 0xC887, 0x48C1, { 0x9C, 0xCA, 0x27, 0x37, 0x7C, 0x8B, 0xFD, 0x30 } }; // {499E0B08-C887-48C1-9CCA-27377C8BFD30}
const GUID ORDER_SHUFFLE_DIRECTORIES = { 0x83C37600, 0xD725, 0x4727, { 0xB5, 0x3C, 0xBD, 0xEF, 0xFE, 0x5F, 0x8D, 0xC7 } }; // {83C37600-D725-4727-B53C-BDEFFE5F8DC7}

DECLARE_COMPONENT_VERSION(
	"CD Art Display",
	"1.0.2",
	"© 2012 - Birunthan Mohanathas");

static initquit_factory_t<foo_cad> foo_interface;

foo_cad::foo_cad() :
	m_Window(),
	m_CadWindow()
{
}

foo_cad::~foo_cad()
{
}

void foo_cad::on_init()
{
	WNDCLASS wc = {0};
	wc.lpfnWndProc = window_proc;
	wc.hInstance = core_api::get_my_instance();
	wc.lpszClassName = L"CD Art Display IPC Class";
	ATOM atom = RegisterClass(&wc);

	m_Window = CreateWindow(
		MAKEINTATOM(atom),
		L"foobar2000",
		WS_DISABLED,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		core_api::get_main_window(),
		nullptr,
		wc.hInstance,
		nullptr);

	if (!m_Window) return;

	register_cad(nullptr);

	static_api_ptr_t<play_callback_manager> pcm;
	pcm->register_callback(
		this,
		play_callback::flag_on_playback_starting |
		play_callback::flag_on_playback_stop |
		play_callback::flag_on_playback_pause |
		play_callback::flag_on_playback_new_track |
		play_callback::flag_on_playback_edited |
		play_callback::flag_on_playback_dynamic_info_track,
		false);
}

void foo_cad::on_quit()
{
	static_api_ptr_t<play_callback_manager>()->unregister_callback(this);

	if (m_CadWindow)
	{
		PostMessage(m_CadWindow, WM_USER, 0, PM_SHUTDOWN);
	}

	if (m_Window)
	{
		DestroyWindow(m_Window);
		UnregisterClass(L"CD Art Display IPC Class", core_api::get_my_instance());
	}
}

void foo_cad::on_playback_starting(play_control::t_track_command command, bool paused)
{
	if (!m_CadWindow) return;

	if (paused)
	{
		PostMessage(m_CadWindow, WM_USER, PS_PAUSED, PM_STATECHANGED);
	}
	else
	{
		switch (command)
		{
		case play_control::track_command_play:
		case play_control::track_command_resume:
		case play_control::track_command_settrack:
			PostMessage(m_CadWindow, WM_USER, PS_PLAYING, PM_STATECHANGED);
			break;
		}
	}
}

void foo_cad::on_playback_stop(play_control::t_stop_reason reason)
{
	if (!m_CadWindow) return;

	switch (reason)
	{
	case play_control::stop_reason_user:
	case play_control::stop_reason_eof:
	case play_control::stop_reason_shutting_down:
		PostMessage(m_CadWindow, WM_USER, PS_STOPPED, PM_STATECHANGED);
		break;
	}
}

void foo_cad::on_playback_pause(bool state)
{
	if (!m_CadWindow) return;

	PostMessage(m_CadWindow, WM_USER, state ? PS_PAUSED : PS_PLAYING, PM_STATECHANGED);
}

void foo_cad::on_playback_new_track(metadb_handle_ptr track)
{
	if (!m_CadWindow) return;

	service_ptr_t<titleformat_object> script;
	pfc::string8 format = "[%title%]\t[%artist%]\t[%album%]\t\t$year(%date%)\t\t$num(%tracknumber%,0)\t%length_seconds%\t%path%\t$mul($min($max(0,%rating%),5),2)\t \t\t\t\t\t\t\t";

	if (static_api_ptr_t<titleformat_compiler>()->compile(script, format))
	{
		static_api_ptr_t<playback_control> pbc;

		pbc->playback_format_title_ex(
			track,
			nullptr,
			format,
			script,
			nullptr,
			playback_control::display_level_titles);

		WCHAR buffer[2048];
		int len = MultiByteToWideChar(CP_UTF8, 0, format.get_ptr(), format.get_length() + 1, buffer, _countof(buffer));

		COPYDATASTRUCT cds;
		cds.dwData = PM_TRACKDATA;
		cds.cbData = len * sizeof(WCHAR);
		cds.lpData = (PVOID)buffer;
		SendMessage(m_CadWindow, WM_COPYDATA, 0, (LPARAM)&cds);
	}
}

void foo_cad::on_playback_dynamic_info_track(const file_info& info)
{
	if (!m_CadWindow) return;

	metadb_handle_ptr track;
	static_api_ptr_t<playback_control> pbc;
	if (pbc->get_now_playing(track))
	{
		on_playback_new_track(track);
	}
}

void foo_cad::register_cad(HWND cad)
{
	if (!cad)
	{
		cad = FindWindow(nullptr, L"CD Art Display 1.x Class");
	}

	if (!m_CadWindow && cad)
	{
		m_CadWindow = cad;

		WCHAR filename[MAX_PATH];
		GetModuleFileName(GetModuleHandle(nullptr), filename, MAX_PATH);
		
		WCHAR buffer[MAX_PATH + 64];
		int len = _snwprintf_s(buffer, _TRUNCATE, L"1\tCD Art Display IPC Class\tfoobar2000\t%s\t", filename);

		COPYDATASTRUCT cds;
		cds.dwData = PM_REGISTER;
		cds.lpData = buffer;
		cds.cbData = (len + 1) * sizeof(WCHAR);
		SendMessage(m_CadWindow, WM_COPYDATA, (WPARAM)m_Window, (LPARAM)&cds);
	}
};

LRESULT CALLBACK foo_cad::window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg != WM_USER)
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	static_api_ptr_t<playback_control> pbc;

	switch (lParam)
	{
	case CM_PLAY:
		{
			if (pbc->is_paused())
			{
				pbc->pause(false);
			}
			else
			{
				pbc->start();
			}
			return 1;
		}

	case CM_PLAYPAUSE:
		{
			pbc->play_or_pause();
			return 1;
		}

	case CM_PAUSE:
		{
			pbc->pause(true);
			return 1;
		}

	case CM_STOP:
		{
			pbc->stop();
			return 1;
		}
			
	case CM_NEXT:
		{
			pbc->start(playback_control::track_command_next);
			return 1;
		}

	case CM_PREVIOUS:
		{
			pbc->start(playback_control::track_command_prev);
			return 1;
		}

	case CM_SETVOLUME:
		{
			LONG scale = (LONG)wParam;
			float gain = 20.0f * log10((scale < 1 ? 1 : scale) / 100.0f) * 2.5f;
			pbc->set_volume(gain);
			return 1;
		}

	case CM_GETVOLUME:
		{
			LRESULT scaledVolume = (LRESULT)floor(audio_math::gain_to_scale(pbc->get_volume() / 2.5) * 100.0f + 0.5f);
			return (scaledVolume == 1) ? 0 : scaledVolume;
		}

	case CM_GETDURATION:
		{
			return (LRESULT)pbc->playback_get_length();
		}

	case CM_GETPOSITION:
		{
			return (LRESULT)pbc->playback_get_position();
		}

	case CM_SETPOSITION:
		{
			pbc->playback_seek((double)wParam);
			return 0;
		}

	case CM_GETSHUFFLE:
		{
			static_api_ptr_t<playlist_manager> plm;
			GUID guid = plm->playback_order_get_guid(plm->playback_order_get_active());

			return (guid == ORDER_RANDOM ||
				guid == ORDER_SHUFFLE_TRACKS ||
				guid == ORDER_SHUFFLE_ALBUMS ||
				guid == ORDER_SHUFFLE_DIRECTORIES) ? 1 : 0;
		}

	case CM_SETSHUFFLE:
		{
			static_api_ptr_t<playlist_manager> plm;
			for (t_size i = 0, isize = plm->playback_order_get_count(); i < isize; ++i)
			{
				if (plm->playback_order_get_guid(i) == ORDER_RANDOM)
				{
					plm->playback_order_set_active(i);
				}
			}

			return 1;
		}

	case CM_GETREPEAT:
		{
			static_api_ptr_t<playlist_manager> plm;
			GUID guid = plm->playback_order_get_guid(plm->playback_order_get_active());

			return (guid == ORDER_REPEAT) ? 1 : 0;
		}

	case CM_SETREPEAT:
		{
			static_api_ptr_t<playlist_manager> plm;
			for (t_size i = 0, isize = plm->playback_order_get_count(); i < isize; ++i)
			{
				if (plm->playback_order_get_guid(i) == ORDER_REPEAT)
				{
					plm->playback_order_set_active(i);
				}
			}

			return 1;
		}

	case CM_SETRATING:
		{
			// Not supported, send back 0
			PostMessage(foo_interface.get_static_instance().m_CadWindow, WM_USER, 0, PM_RATINGCHANGED);
			return 1;
		}

	case CM_REGISTER:
		{
			foo_interface.get_static_instance().register_cad((HWND)wParam);
			return 1;
		}

	case CM_SHOWWINDOW:
		{
			static_api_ptr_t<ui_control>()->activate();
			return 0;
		}

	case CM_GETSTATE:
		{
			return pbc->is_playing() ? PS_PLAYING :
				pbc->is_paused() ? PS_PAUSED :
				PS_STOPPED;
		}

	case PM_SHUTDOWN:
		{
			foo_interface.get_static_instance().m_CadWindow = NULL;
			return 1;
		}

	case CM_CLOSE:
		{
			standard_commands::main_exit();
			return 1;
		}

	case CM_GETCURRENTTRACK:
		{
			metadb_handle_ptr track;
			if (pbc->get_now_playing(track))
			{
				foo_interface.get_static_instance().on_playback_new_track(track);
			}

			return 1;
		}
	}

	return 0;
}
