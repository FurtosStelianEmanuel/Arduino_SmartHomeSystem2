class Message
{
public:
	char identifier;
	Message(char identifier)
	{
		this->identifier = identifier;
	}

	virtual void pack(char output[])
	{
		output[0] = identifier;
	}
	virtual void unpack(char input[])
	{
		identifier = input[0];
	}

	int convertFromJavaByte(char javaByte)
	{
		int converted = (int)javaByte;
		return converted < 0  ? converted + 256 : converted;
	}

	char convertToJavaByte(int arduinoByte)
	{
		return arduinoByte;
	}
};

class Command : public Message
{
public:
	Command(char identifier) : Message(identifier) {}
};

class Query : public Message
{
public:
	Query(char identifier) : Message(identifier) {}
};

class CommandResponse : public Message
{
public:
	CommandResponse(char identifier) : Message(identifier) {}
	CommandResponse() : Message(129) {}
};

class QueryResult : public Message
{
public:
	QueryResult(char identifier) : Message(identifier) {}
};