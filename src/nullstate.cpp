#include "nullstate.h"

NullState::NullState()
{
    printf("New NullState\n");
}

NullState::~NullState()
{
    printf("Deleting a NullState\n");
}

void NullState::OnUpdate()
{
    //printf("NullState Update (something is wrong)\n");
}

void NullState::OnEnter()
{
    //printf("NullState Enter (something is wrong)\n");
}

void NullState::OnLeave()
{
    //printf("NullState Leave (something is wrong)\n");
}

void NullState::OnMessage(std::string * message)
{
    //printf("NullState got message %s (something is wrong)\n", message->c_str());
}