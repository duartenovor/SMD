/*
 * ORIGINAL HEADER 
 *
 * sigev_thread.c
 *
 * Demonstrate use of the SIGEV_THREAD signal mode to handle
 * signals by creating a new thread.
 *
 * Special notes: This program will not compile on Solaris 2.5.
 * It will compile on Digital UNIX 4.0 but will not work.
 * Digital UNIX 4.0c fixes SIGEV_THREAD, and sources inform me
 * that Solaris 2.6 will also fix SIGEV_THREAD. To try this on
 * Solaris 2.5, remove the "#ifdef sun" conditionals in main.
 *
 * Original: 
 * http://ptgmedia.pearsoncmg.com/images/0201633922/sourcecode/sigev_thread.c 
 */

/**
 * CIRO's Header
 * This is a slightly modified version of the original one so that 
 * i can create multiple timers and trigger then in a more 
 * handy way for my project.
 */ 
 
// g++ -o main.elf -pthread timer_test.c -lpthread -lrt

#include <pthread.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <semaphore.h>
#include <time.h>
#include <iostream>

void create_timer1(unsigned i);
void create_timer2(unsigned i);
int diffSeconds(int hour, int min);

int array[] = {58,59};
int i = 0;

void err_abort(int status, char* message)
{
  fprintf(stderr, "%s\n", message);
  exit(status);
}

void errno_abort(char* message)
{
  perror(message);
  exit(EXIT_FAILURE);
}

/*
 * Thread start routine to notify the application when the
 * timer expires. This routine is run "as if" it were a new
 * thread, each time the timer expires.
 *
 * When the timer has expired 5 times, the main thread will
 * be awakened, and will terminate the program.
 */
void timer_thread1(union sigval arg)
{
  int status;

  puts("Entrei Timer 1");

  int seconds = diffSeconds(9,array[i]);
  create_timer1(seconds);

  std::cout << "------------> " << seconds << std::endl;
  create_timer2(62);

  i++;
  //create_timer2(0);
}


void timer_thread2(union sigval arg)
{
  int status;

  puts("Timer 2");
  create_timer1(0);
}

void create_timer1(unsigned i)
{
  timer_t timer_id;
  int status;
  struct itimerspec ts;
  struct sigevent se;
  //long long nanosecs = MILLION * 100 * i * i;

  /*
   * Set the sigevent structure to cause the signal to be
   * delivered by creating a new thread.
   */
  se.sigev_notify = SIGEV_THREAD;
  se.sigev_value.sival_ptr = &timer_id;
  se.sigev_notify_function = timer_thread1;
  se.sigev_notify_attributes = NULL;

  ts.it_value.tv_sec = i;
  ts.it_value.tv_nsec = 0;
  ts.it_interval.tv_sec = 0;
  ts.it_interval.tv_nsec = 0;

  status = timer_create(CLOCK_REALTIME, &se, &timer_id);
  if (status == -1)
    errno_abort((char*)"Create timer");

  // TODO maybe we'll need to have an array of itimerspec
  status = timer_settime(timer_id, 0, &ts, 0);
  if (status == -1)
    errno_abort((char*)"Set timer");
}

void create_timer2(unsigned i)
{
  timer_t timer_id;
  int status;
  struct itimerspec ts;
  struct sigevent se;
  //long long nanosecs = MILLION * 100 * i * i;

  /*
   * Set the sigevent structure to cause the signal to be
   * delivered by creating a new thread.
   */
  se.sigev_notify = SIGEV_THREAD;
  se.sigev_value.sival_ptr = &timer_id;
  se.sigev_notify_function = timer_thread2;
  se.sigev_notify_attributes = NULL;

  ts.it_value.tv_sec = 2;
  ts.it_value.tv_nsec = 0;
  ts.it_interval.tv_sec = 0;
  ts.it_interval.tv_nsec = 0;

  status = timer_create(CLOCK_REALTIME, &se, &timer_id);
  if (status == -1)
    errno_abort((char*)"Create timer");

  // TODO maybe we'll need to have an array of itimerspec
  status = timer_settime(timer_id, 0, &ts, 0);
  if (status == -1)
    errno_abort((char*)"Set timer");
}

int diffSeconds(int hour, int min)
{
  time_t now;
  struct tm timenext;
  time(&now);  /* get current time; same as: now = time(NULL)  */
  timenext = *localtime(&now);

  timenext.tm_hour = hour; 
  timenext.tm_min = min;
  timenext.tm_sec = 0;

  return (difftime(mktime(&timenext),now));
}


int main()
{
  int status;
  unsigned i = 1;

  int seconds = diffSeconds(9,57);

  std::cout << "------------> " << seconds << std::endl;

  create_timer1(seconds);

  while(1);

  return 0;
}
