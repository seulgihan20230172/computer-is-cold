#include "process_manager.h"
#include "opcode.h"
#include "byte_op.h"
#include "setting.h"
#include <cstring>
#include <iostream>
#include <ctime>
#include <math.h>
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
  char buf[BUFLEN];
  ret = (uint8_t *)malloc(BUFLEN);
  int month;
  float monthcycle, avg_temp, avg_humid, discomfort_index, CDD, HDD
  time_t ts;
  struct tm *tm;

  const float Tc_target = 24.4 // https://www.google.com/url?sa=i&url=https%3A%2F%2Fwww.kmib.co.kr%2Farticle%2Fview.asp%3Farcid%3D0010819416&psig=AOvVaw2yIKpm3NqgKfKFBh6xHZQI&ust=1717437373210000&source=images&cd=vfe&opi=89978449&ved=0CBQQjhxqFwoTCJCjoPa-vYYDFQAAAAAdAAAAABAb
  const float Th_target = 23 // no reference :(

  tdata = ds->getTemperatureData();
  hdata = ds->getHumidityData();
  num = ds->getNumHouseData();

  // Example) I will give the minimum daily temperature (1 byte), the minimum daily humidity (1 byte), 
  // the minimum power data (2 bytes), the month value (1 byte) to the network manager

  // getting the month value from the timestamp
  ts = ds->getTimestamp();
  tm = localtime(&ts);
  month = tm->tm_mon + 1;

  const float PI = 3.1415926;

  monthcycle = -cos((month-1)/12 * 2 * PI)

  //

  avg_temp = (float) tdata->getValue();

  avg_humid = (float) hdata->getValue();

  discomfort_index = 1.8 * avg_temp - 0.55 * (1 - avg_humid) * (1.8 * avg_temp - 26) + 32

  //

  if (avg_temp > Tc_target){
    CDD = avg_temp - Tc_target
  }
  else{
    CDD = 0
  }

  //

  if (avg_temp < Th_target){
    HDD = Th_target - avg_temp
  }
  else{
    HDD = 0
  }

  // initializing the memory to send to the network manager
  memset(ret, 0, BUFLEN);
  *dlen = 0;
  p = ret;

  // Example) saving the values in the memory
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
