#pragma once

class Statistics
{
public:
	Statistics(void);
	~Statistics(void);

	static DWORD CALLBACK ThreadFunc(PVOID param);
	static BOOL CALLBACK DlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
	void SetHWND(HWND hWnd);

private:
	HWND   m_hWnd;	
	HANDLE m_hThread;
	volatile LONG   m_BytesRead;
	volatile LONG   m_BytesSent;
	volatile LONG   m_BytesReadLast;
	volatile LONG   m_BytesSentLast;
	volatile LONG   m_StartTime;
	volatile LONG   m_StartTimeLast;
	volatile LONG   m_Connection;

	CRITICAL_SECTION m_cs;
	
public:
	void SetAvgSent(int data);
	void SetAvgRead(int data);
	void SetCurSent(int data);
	void SetCurRead(int data);
	void SetCurConn(int data);
	
	void DataInit(void);
	void CalcStatistics(void);

	void IncreaseConnection(void)
	{
		InterlockedIncrement(&m_Connection);
	}

	void DecreaseConnection(void)
	{
		InterlockedDecrement(&m_Connection);
	}
	void IncreaseBytesSent(DWORD dwBytesTransferred);
	void IncreaseBytesRead(DWORD dwBytesTransferred);
	void SetTime(void);
};
