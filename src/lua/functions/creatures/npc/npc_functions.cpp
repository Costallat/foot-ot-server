/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (C) 2021 OpenTibiaBR <opentibiabr@outlook.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "otpch.h"

#include <boost/range/adaptor/reversed.hpp>

#include "game/game.h"
#include "creatures/creature.h"
#include "creatures/npcs/npc.h"
#include "lua/functions/creatures/npc/npc_functions.hpp"

int NpcFunctions::luaNpcCreate(lua_State* L) {
	// Npc([id or name or userdata])
	Npc* npc;
	if (lua_gettop(L) >= 2) {
		if (isNumber(L, 2)) {
			npc = g_game.getNpcByID(getNumber<uint32_t>(L, 2));
		} else if (isString(L, 2)) {
			npc = g_game.getNpcByName(getString(L, 2));
		} else if (isUserdata(L, 2)) {
			if (getUserdataType(L, 2) != LuaData_Npc) {
				lua_pushnil(L);
				return 1;
			}
			npc = getUserdata<Npc>(L, 2);
		} else {
			npc = nullptr;
		}
	} else {
		npc = getScriptEnv()->getNpc();
	}

	if (npc) {
		pushUserdata<Npc>(L, npc);
		setMetatable(L, -1, "Npc");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int NpcFunctions::luaNpcIsNpc(lua_State* L) {
	// npc:isNpc()
	pushBoolean(L, getUserdata<const Npc>(L, 1) != nullptr);
	return 1;
}

int NpcFunctions::luaNpcSetMasterPos(lua_State* L) {
	// npc:setMasterPos(pos)
	Npc* npc = getUserdata<Npc>(L, 1);
	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_NPC_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	const Position& pos = getPosition(L, 2);
	npc->setMasterPos(pos);
	pushBoolean(L, true);
	return 1;
}

int NpcFunctions::luaNpcGetCurrency(lua_State* L) {
	// npc:getCurrency()
	Npc* npc = getUserdata<Npc>(L, 1);
	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_NPC_NOT_FOUND));
		lua_pushnil(L);
	}

	lua_pushnumber(L, npc->getCurrency());
	return 1;
}

int NpcFunctions::luaNpcGetSpeechBubble(lua_State* L) {
	// npc:getSpeechBubble()
	Npc* npc = getUserdata<Npc>(L, 1);
	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_NPC_NOT_FOUND));
		lua_pushnil(L);
	}

	lua_pushnumber(L, npc->getSpeechBubble());
	return 1;
}

int NpcFunctions::luaNpcSetSpeechBubble(lua_State* L) {
	// npc:setSpeechBubble(speechBubble)
	Npc* npc = getUserdata<Npc>(L, 1);
	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_NPC_NOT_FOUND));
		lua_pushnil(L);
	}

	npc->setSpeechBubble(getNumber<uint8_t>(L, 2));
	return 1;
}

int NpcFunctions::luaNpcGetName(lua_State* L) {
	// npc:getName()
	Npc* npc = getUserdata<Npc>(L, 1);
	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_NPC_NOT_FOUND));
		lua_pushnil(L);
		return 1;
	}

	pushString(L, npc->getName());
	return 1;
}

int NpcFunctions::luaNpcSetName(lua_State* L) {
	// npc:setName(name)
	Npc* npc = getUserdata<Npc>(L, 1);
	const std::string& name = getString(L, 2);
	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_NPC_NOT_FOUND));
		lua_pushnil(L);
	}

	npc->setName(name);
	return 1;
}

int NpcFunctions::luaNpcPlace(lua_State* L) {
	// npc:place(position[, extended = false[, force = true]])
	Npc* npc = getUserdata<Npc>(L, 1);
	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_NPC_NOT_FOUND));
		lua_pushnil(L);
		return 1;
	}

	const Position& position = getPosition(L, 2);
	bool extended = getBoolean(L, 3, false);
	bool force = getBoolean(L, 4, true);
	if (g_game.placeCreature(npc, position, extended, force)) {
		pushUserdata<Npc>(L, npc);
		setMetatable(L, -1, "Npc");
	} else {
		lua_pushnil(L);
	}
	return 1;
}

int NpcFunctions::luaNpcSay(lua_State* L) {
	// npc:say(text[, type = TALKTYPE_PRIVATE_NP[, ghost = false[, target = nullptr[, position]]]])
	int parameters = lua_gettop(L);

	Position position;
	if (parameters >= 6) {
		position = getPosition(L, 6);
		if (!position.x || !position.y) {
			reportErrorFunc("Invalid position specified.");
			pushBoolean(L, false);
			return 1;
		}
	}

	Creature* target = nullptr;
	if (parameters >= 5) {
		target = getCreature(L, 5);
	}

	bool ghost = getBoolean(L, 4, false);

	SpeakClasses type = getNumber<SpeakClasses>(L, 3, TALKTYPE_PRIVATE_NP);
	const std::string& text = getString(L, 2);
	Npc* npc = getUserdata<Npc>(L, 1);
	if (!npc) {
		lua_pushnil(L);
		return 1;
	}

	SpectatorHashSet spectators;
	if (target) {
		spectators.insert(target);
	}

	if (position.x != 0) {
		pushBoolean(L, g_game.internalCreatureSay(npc, type, text, ghost, &spectators, &position));
	} else {
		pushBoolean(L, g_game.internalCreatureSay(npc, type, text, ghost, &spectators));
	}
	return 1;
}

int NpcFunctions::luaNpcTurnToCreature(lua_State* L) {
	// npc:turnToCreature(creature)
	Npc* npc = getUserdata<Npc>(L, 1);
	Creature* creature = getCreature(L, 2);

	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_NPC_NOT_FOUND));
		lua_pushnil(L);
		return 1;
	}

	if (!creature) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnil(L);
		return 1;
	}

	npc->turnToCreature(creature);
	pushBoolean(L, true);
	return 1;
}

int NpcFunctions::luaNpcSetPlayerInteraction(lua_State* L) {
	// npc:setPlayerInteraction(creature, topic = 0)
	Npc* npc = getUserdata<Npc>(L, 1);
	Creature* creature = getCreature(L, 2);
	uint16_t topicId = getNumber<uint16_t>(L, 3, 0);

	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_NPC_NOT_FOUND));
		lua_pushnil(L);
		return 1;
	}

	if (!creature) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnil(L);
		return 1;
	}

	npc->setPlayerInteraction(creature->getID(), topicId);
	pushBoolean(L, true);
	return 1;
}

int NpcFunctions::luaNpcRemovePlayerInteraction(lua_State* L) {
	// npc:removePlayerInteraction()
	Npc* npc = getUserdata<Npc>(L, 1);
	Creature* creature = getCreature(L, 2);

	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_NPC_NOT_FOUND));
		lua_pushnil(L);
		return 1;
	}

	if (!creature) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnil(L);
		return 1;
	}

	npc->removePlayerInteraction(creature->getID());
	pushBoolean(L, true);
	return 1;
}

int NpcFunctions::luaNpcIsInteractingWithPlayer(lua_State* L) {
	// npc:isInteractingWithPlayer(creature)
	Npc* npc = getUserdata<Npc>(L, 1);
	Creature* creature = getCreature(L, 2);

	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_NPC_NOT_FOUND));
		lua_pushnil(L);
		return 1;
	}

	if (!creature) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnil(L);
		return 1;
	}

	pushBoolean(L, npc->isInteractingWithPlayer(creature->getID()));
	return 1;
}

int NpcFunctions::luaNpcIsPlayerInteractingOnTopic(lua_State* L) {
	//npc:isPlayerInteractingOnTopic(creature, topicId = 0)
	Npc* npc = getUserdata<Npc>(L, 1);
	Creature* creature = getCreature(L, 2);
	uint32_t topicId = getNumber<uint32_t>(L, 3, 0);

	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_NPC_NOT_FOUND));
		lua_pushnil(L);
		return 1;
	}

	if (!creature) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		lua_pushnil(L);
		return 1;
	}

	pushBoolean(L, npc->isPlayerInteractingOnTopic(creature->getID(), topicId));
	return 1;
}

int NpcFunctions::luaNpcIsInTalkRange(lua_State* L) {
	// npc:isInTalkRange()
	Npc* npc = getUserdata<Npc>(L, 1);
	const Position& position = getPosition(L, 2);

	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_NPC_NOT_FOUND));
		lua_pushnil(L);
		return 1;
	}

	pushBoolean(L, npc && npc->canSee(position));
	return 1;
}

int NpcFunctions::luaNpcOpenShopWindow(lua_State* L) {
	// npc:openShopWindow(player)
	Npc* npc = getUserdata<Npc>(L, 1);
	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_NPC_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	Player* player = getPlayer(L, 2);
	if (!player) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_PLAYER_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	player->openShopWindow(npc);

	pushBoolean(L, true);
	return 1;
}

int NpcFunctions::luaNpcCloseShopWindow(lua_State* L) {
	//npc:closeShopWindow(player)
	Player* player = getPlayer(L, 2);
	if (!player) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_PLAYER_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	Npc* npc = getUserdata<Npc>(L, 1);
	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	if (player->getShopOwner() == npc) {
		player->closeShopWindow(true);
		// TODO IMPLEMENT CALLBACK PROPERLY.
		//		if (shopCallback != -1) {
		//			luaL_unref(L, LUA_REGISTRYINDEX, shopCallback);
		//		}
	}

	pushBoolean(L, true);
	return 1;
}

int NpcFunctions::luaNpcGetShopItem(lua_State* L) {
	//npc:getShopItem(clientId)
	Npc* npc = getUserdata<Npc>(L, 1);
	if (!npc) {
		reportErrorFunc(getErrorDesc(LUA_ERROR_CREATURE_NOT_FOUND));
		pushBoolean(L, false);
		return 1;
	}

	ShopInfoMap shopItems = npc->getShopItems();
	const ItemType &itemType = Item::items.getItemIdByClientId(getNumber<uint16_t>(L, 2));

	if (shopItems.find(itemType.id) == shopItems.end()) {
		reportErrorFunc("No shop item found for clientId");
		pushBoolean(L, false);
		return 1;
	}

	ShopInfo shopInfo = shopItems[itemType.id];
	setField(L, "clientId", shopInfo.itemClientId);
	setField(L, "name", shopInfo.name);
	setField(L, "subType", shopInfo.subType);
	setField(L, "buyPrice", shopInfo.buyPrice);
	setField(L, "sellPrice", shopInfo.sellPrice);

	pushBoolean(L, true);
	return 1;
}
