#pragma once
#include "utils.h"

// Conversation data returned by Manager; pages decide how to display it.
struct ConversationSummary {
    int conversationId = 0;
    Data peer{};
    Message lastMessage{};
};
