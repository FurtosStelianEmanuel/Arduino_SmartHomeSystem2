#include "message.cpp"

/////////////////////////////////COMMANDS///////////////////////////////////

class ClearOutputBufferCommand : public Command
{
public:
	char nrOfPrints;
	bool newLine;
	ClearOutputBufferCommand(char identifier) : Command(identifier) {}
	ClearOutputBufferCommand(char rawData[]) : Command(rawData[0])
	{
		unpack(rawData);
	}
	void unpack(char input[])
	{
		nrOfPrints = input[1];
		newLine = input[2] == 1 ? true : false;
	}
};

class TestCommsCommand : public Command
{

public:
	char data[63];
	TestCommsCommand(char identifier) : Command(identifier) {}
	TestCommsCommand(char rawData[]) : Command(rawData[0])
	{
		for (int i = 0; i < 63; i++)
		{
			data[i] = rawData[i + 1];
		}
	}
};

class SetSerialSettingsCommand : public Command
{
public:
	int bufferSize;
	int timeout;
	SetSerialSettingsCommand(char identifier) : Command(identifier) {}
	SetSerialSettingsCommand(char rawData[]) : Command(rawData[0])
	{
		bufferSize = Message::convertFromJavaByte(rawData[1]);
		timeout = Message::convertFromJavaByte(rawData[2]);
	}
};

class TurnOnBuiltInLedCommand : public Command
{
public:
	TurnOnBuiltInLedCommand(char identifier) : Command(identifier) {}
	TurnOnBuiltInLedCommand(char rawData[]) : Command(rawData[0]) {}
};

class TurnOffBuiltInLedCommand : public Command
{
public:
	TurnOffBuiltInLedCommand(char identifier) : Command(identifier) {}
	TurnOffBuiltInLedCommand(char rawData[]) : Command(rawData[0]) {}
};

class DoorOpenedPeripheralCommand : public Command
{
public:
	DoorOpenedPeripheralCommand(char identifier) : Command(identifier) {}
	DoorOpenedPeripheralCommand(char rawData[]) : Command(rawData[0]) {}
};

class DoorClosedPeripheralCommand : public Command
{
public:
	DoorClosedPeripheralCommand(char identifier) : Command(identifier) {}
	DoorClosedPeripheralCommand(char rawData[]) : Command(rawData[0]) {}
};

class ModulatePulseWidthCommand : public Command
{
public:
	int pin;
	char pulseWidth;
	ModulatePulseWidthCommand(char identifier) : Command(identifier) {}
	ModulatePulseWidthCommand(char rawData[]) : Command(rawData[0])
	{
		pin = Message::convertFromJavaByte(rawData[1]);
		pulseWidth = Message::convertFromJavaByte(rawData[2]);
	}
};

////////////////////////////COMMAND RESPONSES//////////////////////////////

class ClearOutputBufferCommandResponse : public CommandResponse
{
	static const int MAXIMUM_CONFIRMATION_BYTES_LENGTH = 10;
	const int NUMBER_OF_CONFRIMATION_BYTES = 10;

public:
	char confirmationBytes[MAXIMUM_CONFIRMATION_BYTES_LENGTH];
	char endByte;

	ClearOutputBufferCommandResponse(char identifier) : CommandResponse(identifier) {}
	ClearOutputBufferCommandResponse() : CommandResponse(129) {}
	void pack(char output[])
	{
		CommandResponse::pack(output);
		endByte = identifier;
		for (int i = 0; i < NUMBER_OF_CONFRIMATION_BYTES; i++)
		{
			output[i + 1] = 200;
		}
		output[NUMBER_OF_CONFRIMATION_BYTES + 1] = endByte;
	}
};

class TestCommsCommandResponse : public CommandResponse
{
public:
	char data[63];
	TestCommsCommandResponse(char identifier) : CommandResponse(identifier) {}
	TestCommsCommandResponse(char data[]) : CommandResponse(133)
	{
		for (int i = 0; i < 63; i++)
		{
			this->data[i] = data[i];
		}
	}

	void pack(char output[])
	{
		CommandResponse::pack(output);
		for (int i = 0; i < 63; i++)
		{
			output[i + 1] = data[i];
		}
	}
};

class SetSerialSettingsCommandResponse : public CommandResponse
{
public:
	int bufferSizeSet;
	int timeoutSet;
	SetSerialSettingsCommandResponse(char identifier) : CommandResponse(identifier) {}
	SetSerialSettingsCommandResponse(int bufferSizeSet, int timeoutSet) : CommandResponse(135)
	{
		this->bufferSizeSet = bufferSizeSet;
		this->timeoutSet = timeoutSet;
	}

	void pack(char output[])
	{
		CommandResponse::pack(output);
		output[1] = Message::convertToJavaByte(bufferSizeSet);
		output[2] = Message::convertToJavaByte(timeoutSet);
	}
};

class TurnOnBuiltInLedCommandResponse : public CommandResponse
{
public:
	int pin;
	TurnOnBuiltInLedCommandResponse(char identifier) : CommandResponse(identifier) {}
	TurnOnBuiltInLedCommandResponse(int pin) : CommandResponse(137)
	{
		this->pin = pin;
	}

	void pack(char output[])
	{
		CommandResponse::pack(output);
		output[1] = Message::convertToJavaByte(pin);
	}
};

class TurnOffBuiltInLedCommandResponse : public CommandResponse
{
public:
	int pin;
	TurnOffBuiltInLedCommandResponse(char identifier) : CommandResponse(identifier) {}
	TurnOffBuiltInLedCommandResponse(int pin) : CommandResponse(139)
	{
		this->pin = pin;
	}

	void pack(char output[])
	{
		CommandResponse::pack(output);
		output[1] = Message::convertToJavaByte(pin);
	}
};

class GenericCommandResponse : public CommandResponse
{
public:
	bool isValid;
	GenericCommandResponse(char identifier) : CommandResponse(identifier) {}
	GenericCommandResponse(int isValid) : CommandResponse(143)
	{
		this->isValid = isValid == 1;
	}

	void pack(char output[])
	{
		CommandResponse::pack(output);
		output[1] = Message::convertToJavaByte(isValid ? 1 : 0);
	}
};

/////////////////////////////////QUERIES///////////////////////////////////

class DistanceSensorQuery : public Query
{

public:
	DistanceSensorQuery(char identifier) : Query(identifier) {}
	DistanceSensorQuery(char rawData[]) : Query(rawData[0]) {}
};

class AnalogValueQuery : public Query
{
public:
	int pin;
	AnalogValueQuery(char identifier) : Query(identifier) {}
	AnalogValueQuery(char rawData[]) : Query(rawData[0])
	{
		pin = Message::convertFromJavaByte(rawData[1]);
	}
};

/////////////////////////////QUERY RESULTS/////////////////////////////////

class DistanceSensorQueryResult : public QueryResult
{
public:
	int distance;
	DistanceSensorQueryResult(char identifier) : QueryResult(identifier) {}
	DistanceSensorQueryResult(int distance) : QueryResult(131)
	{
		this->distance = distance;
	}
	void pack(char output[])
	{
		QueryResult::pack(output);
		output[1] = 50;
	}
};

class AnalogValueQueryResult : public QueryResult
{
public:
	char value;
	AnalogValueQueryResult(char identifier) : QueryResult(identifier) {}
	AnalogValueQueryResult(int value) : QueryResult(145)
	{
		this->value = value;
	}

	void pack(char output[])
	{
		QueryResult::pack(output);
		output[1] = Message::convertToJavaByte(value);
	}
};