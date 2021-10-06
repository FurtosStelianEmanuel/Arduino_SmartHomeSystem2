#include "cqrs.cpp"

Message supportedMessages[] = {
	ClearOutputBufferCommand(128),
	ClearOutputBufferCommandResponse(129),
	DistanceSensorQuery(130),
	DistanceSensorQueryResult(131),
	TestCommsCommand(132),
	TestCommsCommandResponse(133),
	SetSerialSettingsCommand(134),
	SetSerialSettingsCommandResponse(135),
	TurnOnBuiltInLedCommand(136),
	TurnOnBuiltInLedCommandResponse(137),
	TurnOffBuiltInLedCommand(138),
	TurnOffBuiltInLedCommandResponse(139),
	DoorOpenedPeripheralCommand(140),
	DoorClosedPeripheralCommand(141),
	ModulatePulseWidthCommand(142),
	GenericCommandResponse(143),
	AnalogValueQuery(144),
	AnalogValueQueryResult(145)
	};

const int MAX_SUPPORTED_MESSAGES = sizeof(supportedMessages) / sizeof(Message);
const int MAX_BUFFER_SIZE = 64;
byte COMMUNICATION_BUFFER[MAX_BUFFER_SIZE];
int currentBufferSize = MAX_BUFFER_SIZE;

long currentMillis = 0;

void resetBuffer();
void printASCIIBuffer();
void writeCommunicationBuffer();
void processIncommingMessage();
void checkEvents();
void checkPeripheralsForEvents();

void setup()
{
	Serial.begin(115200);
	Serial.setTimeout(5);

	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW);

	setupEvents();
}

void loop()
{
	currentMillis = millis();
	checkEvents();
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
	for (int i = 0; i < MAX_BUFFER_SIZE; i++)
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