// uEpgUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "uEpgUI.h"
#include "uEpgUIDlg.h"
#include "EpgControl.h"

#include "moduleAPI.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CuEpgUIDlg::CuEpgUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CuEpgUIDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	_pChl = 0;
}

void CuEpgUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHANNEL_LIST, _channelList);
	DDX_Control(pDX, IDC_SAT_LIST, _satList);
	DDX_Control(pDX, IDC_TAG_LIST, _tagList);
	DDX_Control(pDX, IDC_COMBO_CHANNEL_SORT, _channelSortCombo);
	DDX_Control(pDX, IDC_EDIT_CHANNEL_SEARCH, _channelSearchEdit);
	DDX_Control(pDX, IDC_COMBO_CHANNEL_TYPES, _channelTypesCombo);
}

BEGIN_MESSAGE_MAP(CuEpgUIDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_CHANNEL_SORT, &CuEpgUIDlg::OnCbnSelchangeComboChannelSort)
	ON_EN_CHANGE(IDC_EDIT_CHANNEL_SEARCH, &CuEpgUIDlg::OnEnChangeEditChannelSearch)
	ON_WM_TIMER()
	ON_LBN_SELCHANGE(IDC_SAT_LIST, &CuEpgUIDlg::OnLbnSelchangeSatList)
	ON_CBN_EDITCHANGE(IDC_COMBO_CHANNEL_TYPES, &CuEpgUIDlg::OnCbnEditchangeComboChannelTypes)
	ON_CBN_SELCHANGE(IDC_COMBO_CHANNEL_TYPES, &CuEpgUIDlg::OnCbnSelchangeComboChannelTypes)
END_MESSAGE_MAP()


// CuEpgUIDlg message handlers


void CuEpgUIDlg::UpdateChannelTypesCombo()
{
	int types[] = {CHTYPE_TV, CHTYPE_RADIO, CHTYPE_DATA};
	wchar_t* typesNames[] = {L"TV", L"Radio", L"Data"};

	_channelTypesCombo.ResetContent();

	CString selectedStr;
	int     selectedCount=0;
	for (int x=0; x<sizeof(types)/sizeof(int); x++)
	{
		int n=0;
		if (_channelTypes & (1<<types[x]))
		{
			if (selectedCount)
				selectedStr += L", ";
			
			selectedStr += typesNames[x];
			selectedCount++;
		}
	}

	if (!selectedCount)
		selectedStr = L"<Nothing selected>";
	_channelTypesCombo.InsertString(0, selectedStr);

	for (int x=0; x<sizeof(types)/sizeof(int); x++)
	{
		int n=0;
		if (_channelTypes & (1<<types[x]))
		{
			if (selectedCount)
				selectedStr += L", ";
			
			selectedStr += typesNames[x];
			selectedCount++;

			CString comboStr;
			comboStr.Format(L"Hide %s", typesNames[x]);
			n=_channelTypesCombo.InsertString(-1, comboStr);
		}
		else
		{
			CString comboStr;
			comboStr.Format(L"Show %s", typesNames[x]);
			n=_channelTypesCombo.InsertString(-1, comboStr);
		}

		_channelTypesCombo.SetItemData(n, (1<<types[x]));
	}

	_channelTypesCombo.SetCurSel(0);
}

BOOL CuEpgUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	_channelSortCombo.SetItemData(
		_channelSortCombo.AddString(L"Sort by Name"),
		(int)ChannelSortName);

	_channelSortCombo.SetItemData(
		_channelSortCombo.AddString(L"Sort by SID"),
		(int)ChannelSortSID);

	_channelSortCombo.SetCurSel(0);

	_channelTypes = (1<<CHTYPE_TV)|(1<<CHTYPE_RADIO)|(1<<CHTYPE_DATA);
	UpdateChannelTypesCombo();

	CWnd* pStatic = GetDlgItem(IDC_STATIC);
	pStatic->ShowWindow(SW_HIDE);

	CRect pStaticRect;
	pStatic->GetWindowRect(pStaticRect);
	ScreenToClient(pStaticRect);

	//_epgControl.Create(this, CRect(CPoint(150, 20), CSize(750, 480)));
	_epgControl.Create(this, pStaticRect);
	UpdateChlDataFromFile("c:\\code\\epg.bin");

	ASSERT(_epgDatabase.Open("c:\\code\\epg.db")==0);
	UpdateEpgGrid();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CuEpgUIDlg::UpdateEpgGrid()
{

}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CuEpgUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CuEpgUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CuEpgUIDlg::UpdateChlDataFromFile(const char* filename)
{
	TCHL* pChl = new TCHL;
	FILE* file;

	fopen_s(&file, filename, "rb");
	if (file)
	{
		fread(pChl, sizeof(TCHL), 1, file);
		UpdateChlData(pChl);
	}

	delete pChl;
}

bool TChlChannel_CompareName(TChlChannel* first, TChlChannel* second)
{
	return (strcmp(first->ChName, second->ChName)<0);
}

bool TChlChannel_CompareSID(TChlChannel* first, TChlChannel* second)
{
	return (first->wSID<second->wSID);
}

const char *strnistr(const char* str, const char* strSearch)
{
	size_t n = strlen(strSearch);

	while (*str)
	{
		const char* s1 = str;
		const char* s2 = strSearch;

		while (tolower(*s1++) == tolower(*s2++))
		{
			if (!*s2)
				return str;
			else if (!*s1)
				break;
		}

		str++;
	}

	return 0;
}

void CuEpgUIDlg::BuildChannelInfoList()
{
	TChlChannel* pChannels = _pChl->Channels;
	CString searchStrTemp;
	char searchStr[32];
	int  searchSats[256];

	bool doSearchName = false;
	bool doSearchSat = false;
	
	_channelSearchEdit.GetWindowText(searchStrTemp);
	searchStrTemp.MakeLower();
	if (searchStrTemp.GetLength())
	{
		doSearchName = true;
		size_t n;
		wcstombs_s(&n,
			searchStr, sizeof(searchStr),
			searchStrTemp, searchStrTemp.GetLength());
	}

	int * pSearchSat = searchSats;
	for (int s=0; s<_satList.GetCount(); s++)
	{
		if (_satList.GetSel(s))
		{
			TChlSat* pSatellite = (TChlSat*)_satList.GetItemData(s);
			if (pSatellite)
			{
				*pSearchSat++ = pSatellite->wPos;
				doSearchSat = true;
			}
			else
			{
				doSearchSat = false;
				break;
			}
		}
	}
	
	*pSearchSat = 0;
	

	_channelInfoList.clear();

	for (int c=0; c<MAX_CHANNELS; c++)
	{
		TChlChannel* pChannel = &pChannels[c];
		
		if (pChannel->wSatPos)
		{
			if (doSearchName)
			{
				if (!strnistr(pChannel->ChName, searchStr))
					continue;
			}

			if (doSearchSat)
			{
				int* pSat =0;
				for (pSat=searchSats; *pSat!=pChannel->wSatPos && *pSat; pSat++);
				if (!*pSat)
					continue;
			}
			
			if (_channelTypes & (1<<pChannel->ChType))
				_channelInfoList.push_back(pChannel);
		}
	}
}

void CuEpgUIDlg::UpdateChannelList()
{

	switch(_channelSortCombo.GetItemData(_channelSortCombo.GetCurSel()))
	{
	case ChannelSortName:
		_channelInfoList.sort(TChlChannel_CompareName);
		break;

	case ChannelSortSID:
		_channelInfoList.sort(TChlChannel_CompareSID);
		break;
	}
	
	CString str;

	_channelList.SetRedraw(false);
	_channelList.ResetContent();

	str.Format(L"<All Channels (%d)>", _channelInfoList.size());
	int first = _channelList.InsertString(0, str);

	ChannelInfoList::iterator i;
	for (i=_channelInfoList.begin(); i!=_channelInfoList.end(); i++)
	{
		CString chStr;
		TChlChannel* pChannel = *i;
		chStr.Format(L"[%04d] %S", pChannel->wSID, pChannel->ChName);
		int n = _channelList.InsertString(-1, chStr);
		_channelList.SetItemData(n, (DWORD_PTR)pChannel);
	}
	_channelList.SetSel(first, TRUE);
	_channelList.SetRedraw(true);
}

void CuEpgUIDlg::UpdateSatList()
{
	int count = 0;
	_satList.SetRedraw(false);
	_satList.ResetContent();

	for (int x=0; x<MAX_SATELLITES; x++)
	{
		TChlSat* pSatellite = &_pChl->Satellites[x];

		if (pSatellite->wPos)
		{
			CString satStr;
			satStr.Format(L"[%04d] %S", pSatellite->wPos, pSatellite->SatName);
			int n = _satList.InsertString(-1, satStr);
			_satList.SetItemData(n, (DWORD_PTR)pSatellite);
			count++;
		}
	}


	CString str;
	str.Format(L"<All Satellites (%d)>", count);
	int n = _satList.InsertString(0, str);
	_satList.SetItemData(n, NULL);
	_satList.SetSel(n, TRUE);

	_satList.SetRedraw(true);
}

void CuEpgUIDlg::UpdateChlData(TCHL* pChl)
{
	if (!_pChl)
		_pChl = new TCHL;

	memcpy(_pChl, pChl, sizeof(TCHL));

	UpdateSatList();

	BuildChannelInfoList();
	UpdateChannelList();
}

void CuEpgUIDlg::OnCbnSelchangeComboChannelSort()
{
	UpdateChannelList();
}

void CuEpgUIDlg::OnEnChangeEditChannelSearch()
{
	KillTimer(TimerEventChannelSearchEdit);
	SetTimer(TimerEventChannelSearchEdit, 500, NULL);
}

void CuEpgUIDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);

	switch (nIDEvent)
	{
	case TimerEventChannelSearchEdit:
		BuildChannelInfoList();
		UpdateChannelList();
		break;
	}

	KillTimer(nIDEvent);
}

void CuEpgUIDlg::OnLbnSelchangeSatList()
{
	BuildChannelInfoList();
	UpdateChannelList();
}

void CuEpgUIDlg::OnCbnEditchangeComboChannelTypes()
{
	// TODO: Add your control notification handler code here
}

void CuEpgUIDlg::OnCbnSelchangeComboChannelTypes()
{
	_channelTypes ^= _channelTypesCombo.GetItemData(_channelTypesCombo.GetCurSel());
	UpdateChannelTypesCombo();

	BuildChannelInfoList();
	UpdateChannelList();
}

void CuEpgUIDlg::FindEventsCallback(void* param,
	const int eventid,
	const int starttime,
	const int duration,
	const char* eventname,
	const char* shortdescr,
	const char* extendeddescr)
{

}
