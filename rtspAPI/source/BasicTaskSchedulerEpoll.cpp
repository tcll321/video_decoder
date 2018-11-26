/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 1996-2010 Live Networks, Inc.  All rights reserved.
// Basic Usage Environment: for a simple, non-scripted, console application
// Implementation

#if defined(WIN32) || defined(_WIN64)
#else
#include "BasicUsageEnvironment.hh"
#include "HandlerSet.hh"
#include <stdio.h>
#if defined(_QNX4)
#include <sys/select.h>
#include <unix.h>
#endif
#include <sys/epoll.h>
#include <sys/prctl.h>
#include "BasicTaskSchedulerEpoll.hh"

////////// BasicTaskSchedulerEpoll //////////

BasicTaskSchedulerEpoll* BasicTaskSchedulerEpoll::createNew() {
	return new BasicTaskSchedulerEpoll();
}

BasicTaskSchedulerEpoll::BasicTaskSchedulerEpoll()
  : fEpollEventMax(BASIC_TS_EPOLL_EVENT_MAX), fEpollFd(-1) {
//NPLogInfo((_T("BasicTaskSchedulerEpoll construct %d epoll event"), BASIC_TS_EPOLL_EVENT_MAX));
  fEpollFd = epoll_create(fEpollEventMax + 1);
  if (fEpollFd == -1){
	printf("BasicTaskSchedulerEpoll epoll_create error\n");
  }
  //printf("BasicTaskSchedulerEpoll epoll_create ok\n");
}

BasicTaskSchedulerEpoll::~BasicTaskSchedulerEpoll() {
	//TODO 
	if (fEpollFd != -1){
		close(fEpollFd);
		fEpollFd = -1;
	}
	//printf("BasicTaskSchedulerEpoll::~BasicTaskSchedulerEpoll()\n");
}

#ifndef MILLION
#define MILLION 1000000
#endif

static int getIndexEventByFd(struct epoll_event *events, int fd, int eventMax)
{
  int i = -1;
  for (i = 0; i < eventMax; i ++){
	if (events[i].data.fd == fd){
		return i;
	}
  }
  return -1;
}

void BasicTaskSchedulerEpoll::SingleStep(unsigned maxDelayTime) {
  struct epoll_event events[BASIC_TS_EPOLL_EVENT_MAX];
  int epollTimeout = 10;
  int eventIdx = -1;
  
  //memset(events, 0, BASIC_TS_EPOLL_EVENT_MAX * sizeof(struct epoll_event));
  DelayInterval const& timeToDelay = fDelayQueue.timeToNextAlarm();
  struct timeval tv_timeToDelay;
  tv_timeToDelay.tv_sec = timeToDelay.seconds();
  tv_timeToDelay.tv_usec = timeToDelay.useconds();
  // Very large "tv_sec" values cause select() to fail.
  // Don't make it any larger than 1 million seconds (11.5 days)
  const long MAX_TV_SEC = MILLION;
  if (tv_timeToDelay.tv_sec > MAX_TV_SEC) {
    tv_timeToDelay.tv_sec = MAX_TV_SEC;
  }
  // Also check our "maxDelayTime" parameter (if it's > 0):
  if (maxDelayTime > 0 &&
      (tv_timeToDelay.tv_sec > (long)maxDelayTime/MILLION ||
       (tv_timeToDelay.tv_sec == (long)maxDelayTime/MILLION &&
	tv_timeToDelay.tv_usec > (long)maxDelayTime%MILLION))) {
    tv_timeToDelay.tv_sec = maxDelayTime/MILLION;
    tv_timeToDelay.tv_usec = maxDelayTime%MILLION;
  }
  epollTimeout = tv_timeToDelay.tv_sec * 1000 + tv_timeToDelay.tv_usec / 1000;
  /* Frist process the Delay Queue when time out is 0 */
  if (epollTimeout == 0){ 
  	fDelayQueue.handleAlarm();
	return ;
  }

//   int ret = -1;
//   while(1) {
// 	  ret = epoll_wait(fEpollFd, events, BASIC_TS_EPOLL_EVENT_MAX, epollTimeout);
// 	  if (ret < 0) {
// 		  if (errno == EINTR) {
// 			  continue;
// 		  } 
// 		  else {
// 			  printf("BasicTaskSchedulerEpoll epoll_wait error %s\n", strerror(errno));
// 			  return;
// 		  }
// 	  }
// 	  
// 	  break;
//   }
  int ret = epoll_wait(fEpollFd, events, BASIC_TS_EPOLL_EVENT_MAX, epollTimeout);
  if (ret < 0) {
	printf("BasicTaskSchedulerEpoll epoll_wait error %s\n", strerror(errno));
	return;
  }
  // Call the handler function for one readable socket:
  HandlerIterator iter(*fHandlers);
  HandlerDescriptor* handler;
  iter.reset();

  while ((handler = iter.next()) != NULL) {
    int sock = handler->socketNum; // alias
    int resultConditionSet = 0;
	eventIdx = getIndexEventByFd(events, sock, ret);
	if (eventIdx != -1){
		/*
		if (events[eventIdx].events & EPOLLET){
			printf("%s-----%d sock %d\n", __FILE__, __LINE__, sock);
		}*/
		if (events[eventIdx].events & EPOLLIN || events[eventIdx].events & EPOLLET){
			resultConditionSet |= SOCKET_READABLE;
		}
		if (events[eventIdx].events & EPOLLOUT){
			resultConditionSet |= SOCKET_WRITABLE;
		}
		if (events[eventIdx].events & EPOLLERR){
			resultConditionSet |= SOCKET_EXCEPTION;
		}	
	}
    if ((resultConditionSet&handler->conditionSet) != 0 && handler->handlerProc != NULL) {
      fLastHandledSocketNum = sock;
          // Note: we set "fLastHandledSocketNum" before calling the handler,
          // in case the handler calls "doEventLoop()" reentrantly.
      (*handler->handlerProc)(handler->clientData, resultConditionSet);
      //break;
    }
  }
#if 0
  if (handler == NULL && fLastHandledSocketNum >= 0) {
    // We didn't call a handler, but we didn't get to check all of them,
    // so try again from the beginning:
    iter.reset();
    while ((handler = iter.next()) != NULL) {
      int sock = handler->socketNum; // alias
      int resultConditionSet = 0;
      eventIdx = getIndexEventByFd(events, sock, ret);
	  if (eventIdx != -1){
			if (events[eventIdx].events & EPOLLIN || events[eventIdx].events & EPOLLET){
				resultConditionSet |= SOCKET_READABLE;
			}
			if (events[eventIdx].events & EPOLLOUT){
				resultConditionSet |= SOCKET_WRITABLE;
			}
			if (events[eventIdx].events & EPOLLERR){
				resultConditionSet |= SOCKET_EXCEPTION;
			}
		}
      if ((resultConditionSet&handler->conditionSet) != 0 && handler->handlerProc != NULL) {
		fLastHandledSocketNum = sock;
	    // Note: we set "fLastHandledSocketNum" before calling the handler,
            // in case the handler calls "doEventLoop()" reentrantly.
		(*handler->handlerProc)(handler->clientData, resultConditionSet);
		//break;
      }
    }
    if (handler == NULL) fLastHandledSocketNum = -1;//because we didn't call a handler
  }
#endif
end:
  // Also handle any delayed event that may have come due.  (Note that we do this *after* calling a socket
  // handler, in case the delayed event handler modifies the set of readable sockets.)
  fDelayQueue.handleAlarm();
}

void BasicTaskSchedulerEpoll
  ::setBackgroundHandling(int socketNum, int conditionSet, BackgroundHandlerProc* handlerProc, void* clientData) {
  struct epoll_event ev;
  
  if (socketNum < 0 || fEpollFd == -1) return;
  
  memset(&ev, 0, sizeof(struct epoll_event));
  
  epoll_ctl(fEpollFd, EPOLL_CTL_DEL, socketNum, &ev);
  
  if (conditionSet == 0) {
    fHandlers->clearHandler(socketNum);
  } else {
    fHandlers->assignHandler(socketNum, conditionSet, handlerProc, clientData);

    if (conditionSet&SOCKET_READABLE){
	    ev.data.fd = socketNum;
		//ev.events = EPOLLIN | EPOLLET;
		ev.events = EPOLLIN;
		 if (epoll_ctl(fEpollFd, EPOLL_CTL_ADD, socketNum, &ev) == -1){
			printf("BasicTaskSchedulerEpoll epoll_ctl  EPOLL_CTL_ADD EPOLLIN error\n");
		 }
	}
    if (conditionSet&SOCKET_WRITABLE){
	    ev.data.fd = socketNum;
		ev.events = EPOLLOUT;
		 if (epoll_ctl(fEpollFd, EPOLL_CTL_ADD, socketNum, &ev) == -1){
			printf("BasicTaskSchedulerEpoll epoll_ctl  EPOLL_CTL_ADD EPOLLOUT error\n");
		 }
	}
    if (conditionSet&SOCKET_EXCEPTION){
	    ev.data.fd = socketNum;
		ev.events = EPOLLERR;
		 if (epoll_ctl(fEpollFd, EPOLL_CTL_ADD, socketNum, &ev) == -1){
			printf("BasicTaskSchedulerEpoll epoll_ctl  EPOLL_CTL_ADD EPOLLERR error\n");
		 }
	}
  }
}

void BasicTaskSchedulerEpoll::moveSocketHandling(int oldSocketNum, int newSocketNum) {
  struct epoll_event ev;
  
  if (oldSocketNum < 0 || newSocketNum < 0 || fEpollFd == -1) return; // sanity check
  
  memset(&ev, 0, sizeof(struct epoll_event));
  epoll_ctl(fEpollFd, EPOLL_CTL_DEL, oldSocketNum, &ev);
  ev.data.fd = newSocketNum;
  ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
  epoll_ctl(fEpollFd, EPOLL_CTL_ADD, newSocketNum, &ev);
  fHandlers->moveHandler(oldSocketNum, newSocketNum);
}

#endif/*#ifdef LINUX*/