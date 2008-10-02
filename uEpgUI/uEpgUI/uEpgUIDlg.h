// uEpgUIDlg.h : header file
//

#pragma once

#include "afxwin.h"
#include "EpgControl.h"
#include "moduleAPI.h"
#include "EpgDatabase.h"

// CuEpgUIDlg dialog
class CuEpgUIDlg : public CDialog, public CEpgDatabaseListener
{
// Construction
public:
	CuEpgUIDlg(CWnd* pParent = NULL);	// standard constructor

	enum { IDD = IDD_UEPGUI_DIALOG };

	protected:
	
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void FindEventsCallback(void* param,
		const int eventid,
		const int starttime,
		const int duration,
		const char* eventname,
		const char* shortdescr,
		const char* extendeddescr);

	void UpdateChlDataFromFile(const char* filename);
	void UpdateChlData(TCHL* pChl);

	void BuildChannelInfoList();
	void UpdateChannelList();
	void UpdateSatList();
	void UpdateChannelTypesCombo();
	void UpdateEpgGrid();

	enum ChannelSort
	{
		ChannelSortName=1,
		ChannelSortSID=2
	};

	enum TimerEvent
	{
		TimerEventChannelSearchEdit=1
	};

protected:
	HICON m_hIcon;
	CEpgControl _epgControl;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnCbnSelchangeComboChannelSort();
	afx_msg void OnEnChangeEditChannelSearch();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLbnSelchangeSatList();
	afx_msg void OnCbnEditchangeComboChannelTypes();
	afx_msg void OnCbnSelchangeComboChannelTypes();

	DECLARE_MESSAGE_MAP()

public:
	typedef std::list<TChlChannel*> ChannelInfoList;

	CListBox _channelList;
	CListBox _satList;
	CListBox _tagList;
	TCHL*    _pChl;

	ChannelInfoList _channelInfoList;
	int _channelTypes;
	CEpgDatabase _epgDatabase;

public:
	CComboBox _channelSortCombo;
	CEdit _channelSearchEdit;
	CComboBox _channelTypesCombo;

};
