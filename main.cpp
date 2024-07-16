/**************************
 * Includes
 *
 **************************/

#include <windows.h>
#include <gl/gl.h>
#include <math.h>

/**************************
 * Function Declarations
 *
 **************************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
WPARAM wParam, LPARAM lParam);
void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC);
void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC);

POINTFLOAT *mas = NULL;
int cnt;
float scaleY;
float curX;
void DrawOs(float alfa)
{
	static float d = 0.05;
	glPushMatrix();
	glRotatef(alfa, 0, 0, 1);
	glBegin(GL_LINES);
		glVertex2f(-1,0);
		glVertex2f(1,0);
		glVertex2f(1,0);
		glVertex2f(1-d,0+d);
		glVertex2f(1,0);
		glVertex2f(1-d,0-d);
		glVertex2f(-1,0);
	glEnd();
	glPopMatrix();
}

void Init(float start, float finish, int count)
{
	cnt = count;
	
	mas = (POINTFLOAT*)realloc(mas, sizeof(POINTFLOAT) * cnt);
	//mas = malloc(sizeof(float*) * cnt);_
	float dx = (finish - start) / (cnt - 1);
	
	for (int i = 0; i < cnt; i++)
	{
		mas[i].x = start;
		mas[i].y = sin(start);
		start += dx;
	}
}

void Show()
{
	float sx = 2.0 / (mas[cnt-1].x - mas[0].x);
	float dx = (mas[cnt - 1].x + mas[0].x) * 0.5;
	glPushMatrix();
	glScalef(sx, scaleY, 1);
	glTranslatef(-dx,0,0);
	glBegin(GL_LINE_STRIP);
	for (int i=0; i < cnt; i++)
		glVertex2f(mas[i].x, mas[i].y);
	glEnd();
	
	glPopMatrix();
}

void Add(float x, float y)
{
	for (int i = 1; i < cnt; i++)
		mas[i-1] = mas[i];
	mas[cnt - 1].x = x;
	mas[cnt - 1].y = y;
}
/**************************
 * WinMain
 *
 **************************/

int WINAPI WinMain (HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int iCmdShow)
{
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;        
    MSG msg;
    BOOL bQuit = FALSE;
    float theta = 0.0f;

    /* register window class */
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor (NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "GLSample";
    RegisterClass (&wc);

    /* create main window */
    hWnd = CreateWindow (
      "GLSample", "OpenGL Tale", 
      WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE,
      0, 0, 700, 700,
      NULL, NULL, hInstance, NULL);

    /* enable OpenGL for the window */
    EnableOpenGL (hWnd, &hDC, &hRC);
    scaleY = 1.0;
    curX = 10;
    Init(10, 20, 100);

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
            glClear (GL_COLOR_BUFFER_BIT);

			glLoadIdentity();
			glLineWidth(2);
			glColor3f(1,0,0);
			DrawOs(0);
			glColor3f(0,1,0);
			DrawOs(90);
			
			curX = curX + 0.1;
			Add(curX,sin(curX));
			
			glColor3f(0,1,1);
			Show();
			
            SwapBuffers (hDC);
            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL (hWnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow (hWnd);

    return msg.wParam;
}


/********************
 * Window Procedure
 *
 ********************/

LRESULT CALLBACK WndProc (HWND hWnd, UINT message,
                          WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage (0);
        return 0;
    case WM_MOUSEWHEEL:
    	if ((int)wParam > 0) scaleY *=1.5;// scaleY = scaleY * 1.5
    		else scaleY *= 0.7;
    	if (scaleY < 0.02) scaleY = 0.02;
    	return 0;

    case WM_DESTROY:
        return 0;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        }
        return 0;

    default:
        return DefWindowProc (hWnd, message, wParam, lParam);
    }
}


/*******************
 * Enable OpenGL
 *
 *******************/

void EnableOpenGL (HWND hWnd, HDC *hDC, HGLRC *hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC (hWnd);

    /* set the pixel format for the DC */
    ZeroMemory (&pfd, sizeof (pfd));
    pfd.nSize = sizeof (pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | 
      PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat (*hDC, &pfd);
    SetPixelFormat (*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext( *hDC );
    wglMakeCurrent( *hDC, *hRC );

}


/******************
 * Disable OpenGL
 *
 ******************/

void DisableOpenGL (HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent (NULL, NULL);
    wglDeleteContext (hRC);
    ReleaseDC (hWnd, hDC);
}
