#ifndef TAVGFILTER_H
#define TAVGFILTER_H

#include <QVector>

// Рекурсивный сглаживающий фильтр

class TAVGFilter
{
private:
  // Флаг, что первое значение в фильтр уже отправлено и начальные FPrevY установлены
  bool FInitialized;
  // Коэффициент в формуле фильтра, задается в конструкторе
  uint8_t FA;
  // Требуемое число рекурсий, задается в конструкторе
  uint8_t  FPower;
  // Предыдущее значение выходного сигнала на каждом шаге, см. код метода Filtrate
//  FPrevY: array of Integer;
  QVector<double> FPrevY;

public:
  // AAlpha - Коэффициент в формуле фильтра.
  // APower - Требуемое число рекурсий, см. код метода Filtrate.
  // Чем больше Alpha, тем больше степень сглаживания (усреднения) и сдвиг фазы.
  // Если надо сильно погладить шум с меньшим смещением фазы,
  // то можно уменьшить Alpha и при этом увеличить Power.
  TAVGFilter(uint8_t AAlpha, uint8_t APower=1);
  void Reset();
  // Фильтрация очередного значения входного сигнала.
  double Filtrate(double AValue);
};

#endif // TAVGFILTER_H

