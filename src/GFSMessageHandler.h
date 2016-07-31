// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_GFSMESSAGEHANDLER_H
#define LACHEPAS_GFSMESSAGEHANDLER_H

#include <string>

#include "MessageHandlerAdapter.h"
#include "Message.h"


namespace lachepas {

class GFSMessageHandler : public tonnerre::MessageHandlerAdapter {

public:
   /**
    *
    * @param responseMessage
    * @param error
    */
   void encodeError(tonnerre::Message& responseMessage,
                    const std::string& error);

   /**
    *
    * @param responseMessage
    * @param isSuccess
    */
   void encodeBool(tonnerre::Message& responseMessage, bool isSuccess);

   /**
    *
    * @param responseMessage
    */
   void encodeSuccess(tonnerre::Message& responseMessage);

};

}

#endif

