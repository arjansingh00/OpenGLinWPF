#pragma once

// Exclude rarely used parts of the windows headers
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include "Helper.h"
#include "OpenGL.h"

// To use these, we must add some references...
//	o PresentationFramework (for HwndHost)
//		* PresentationCore
//		* WindowsBase
using namespace System;
using namespace System::Windows;
using namespace System::Windows::Interop;
using namespace System::Windows::Input;
using namespace System::Windows::Media;
using namespace System::Runtime::InteropServices;

namespace OpenGLCpp 
{
	LRESULT WINAPI MyMsgProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
	{
		switch (_msg)
		{
			// Make sure the window gets focus when it has to!
		case WM_IME_SETCONTEXT:
			// LOWORD(wParam) = 0 stands for deactivation, so don't set
			// focus then (results in a rather, err... 'greedy' window...)
			if (LOWORD(_wParam) > 0)
				SetFocus(_hWnd);

			return 0;

		default:
			return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
		}
	}

	//
	// This class implements HwndHost
	//
	// We have to overwrite BuildWindowCore and DestroyWindowCore
	//
	// A very simple example which creates a Win32 ListBox can be found in the MSDN:
	// http://msdn2.microsoft.com/en-us/library/aa970061.aspx
	//
	public ref class OpenGLHwnd : public HwndHost
	{
	public:
		OpenGLHwnd() : m_hRC(NULL),
			m_hDC(NULL),
			m_hWnd(NULL),
			m_hInstance(NULL),
			m_sWindowName(NULL),
			m_sClassName(NULL),
			m_fRotationAngle(0.0f)
		{
		}

	private:
		HGLRC					m_hRC;
		HDC						m_hDC;
		HWND					m_hWnd;
		HINSTANCE				m_hInstance;
		LPCWSTR					m_sWindowName;
		LPCWSTR					m_sClassName;

		float					m_fRotationAngle;

		// DPI Scaling
		double					m_dScaleX;
		double					m_dScaleY;

	protected:
		/*virtual IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, bool% handled) override
		{
			switch( msg )
			{
				case WM_IME_SETCONTEXT:
					if(LOWORD(wParam.ToInt32()) > 0)
						SetFocus(m_hWnd);

					handled = true;
					return System::IntPtr::Zero;
			}

			handled = false;
			return System::IntPtr::Zero;
		}*/
	public:
		virtual void OnRenderSizeChanged(SizeChangedInfo^ sizeInfo) override
		{
			if (m_hDC == NULL || m_hRC == NULL)
				return;

			// Apply DPI correction
			// NOTE: sizeInfo->NewSize contains doubles, so we do the multiplication before
			// converting to int.
			int iHeight = (int)(sizeInfo->NewSize.Height * m_dScaleY);
			int iWidth = (int)(sizeInfo->NewSize.Width * m_dScaleX);

			if (iWidth == 0 || iHeight == 0)
				return;

			wglMakeCurrent(m_hDC, m_hRC);
			glViewport(0, 0, iWidth, iHeight);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glOrtho(-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);
			// gluPerspective( 67.5, ((double)(iWidth) / (double)(iHeight)), 1.0, 500.0);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}

		//
		// Communicating with this method will be considerably more complicated in a real-
		// world application...
		//
		virtual void OnRender(System::Windows::Media::DrawingContext^ drawingContext) override
		{
			UNREF(drawingContext);

			if (m_hDC == NULL || m_hRC == NULL)
				return;

			wglMakeCurrent(m_hDC, m_hRC);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glLoadIdentity();

			glTranslatef(0.0f, 0.0f, -2.6f);
			glRotatef(m_fRotationAngle, 0.0f, 1.0f, 0.0f);

			glBegin(GL_TRIANGLES);
			glColor3f(1.0f, 0.0f, 0.0f);	glVertex3f(-1.0f, -1.0f, 0.0f);
			glColor3f(0.0f, 1.0f, 0.0f);	glVertex3f(0.0f, 1.0f, 0.0f);
			glColor3f(0.0f, 0.0f, 1.0f);	glVertex3f(1.0f, -1.0f, 0.0f);
			glEnd();

			glFlush();
			SwapBuffers(m_hDC); // NOTE: This is no longer wglSwapBuffers

			// For constant rotation speed, and correct rotation speeds on different machines,
			// we'd need a timer here. However, I don't want to bloat the example code.
			m_fRotationAngle += 1.0;
		}

		virtual void DestroyWindowCore(HandleRef hwnd) override
		{
			if (NULL != m_hRC)
			{
				wglDeleteContext(m_hRC);
				m_hRC = NULL;
			}

			if (NULL != m_hWnd && NULL != m_hDC)
			{
				ReleaseDC(m_hWnd, m_hDC);
				m_hDC = NULL;
			}

			if (NULL != m_hWnd && m_hWnd == (HWND)hwnd.Handle.ToPointer())
			{
				::DestroyWindow(m_hWnd);
				m_hWnd = NULL;
			}

			UnregisterClass(m_sClassName, m_hInstance);
		}

		bool RegisterWindowClass()
		{
			//
			// Create custom WNDCLASS
			//
			WNDCLASS wndClass;

			if (GetClassInfo(m_hInstance, m_sClassName, &wndClass))
			{
				// Class is already registered!
				return true;
			}

			wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

			// Not providing a WNDPROC here results in a crash on my system:
			wndClass.lpfnWndProc = (WNDPROC)MyMsgProc;
			wndClass.cbClsExtra = 0;
			wndClass.cbWndExtra = 0;
			wndClass.hInstance = m_hInstance;
			wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
			wndClass.hCursor = LoadCursor(0, IDC_ARROW);
			wndClass.hbrBackground = 0;
			wndClass.lpszMenuName = 0; // No menu
			wndClass.lpszClassName = m_sClassName;

			// ... and register it
			if (!RegisterClass(&wndClass))
			{
				Helper::ErrorExit(L"RegisterWindowClass");
				return false;
			}

			return true;
		}

		//
		// This is the key method to override
		//
		virtual HandleRef BuildWindowCore(HandleRef hwndParent) override
		{
			// Get HINSTANCE
			m_hInstance = (HINSTANCE)GetModuleHandle(NULL);
			m_sWindowName = L"OpenGL in HwndHost";
			m_sClassName = L"OGLClassHwnd";

			if (RegisterWindowClass())
			{
				// some default size
				int iWidth = 2;
				int iHeight = 2;

				DWORD dwStyle = WS_CHILD | WS_VISIBLE;

				// Get the parent (WPF) Hwnd. This is important: Windows won't let you create
				// the Hwnd otherwise.
				HWND parentHwnd = (HWND)hwndParent.Handle.ToPointer();

				m_hWnd = CreateWindowEx(0,
					m_sClassName,
					m_sWindowName,
					dwStyle,
					0,	// X Pos
					0,	// Y Pos
					iWidth,
					iHeight,
					parentHwnd, // Parent
					0,	// Menu
					m_hInstance,
					0); // Param

				if (!m_hWnd)
				{
					Helper::ErrorExit(L"BuildWindowCore");
				}

				m_hDC = GetDC(m_hWnd);
				if (!m_hDC)
				{
					Helper::ErrorExit(L"BuildWindowCore");
				}

				//
				// Create PixelFormatDescriptor and choose pixel format
				//
				uint PixelFormat;

				BYTE iAlphaBits = 0;
				BYTE iColorBits = 32;
				BYTE iDepthBits = 16;
				BYTE iAccumBits = 0;
				BYTE iStencilBits = 0;

				static PIXELFORMATDESCRIPTOR pfd =
				{
					sizeof(PIXELFORMATDESCRIPTOR),	//size
					1,								//version
					PFD_DRAW_TO_WINDOW |				//flags
					PFD_SUPPORT_OPENGL |
					PFD_DOUBLEBUFFER,
					PFD_TYPE_RGBA,					//pixeltype
					iColorBits,
					0, 0, 0, 0, 0, 0,				//color bits ignored
					iAlphaBits,
					0,								//alpha shift ignored
					iAccumBits,						//accum. buffer
					0, 0, 0, 0,						//accum bits ignored
					iDepthBits,						//depth buffer
					iStencilBits,					//stencil buffer
					0,								//aux buffer
					PFD_MAIN_PLANE,					//layer type
					0,								//reserved
					0, 0, 0							//masks ignored
				};

				PixelFormat = ChoosePixelFormat(m_hDC, &pfd);
				if (!PixelFormat)
				{
					Helper::ErrorExit(L"BuildWindowCore");
				}

				if (!SetPixelFormat(m_hDC, PixelFormat, &pfd))
				{
					Helper::ErrorExit(L"BuildWindowCore");
				}

				m_hRC = wglCreateContext(m_hDC);
				if (!m_hRC)
				{
					Helper::ErrorExit(L"BuildWindowCore");
				}

				if (!wglMakeCurrent(m_hDC, m_hRC))
				{
					Helper::ErrorExit(L"BuildWindowCore");
				}

				//
				// Get DPI information and store scaling relative to 96 DPI.
				// See http://msdn2.microsoft.com/en-us/library/ms969894.aspx
				//
				m_dScaleX = GetDeviceCaps(m_hDC, LOGPIXELSX) / 96.0;
				m_dScaleY = GetDeviceCaps(m_hDC, LOGPIXELSY) / 96.0;

				glEnable(GL_DEPTH_TEST);
				glDisable(GL_TEXTURE_2D);

				return HandleRef(this, IntPtr(m_hWnd));
			}

			return HandleRef(nullptr, System::IntPtr::Zero);
		}
	};
}
