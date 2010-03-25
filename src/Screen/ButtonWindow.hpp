/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009

	M Roberts (original release)
	Robin Birch <robinb@ruffnready.co.uk>
	Samuel Gisiger <samuel.gisiger@triadis.ch>
	Jeff Goodenough <jeff@enborne.f2s.com>
	Alastair Harrison <aharrison@magic.force9.co.uk>
	Scott Penrose <scottp@dd.com.au>
	John Wharington <jwharington@gmail.com>
	Lars H <lars_hn@hotmail.com>
	Rob Dunning <rob@raspberryridgesheepfarm.com>
	Russell King <rmk@arm.linux.org.uk>
	Paolo Ventafridda <coolwind@email.it>
	Tobias Lohner <tobias@lohner-net.de>
	Mirek Jezek <mjezek@ipplc.cz>
	Max Kellermann <max@duempel.org>
	Tobias Bieniek <tobias.bieniek@gmx.de>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#ifndef XCSOAR_SCREEN_BUTTON_WINDOW_HXX
#define XCSOAR_SCREEN_BUTTON_WINDOW_HXX

#include <Screen/Window.hpp>
#include "Util/tstring.hpp"

class ButtonWindowStyle : public WindowStyle {
public:
  ButtonWindowStyle() {
#ifndef ENABLE_SDL
    style |= BS_PUSHBUTTON | BS_CENTER | BS_VCENTER;
#endif
  }

  void multiline() {
#ifndef ENABLE_SDL
    style |= BS_MULTILINE;
#endif
  }
};

#ifdef ENABLE_SDL

#include "Screen/PaintWindow.hpp"

/**
 * A clickable button.
 */
class ButtonWindow : public PaintWindow
{
  tstring text;
  unsigned id;
  bool down;
  Font font;

public:
  ButtonWindow():down(false) {}

public:
  void set(ContainerWindow &parent, const TCHAR *text, unsigned id,
           int left, int top, unsigned width, unsigned height,
           const ButtonWindowStyle style=ButtonWindowStyle());

  void set_text(const TCHAR *_text) {
    assert_none_locked();

    text = _text;
    invalidate();
  }

  const tstring &get_text() const {
    return text;
  }

protected:
  virtual bool on_mouse_down(int x, int y);
  virtual bool on_mouse_up(int x, int y);
  virtual void on_paint(Canvas &canvas);
};

#else /* !ENABLE_SDL */

#include "Screen/Window.hpp"

#include <tchar.h>

/**
 * A clickable button.
 */
class ButtonWindow : public Window {
public:
  void set(ContainerWindow &parent, const TCHAR *text, unsigned id,
           int left, int top, unsigned width, unsigned height,
           const ButtonWindowStyle style=ButtonWindowStyle());

  void set_text(const TCHAR *text) {
    assert_none_locked();

    ::SetWindowText(hWnd, text);
  }

  const tstring get_text() const;
};

#endif /* !ENABLE_SDL */

#endif
