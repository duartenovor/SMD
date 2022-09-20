#include "../inc/auxiliaries.h"

#include <sstream>
#include <iostream>

int diffSeconds(int hour, int min)
{
  time_t now;
  struct tm timenext;
  ::time(&now);
  timenext = *localtime(&now);

  timenext.tm_hour = hour;
  timenext.tm_min = min;
  timenext.tm_sec = 0;

  return (difftime(mktime(&timenext),now));
}

void string_to_vector(std::string str, std::list<int> &v)
{
    const char delim = ' ';
    std::stringstream ss(str);

    std::string s;
    while (std::getline(ss, s, delim))
        v.push_back(std::stoi(s));
}

struct tm getTimeDate()
{
    time_t now;
    struct tm actualtime;
    ::time(&now);
    actualtime = *localtime(&now);

    return actualtime;
}

//for(auto const& i: vect_id)
//{
//    std::cout << i << std::endl;
//}

//    qDebug() << "tPatRFIDFunction Init" << endl;

//    CSMD *smd;
//    smd = (CSMD *)arg;

//    while(1)
//    {
//        usleep(1000000 * 10);
//        emit smd->sendToQml("mainMenuPage");
//        usleep(1000000 * 10);
//        emit smd->sendToQml("pinPage");
//    }

//for (auto v : notificationsQML.idNotList)
//        std::cout << v << "\n";
