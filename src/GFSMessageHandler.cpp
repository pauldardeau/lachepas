// Copyright Paul Dardeau, 2016
// GFSMessageHandler.cpp

#include "GFSMessageHandler.h"
#include "GFSMessage.h"

using namespace std;
using namespace lachepas;
using namespace tonnerre;

//******************************************************************************

void GFSMessageHandler::encodeError(Message& responseMessage,
                                    const string& error) {
   encodeBool(responseMessage, false);
   GFSMessage::setError(responseMessage, error);
}

//******************************************************************************

void GFSMessageHandler::encodeBool(Message& responseMessage, bool isSuccess) {
   GFSMessage::setRC(responseMessage, isSuccess);
}

//******************************************************************************

void GFSMessageHandler::encodeSuccess(Message& responseMessage) {
   encodeBool(responseMessage, true);
}

//******************************************************************************

