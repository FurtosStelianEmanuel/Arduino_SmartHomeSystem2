#include "Arduino.h"
#include "cqrs.cpp"

enum ExecutionMode
{
	TIME_BASED,
	LOOP_COUNTER_BASED
};

class Subroutine
{
protected:
	long period;
	long lastRun = 0;
	ExecutionMode executionMode = TIME_BASED;
	long loopCounter, currentMillis;

public:
	bool hasOutput = false;

	bool shouldExecute(long loopCounter, long currentMillis)
	{
		if (executionMode == TIME_BASED)
		{
			return currentMillis - lastRun >= period;
		}

		return loopCounter % period == 0;
	}

	void prepareForExecution(long loopCounter, long currentMillis)
	{
		this->loopCounter = loopCounter;
		this->currentMillis = currentMillis;
		hasOutput = false;
	}

	virtual void execute(char *output)
	{
		if (executionMode == TIME_BASED)
		{
			lastRun = currentMillis;
		}
		else
		{
			lastRun = loopCounter;
		}
	}
};

class PeripheralEventSubroutine : public Subroutine
{
private:
	int doorPin = 3;
	void checkDoor(char *output)
	{
		static bool doorClosed = true;

		if (digitalRead(doorPin) == LOW)
		{
			if (doorClosed)
			{
				hasOutput = true;
				output[0] = 140;
			}

			doorClosed = false;
		}
		else
		{
			if (!doorClosed)
			{
				hasOutput = true;
				output[0] = 141;
			}

			doorClosed = true;
		}
	}

public:
	PeripheralEventSubroutine()
	{
		period = 1000;
		pinMode(doorPin, INPUT);
	}
	void execute(char *output) override
	{
		Subroutine::execute(output);
		checkDoor(output);
	}
};