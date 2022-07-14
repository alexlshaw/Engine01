#ifndef ENGINE01_TIME_H
#define ENGINE01_TIME_H

const enum SEASONS {SUMMER, AUTUMN, WINTER, SPRING};

const static int TIME_SCALE = 60;	//Every minute in real time = 60 minutes in game time

//boring time division constants
const static float MINUTES_IN_HOUR = 60.0f;
const static int HOURS_IN_DAY = 24;
const static int DAYS_IN_MONTH = 30;
const static int MONTHS_IN_SEASON = 3;
const static int MONTHS_IN_YEAR = 12;

static float currentMinute = 0;	//Using float for this because it will change at a more precise level
static int currentHour = 12;	//default to noon on the first day of the year
static int currentDay = 1;
static int currentMonth = 1;

void timePassing(float delta);
void hourChanging();
void dayChanging();
void monthChanging();
void seasonChanging();	//possibly don't need to have events go out this far - depends on whether seasonal stuff should happen monthly or 3 monthly


#endif