#include "Time.h"

//handle the passing of time and trigger events (from largest scale to smallest) that are dependent on world time
void timePassing(float delta)
{
	currentMinute += delta * TIME_SCALE / 60.0f;
	if (currentMinute > MINUTES_IN_HOUR)
	{
		currentMinute -= MINUTES_IN_HOUR;
		currentHour++;
		if (currentHour >= HOURS_IN_DAY)
		{
			currentHour -= HOURS_IN_DAY;
			currentDay++;
			if (currentDay >= DAYS_IN_MONTH)
			{
				currentDay -= DAYS_IN_MONTH;
				currentMonth++;
				//possible months in season stuff here, depending on scaling needed
				monthChanging();
			}
			dayChanging();
		}
		hourChanging();
	}
}

//handle events that may occur on the hour (such as weather changing)
void hourChanging()
{

}

//handle events that may occur daily
void dayChanging()
{

}

//handle events that may occur monthly
void monthChanging()
{

}