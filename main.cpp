// ��������� ������ �� ������� y = A + B * sin ( C * t ). ��� t � ����� � ��������.
// ������������ A , B ,C �������������� ������ ����� �������. �������� ��������� A [3-7], B [1-3], c [1-10].
// ������� �����������
// ���������������� ����� �����,
// �������� ������������ �������.
// ��� ������ ������
// ��������������� �����������
// �������� � ��������� �������� � ����������� �� �������  ��  ���������
// � �������� ���������� ��������.
// �� ������� ��������� ����������� ����� ����������� �� ������ ��������
// � ����� ������ ������� � ��������� ����.
// �� ������� ��������� ����������� ����� ����������� �� ������ ��������
// � ����� ������ ������� � ��������� ����.


// ���������� ��������:
//
// ��� ������� ��������� ������������- ���������� �����. ����� � ����� �����.
// ��������� ������� ����� - ������ ������.
//
// ����������� �: ������ 'q' 'w' - ��������� � ����� 1. 'e' - ����� ������
// ����������� B: ������ 'a' 's' - ��������� � ����� 1. 'd' - ����� ������
// ����������� C: ������ 'z' 'x' - ��������� � ����� 1. 'c' - ����� ������
//
// ��������� ������� (��) ����������: ������ '1' '2' - ��������� � ����� 1.
//
// ������ ������ ������ ������� � ���� �� �����: ������ 'f'
#include "header.h"

std::mutex* MutexDevice::m_ = nullptr;

int main()
{
    // ������� - �� ���� �����
    HWND hwnd;
    char Title[1024];
    int iWidth = GetSystemMetrics(SM_CXSCREEN);  // ���������� ������ �� �����������
    int iHeight = GetSystemMetrics(SM_CYSCREEN); // ���������� ������ �� ���������
    GetConsoleTitle(Title, 1024); // ������ ��� ����
    hwnd=FindWindow(NULL, Title); // ������ hwnd ����
    ShowWindow(hwnd,SW_SHOWMAXIMIZED);

    // ����
    HPEN hPen = (HPEN)GetStockObject(WHITE_PEN);

    // �������� �������� �����������
    HDC hDC = GetDC(NULL);

    //���������� ������� ����������, ��� ����������
    SetMapMode(hDC, MM_ISOTROPIC);

    //����� ����
    SetWindowExtEx(hDC, 500, 500, NULL);

    int xView = 500;
    int yView = xView;
    //���������� ������ ������
    SetViewportExtEx(hDC, xView, -yView, NULL);
    //������ ���������
    SetViewportOrgEx(hDC, xView / 6, yView / 2, NULL);

    // ����� ����� �������
    auto pauseA = false;
    auto pauseB = false;
    auto pauseC = false;

    // ������������ A , B , C
    auto a = static_cast<double>(LOWER_BOUND_A);
    auto b = static_cast<double>(LOWER_BOUND_B);
    auto c = static_cast<double>(LOWER_BOUND_C);

    // ������� ��������� �������������, ��
    auto frequency = 5.0;

    // ���� ������ ������� � ��������� ����
    auto isFileOutput = false;

    // ������� �������
    std::unique_ptr<std::mutex> mainMutex( MutexDevice::getInstance() );

    // ����� + ��� �������
    const auto BUFF_SIZE = 150;
    double xBuff[BUFF_SIZE];
    double yBuff[BUFF_SIZE];
    std::uninitialized_fill(xBuff, xBuff + BUFF_SIZE, 0.0);
    std::uninitialized_fill(yBuff, yBuff + BUFF_SIZE, 0.0);

    // ������ ������, ������������ ������� ��������� ������� ����������
    std::thread control (&controlKeyboard0,
                          std::ref(a),
                          std::ref(b),
                          std::ref(c),
                          std::ref(frequency),
                          LOWER_BOUND_A,
                          UPPER_BOUND_A,
                          std::ref(pauseA),
                          LOWER_BOUND_B,
                          UPPER_BOUND_B,
                          std::ref(pauseB),
                          LOWER_BOUND_C,
                          UPPER_BOUND_C,
                          std::ref(pauseC),
                          std::ref(isFileOutput),
                          xBuff,
                          yBuff,
                          BUFF_SIZE,
                          std::ref(mainMutex));
    control.detach();

    // ������ �������, ����������� ������� ��������� ������������ �/b/�
    std::thread coeffManager1(&controlKeyboard1,
                              std::ref(a),
                              std::ref(frequency),
                              LOWER_BOUND_A,
                              UPPER_BOUND_A,
                              1,
                              std::ref(pauseA),
                              std::ref(pauseB),
                              std::ref(pauseC),
                              std::ref(mainMutex));
    coeffManager1.detach();

    std::thread coeffManager2(&controlKeyboard1,
                              std::ref(b),
                              std::ref(frequency),
                              LOWER_BOUND_B,
                              UPPER_BOUND_B,
                              2,
                              std::ref(pauseA),
                              std::ref(pauseB),
                              std::ref(pauseC),
                              std::ref(mainMutex));
    coeffManager2.detach();

    std::thread coeffManager3(&controlKeyboard1,
                              std::ref(c),
                              std::ref(frequency),
                              LOWER_BOUND_C,
                              UPPER_BOUND_C,
                              3,
                              std::ref(pauseA),
                              std::ref(pauseB),
                              std::ref(pauseC),
                              std::ref(mainMutex));
    coeffManager3.detach();

    // ���������� �������
    while(true)
    {
      // RAII �������� ���������
      std::lock_guard<std::mutex> guard (*mainMutex);

      // ���������� �������
      drawFunc(hPen,          // ����
               hDC,           // �������� �����������
               Y_OFFSET,      // �������� �� �
               Y_MULTIPLIER1, // ������������ ��������������� �� �
               Y_MULTIPLIER2,
               a,             // ������������ ������� y = A + B * sin ( C * t )
               b,
               c,
               isFileOutput,  // ���� ������ � ����
               BUFF_SIZE,     // ������ ������-����
               xBuff,         // ��������� �� ������-����
               yBuff,
               25,            // RED
               255,           // GREEN
               0);            // BLUE

      // ����� �������������
      std::cout << "                                                          ";
      std::cout << std::fixed << std::setprecision(2) << std::setfill('0')
                << "\rA = " << a
                << "  B = " << b
                << "  C = " << c
                << "  Frequency, Hertz = "
                << frequency << "\r" ;

      // �������� �������
      drawFunc(hPen,
               hDC,
               Y_OFFSET,
               Y_MULTIPLIER1,
               Y_MULTIPLIER2,
               a,
               b,
               c,
               isFileOutput,
               BUFF_SIZE,
               xBuff,
               yBuff,
               0,
               0,
               0);
    }

    ReleaseDC(NULL, hDC); // ���������� �������� �����������

    _getch();
}
