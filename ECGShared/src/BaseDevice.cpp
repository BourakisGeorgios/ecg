#include "BaseDevice.h"

void BaseDevice::registerSystem(BaseSystem *system)
{
    messageForwarder->addSystem(system);
    if (system->getInStream() != nullptr)
    {
        messageBuses.push_back(new MessageBus(this, system));
    }
}

void BaseDevice::dispatchMessage(BaseSystem *system, CommandMessage *message)
{
    MessageForwarder(system).write(*message);
     if (message->payload != nullptr) {
        delete message->payload;
    }
    delete message;
}

void BaseDevice::dispatchMessage(CommandMessage *message)
{
    messageForwarder->write(*message);
    if (message->payload != nullptr) {
        delete message->payload;
    }
    delete message;
}

BaseDevice::BaseDevice(LoRaBuSMode loraBusMode)
{
    messageForwarder = new MessageForwarder();
    com = new COM();
    lora = new LoRaBus(loraBusMode);
}

std::vector<MessageBus *> BaseDevice::getMessageBuses()
{
    return std::vector<MessageBus *>(messageBuses);
}

bool BaseDevice::isOk()
{
    return com->isInitialized();
}
void BaseDevice::init()
{
    initSerial();
    initLoRa();
}

void BaseDevice::loop()
{
    for (auto messageBus : messageBuses)
    {
        messageBus->getSystem()->loop();
        messageBus->loop();
    }
}

bool BaseDevice::initSerial()
{
    auto comInit = com->init();

    if (comInit)
    {
        registerSystem(com);
    }

    return comInit;
}

bool BaseDevice::initLoRa()
{
    auto loraInit = lora->init();

    if (loraInit)
    {
        registerSystem(lora);
    }

    return loraInit;
}