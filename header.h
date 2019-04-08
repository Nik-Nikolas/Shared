#ifndef THISHEADER
#define THISHEADER

#include "stdafx.h"

#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <cmath>
#include <cassert>
#include <iomanip>

#include <mutex>
#include <thread>

#include <fstream>

// Начало координат и мультипликаторы (для графики)
const auto Y_OFFSET = -400.0;
const auto X_OFFSET = 100.0;
const auto Y_MULTIPLIER1 = 50.0;
const auto Y_MULTIPLIER2 = 0.02;

// Коэффициенты A , B , C - границы диапазонов
const auto LOWER_BOUND_A = 3;
const auto UPPER_BOUND_A = 7;
const auto LOWER_BOUND_B = 1;
const auto UPPER_BOUND_B = 3;
const auto LOWER_BOUND_C = 1;
const auto UPPER_BOUND_C = 10;



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
                      std::unique_ptr<std::mutex>& mainMutex);



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
              const int R,
              const int G,
              const int B);


// Функция изменения коэффициентов а/b/с и постановки соотв. потоков на паузу
void controlKeyboard1(double& coeff,
                      const double& frequency,
                      const int LOWER_BOUND,
                      const int UPPER_BOUND,
                      const int threadId,
                      bool& pauseA,
                      bool& pauseB,
                      bool& pauseC,
                      std::unique_ptr<std::mutex>& mainMutex);



// Синглтон мьютекса
class MutexDevice{
public:
  MutexDevice()                                = delete;
  MutexDevice(const MutexDevice&)              = delete;
  MutexDevice& operator = (const MutexDevice&) = delete;

  static std::mutex* getInstance(){
    if ( m_ == nullptr )
      m_ = new std::mutex;

    return m_;
  }

  static void deleteInstance(){
    delete m_;
    m_ = nullptr;
  }

private:
  static std::mutex* m_;
};
#endif











