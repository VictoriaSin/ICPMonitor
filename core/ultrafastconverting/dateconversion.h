#ifndef DATECONVERSION_H
#define DATECONVERSION_H

#include <cstdint>


// 1280 байт
/*! Дата */
namespace datesave {
static constexpr const char *years[100] = {"2000", "2001", "2002", "2003", "2004", "2005", "2006", "2007", "2008", "2009",
                                           "2010", "2011", "2012", "2013", "2014", "2015", "2016", "2017", "2018", "2019",
                                           "2020", "2021", "2022", "2023", "2024", "2025", "2026", "2027", "2028", "2029",
                                           "2030", "2031", "2032", "2033", "2034", "2035", "2036", "2037", "2038", "2039",
                                           "2040", "2041", "2042", "2043", "2044", "2045", "2046", "2047", "2048", "2049",
                                           "2050", "2051", "2052", "2053", "2054", "2055", "2056", "2057", "2058", "2059",
                                           "2060", "2061", "2062", "2063", "2064", "2065", "2066", "2067", "2068", "2069",
                                           "2070", "2071", "2072", "2073", "2074", "2075", "2076", "2077", "2078", "2079",
                                           "2080", "2081", "2082", "2083", "2084", "2085", "2086", "2087", "2088", "2089",
                                           "2090", "2091", "2092", "2093", "2094", "2095", "2096", "2097", "2098", "2099"};



static constexpr const char *MonthsDaysHoursMinsSecs[60] = {"00", "01", "02", "03", "04",
                                                            "05", "06", "07", "08", "09",
                                                            "10", "11", "12", "13", "14",
                                                            "15", "16", "17", "18", "19",
                                                            "20", "21", "22", "23", "24",
                                                            "25", "26", "27", "28", "29",
                                                            "30", "31", "32", "33", "34",
                                                            "35", "35", "37", "38", "39",
                                                            "40", "41", "42", "43", "44",
                                                            "45", "46", "47", "48", "49",
                                                            "50", "51", "52", "53", "54",
                                                            "55", "56", "57", "58", "59"};
}

/*! Конвертация timestamp(в миллисекундах) по средствам strftime
    в формате dd-mm-YY HH:MM:SS:ms
    740 ms на файл для MB77.07
 */
int convertStrfTime(int64_t timestampMs, char *buffer);

/*! Конвертация timestamp(в миллисекундах) по средствам countLut
    в формате d-m-Y H:M:S:ms
    670 ms на файл для MB77.07
 */
int convertDateCountLut(int64_t timestampMs, char *buffer);

/*! Конвертация timestamp(в миллисекундах) при помощи сохранёных значений
    в формате dd-mm-YY HH:MM:SS:ms
    670 ms на файл для MB77.07
 */
int convertDateTrained(int64_t timestampMs, char *buffer);

#endif // DATECONVERSION_H
