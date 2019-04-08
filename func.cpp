#include "header.h"

// ������� ������������ ������� ��������� x y � �������� ����
void saveChartToFile(const char* charFullPathDoubleBackSlashed,
                     double* xBuff,
                     double* yBuff,
                     const size_t BUFF_SIZE,
                     std::unique_ptr<std::mutex>& mainMutex){
   while(true){

      // RAII �������� ���������
      std::lock_guard<std::mutex> guard (*mainMutex);

      std::ofstream outputToFile(charFullPathDoubleBackSlashed, std::ios::app | std::ios::binary);

      if(!outputToFile.good())
          return;

      outputToFile.write (reinterpret_cast<char*>(xBuff), BUFF_SIZE * sizeof(double));
      outputToFile.write (reinterpret_cast<char*>(yBuff), BUFF_SIZE * sizeof(double));

      outputToFile.close();
   }
}


// ��������� �������
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
              const int R,
              const int G,
              const int B){

    MoveToEx(hDC, 0, Y_OFFSET + a * Y_MULTIPLIER1 + b, NULL); //������������ � ������ ���������

    //�������� ����
    hPen = CreatePen(1, 2, RGB(R, G, B));
    SelectObject(hDC, hPen);

    // ������ ������ �� BUFF_SIZE �����. ������������ Y_MULTIPLIER - ��� �������
    // ������ ����������������� ������� �� �������
    auto y = 0;

    // ������ ������ (����� �� ������� ����)
    if( R != 0 || G != 0 || B != 0){
      for(auto x = 0; x < BUFF_SIZE; ++x){
        y = Y_OFFSET + a * Y_MULTIPLIER1 + b * Y_MULTIPLIER1 * sin(c * Y_MULTIPLIER2 * x);

        // ����������� � ����������� ��������� x y
        xBuff[x] = x;
        yBuff[x] = y;

        // ������ �������� ������� - �����
        LineTo(hDC, x, y);
      }
    }
    // ������� ������
    else {
      for(auto x = 0; x < BUFF_SIZE; ++x){
        // ����� ���������� �� ������
        LineTo(hDC, xBuff[x], yBuff[x]);
      }
    }

    // ������� ������ ����
    DeleteObject(hPen);
}



// ������� ��������� ������� ����������
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

      // RAII �������� ���������
      std::lock_guard<std::mutex> guard (*mainMutex);

      auto keyCode = _getch();

      while(_kbhit())
        _getch();

      switch(keyCode){
        case 113: // 'q'
          if(LOWER_BOUND_A < coeff1)
            coeff1 -= 1.0;

          // �������� ������ ��� ������ ���������
          coeff1 = coeff1 < LOWER_BOUND_A ? LOWER_BOUND_A : coeff1;
          break;
        case 119 : // 'w'
          if(UPPER_BOUND_A > coeff1)
            coeff1 += 1.0;

          // �������� ������ ��� ������ ���������
          coeff1 = coeff1 > UPPER_BOUND_A ? UPPER_BOUND_A : coeff1;
          break;
        case 97: // 'a'
          if(LOWER_BOUND_B < coeff2)
            coeff2 -= 1.0;

          // �������� ������ ��� ������ ���������
          coeff2 = coeff2 < LOWER_BOUND_B ? LOWER_BOUND_B : coeff2;
          break;
        case 115 : // 's'
          if(UPPER_BOUND_B > coeff2)
            coeff2 += 1.0;

          // �������� ������ ��� ������ ���������
          coeff2 = coeff2 > UPPER_BOUND_B ? UPPER_BOUND_B : coeff2;
          break;
        case 122: // 'z'
          if(LOWER_BOUND_C < coeff3)
            coeff3 -= 1.0;

          // �������� ������ ��� ������ ���������
          coeff3 = coeff3 < LOWER_BOUND_C ? LOWER_BOUND_C : coeff3;
          break;
        case 120 : // 'x'
          if(UPPER_BOUND_C > coeff3)
            coeff3 += 1.0;

          // �������� ������ ��� ������ ���������
          coeff3 = coeff3 > UPPER_BOUND_C ? UPPER_BOUND_C : coeff3;
          break;
        case 49: // 'e'
          if(1.0 < frequency)
            frequency -= 1.0;
          break;
        case 50 : // 'r'
            frequency += 1.0;
          break;
        case 101 : // 'e'
            pauseA = pauseA == true? false : true;
          break;
        case 100 : // 'd'
            pauseB = pauseB == true? false : true;
          break;
        case 99 : // 'c'
            pauseC = pauseC == true? false : true;
          break;
        case 102 : // 'f'  ���� ����� ���� ������ � ���� - �������
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



// ������� ��������� ������������� �/b/� � ���������� �����. ������� �� �����
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
  auto delayMsecs = 0;

  auto frequencyLocal = 0.0;
  auto pauseALocal = false;
  auto pauseBLocal = false;
  auto pauseCLocal = false;
  while(true){

    // ����������� ������ - ������ ������� ��������� ������������� �/b/c
    // � ������ ����
    mainMutex->lock();
    frequencyLocal = frequency;
    pauseALocal = pauseA;
    pauseBLocal = pauseB;
    pauseCLocal = pauseC;
    mainMutex->unlock();

    // ������ ����� � �����. ID �� ����� (���� ����� �� ����� ����� (�������� �����))
    if((1 == threadId && pauseALocal) ||
       (2 == threadId && pauseBLocal) ||
       (3 == threadId && pauseCLocal)){
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      continue;
    }

    // ���������� ���� ��������� ������������ �/b/c
    steps = (UPPER_BOUND - LOWER_BOUND);
    assert(steps > 0.0);

    step = 1.0 * 3.14159 / steps;

    // ���������� ������� ��� ������������ �/b/c
    time += step;

    // ���������� ������� (�������� ������������ double)
    if(time > std::numeric_limits<double>::max() / 2.0)
      time = MIN_SIN_ANGLE;

    // ����� ����� ������ ��������� ������������ �/b/c
    delayMsecs = 1.0 / frequencyLocal * 1000.0;
    delayMsecs /= steps;
    assert(delayMsecs > 0);
    Sleep(delayMsecs);

    // ����������� ������ - ��������� ������������ �/b/c
    mainMutex->lock();
    coeff =  (UPPER_BOUND - LOWER_BOUND) / 2 +
             (UPPER_BOUND - LOWER_BOUND) / 2 * sin(time) +
             LOWER_BOUND;
    mainMutex->unlock();
  }
}
