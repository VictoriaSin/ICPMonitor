#include "tavgfilter.h"
#include <QDebug>

//Простой рекурсивный сглаживающий фильтр.
// Прекрасно подходит для расчета среднего значения или сглаживания дребезга.
// Y_n = Alpha * Y_(n-1) + (1 - Alpha) * X_n.

// Чем больше Alpha, тем больше степень сглаживания (усреднения) и сдвиг фазы.

// Если надо сильно погладить шум с меньшим смещением фазы, то можно уменьшить Alpha
// и при этом увеличить показатель Power в конструкторе (что приведет к нескольким
// последовательным вызовам фильтра, см. код метода Filtrate).


// AAlpha - Коэффициент в формуле фильтра
// APower - Требуемое число рекурсий, см. код метода Filtrate

TAVGFilter::TAVGFilter(uint8_t AAlpha, uint8_t APower)
{
  if (AAlpha == 0){
    qDebug() << "TAvgFilter: Alpha should be positive!";
  }
  if (APower == 0){
    qDebug() << "TAvgFilter: Power should be positive!";
  }
  FA = AAlpha;
  FPower = APower;

  FPrevY.resize(FPower);
  Reset();
}

void TAVGFilter::Reset()
{
  FInitialized = false;
}

double TAVGFilter::Filtrate(double AValue)
{
  int i;
  for(i=0;i< FPower-1;i++)
  {
    if(!FInitialized)
    {
      if(i==0){FPrevY[i]=AValue;}
      else    {FPrevY[i]=FPrevY[i-1];}
    }

    if(i!=0){AValue = FPrevY[i - 1];}

    FPrevY[i]= (((double)FA * FPrevY[i]) / 256.0) + (((double)(256 - FA) * AValue) / 256.0);
  }

  FInitialized = true;
  double Result;
  Result = FPrevY[FPower - 1 -1];
  return Result;
}
