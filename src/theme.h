#pragma once

//define for Basalt
#ifdef PBL_PLATFORM_BASALT

//UI
#define MAX_RADIUS 65 //20
#define HOURS_THICK 5
#define MIN_THICK 4
#define SEC_THICK 2
  
//Colors
//Default Theme

#define HOURS_COLOR GColorRed
#define MIN_COLOR GColorMayGreen
#define SEC_COLOR GColorPurple
#define BACKGROUND_COLOR GColorWhite

// Colorful
/*
#define HOURS_COLOR GColorLiberty
#define MIN_COLOR GColorMayGreen
#define SEC_COLOR GColorPurple
#define BACKGROUND_COLOR GColorPastelYellow
*/
// Omonoia Theme
/*
#define HOURS_COLOR GColorWhite
#define MIN_COLOR GColorWhite
#define SEC_COLOR GColorWhite
#define BACKGROUND_COLOR GColorMayGreen
*/
// Apoel Theme
/*
#define HOURS_COLOR GColorBlue
#define MIN_COLOR GColorBlue
#define SEC_COLOR GColorBlue
#define BACKGROUND_COLOR GColorPastelYellow
*/
  
//Battery colors
  #define BATTERY_FULL_COLOR GColorDarkGreen
  #define BATTERY_WARN_COLOR GColorOrange
  #define BATTERY_EMPTY_COLOR GColorRed
#endif
  
// define function for Aplite
#ifndef PBL_PLATFORM_BASALT
  //UI
  #define MAX_RADIUS 65 //20
  #define HOURS_THICK 1
  #define MIN_THICK 1
  #define SEC_THICK 1
  
  //B-W
  #define HOURS_COLOR GColorBlack
  #define MIN_COLOR GColorBlack
  #define SEC_COLOR GColorBlack
  #define BACKGROUND_COLOR GColorWhite
  //define battery colors
  #define BATTERY_FULL_COLOR GColorBlack
  #define BATTERY_WARN_COLOR GColorBlack
  #define BATTERY_EMPTY_COLOR GColorBlack
 #endif