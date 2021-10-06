ClearOutputBufferCommandResponse handleCommand(ClearOutputBufferCommand command)
{
    return ClearOutputBufferCommandResponse();
}

DistanceSensorQueryResult handleQuery(DistanceSensorQuery query)
{
    return DistanceSensorQueryResult(50);
}

TestCommsCommandResponse handleCommand(TestCommsCommand command)
{
    return TestCommsCommandResponse(command.data);
}

SetSerialSettingsCommandResponse handleCommand(SetSerialSettingsCommand command)
{
    if (command.timeout < 5)
    {
        Serial.setTimeout(5);
    }
    else
    {
        Serial.setTimeout(command.timeout);
    }

    currentBufferSize = command.bufferSize;

    return SetSerialSettingsCommandResponse(command.bufferSize, command.timeout);
}

TurnOnBuiltInLedCommandResponse handleCommand(TurnOnBuiltInLedCommand command)
{
    digitalWrite(LED_BUILTIN, HIGH);

    return TurnOnBuiltInLedCommandResponse(LED_BUILTIN);
}

TurnOffBuiltInLedCommandResponse handleCommand(TurnOffBuiltInLedCommand command)
{

    digitalWrite(LED_BUILTIN, LOW);

    return TurnOffBuiltInLedCommandResponse(LED_BUILTIN);
}

GenericCommandResponse handleCommand(ModulatePulseWidthCommand command)
{
    analogWrite(command.pin, command.pulseWidth);
    return GenericCommandResponse(true);
}

AnalogValueQueryResult handleQuery(AnalogValueQuery query)
{
    int value = analogRead(query.pin);

    return AnalogValueQueryResult(value);
}

char TRANSLATED_COMMUNICATION_BUFFER[MAX_BUFFER_SIZE];

void translateCommunicationBufferIntoChar();

void handleMessage()
{
    int identifier = COMMUNICATION_BUFFER[0];
    int index = identifier >= 128 ? identifier - 128 : identifier;
    if (index >= MAX_SUPPORTED_MESSAGES)
        return;

    translateCommunicationBufferIntoChar();

    switch (index)
    {
    case 0:
        handleCommand(ClearOutputBufferCommand(TRANSLATED_COMMUNICATION_BUFFER)).pack(COMMUNICATION_BUFFER);
        break;
    case 2:
        handleQuery(DistanceSensorQuery(TRANSLATED_COMMUNICATION_BUFFER)).pack(COMMUNICATION_BUFFER);
        break;
    case 4:
        handleCommand(TestCommsCommand(TRANSLATED_COMMUNICATION_BUFFER)).pack(COMMUNICATION_BUFFER);
        break;
    case 6:
        handleCommand(SetSerialSettingsCommand(TRANSLATED_COMMUNICATION_BUFFER)).pack(COMMUNICATION_BUFFER);
        break;
    case 8:
        handleCommand(TurnOnBuiltInLedCommand(TRANSLATED_COMMUNICATION_BUFFER)).pack(COMMUNICATION_BUFFER);
        break;
    case 10:
        handleCommand(TurnOffBuiltInLedCommand(TRANSLATED_COMMUNICATION_BUFFER)).pack(COMMUNICATION_BUFFER);
        break;
    case 14:
        handleCommand(ModulatePulseWidthCommand(TRANSLATED_COMMUNICATION_BUFFER)).pack(COMMUNICATION_BUFFER);
        break;
    case 16:
        handleQuery(AnalogValueQuery(TRANSLATED_COMMUNICATION_BUFFER)).pack(COMMUNICATION_BUFFER);
        break;
    }

    writeCommunicationBuffer();
}

// typedef CommandResponse (*Handler)(Command command);

// Handler handlers[] = {
//     handleClearOutputBufferCommand};

void translateCommunicationBufferIntoChar()
{
    for (int i = 0; i < MAX_BUFFER_SIZE; i++)
    {
        TRANSLATED_COMMUNICATION_BUFFER[i] = COMMUNICATION_BUFFER[i];
    }
}

void translateCommunicationBufferIntoByte()
{
    for (int i = 0; i < MAX_BUFFER_SIZE; i++)
    {
        COMMUNICATION_BUFFER[i] = TRANSLATED_COMMUNICATION_BUFFER[i];
    }
}