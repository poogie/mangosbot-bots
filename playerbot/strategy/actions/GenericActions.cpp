#include "botpch.h"
#include "../../playerbot.h"
#include "GenericActions.h"
#include "PlayerbotFactory.h"

using namespace ai;

bool MeleeAction::isUseful()
{
    // do not allow if can't attack from vehicle
    if (ai->IsInVehicle() && !ai->IsInVehicle(false, false, true))
        return false;

    return true;
}

bool UpdateStrategyDependenciesAction::Execute(Event& event)
{
    if (!strategiesToAdd.empty() || !strategiesToRemove.empty())
    {
        // Strategies to add
        for (const StrategyToUpdate* strategy : strategiesToAdd)
        {
            std::stringstream changeStr;
            changeStr << "+" << strategy->name;
            ai->ChangeStrategy(changeStr.str(), strategy->state);
        }

        // Strategies to remove
        for (const StrategyToUpdate* strategy : strategiesToRemove)
        {
            std::stringstream changeStr;
            changeStr << "-" << strategy->name;
            ai->ChangeStrategy(changeStr.str(), strategy->state);
        }

        return true;
    }

    return false;
}

bool UpdateStrategyDependenciesAction::isUseful()
{
    if (!strategiesToUpdate.empty())
    {
        strategiesToAdd.clear();
        strategiesToRemove.clear();
        for (const StrategyToUpdate& strategy : strategiesToUpdate)
        {
            // Ignore if the strategies required are not found
            bool requiredStrategyMissing = false;
            for (const std::string& strategyRequired : strategy.strategiesRequired)
            {
                // Check if the strategy required has any aliases
                std::vector<std::string> strategyRequiredAliases = { strategyRequired };
                if (strategyRequired.find("/") != std::string::npos)
                {
                    strategyRequiredAliases.clear();
                    std::string alias;
                    std::stringstream ss(strategyRequired);
                    while (std::getline(ss, alias, '/'))
                    {
                        strategyRequiredAliases.push_back(alias);
                    }
                }

                bool synonymFound = false;
                for (const std::string& strategyRequiredAlias : strategyRequiredAliases)
                {
                    if (ai->HasStrategy(strategyRequiredAlias, strategy.state))
                    {
                        synonymFound = true;
                        break;
                    }
                }

                if (!synonymFound)
                {
                    requiredStrategyMissing = true;
                    break;
                }
            }

            if (requiredStrategyMissing)
            {
                // Check if we need to remove the strategy
                if (ai->HasStrategy(strategy.name, strategy.state))
                {
                    strategiesToRemove.emplace_back(&strategy);
                }
            }
            else
            {
                // Check if we need to add the strategy
                if (!ai->HasStrategy(strategy.name, strategy.state))
                {
                    strategiesToAdd.emplace_back(&strategy);
                }
            }
        }
    }

    return !strategiesToAdd.empty() || !strategiesToRemove.empty();
}

bool InitializePetAction::Execute(Event& event)
{
    PlayerbotFactory factory(bot, bot->GetLevel(), ITEM_QUALITY_LEGENDARY);
    factory.InitPet();
    factory.InitPetSpells();
    return true;
}

bool InitializePetAction::isUseful()
{
    // Only for random bots with item cheats enabled
    if (ai->HasCheat(BotCheatMask::item) && sPlayerbotAIConfig.IsInRandomAccountList(bot->GetSession()->GetAccountId()))
    {
        if (bot->getClass() == CLASS_HUNTER)
        {
            return !bot->GetPet();
        }
        else if (bot->getClass() == CLASS_WARLOCK)
        {
            // Only initialize if warlock has the pet summoned
            Pet* pet = bot->GetPet();
            if (pet)
            {
                constexpr uint32 PET_IMP = 416;
                constexpr uint32 PET_FELHUNTER = 417;
                constexpr uint32 PET_VOIDWALKER = 1860;
                constexpr uint32 PET_SUCCUBUS = 1863;
                constexpr uint32 PET_FELGUARD = 17252;

                //      pet type                    pet level  pet spell id
                std::map<uint32, std::vector<std::pair<uint32, uint32>>> spellList;

                // Imp spells
                {
                    // Blood Pact
                    spellList[PET_IMP].push_back(std::pair(4, 6307));
                    spellList[PET_IMP].push_back(std::pair(14, 7804));
                    spellList[PET_IMP].push_back(std::pair(26, 7805));
                    spellList[PET_IMP].push_back(std::pair(38, 11766));
                    spellList[PET_IMP].push_back(std::pair(50, 11767));
                    spellList[PET_IMP].push_back(std::pair(62, 27268));
                    spellList[PET_IMP].push_back(std::pair(74, 47982));

                    // Fire Shield
                    spellList[PET_IMP].push_back(std::pair(14, 2947));
                    spellList[PET_IMP].push_back(std::pair(24, 8316));
                    spellList[PET_IMP].push_back(std::pair(34, 8317));
                    spellList[PET_IMP].push_back(std::pair(44, 11770));
                    spellList[PET_IMP].push_back(std::pair(54, 11771));
                    spellList[PET_IMP].push_back(std::pair(64, 27269));
                    spellList[PET_IMP].push_back(std::pair(76, 47983));

                    // Firebolt
                    spellList[PET_IMP].push_back(std::pair(1, 3110));
                    spellList[PET_IMP].push_back(std::pair(8, 7799));
                    spellList[PET_IMP].push_back(std::pair(18, 7800));
                    spellList[PET_IMP].push_back(std::pair(28, 7801));
                    spellList[PET_IMP].push_back(std::pair(38, 7802));
                    spellList[PET_IMP].push_back(std::pair(48, 11762));
                    spellList[PET_IMP].push_back(std::pair(58, 11763));
                    spellList[PET_IMP].push_back(std::pair(68, 27267));
                    spellList[PET_IMP].push_back(std::pair(78, 47964));

                    // Phase Shift
                    spellList[PET_IMP].push_back(std::pair(12, 4511));
                }

                // Felhunter spells
                {
                    // Devour Magic
                    spellList[PET_FELHUNTER].push_back(std::pair(30, 19505));
                    spellList[PET_FELHUNTER].push_back(std::pair(38, 19731));
                    spellList[PET_FELHUNTER].push_back(std::pair(46, 19734));
                    spellList[PET_FELHUNTER].push_back(std::pair(54, 19736));
                    spellList[PET_FELHUNTER].push_back(std::pair(62, 27276));
                    spellList[PET_FELHUNTER].push_back(std::pair(70, 27277));
                    spellList[PET_FELHUNTER].push_back(std::pair(77, 48011));

                    // Paranoia
                    spellList[PET_FELHUNTER].push_back(std::pair(42, 19480));

                    // Spell Lock
                    spellList[PET_FELHUNTER].push_back(std::pair(36, 19244));
                    spellList[PET_FELHUNTER].push_back(std::pair(52, 19647));

                    // Tainted Blood
                    spellList[PET_FELHUNTER].push_back(std::pair(32, 19478));
                    spellList[PET_FELHUNTER].push_back(std::pair(40, 19655));
                    spellList[PET_FELHUNTER].push_back(std::pair(48, 19656));
                    spellList[PET_FELHUNTER].push_back(std::pair(56, 19660));
                    spellList[PET_FELHUNTER].push_back(std::pair(64, 27280));
                }

                // Voidwalker
                {
                    // Consume Shadows
                    spellList[PET_VOIDWALKER].push_back(std::pair(18, 17767));
                    spellList[PET_VOIDWALKER].push_back(std::pair(26, 17850));
                    spellList[PET_VOIDWALKER].push_back(std::pair(34, 17851));
                    spellList[PET_VOIDWALKER].push_back(std::pair(42, 17852));
                    spellList[PET_VOIDWALKER].push_back(std::pair(50, 17853));
                    spellList[PET_VOIDWALKER].push_back(std::pair(58, 17854));
                    spellList[PET_VOIDWALKER].push_back(std::pair(66, 27272));
                    spellList[PET_VOIDWALKER].push_back(std::pair(73, 47987));
                    spellList[PET_VOIDWALKER].push_back(std::pair(78, 47988));

                    // Sacrifice
                    spellList[PET_VOIDWALKER].push_back(std::pair(16, 7812));
                    spellList[PET_VOIDWALKER].push_back(std::pair(24, 19438));
                    spellList[PET_VOIDWALKER].push_back(std::pair(32, 19440));
                    spellList[PET_VOIDWALKER].push_back(std::pair(40, 19441));
                    spellList[PET_VOIDWALKER].push_back(std::pair(48, 19442));
                    spellList[PET_VOIDWALKER].push_back(std::pair(56, 19443));
                    spellList[PET_VOIDWALKER].push_back(std::pair(64, 27273));
                    spellList[PET_VOIDWALKER].push_back(std::pair(72, 47985));
                    spellList[PET_VOIDWALKER].push_back(std::pair(79, 47986));

                    // Suffering
                    spellList[PET_VOIDWALKER].push_back(std::pair(24, 17735));
                    spellList[PET_VOIDWALKER].push_back(std::pair(36, 17750));
                    spellList[PET_VOIDWALKER].push_back(std::pair(48, 17751));
                    spellList[PET_VOIDWALKER].push_back(std::pair(60, 17752));
                    spellList[PET_VOIDWALKER].push_back(std::pair(63, 27271));
                    spellList[PET_VOIDWALKER].push_back(std::pair(69, 33701));
                    spellList[PET_VOIDWALKER].push_back(std::pair(75, 47989));
                    spellList[PET_VOIDWALKER].push_back(std::pair(80, 47990));

                    // Torment
                    spellList[PET_VOIDWALKER].push_back(std::pair(10, 3716));
                    spellList[PET_VOIDWALKER].push_back(std::pair(20, 7809));
                    spellList[PET_VOIDWALKER].push_back(std::pair(30, 7810));
                    spellList[PET_VOIDWALKER].push_back(std::pair(40, 7811));
                    spellList[PET_VOIDWALKER].push_back(std::pair(50, 11774));
                    spellList[PET_VOIDWALKER].push_back(std::pair(60, 11775));
                    spellList[PET_VOIDWALKER].push_back(std::pair(70, 27270));
                    spellList[PET_VOIDWALKER].push_back(std::pair(80, 47984));
                }

                // Succubus
                {
                    // Lash of Pain
                    spellList[PET_SUCCUBUS].push_back(std::pair(20, 7814));
                    spellList[PET_SUCCUBUS].push_back(std::pair(28, 7815));
                    spellList[PET_SUCCUBUS].push_back(std::pair(36, 7816));
                    spellList[PET_SUCCUBUS].push_back(std::pair(44, 11778));
                    spellList[PET_SUCCUBUS].push_back(std::pair(52, 11779));
                    spellList[PET_SUCCUBUS].push_back(std::pair(60, 11780));
                    spellList[PET_SUCCUBUS].push_back(std::pair(68, 27274));
                    spellList[PET_SUCCUBUS].push_back(std::pair(74, 47991));
                    spellList[PET_SUCCUBUS].push_back(std::pair(80, 47992));

                    // Lesser Invisibility
                    spellList[PET_SUCCUBUS].push_back(std::pair(32, 7870));

                    // Seduction
                    spellList[PET_SUCCUBUS].push_back(std::pair(26, 6358));

                    // Soothing Kiss
                    spellList[PET_SUCCUBUS].push_back(std::pair(22, 6360));
                    spellList[PET_SUCCUBUS].push_back(std::pair(34, 7813));
                    spellList[PET_SUCCUBUS].push_back(std::pair(46, 11784));
                    spellList[PET_SUCCUBUS].push_back(std::pair(58, 11785));
                    spellList[PET_SUCCUBUS].push_back(std::pair(70, 27275));
                }

                // Felguard
                {
                    // Anguish
                    spellList[PET_FELGUARD].push_back(std::pair(50, 33698));
                    spellList[PET_FELGUARD].push_back(std::pair(60, 33699));
                    spellList[PET_FELGUARD].push_back(std::pair(69, 33700));
                    spellList[PET_FELGUARD].push_back(std::pair(78, 47993));

                    // Avoidance
                    spellList[PET_FELGUARD].push_back(std::pair(60, 32233));

                    // Cleave
                    spellList[PET_FELGUARD].push_back(std::pair(50, 30213));
                    spellList[PET_FELGUARD].push_back(std::pair(60, 30219));
                    spellList[PET_FELGUARD].push_back(std::pair(68, 30223));
                    spellList[PET_FELGUARD].push_back(std::pair(76, 47994));

                    // Demonic Frenzy
                    spellList[PET_FELGUARD].push_back(std::pair(56, 32850));

                    // Intercept
                    spellList[PET_FELGUARD].push_back(std::pair(52, 30151));
                    spellList[PET_FELGUARD].push_back(std::pair(61, 30194));
                    spellList[PET_FELGUARD].push_back(std::pair(69, 30198));
                    spellList[PET_FELGUARD].push_back(std::pair(79, 47996));
                }

                // Get the appropriate spell list by level and type
                const auto& petSpellListItr = spellList.find(pet->GetEntry());
                if (petSpellListItr != spellList.end())
                {
                    const auto& petSpellList = petSpellListItr->second;
                    for (const auto& pair : petSpellListItr->second)
                    {
                        const uint32& levelRequired = pair.first;
                        const uint32& spellID = pair.second;

                        // Check if the pet is missing the spells for it's current level
                        if (pet->GetLevel() >= levelRequired && !pet->HasSpell(spellID))
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}
