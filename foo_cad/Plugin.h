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

#ifndef FOOCAD_PLUGIN_H_
#define FOOCAD_PLUGIN_H_

#include "../../SDK/foobar2000.h"

class foo_cad :
	public initquit,
	public play_callback
{
public:
	foo_cad();
	~foo_cad();

	void on_init();
	void on_quit();

	void on_playback_starting(play_control::t_track_command command, bool paused);
	void on_playback_stop(play_control::t_stop_reason reason);
	void on_playback_pause(bool state);
	void on_playback_new_track(metadb_handle_ptr track);
	void on_playback_edited(metadb_handle_ptr track) { on_playback_new_track(track); }
	void on_playback_dynamic_info_track(const file_info& info);

	// Not implemented
	void on_playback_time(double time) {}
	void on_playback_seek(double time) {}
	void on_playback_dynamic_info(file_info const& info) {}
	void on_volume_change(float p_new_val) {}

private:
	void register_cad(HWND cad);
	static LRESULT CALLBACK window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND m_window;
	HWND m_cad_window;
};

#endif
