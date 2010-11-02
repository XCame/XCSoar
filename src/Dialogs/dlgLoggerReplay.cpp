/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2010 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

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

#include "Dialogs/Internal.hpp"
#include "Units.hpp"
#include "Components.hpp"
#include "Replay/Replay.hpp"
#include "DataField/FileReader.hpp"
#include "DataField/Float.hpp"
#include "MainWindow.hpp"

static WndForm *wf = NULL;

static void
OnStopClicked(WndButton &Sender)
{
	(void)Sender;
  replay.Stop();
}

static void
OnStartClicked(WndButton &Sender)
{
	(void)Sender;
  WndProperty* wp;
  wp = (WndProperty*)wf->FindByName(_T("prpIGCFile"));
  if (wp) {
    DataFieldFileReader* dfe;
    dfe = (DataFieldFileReader*)wp->GetDataField();
    replay.SetFilename(dfe->GetPathFile());
  }
  replay.Start();
}

static void
OnCloseClicked(WndButton &Sender)
{
  (void)Sender;
  wf->SetModalResult(mrOK);
}

static void
OnRateData(DataField *Sender, DataField::DataAccessKind_t Mode)
{
  switch (Mode) {
  case DataField::daChange:
    replay.SetTimeScale(Sender->GetAsFixed());
    break;

  case DataField::daInc:
  case DataField::daDec:
  case DataField::daSpecial:
    return;
  }
}

static CallBackTableEntry CallBackTable[] = {
  DeclareCallBackEntry(OnStopClicked),
  DeclareCallBackEntry(OnStartClicked),
  DeclareCallBackEntry(OnRateData),
  DeclareCallBackEntry(OnCloseClicked),
  DeclareCallBackEntry(NULL)
};

void
dlgLoggerReplayShowModal(void)
{
  wf = LoadDialog(CallBackTable,
                      XCSoarInterface::main_window, _T("IDR_XML_LOGGERREPLAY"));
  if (!wf)
    return;

  WndProperty* wp;

  wp = (WndProperty*)wf->FindByName(_T("prpRate"));
  if (wp) {
    wp->GetDataField()->SetAsFloat(replay.GetTimeScale());
    wp->RefreshDisplay();
  }

  wp = (WndProperty*)wf->FindByName(_T("prpIGCFile"));
  if (wp) {
    DataFieldFileReader* dfe;
    dfe = (DataFieldFileReader*)wp->GetDataField();
    dfe->ScanDirectoryTop(_T("*.igc"));
    dfe->ScanDirectoryTop(_T("*.nmea"));
    dfe->ScanDirectoryTop(_T("xcsoar-nmea.log"));
    dfe->Lookup(replay.GetFilename());
    wp->RefreshDisplay();
  }

  wf->ShowModal();

  delete wf;
}
