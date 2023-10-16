#include "botpch.h"
#include "../../playerbot.h"
#include "DruidTriggers.h"
#include "DruidActions.h"

using namespace ai;

bool EntanglingRootsKiteTrigger::IsActive() 
{ 
	if (!DebuffTrigger::IsActive())
		return false;

    if (AI_VALUE(uint8, "attackers count") > 3)
        return false;

	if (!GetTarget()->HasMana())
		return false;

    list<ObjectGuid> attackers = context->GetValue<list<ObjectGuid>>("attackers")->Get();
    for (list<ObjectGuid>::iterator i = attackers.begin(); i != attackers.end(); i++)
    {
        Unit* unit = ai->GetUnit(*i);
        if (!unit || !sServerFacade.IsAlive(unit))
            continue;

        if (ai->HasMyAura("entangling roots", unit))
            return false;
    }

    return !HasMaxDebuffs();
}

Value<Unit*>* LifebloomTankTrigger::GetTargetValue()
{
    return context->GetValue<Unit*>("party tank without lifebloom", "lifebloom");
}