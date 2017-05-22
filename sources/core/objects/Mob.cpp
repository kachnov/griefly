#include "Mob.h"
#include "../Game.h"
#include "../Map.h"

#include "net/MagicStrings.h"
#include "net/NetworkMessagesTypes.h"

using namespace kv;

Mob::Mob()
{
    // Nothing
}

void Mob::AfterWorldCreation()
{
    SetFreq(1);
}

void Mob::GenerateInterfaceForFrame()
{
}

void Mob::Delete()
{
    DeinitGui();
    MaterialObject::Delete();
}

void Mob::ProcessMessage(const Message2 &msg)
{
    if (msg.type != MessageType::ORDINARY)
    {
        return;
    }

    QJsonObject obj = Network2::ParseJson(msg);

    if (Network2::IsKey(obj, Input::MOVE_UP))
    {
        TryMove(Dir::UP);
    }
    else if (Network2::IsKey(obj, Input::MOVE_DOWN))
    {
        TryMove(Dir::DOWN);
    }
    else if (Network2::IsKey(obj, Input::MOVE_LEFT))
    {
        TryMove(Dir::LEFT);
    }
    else if (Network2::IsKey(obj, Input::MOVE_RIGHT))
    {
        TryMove(Dir::RIGHT);
    }
}

void Mob::MoveMindTo(IdPtr<Mob> other)
{
    quint32 net_id = GetGame().GetNetId(GetId());
    if (net_id == 0)
    {
        return;
    }

    DeinitGui();

    GetGame().SetPlayerId(net_id, other.Id());
    if (GetId() == GetGame().GetMob().Id())
    {
        GetGame().SetMob(other.Id());
    }

    other->InitGui();
}

bool Mob::IsMinded() const
{
    quint32 net_id = GetGame().GetNetId(GetId());
    if (net_id == 0)
    {
        return false;
    }
    return true;
}
