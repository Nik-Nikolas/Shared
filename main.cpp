#include "header.h"

std::mutex* MutexDevice::m_ = nullptr;

int main()
{
    // Делаем консоль на весь экран
    HWND hwnd;
    char Title[1024];
    auto iWidth = GetSystemMetrics(SM_CXSCREEN);  // разрешение экрана по горизонтали
    auto iHeight = GetSystemMetrics(SM_CYSCREEN); // разрешение экрана по вертикали
    GetConsoleTitle(Title, 1024); // Узнаем имя окна
    hwnd = FindWindow(NULL, Title); // Узнаем hwnd окна
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);

    // Создаем объект пера
    HPEN hPen = (HPEN)GetStockObject(WHITE_PEN);

    // Получаем контекст отображения
    HDC hDC = GetDC(NULL);

    //логические единицы отображаем, как физические
    SetMapMode(hDC, MM_ISOTROPIC);

    //Длина осей
    SetWindowExtEx(hDC, 500, 500, NULL);

    //Определяем область вывода
    auto xView = 500;
    auto yView = xView;
    SetViewportExtEx(hDC, xView, -yView, NULL);

    // Задаем начало координат
    SetViewportOrgEx(hDC, xView / 6, yView / 2, NULL);

    // Флаги паузы потоков
    auto pauseA = false;
    auto pauseB = false;
    auto pauseC = false;

    // Коэффициенты A ,B ,C
    auto a = static_cast<double>(LOWER_BOUND_A);
    auto b = static_cast<double>(LOWER_BOUND_B);
    auto c = static_cast<double>(LOWER_BOUND_C);

    // Частота изменения коэффициентов, Гц
    auto frequency = 5.0;

    // Флаг вывода графика в текстовый файл
    auto isFileOutput = false;

    // Создаем объект мьютекса
    std::unique_ptr<std::mutex> mainMutex( MutexDevice::getInstance() );

    // Создаем буфер + кэш графика
    double xBuff[BUFF_SIZE];
    double yBuff[BUFF_SIZE];
    std::uninitialized_fill(xBuff, xBuff + BUFF_SIZE, 0.0);
    std::uninitialized_fill(yBuff, yBuff + BUFF_SIZE, 0.0);

    // Запуск потока, запускающего функцию обработки событий клавиатуры
    // т.е. в процессе после создания данного потока будет 2 потока (1- main и 2- control)
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

    // Построение графика
    while(true)
    {
      // RAII Владение мьютексом
      std::lock_guard<std::mutex> guard (*mainMutex);

      // Построение графика
      drawFunc(hPen,          // перо
               hDC,           // контекст отображения
               Y_OFFSET,      // смещение по у
               Y_MULTIPLIER1, // коэффициенты масштабирования по у
               Y_MULTIPLIER2,
               a,             // коэффициенты графика y = A + B * sin ( C * t )
               b,
               c,
               isFileOutput,  // флаг вывода в файл
               BUFF_SIZE,     // размер буфера-кеша
               xBuff,         // указатели на буферы-кеши
               yBuff,
               25,            // RED
               255,           // GREEN
               0);            // BLUE

      // Вывод коэффициентов
      std::cout << "                                                          ";
      std::cout << std::fixed << std::setprecision(2) << std::setfill('0')
                << "\rA = " << a
                << "  B = " << b
                << "  C = " << c
                << "  Frequency, Hertz = "
                << frequency << "\r" ;

      // Удаление графика
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
               yBuff);
    }

    ReleaseDC(NULL, hDC); // освободить контекст отображения

    _getch();
}
