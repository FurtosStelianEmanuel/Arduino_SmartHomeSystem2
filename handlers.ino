char TRANSLATED_COMMUNICATION_BUFFER[MAX_BUFFER_SIZE];

////just for IntelliSense in vsCode
// #include "cqrs.cpp"
// #include "SmartHomeSystem.ino"

class State
{
public:
    int messageIndex;
    int maxSupportedMessages;

    State(int maxSupportedMessages)
    {
        this->maxSupportedMessages = maxSupportedMessages;
    }

    void calculateMessageIndexFromInput(char input[])
    {
        messageIndex = (int)input[0];
        messageIndex = messageIndex < 0 ? messageIndex + 128 : messageIndex;
    }

    virtual void processInput(char input[])
    {
        //Serial.println("Default implementation");
    }
};

class GiveNoResponseState : public State
{
public:
    GiveNoResponseState() : State(MAX_SUPPORTED_MESSAGES) {}
    void processInput(char input[]) override
    {
        State::calculateMessageIndexFromInput(input);
        switch (messageIndex)
        {
        case 14:
            handleCommand(ModulatePulseWidthCommand(TRANSLATED_COMMUNICATION_BUFFER));
            break;
        case 18:
            handleCommand(TransitionStateCommand(TRANSLATED_COMMUNICATION_BUFFER)).pack(COMMUNICATION_BUFFER);
            writeCommunicationBuffer();
            break;
        }
    }
};

class GiveResponseState : public State
{
public:
    GiveResponseState() : State(MAX_SUPPORTED_MESSAGES) {}
    void processInput(char input[]) override
    {
        State::calculateMessageIndexFromInput(input);

        switch (messageIndex)
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
        case 18:
            handleCommand(TransitionStateCommand(TRANSLATED_COMMUNICATION_BUFFER)).pack(COMMUNICATION_BUFFER);
            break;
        case 20:
            handleQuery(MicroControllerQuery(TRANSLATED_COMMUNICATION_BUFFER)).pack(COMMUNICATION_BUFFER);
            break;
        case 22:
            handleCommand(SetRgbStripColorCommand(TRANSLATED_COMMUNICATION_BUFFER)).pack(COMMUNICATION_BUFFER);
            break;
        case 23:
            handleCommand(SetColorSmoothlyCommand(TRANSLATED_COMMUNICATION_BUFFER)).pack(COMMUNICATION_BUFFER);
            break;
        }

        writeCommunicationBuffer();
    }
};

State *state = new GiveResponseState();

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

ModulatePulseWidthCommandResponse handleCommand(ModulatePulseWidthCommand command)
{
    analogWrite(command.pin, command.pulseWidth);

    return ModulatePulseWidthCommandResponse(command.pin, command.pulseWidth);
}

AnalogValueQueryResult handleQuery(AnalogValueQuery query)
{
    int value = analogRead(query.pin);

    return AnalogValueQueryResult(value);
}

GenericCommandResponse handleCommand(TransitionStateCommand command)
{
    bool stateTransitionOccurred = false;

    switch (command.desiredState)
    {
    case 0:
        state = new GiveResponseState();
        stateTransitionOccurred = true;
        break;
    case 1:
        state = new GiveNoResponseState();
        stateTransitionOccurred = true;
        break;
    }

    return GenericCommandResponse(stateTransitionOccurred);
}

MicroControllerQueryResult handleQuery(MicroControllerQuery query)
{
    return MicroControllerQueryResult(microControllerSignature, version);
}

GenericCommandResponse handleCommand(SetRgbStripColorCommand command)
{
    analogWrite(command.redPin, command.red);
    analogWrite(command.greenPin, command.green);
    analogWrite(command.blue, command.blue);

    return GenericCommandResponse(true);
}

GenericCommandResponse handleCommand(SetColorSmoothlyCommand command)
{
    colorTransitionSubroutine->setup(command);

    return GenericCommandResponse(true);
}

void handleMessage()
{
    translateCommunicationBufferIntoChar();
    state->processInput(TRANSLATED_COMMUNICATION_BUFFER);
}

void translateCommunicationBufferIntoChar()
{
    for (int i = 0; i < currentBufferSize; i++)
    {
        TRANSLATED_COMMUNICATION_BUFFER[i] = COMMUNICATION_BUFFER[i];
    }
}

void translateCommunicationBufferIntoByte()
{
    for (int i = 0; i < currentBufferSize; i++)
    {
        COMMUNICATION_BUFFER[i] = TRANSLATED_COMMUNICATION_BUFFER[i];
    }
}