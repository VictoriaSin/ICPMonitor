#include "export_enums.h"


int dateConverterFunc(ExportDataFunc func, int64_t timestampMs, char *buffer)
{
   switch (func) {
   case ExportDataFunc::TimeStamp:
       return i64toa_countlut(timestampMs, buffer);
   case ExportDataFunc::DateTimeStrfTime:
       return convertStrfTime(timestampMs, buffer);
   case ExportDataFunc::DateTimeCountLut:
       return convertDateCountLut(timestampMs, buffer);
   case ExportDataFunc::DateTimeTrained:
       return convertDateTrained(timestampMs, buffer);
   }

   return 0;
}
