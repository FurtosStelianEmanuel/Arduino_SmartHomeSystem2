#include "Arduino.h"
#include "cqrs.cpp"

enum ExecutionMode
{
	TIME_BASED,
	LOOP_COUNTER_BASED
};

enum StripType
{
	RGB = 0,
	ARGB = 1
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
	bool isBlocked = false;

	bool shouldExecute(long loopCounter, long currentMillis)
	{
		if (isBlocked)
			return false;

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
				((Command *)new DoorClosedPeripheralCommand(140))->pack(output);
			}

			doorClosed = false;
		}
		else
		{
			if (!doorClosed)
			{
				hasOutput = true;
				((Command *)new DoorClosedPeripheralCommand(141))->pack(output);
			}

			doorClosed = true;
		}
	}
};

class ColorTransitionSubroutine : public Subroutine
{
public:
	StripType stripType;
	int argbDataPin;
	int redPin, greenPin, bluePin;

	int increment = 1;

	int currentRed, currentGreen, currentBlue;
	int targetRed, targetGreen, targetBlue;

	ColorTransitionSubroutine()
	{
		executionMode = LOOP_COUNTER_BASED;
		period = 100;
		isBlocked = true;
	}

	void execute(char *output) override
	{
		Subroutine::execute(output);
		rgbStripTransition(output);
	}

	void setup(SetColorSmoothlyCommand command)
	{
		redPin = command.redPin;
		greenPin = command.greenPin;
		bluePin = command.bluePin;
		currentRed = command.currentRed;
		currentGreen = command.currentGreen;
		currentBlue = command.currentBlue;
		targetRed = command.targetRed;
		targetGreen = command.targetGreen;
		targetBlue = command.targetBlue;
		stripType = (StripType)command.stripType;
		increment = command.increment;

		isBlocked = false;
	}

private:
	void rgbStripTransition(char *output)
	{
		if (stripType != RGB)
			return;

		int dRed = getDirectionToColor(currentRed, targetRed);
		int dGreen = getDirectionToColor(currentGreen, targetGreen);
		int dBlue = getDirectionToColor(currentBlue, targetBlue);

		currentRed += dRed * increment;
		currentGreen += dGreen * increment;
		currentBlue += dBlue * increment;

		currentRed = boundToAnalogWriteValue(currentRed);
		currentGreen = boundToAnalogWriteValue(currentGreen);
		currentBlue = boundToAnalogWriteValue(currentBlue);

		analogWrite(redPin, currentRed);
		analogWrite(greenPin, currentGreen);
		analogWrite(bluePin, currentBlue);

		if (dRed == 0 && dGreen == 0 && dBlue == 0)
		{
			StripTransitionedToColorCommand *command = new StripTransitionedToColorCommand(152);
			command->red = currentRed;
			command->green = currentGreen;
			command->blue = currentBlue;
			command->redPin = redPin;
			command->greenPin = greenPin;
			command->bluePin = bluePin;

			command->pack(output);

			hasOutput = true;
			isBlocked = true;
		}
	}

	int getDirectionToColor(int current, int target)
	{
		return current < target ? 1 : current == target ? 0
														: -1;
	}

	int boundToAnalogWriteValue(int value)
	{
		return value < 0 ? 0 : value > 255 ? 255
										   : value;
	}
};