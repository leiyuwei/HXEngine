#include "..\include\HXGDIRenderSystem.h"
#include<ctime>
#include "HXDisplayListener.h"
#include "HXGraphics.h"
#include "HXGDIRenderable.h"
#include "HXGDICamera.h"
#include "HXGDITransform.h"

namespace HX3D
{
	// 2�����ڹ��̺���
	LRESULT CALLBACK HXGDIRenderSystem::MyWindowProc(
		_In_ HWND   hwnd,
		_In_ UINT   uMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam
	)
	{
		if (NULL == m_pDisplayListener)
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		//�ж���ϢID
		switch (uMsg)
		{
		case WM_MOVING:
		case WM_PAINT:
		{
			/*HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(hwnd, &ps);
			HXWindow::pInstance->OnPaint(hdc);
			EndPaint(hwnd, &ps);*/
			break;
		}
		case WM_TIMER:
		{
			// ���ϣ���Ȼ���治��ˢ��
			// HXWindow::pInstance->UpdateMyWnd(hwnd);
			break;
		}
		case WM_KEYDOWN:
		{
			// HXWindow::GetInstance()->OnKeyDown(wParam);
			/*if (m_pDisplayListener)
			{
				m_pDisplayListener->OnKeyboard(wParam, 0, 0);
			}*/
			Keyboard(wParam, 0, 0);
			break;
		}
		case WM_KEYUP:
		{
			// HXWindow::GetInstance()->OnKeyUp(wParam);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			/*if (m_pDisplayListener)
			{
				int x = (int)LOWORD(lParam);
				int y = (int)HIWORD(lParam);
				m_pDisplayListener->OnMouseMove(x, y);
			}*/
			mouseLDown = true;
			mLastX = (int)LOWORD(lParam);
			mLastY = (int)HIWORD(lParam);
			break;
		}
		case WM_LBUTTONUP:
		{
			mouseLDown = false;
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (mouseLDown)
			{
				int x = (int)LOWORD(lParam);
				int y = (int)HIWORD(lParam);
				/*std::cout << "x = " << x << ";     y = " << y << std::endl;*/
				MouseMotion(x, y);
			}
			break;
		}
		case WM_MOUSEWHEEL:
		{
			short zDelta = (short)HIWORD(wParam);
			if (zDelta < 0)
			{
				// ����
				Mouse(4, 0, 0, 0);
			}
			else if (zDelta > 0)
			{
				// ��ǰ��
				Mouse(3, 0, 0, 0);
			}
			break;
		}
		case WM_DESTROY:	// ����������Ϣ
		{
			PostQuitMessage(0);	//  �����˳���Ϣ
			break;
		}
		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			break;
		}
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);	// ��������Ϣ����ȱʡ����Ϣ��������
		}
		return 0;
	}

	bool HXGDIRenderSystem::mouseLDown = false;
	HXGDIRenderSystem::HXGDIRenderSystem()
	{
		HXGraphics::GetInstance()->InitGraphics();
	}

	HXGDIRenderSystem::~HXGDIRenderSystem()
	{
		HXGraphics::GetInstance()->ShutDownGraphics();
	}

	void HXGDIRenderSystem::CreateRenderWindow(std::string strName, int nWidth, int nHeight, bool bFullScreen)
	{
		LPCTSTR lpClassName = "HXWinClass";
		strName += "-GDI";
		LPCTSTR lpWinName = strName.c_str();
		HINSTANCE hInstance = ::GetModuleHandle(NULL);
		RegisterMyWindow(lpClassName, hInstance);
		m_hWnd = CreateMyWindow(lpClassName, lpWinName, hInstance);
		DisplayMyWnd(m_hWnd);
	}

	void HXGDIRenderSystem::MainLoop()
	{
		MSG msg = { 0 };
		// ��ȡ��Ϣ
		//while (GetMessage(&msg, NULL, 0, 0))	// �����յ�WM_QIUT��Ϣʱ��GetMessage��������0������ѭ��
		//{
		//	DispatchMessage(&msg); // �ɷ���Ϣ����WindowPro��������
		//}
		while (TRUE)
		{
			if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					// һ��Ҫ���ϣ���Ȼ�رհ�ť���º���Ϸ���ڽ�����
					break;
				}
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			/*else
			{
			::Sleep(10);
			}*/

			HDC hdc;
			PAINTSTRUCT ps;
			hdc = BeginPaint(m_hWnd, &ps);
			// HXWindow::pInstance->OnPaint(hdc);
			Display(hdc);
			EndPaint(m_hWnd, &ps);
			UpdateMyWnd(m_hWnd);
		}
	}

	void HXGDIRenderSystem::Display(HDC hdc)
	{
		if (m_pDisplayListener)
		{
			HXGraphics::GetInstance()->ClearBuffer();

			m_pDisplayListener->OnDisplay();
			// HXStatus::GetInstance()->ShowStatusInfo();

			// ˫���弼����ֹ��˸
			HXGraphics::GetInstance()->SwapBuffer(hdc);
		}
	}

	void HXGDIRenderSystem::Reshape(int nWidth, int nHeight)
	{

	}

	HXRenderable* HXGDIRenderSystem::GenerateRenderable(HXSubMesh* pSubMesh)
	{
		HXRenderable* pRenderable = new HXGDIRenderable();
		pRenderable->GenerateArguments(pSubMesh);
		return pRenderable;
	}

	void HXGDIRenderSystem::RenderSingle(HXRenderable* pRenderable)
	{
		pRenderable->Render();
	}

	HXICamera* HXGDIRenderSystem::CreateCamera(const HXVector3D& eye, const HXVector3D& at, const HXVector3D& up,
		float ffov, float nearZ, float farZ, float viewportWidth, float viewportHeigth,
		float left, float right, float bottom, float top)
	{
		HXICamera* pCam = new HXGDICamera();
		pCam->Initialize(eye, at, up, ffov, nearZ, farZ, viewportWidth, viewportHeigth, left, right, bottom, top);
		return pCam;
	}

	/*void HXGDIRenderSystem::BeginText()
	{

	}*/

	void HXGDIRenderSystem::_DrawText(float x, float y, std::string text, const HXCOLOR& color)
	{
		HXGraphics::GetInstance()->DrawString(text, x, y, color, HXCOLOR(128,128,128,255));
	}

	HXITransform* HXGDIRenderSystem::CreateTransform()
	{
		return new HXGDITransform();
	}

	/*void HXGDIRenderSystem::EndText()
	{

	}*/

	// 3��ע�ᴰ������
	BOOL HXGDIRenderSystem::RegisterMyWindow(LPCSTR lpcWndName, HINSTANCE hInstance)
	{
		ATOM nAtom = 0;

		// ���촴�����ڲ���
		WNDCLASS wndClass = { 0 };

		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = MyWindowProc;      // ָ�򴰿ڹ��̺���
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = hInstance;
		//wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(100));// 100Ϊ��HelloWorld.rc�ļ��ж����ID��
		wndClass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
		/*wndClass.hCursor = NULL;*/	// ��Ϸ���������ָ�뻹תȦȦ
		wndClass.hCursor = LoadCursor(0, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)COLOR_GRAYTEXT;
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = lpcWndName;    // ע��Ĵ������ƣ����Ǳ��⣬�Ժ󴴽����ڸ��ݴ�ע������ƴ���

		nAtom = RegisterClass(&wndClass);
		if (0 == nAtom)
		{
			MessageBox(NULL, "Register Failed",
				"Error", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		else
		{
			/*MessageBox(NULL, "Register Successed",
			"Sucessed", MB_OK);*/
		}
		return TRUE;
	}

	// 4���������ڣ�lpClassName һ�����Ѿ�ע����Ĵ������ͣ�
	HWND HXGDIRenderSystem::CreateMyWindow(LPCTSTR lpClassName, LPCTSTR lpWinName, HINSTANCE hInstance)
	{
		HWND hWnd = NULL;
		// ��������
		hWnd = CreateWindow(lpClassName, lpWinName,
			WS_OVERLAPPEDWINDOW & (~WS_MAXIMIZEBOX)& (~WS_THICKFRAME),
			0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
			NULL, NULL, hInstance, NULL);

		if (NULL == hWnd)
		{
			MessageBox(NULL, "Create Window Failed",
				"Error", MB_OK | MB_ICONERROR);
		}
		else
		{
			/*MessageBox(NULL, "Create Winodw Successed",
			"Successed", MB_OK);*/
		}

		// ::SetTimer(hWnd, WM_TIMER, 15, NULL);	// ÿ15���봫��һ��WM_TIMER��Ϣ 

		// �ֶ�����
		srand((unsigned)time(NULL));

		return hWnd;
	}

	// 5�� ��ʾ����
	void HXGDIRenderSystem::DisplayMyWnd(HWND hWnd)
	{
		::ShowWindow(hWnd, SW_SHOW);
		::UpdateWindow(hWnd);
	}

	void HXGDIRenderSystem::UpdateMyWnd(HWND hWnd)
	{
		::InvalidateRect(hWnd, NULL, FALSE);	// Ҫ�����»���
		::UpdateWindow(hWnd);
	}
}