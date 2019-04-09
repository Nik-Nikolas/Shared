// Построить график по формуле y = A + B * sin ( C * t ). Где t – время в секундах.
// Коэффициенты A , B ,C контролируются каждый своим потоком. Диапазон изменений A [3-7], B [1-3], c [1-10].
// Сделать возможность
// приостанавливать любой поток,
// задавать коэффициенты вручную.
// При работе потока
// соответствующий коэффициент
// меняется в указанных пределах в зависимости от времени  по  синусойде
// с заданной оператором частотой.
// По команде оператора запускается поток отображения на экране монитора
// и поток записи графика в текстовый файл.
// По команде оператора запускается поток отображения на экране монитора
// и поток записи графика в текстовый файл.


// Управление потоками:
//
// Для ручного изменения коэффициента- переведите соотв. поток в режим паузы.
// Повторное нажатие паузы - запуск потока.
//
// Коэффициент А: кнопки 'q' 'w' - изменение с шагом 1. 'e' - пауза потока
// Коэффициент B: кнопки 'a' 's' - изменение с шагом 1. 'd' - пауза потока
// Коэффициент C: кнопки 'z' 'x' - изменение с шагом 1. 'c' - пауза потока
//
// Изменение частоты (Гц) оператором: кнопки '1' '2' - изменение с шагом 1.
//
// Запуск потока записи графика в файл на диске: кнопка 'f'
#include "header.h"

std::mutex* MutexDevice::m_ = nullptr;

int main()
{
    // Консоль - на весь экран
    HWND hwnd;
    char Title[1024];
    int iWidth = GetSystemMetrics(SM_CXSCREEN);  // разрешение экрана по горизонтали
    int iHeight = GetSystemMetrics(SM_CYSCREEN); // разрешение экрана по вертикали
    GetConsoleTitle(Title, 1024); // Узнаем имя окна
    hwnd=FindWindow(NULL, Title); // Узнаем hwnd окна
    ShowWindow(hwnd,SW_SHOWMAXIMIZED);

    // Перо
    HPEN hPen = (HPEN)GetStockObject(WHITE_PEN);

    // получить контекст отображения
    HDC hDC = GetDC(NULL);

    //логические единицы отображаем, как физические
    SetMapMode(hDC, MM_ISOTROPIC);

    //Длина осей
    SetWindowExtEx(hDC, 500, 500, NULL);

    //Определяем облась вывода
    int xView = 500;
    int yView = xView;
    SetViewportExtEx(hDC, xView, -yView, NULL);

    //Начало координат
    SetViewportOrgEx(hDC, xView / 6, yView / 2, NULL);

    // Флаги паузы потоков
    auto pauseA = false;
    auto pauseB = false;
    auto pauseC = false;

    // Коэффициенты A , B , C
    auto a = static_cast<double>(LOWER_BOUND_A);
    auto b = static_cast<double>(LOWER_BOUND_B);
    auto c = static_cast<double>(LOWER_BOUND_C);

    // Частота изменения коэффициентов, Гц
    auto frequency = 5.0;

    // Флаг вывода графика в текстовый файл
    auto isFileOutput = false;

    // Создали мьютекс
    std::unique_ptr<std::mutex> mainMutex( MutexDevice::getInstance() );

    // Буфер + кэш графика
    const auto BUFF_SIZE = 150;
    double xBuff[BUFF_SIZE];
    double yBuff[BUFF_SIZE];
    std::uninitialized_fill(xBuff, xBuff + BUFF_SIZE, 0.0);
    std::uninitialized_fill(yBuff, yBuff + BUFF_SIZE, 0.0);

    // Запуск потока, запускающего функцию обработки событий клавиатуры
    // т.е. сейчас в процессе -2 потока
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
               yBuff,
               0,
               0,
               0);
    }

    ReleaseDC(NULL, hDC); // освободить контекст отображения

    _getch();
}
