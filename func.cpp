#include "header.h"

// Функция сериализации буферов координат x y в бинарный файл
void saveChartToFile(const char* charFullPathDoubleBackSlashed,
                     double* xBuff,
                     double* yBuff,
                     const int BUFF_SIZE,
                     std::unique_ptr<std::mutex>& mainMutex){
   while(true){

      // RAII Владение мьютексом
      std::lock_guard<std::mutex> guard (*mainMutex);

      std::ofstream outputToFile(charFullPathDoubleBackSlashed, std::ios::app | std::ios::binary);

      if(!outputToFile.good())
          return;

      outputToFile.write (reinterpret_cast<char*>(xBuff), BUFF_SIZE * sizeof(double));
      outputToFile.write (reinterpret_cast<char*>(yBuff), BUFF_SIZE * sizeof(double));

      outputToFile.close();
   }
}


// Рисование графика
void drawFunc(HPEN hPen,
              HDC hDC,
              const double Y_OFFSET,
              const double Y_MULTIPLIER1,
              const double Y_MULTIPLIER2,
              double a,
              double b,
              double c,
              bool& isFileOutput,
              const int BUFF_SIZE,
              double* xBuff,
              double* yBuff,
              int R,
              int G,
              int B){

    MoveToEx(hDC, 0, Y_OFFSET + a * Y_MULTIPLIER1 + b, NULL); //перемещаемся в начало координат

    //Создание пера
    hPen = CreatePen(PS_SOLID, 2, RGB(R, G, B));
    SelectObject(hDC, hPen);

    // Рисуем график на BUFF_SIZE точек. Коэффициенты Y_MULTIPLIER - для подбора
    // вывода масштабированного графика на монитор

    // Рисуем график (задан не нулевой цвет)
    if( R != 0 || G != 0 || B != 0){
      auto y = 0;

      for(auto x = 0; x < BUFF_SIZE; ++x){
        y = Y_OFFSET + a * Y_MULTIPLIER1 + b * Y_MULTIPLIER1 * sin(c * Y_MULTIPLIER2 * x);

        // Буферизация и кэширование координат x y
        xBuff[x] = x;
        yBuff[x] = y;

        // Рисуем примитив графика - линию
        LineTo(hDC, x, y);
      }
    }
    // Стираем график
    else {
      for(auto x = 0; x < BUFF_SIZE; ++x){
        // Берем координаты из буфера
        LineTo(hDC, xBuff[x], yBuff[x]);
      }
    }

    // Удаляем объект пера
    DeleteObject(hPen);
}



// Функция обработки событий клавиатуры
void controlKeyboard0(double& coeff1,
                      double& coeff2,
                      double& coeff3,
                      double& frequency,
                      const int LOWER_BOUND_A,
                      const int UPPER_BOUND_A,
                      bool& pauseA,
                      const int LOWER_BOUND_B,
                      const int UPPER_BOUND_B,
                      bool& pauseB,
                      const int LOWER_BOUND_C,
                      const int UPPER_BOUND_C,
                      bool& pauseC,
                      bool& isFileOutput,
                      double* xBuff,
                      double* yBuff,
                      const size_t BUFF_SIZE,
                      std::unique_ptr<std::mutex>& mainMutex){
  while(true){
    if(_kbhit()){

      // RAII Владение мьютексом
      std::lock_guard<std::mutex> guard (*mainMutex);

      auto keyCode = _getch();

      while(_kbhit())
        _getch();

      switch(keyCode){
        case 113: // 'q' 1 поток: у
          if(LOWER_BOUND_A < coeff1)
            coeff1 -= 1.0;

          // контроль границ при ручном изменении
          coeff1 = coeff1 < LOWER_BOUND_A ? LOWER_BOUND_A : coeff1;
          break;
        case 119 : // 'w'
          if(UPPER_BOUND_A > coeff1)
            coeff1 += 1.0;

          // контроль границ при ручном изменении
          coeff1 = coeff1 > UPPER_BOUND_A ? UPPER_BOUND_A : coeff1;
          break;
        case 97: // 'a'
          if(LOWER_BOUND_B < coeff2)
            coeff2 -= 1.0;

          // контроль границ при ручном изменении
          coeff2 = coeff2 < LOWER_BOUND_B ? LOWER_BOUND_B : coeff2;
          break;
        case 115 : // 's'
          if(UPPER_BOUND_B > coeff2)
            coeff2 += 1.0;

          // контроль границ при ручном изменении
          coeff2 = coeff2 > UPPER_BOUND_B ? UPPER_BOUND_B : coeff2;
          break;
        case 122: // 'z'
          if(LOWER_BOUND_C < coeff3)
            coeff3 -= 1.0;

          // контроль границ при ручном изменении
          coeff3 = coeff3 < LOWER_BOUND_C ? LOWER_BOUND_C : coeff3;
          break;
        case 120 : // 'x'
          if(UPPER_BOUND_C > coeff3)
            coeff3 += 1.0;

          // контроль границ при ручном изменении
          coeff3 = coeff3 > UPPER_BOUND_C ? UPPER_BOUND_C : coeff3;
          break;
        case 49: // '1' уменьшаем частоту изменения коэффициента
          if(1.0 < frequency)
            frequency -= 1.0;
          break;
        case 50 : // '2' увеличиваем частоту изменения коэффициента
            frequency += 1.0;
          break;
        case 51 : // '3'
        {
          // Запуск 3 потоков (3, 4, 5 по порядковому номеру для процесса),
          // запускающих функцию изменения коэффициента A/B/C.
          // Потоки запускаются только один раз для текущего процесса.
          static auto isThreadsPackLaunched = false;

          if(false == isThreadsPackLaunched){

            std::thread coeffManager1(&controlKeyboard1,
                                      std::ref(coeff1),
                                      std::ref(frequency),
                                      LOWER_BOUND_A,
                                      UPPER_BOUND_A,
                                      3,
                                      std::ref(pauseA),
                                      std::ref(pauseB),
                                      std::ref(pauseC),
                                      std::ref(mainMutex));
            coeffManager1.detach();

            std::thread coeffManager2(&controlKeyboard1,
                                      std::ref(coeff2),
                                      std::ref(frequency),
                                      LOWER_BOUND_B,
                                      UPPER_BOUND_B,
                                      4,
                                      std::ref(pauseA),
                                      std::ref(pauseB),
                                      std::ref(pauseC),
                                      std::ref(mainMutex));
            coeffManager2.detach();

            std::thread coeffManager3(&controlKeyboard1,
                                      std::ref(coeff3),
                                      std::ref(frequency),
                                      LOWER_BOUND_C,
                                      UPPER_BOUND_C,
                                      5,
                                      std::ref(pauseA),
                                      std::ref(pauseB),
                                      std::ref(pauseC),
                                      std::ref(mainMutex));
            coeffManager3.detach();

            isThreadsPackLaunched = true;
          }

          break;
        }
        case 101 : // 'e' пауза 1 потока
          pauseA = pauseA == true? false : true;
          break;
        case 100 : // 'd' пауза 2 потока
          pauseB = pauseB == true? false : true;
          break;
        case 99 : // 'c' пауза 3 потока
          pauseC = pauseC == true? false : true;
          break;
        case 102 : // 'f'  Если задан флаг вывода в файл - выводим
        {
            isFileOutput = isFileOutput == true? false : true;

            std::thread fileOutput(&saveChartToFile,
                                    "saveChartToFile.txt",
                                    xBuff,
                                    yBuff,
                                    BUFF_SIZE,
                                    std::ref(mainMutex));
            fileOutput.detach();

            break;
        }
        // space
        case 32 :
          std::cout << " PAUSE MODE - PRINT ANY KEY TO PROCEED:               ";
          _getch();
          break;
        // ESC
        case 27 :
          exit( 0 );
        default:
          break;
      }
    }
  }
}



// Функция изменения коэффициентов A/B/C и постановки соотв. потоков на паузу
void controlKeyboard1(double& coeff,
                      const double& frequency,
                      const int LOWER_BOUND,
                      const int UPPER_BOUND,
                      const int threadId,
                      bool& pauseA,
                      bool& pauseB,
                      bool& pauseC,
                      std::unique_ptr<std::mutex>& mainMutex){

  const auto MIN_SIN_ANGLE = 3.0 * 3.14159 / 2.0; // 3 pi/2
  const auto MAX_SIN_ANGLE = 5.0 * 3.14159 / 2.0; // 5 pi/2

  auto time = MIN_SIN_ANGLE;

  auto steps = 0.0;
  auto step = 0.0;
  auto delayMicroSecs = 0;

  auto frequencyLocal = 0.0;
  auto pauseALocal = false;
  auto pauseBLocal = false;
  auto pauseCLocal = false;
  while(true){
    // Критическая секция - чтение частоты изменения коэффициентов A/B/C
    // и флагов пауз
    mainMutex->lock();

    frequencyLocal = frequency;
    pauseALocal = pauseA;
    pauseBLocal = pauseB;
    pauseCLocal = pauseC;
    mainMutex->unlock();

    // Ставим поток с соотв. ID на паузу (пока пауза не будет снята (проверка флага))
    if((3 == threadId && pauseALocal) ||
       (4 == threadId && pauseBLocal) ||
       (5 == threadId && pauseCLocal)){
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }

    // вычисление шага изменения коэффициента A/B/C
    steps = (UPPER_BOUND - LOWER_BOUND);
    assert(steps > 0.0);

    step = 1.0 * 3.14159 / steps;

    // вычисление периода для коэффициента A/B/C
    time += step;

    // Перезапуск периода (контроль переполнения double)
    if(time > std::numeric_limits<double>::max() / 2.0)
      time = MIN_SIN_ANGLE;

    // Спать между шагами изменения коэффициента A/B/C
    delayMicroSecs = static_cast<int>(1.0 / frequencyLocal * 1000000.0 / steps);
    assert(delayMicroSecs > 0);
    std::this_thread::sleep_for(std::chrono::microseconds(delayMicroSecs));

    // Критическая секция - изменение коэффициента A/B/C
    mainMutex->lock();
    coeff =  (UPPER_BOUND - LOWER_BOUND) / 2 +
             (UPPER_BOUND - LOWER_BOUND) / 2 * sin(time) +
              LOWER_BOUND;
    mainMutex->unlock();
  }
}
