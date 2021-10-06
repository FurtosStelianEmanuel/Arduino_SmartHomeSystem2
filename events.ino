long eventCheckerInterval = 1000;
long previusEventMillis = 0;
int doorPin = 3;

void checkDoorEvents();

void setupEvents()
{
	pinMode(doorPin, INPUT);
}

void checkEvents()
{
	if (currentMillis - previusEventMillis < 1000)
		return;

	previusEventMillis = currentMillis;

	checkPeripheralsForEvents();
}

void checkPeripheralsForEvents()
{
	checkDoorEvents();
}

void checkDoorEvents()
{
	static bool doorClosed = true;

	if (digitalRead(doorPin) == LOW)
	{
		if (doorClosed)
		{
			COMMUNICATION_BUFFER[0] = 140;
			writeCommunicationBuffer();
			resetBuffer();
		}

		doorClosed = false;
	}
	else
	{
		if (!doorClosed)
		{
			COMMUNICATION_BUFFER[0] = 141;
			writeCommunicationBuffer();
			resetBuffer();
		}

		doorClosed = true;
	}
}