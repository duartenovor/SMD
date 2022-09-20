#ifndef _AUXILIARIES_H_
#define _AUXILIARIES_H_

#include <string>
#include <list>

/*seconds calculation*/
int diffSeconds(int hour, int min);

/**/
void string_to_vector(std::string str, std::list<int>& v);
struct tm getTimeDate();

#endif
