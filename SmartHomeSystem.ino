#include "subroutine.cpp"

Subroutine *subroutines = {
	new PeripheralEventSubroutine()};

Message supportedMessages[] = {
	ClearOutputBufferCommand(128),			//0
	ClearOutputBufferCommandResponse(129),	//1
	DistanceSensorQuery(130),				//2
	DistanceSensorQueryResult(131),			//3
	TestCommsCommand(132),					//4
	TestCommsCommandResponse(133),			//5
	SetSerialSettingsCommand(134),			//6
	SetSerialSettingsCommandResponse(135),	//7
	TurnOnBuiltInLedCommand(136),			//8
	TurnOnBuiltInLedCommandResponse(137),	//9
	TurnOffBuiltInLedCommand(138),			//10
	TurnOffBuiltInLedCommandResponse(139),	//11
	DoorOpenedPeripheralCommand(140),		//12
	DoorClosedPeripheralCommand(141),		//13
	ModulatePulseWidthCommand(142),			//14
	GenericCommandResponse(143),			//15
	AnalogValueQuery(144),					//16
	AnalogValueQueryResult(145),			//17
	TransitionStateCommand(146),			//18
	ModulatePulseWidthCommandResponse(147), //19
	MicroControllerQuery(148),				//20
	MicroControllerQueryResult(149),		//21
	SetRgbStripColorCommand(150)			//22
};

const int MAX_SUPPORTED_MESSAGES = sizeof(supportedMessages) / sizeof(Message);
const int MAX_BUFFER_SIZE = 64;
const int NR_OF_SUBROUTINES = 1;

byte COMMUNICATION_BUFFER[MAX_BUFFER_SIZE];
int currentBufferSize = MAX_BUFFER_SIZE;

const int microControllerSignature = 1, version = 0;

void resetBuffer();
void printASCIIBuffer();
void writeCommunicationBuffer();
void processIncommingMessage();

void setup()
{
	Serial.begin(115200);
	Serial.setTimeout(5);

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);
}

void loop()
{
	static long currentMillis = 0;
	static long loopCounter = 0;

	for (int i = 0; i < NR_OF_SUBROUTINES; i++)
	{
		if (subroutines[i].shouldExecute(loopCounter, currentMillis))
		{
			subroutines[i].prepareForExecution(loopCounter, currentMillis);
			subroutines[i].execute(COMMUNICATION_BUFFER);
			if (subroutines[i].hasOutput)
			{
				writeCommunicationBuffer();
				resetBuffer();
			}
		}
	}

	currentMillis = millis();
	loopCounter++;
}

void serialEvent()
{
	Serial.readBytes(COMMUNICATION_BUFFER, currentBufferSize);
	handleMessage();
	resetBuffer();
}

void resetBuffer()
{
	for (int i = 0; i < MAX_BUFFER_SIZE; i++)
	{
		COMMUNICATION_BUFFER[i] = 0;
	}
}

void printASCIIBuffer()
{
	for (int i = 0; i < currentBufferSize; i++)
	{
		if (i % 10 == 0 && i != 0)
			Serial.println();
		Serial.print(COMMUNICATION_BUFFER[i]);
		Serial.print(" ");
	}
	Serial.println();
}

void writeCommunicationBuffer()
{
	Serial.write(COMMUNICATION_BUFFER, MAX_BUFFER_SIZE);
}