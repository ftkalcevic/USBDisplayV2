/*
 * common.h
 *
 * Created: 26/02/2015 5:55:23 PM
 *  Author: Frank Tkalcevic
 */ 


#ifndef __COMMON_H__
#define __COMMON_H__


#define countof(x)		(sizeof(x)/sizeof((x)[0]))
#define HEX(x)			( (x) < 10 ? (x) + '0' : (x) + 'A' - 10 )


#endif /* COMMON_H_ */