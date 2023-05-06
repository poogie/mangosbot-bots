#pragma once
#include "ListSpellsAction.h"
#include "../values/CraftValues.h"
#include "../values/ItemUsageValue.h"
#include "GenericActions.h"

namespace ai
{
    class CastCustomSpellAction : public ChatCommandAction
    {
    public:
        CastCustomSpellAction(PlayerbotAI* ai, string name = "cast custom spell") : ChatCommandAction(ai, name) {}
        virtual bool Execute(Event& event) override;
        virtual string castString(WorldObject* target) { return "cast"; }
        virtual uint32 getDuration() const { return 3000U; }

    private:
        bool CastSummonPlayer(std::string command);

    protected:
        bool ncCast = false;
    };

    class CastCustomNcSpellAction : public CastCustomSpellAction
    {
    public:
        CastCustomNcSpellAction(PlayerbotAI* ai, string name = "cast custom nc spell") : CastCustomSpellAction(ai, name) {}
        virtual bool isUseful() { return !bot->IsInCombat(); }
        virtual string castString(WorldObject* target) { return "castnc" +(target ? " "+ chat->formatWorldobject(target):""); }
    };

    class CastRandomSpellAction : public ListSpellsAction
    {
    public:
        CastRandomSpellAction(PlayerbotAI* ai, string name = "cast random spell") : ListSpellsAction(ai, name) {}

        virtual bool AcceptSpell(const SpellEntry* pSpellInfo)
        {
            bool isTradeSkill = pSpellInfo->Effect[0] == SPELL_EFFECT_CREATE_ITEM && pSpellInfo->ReagentCount[0] > 0 &&
#ifdef MANGOSBOT_ZERO
                pSpellInfo->School == 0;
#else
                pSpellInfo->SchoolMask == 1;
#endif

            return !isTradeSkill && (GetSpellRecoveryTime(pSpellInfo) < MINUTE * IN_MILLISECONDS || !ai->HasActivePlayerMaster());
        }

        virtual uint32 GetSpellPriority(const SpellEntry* pSpellInfo) { return 1; }
        virtual bool Execute(Event& event) override;

        virtual bool castSpell(uint32 spellId, WorldObject* wo);
    };
       
    class CraftRandomItemAction : public CastRandomSpellAction
    {
    public:
        CraftRandomItemAction(PlayerbotAI* ai) : CastRandomSpellAction(ai, "enchant random item") {}
    public:
        virtual bool Execute(Event& event) override;
    };

    class DisEnchantRandomItemAction : public CastCustomSpellAction
    {
    public:
        DisEnchantRandomItemAction(PlayerbotAI* ai) : CastCustomSpellAction(ai, "disenchant random item")  {}
        virtual bool isUseful() { return ai->HasSkill(SKILL_ENCHANTING) && !bot->IsInCombat() && AI_VALUE2(uint32, "item count", "usage " + to_string(ITEM_USAGE_DISENCHANT)) > 0; }
        virtual bool Execute(Event& event) override;
    };

    class EnchantRandomItemAction : public CastRandomSpellAction
    {
    public:
        EnchantRandomItemAction(PlayerbotAI* ai) : CastRandomSpellAction(ai, "enchant random item") {}
        virtual bool isUseful() { return ai->HasSkill(SKILL_ENCHANTING) && !bot->IsInCombat() ; }

        virtual bool AcceptSpell(const SpellEntry* pSpellInfo)
        {
            return pSpellInfo->Effect[0] == SPELL_EFFECT_ENCHANT_ITEM && pSpellInfo->ReagentCount[0] > 0;
        }

        virtual uint32 GetSpellPriority(const SpellEntry* pSpellInfo) {
            if (pSpellInfo->Effect[0] == SPELL_EFFECT_ENCHANT_ITEM)
            {
                if(AI_VALUE2(Item*, "item for spell", pSpellInfo->Id) && ShouldCraftSpellValue::SpellGivesSkillUp(pSpellInfo->Id, bot))
                   return 10;
            }
            return 1;
        }

    };
}
