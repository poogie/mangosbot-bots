#pragma once
#include "../Multiplier.h"
#include "../Strategy.h"

namespace ai
{
    class CastTimeMultiplier : public Multiplier
    {
    public:
        CastTimeMultiplier(PlayerbotAI* ai) : Multiplier(ai, "cast time") {}

    public:
        virtual float GetValue(Action* action);
    };

    class CastTimeStrategy : public Strategy
    {
    public:
        CastTimeStrategy(PlayerbotAI* ai) : Strategy(ai) {}
        string getName() override { return "cast time"; }

    private:
        void InitCombatMultipliers(std::list<Multiplier*> &multipliers) override;
    };
}
