#include "process_manager.h"
#include "opcode.h"
#include "byte_op.h"
#include "setting.h"
#include <cstring>
#include <iostream>
#include <ctime>
#include <math.h>
#include "../edge/setting.h"
#include "../edge/edge.h"
#include "../edge/byte_op.h"
using namespace std;

ProcessManager::ProcessManager()
{
  this->num = 0;
}

void ProcessManager::init()
{
}

// TODO: You should implement this function if you want to change the result of the aggregation
uint8_t *ProcessManager::processData(DataSet *ds, int *dlen)
{
  uint8_t *ret, *p;
  int num, len;
  HouseData *house;
  Info *info;
  TemperatureData *tdata;
  HumidityData *hdata;
  PowerData *pdata;
  DataReceiver *dr;
  char buf[BUFLEN];
  ret = (uint8_t *)malloc(BUFLEN);
  int month, monthcycle, avg_temp, avg_humid, discomfort_index, CDD, HDD, sum_power, avg_power;
  time_t curr, ts;
  struct tm *tm;

  const double Tc_target = 24.4;
  const double Th_target = 23;

  const int vectorcode = 1;

  // getting the month value from the timestamp
  ts = ds->getTimestamp();
  tm = localtime(&ts);
  month = tm->tm_mon + 1;

  curr = ts;
  dr = new DataReceiver();
  ds = dr->getDataSet(curr);

  num = ds->getNumHouseData();

  sum_power = 0;
  for (int i=0; i<num; i++)
  {
    house = ds->getHouseData(i);
    pdata = house->getPowerData();
    sum_power += pdata->getValue();
  }
  avg_power = sum_power / num;

  tdata = ds->getTemperatureData();
  hdata = ds->getHumidityData();
  num = ds->getNumHouseData();


  const double PI = 3.1415926;

  monthcycle = -cos((month-1)/12 * 2 * PI) * 1000;

  //

  avg_temp = (double) tdata->getValue();

  avg_humid = (double) hdata->getValue();

  discomfort_index = (1.8 * avg_temp - 0.55 * (1 - avg_humid) * (1.8 * avg_temp - 26) + 32) * 1000;

  //

  if (avg_temp > Tc_target){
    CDD = 1000 * (avg_temp - Tc_target);
  }
  else{
    CDD = 0;
  }

  //

  if (avg_temp < Th_target){
    HDD = 1000 * (Th_target - avg_temp);
  }
  else{
    HDD = 0;
  }

  // initializing the memory to send to the network manager
  memset(ret, 0, BUFLEN);
  *dlen = 0;
  p = ret;

  // Example) saving the values in the memory
  VAR_TO_MEM_2BYTES_BIG_ENDIAN(vectorcode, p);
  *dlen += 2;
  VAR_TO_MEM_2BYTES_BIG_ENDIAN(avg_power, p);
  *dlen += 2;
  VAR_TO_MEM_2BYTES_BIG_ENDIAN(monthcycle, p);
  *dlen += 2;
  VAR_TO_MEM_2BYTES_BIG_ENDIAN(discomfort_index, p);
  *dlen += 2;
  VAR_TO_MEM_2BYTES_BIG_ENDIAN(CDD, p);
  *dlen += 2;
  VAR_TO_MEM_2BYTES_BIG_ENDIAN(HDD, p);
  *dlen += 2;

  return ret;
}


// uint8_t *ProcessManager::processData(DataSet *ds, int *dlen)
// {
//   uint8_t *ret, *p;
//   int num, len;
//   HouseData *house;
//   Info *info;
//   TemperatureData *tdata;
//   HumidityData *hdata;
//   PowerData *pdata;
//   DataReceiver *dr;
//   char buf[BUFLEN];
//   ret = (uint8_t *)malloc(BUFLEN);
//   int month, monthcycle, avg_temp, avg_humid, discomfort_index, TDD, sum_power, avg_power;
//   time_t curr, ts;
//   struct tm *tm;

//   const double T_target = 23.5;

//   const int vectorcode = 2;

//   // getting the month value from the timestamp
//   ts = ds->getTimestamp();
//   tm = localtime(&ts);
//   month = tm->tm_mon + 1;

//   curr = ts;
//   dr = new DataReceiver();
//   ds = dr->getDataSet(curr);

//   num = ds->getNumHouseData();

//   sum_power = 0;
//   for (int i=0; i<num; i++)
//   {
//     house = ds->getHouseData(i);
//     pdata = house->getPowerData();
//     sum_power += pdata->getValue();
//   }
//   avg_power = sum_power / num;

//   tdata = ds->getTemperatureData();
//   hdata = ds->getHumidityData();
//   num = ds->getNumHouseData();


//   const double PI = 3.1415926;

//   monthcycle = -cos((month-1)/12 * 2 * PI) * 1000;

//   //

//   avg_temp = (double) tdata->getValue();

//   avg_humid = (double) hdata->getValue();

//   discomfort_index = (1.8 * avg_temp - 0.55 * (1 - avg_humid) * (1.8 * avg_temp - 26) + 32) * 1000;

//   //

//   if (avg_temp > T_target){
//     TDD = 1000 * (avg_temp - Tc_target);
//   }
//   else{
//     TDD = 1000 * (Tc_target - avg_temp);
//   }
//
//   // initializing the memory to send to the network manager
//   memset(ret, 0, BUFLEN);
//   *dlen = 0;
//   p = ret;

//   // Example) saving the values in the memory
//   VAR_TO_MEM_2BYTES_BIG_ENDIAN(vectorcode, p);
//   *dlen += 2;
//   VAR_TO_MEM_2BYTES_BIG_ENDIAN(avg_power, p);
//   *dlen += 2;
//   VAR_TO_MEM_2BYTES_BIG_ENDIAN(monthcycle, p);
//   *dlen += 2;
//   VAR_TO_MEM_2BYTES_BIG_ENDIAN(discomfort_index, p);
//   *dlen += 2;
//   VAR_TO_MEM_2BYTES_BIG_ENDIAN(TDD, p);
//   *dlen += 2;
//   return ret;
// }

// uint8_t *ProcessManager::processData(DataSet *ds, int *dlen)
// {
//   uint8_t *ret, *p;
//   int num, len;
//   HouseData *house;
//   Info *info;
//   TemperatureData *tdata;
//   HumidityData *hdata;
//   PowerData *pdata;
//   DataReceiver *dr;
//   char buf[BUFLEN];
//   ret = (uint8_t *)malloc(BUFLEN);
//   int month, avg_temp, avg_humid, discomfort_index, TDD, sum_power, avg_power;
//   time_t curr, ts;
//   struct tm *tm;

//   const double T_target = 23.5;

//   const int vectorcode = 3;

//   // getting the month value from the timestamp
//   ts = ds->getTimestamp();
//   tm = localtime(&ts);
//   month = tm->tm_mon + 1;

//   curr = ts;
//   dr = new DataReceiver();
//   ds = dr->getDataSet(curr);

//   num = ds->getNumHouseData();

//   sum_power = 0;
//   for (int i=0; i<num; i++)
//   {
//     house = ds->getHouseData(i);
//     pdata = house->getPowerData();
//     sum_power += pdata->getValue();
//   }
//   avg_power = sum_power / num;

//   tdata = ds->getTemperatureData();
//   hdata = ds->getHumidityData();
//   num = ds->getNumHouseData();

//   //

//   avg_temp = (double) tdata->getValue();

//   avg_humid = (double) hdata->getValue();

//   discomfort_index = (1.8 * avg_temp - 0.55 * (1 - avg_humid) * (1.8 * avg_temp - 26) + 32) * 1000;

//   //

//   if (avg_temp > T_target){
//     TDD = 1000 * (avg_temp - Tc_target);
//   }
//   else{
//     TDD = 1000 * (Tc_target - avg_temp);
//   }
//
//   // initializing the memory to send to the network manager
//   memset(ret, 0, BUFLEN);
//   *dlen = 0;
//   p = ret;

//   // Example) saving the values in the memory
//   VAR_TO_MEM_2BYTES_BIG_ENDIAN(vectorcode, p);
//   *dlen += 2;
//   VAR_TO_MEM_2BYTES_BIG_ENDIAN(avg_power, p);
//   *dlen += 2;
//   VAR_TO_MEM_2BYTES_BIG_ENDIAN(month, p);
//   *dlen += 2;
//   VAR_TO_MEM_2BYTES_BIG_ENDIAN(discomfort_index, p);
//   *dlen += 2;
//   VAR_TO_MEM_2BYTES_BIG_ENDIAN(TDD, p);
//   *dlen += 2;
//   return ret;
// }
