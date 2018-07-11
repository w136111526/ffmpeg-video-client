#ifndef __UIBASE_H__
#define __UIBASE_H__


#pragma once

namespace DuiLib {
	/////////////////////////////////////////////////////////////////////////////////////
	//

#define UI_WNDSTYLE_CONTAINER  (0)
#define UI_WNDSTYLE_FRAME      (WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define UI_WNDSTYLE_CHILD      (WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define UI_WNDSTYLE_DIALOG     (WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define UI_WNDSTYLE_EX_FRAME   (WS_EX_WINDOWEDGE)
#define UI_WNDSTYLE_EX_DIALOG  (WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define UI_CLASSSTYLE_CONTAINER  (0)
#define UI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define UI_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define UI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)


	/////////////////////////////////////////////////////////////////////////////////////
	//
#ifndef ASSERT
#define ASSERT(expr)  _ASSERTE(expr)
#endif

#ifdef _DEBUG
#ifndef DUITRACE
#define DUITRACE DUI__Trace
#endif
#define DUITRACEMSG DUI__TraceMsg
#else
#ifndef DUITRACE
#define DUITRACE
#endif
#define DUITRACEMSG _T("")
#endif

	void UILIB_API DUI__Trace(LPCTSTR pstrFormat, ...);
	LPCTSTR UILIB_API DUI__TraceMsg(UINT uMsg);

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class UILIB_API CNotifyPump
	{
	public:
		bool AddVirtualWnd(CDuiString strName,CNotifyPump* pObject);
		bool RemoveVirtualWnd(CDuiString strName);
		void NotifyPump(TNotifyUI& msg);
		bool LoopDispatch(TNotifyUI& msg);
		DUI_DECLARE_MESSAGE_MAP()
	private:
		CStdStringPtrMap m_VirtualWndMap;
	};

	class UILIB_API CWindowWnd
	{
	public:
		CWindowWnd();

		HWND GetHWND() const;
		operator HWND() const;

		bool RegisterWindowClass();
		bool RegisterSuperclass();

		HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
		HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
		HWND CreateDuiWindow(HWND hwndParent, LPCTSTR pstrWindowName,DWORD dwStyle =0, DWORD dwExStyle =0);
		HWND Subclass(HWND hWnd);
		void Unsubclass();
		void ShowWindow(bool bShow = true, bool bTakeFocus = true);
		UINT ShowModal();
		void Close(UINT nRet = IDOK);
		void CenterWindow();	// 居中，支持扩展屏幕
		void SetIcon(UINT nRes);

		LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
		LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
		void ResizeClient(int cx = -1, int cy = -1);

	protected:
		virtual LPCTSTR GetWindowClassName() const = 0;
		virtual LPCTSTR GetSuperClassName() const;
		virtual UINT GetClassStyle() const;

		virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		virtual void OnFinalMessage(HWND hWnd);

		static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	protected:
		HWND m_hWnd;
		WNDPROC m_OldWndProc;
		bool m_bSubclassed;
	};

	class UILIB_API CharConverter
	{
	protected:
		CharConverter(void);
		~CharConverter(void);

	public:
		/**
		* @brief 宽字符->多字节符
		* @param [in] lpWChar 要转换的宽字符字符串。传NULL不转换，只返回转换后的字符串长度
		* @param [out] lpMByte 转换后的多字节符字符串
		* @return 返回转换后的字符串长度，包含字符串结束符
		*/
		static int WChar2MByte(const wchar_t* lpWChar, char* lpMByte);

		/**
		* @brief 多字节符->宽字符
		* @param [in] lpMByte 要转换的多字节符字符串。传NULL不转换，只返回转换后的字符串长度
		* @param [out] lpWChar 转换后的宽字符字符串
		* @return 返回转换后的字符串长度，包含字符串结束符
		*/
		static int MByte2WChar(const char* lpMByte, wchar_t* lpWChar);

		/**
		* @brief 宽字符->Utf8
		* @param [in] lpWChar 要转换的宽字符字符串。传NULL不转换，只返回转换后的字符串长度
		* @param [out] lpUtf8 转换后的Utf8字符串
		* @return 返回转换后的字符串长度，包含字符串结束符
		*/
		static int WChar2Utf8(const wchar_t* lpWChar, char* lpUtf8);

		/**
		* @brief Utf8->宽字符
		* @param [in] lpMByte 要转换的Utf8字符串。传NULL不转换，只返回转换后的字符串长度
		* @param [out] lpWChar 转换后的宽字符字符串
		* @return 返回转换后的字符串长度，包含字符串结束符
		*/
		static int Utf82WChar(const char* lpUtf8, wchar_t* lpWChar);

		/**
		* @brief 多字节符->Utf8
		* @param [in] lpChar 要转换的宽字符字符串。传NULL不转换，只返回转换后的字符串长度
		* @param [out] lpUtf8 转换后的Utf8字符串
		* @return 返回转换后的字符串长度，包含字符串结束符
		*/
		static int MByte2Utf8(const char* lpChar, char* lpUtf8);

		/**
		* @brief Utf8->多字节符
		* @param [in] lpUtf8 要转换的Utf8字符串。传NULL不转换，只返回转换后的字符串长度
		* @param [out] lpChar 转换后的宽字符字符串
		* @return 返回转换后的字符串长度，包含字符串结束符
		*/
		static int Utf82MByte(const char* lpUtf8, char* lpChar);

		static int RTrim(char* lpChar);
		static int LTrim(char* lpChar);
	protected:
		static int WChar2MByteBase(const wchar_t* lpWChar, char* lpMByte, UINT uPageCode);
		static int MByte2WCharBase(const char* lpMByte, wchar_t* lpWChar, UINT uPageCode);
	};
} // namespace DuiLib


#endif // __UIBASE_H__
