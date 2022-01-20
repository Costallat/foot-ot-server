exerciseWeaponsTable = {
	-- MELE
	[28540] = { skill = SKILL_SWORD },
	[28552] = { skill = SKILL_SWORD },
	[35279] = { skill = SKILL_SWORD },
	[35285] = { skill = SKILL_SWORD },
	[28553] = { skill = SKILL_AXE },
	[28541] = { skill = SKILL_AXE },
	[35280] = { skill = SKILL_AXE },
	[35286] = { skill = SKILL_AXE },
	[28554] = { skill = SKILL_CLUB },
	[28542] = { skill = SKILL_CLUB },
	[35281] = { skill = SKILL_CLUB },
	[35287] = { skill = SKILL_CLUB },
	-- ROD
	[28544] = { skill = SKILL_MAGLEVEL, effect = CONST_ANI_SMALLICE, allowFarUse = true },
	[28556] = { skill = SKILL_MAGLEVEL, effect = CONST_ANI_SMALLICE, allowFarUse = true },
	[35283] = { skill = SKILL_MAGLEVEL, effect = CONST_ANI_SMALLICE, allowFarUse = true },
	[35289] = { skill = SKILL_MAGLEVEL, effect = CONST_ANI_SMALLICE, allowFarUse = true },
	-- RANGE
	[28543] = { skill = SKILL_DISTANCE, effect = CONST_ANI_SIMPLEARROW, allowFarUse = true },
	[28555] = { skill = SKILL_DISTANCE, effect = CONST_ANI_SIMPLEARROW, allowFarUse = true },
	[35282] = { skill = SKILL_DISTANCE, effect = CONST_ANI_SIMPLEARROW, allowFarUse = true },
	[35288] = { skill = SKILL_DISTANCE, effect = CONST_ANI_SIMPLEARROW, allowFarUse = true },
	-- WAND
	[28545] = { skill = SKILL_MAGLEVEL, effect = CONST_ANI_FIRE, allowFarUse = true },
	[28557] = { skill = SKILL_MAGLEVEL, effect = CONST_ANI_FIRE, allowFarUse = true },
	[35284] = { skill = SKILL_MAGLEVEL, effect = CONST_ANI_FIRE, allowFarUse = true },
	[35290] = { skill = SKILL_MAGLEVEL, effect = CONST_ANI_FIRE, allowFarUse = true }
}

freeDummies = {28558, 28565}
maxAllowedOnADummy = configManager.getNumber(configKeys.MAX_ALLOWED_ON_A_DUMMY)
houseDummies = {28559, 28560, 28561, 28562, 28563, 28564}

local magicLevelRate = configManager.getNumber(configKeys.RATE_MAGIC)
local skillLevelRate = configManager.getNumber(configKeys.RATE_SKILL)

function leaveTraining(playerId)
	if onExerciseTraining[playerId] then
		stopEvent(onExerciseTraining[playerId].event)
		onExerciseTraining[playerId] = nil
	end

	local player = Player(playerId)
	if player then
		player:setTraining(false)
	end
	return
end

function exerciseEvent(playerId, tilePosition, weaponId, dummyId)
	local player = Player(playerId)
	if not player then
		return leaveTraining(playerId)
	end

	if player:isTraining() == 0 then
		return leaveTraining(playerId)
	end

	if not Tile(tilePosition):getItemById(dummyId) then
		player:sendTextMessage(MESSAGE_FAILURE, "Someone has moved the dummy, the training has stopped.")
		leaveTraining(playerId)
		return
	end

	local playerPosition = player:getPosition()
	if not getTilePzInfo(playerPosition) then
		player:sendTextMessage(MESSAGE_FAILURE, "You are no longer in a protection zone, the training has stopped.")
		leaveTraining(playerId)
		return
	end

	if player:getItemCount(weaponId) <= 0 then
		player:sendTextMessage(MESSAGE_FAILURE, "You need the training weapon in the backpack, the training has stopped.")
		leaveTraining(playerId)
		return
	end

	local weapon = player:getItemById(weaponId, true)
	if not weapon:isItem() or not weapon:hasAttribute(ITEM_ATTRIBUTE_CHARGES) then
		player:sendTextMessage(MESSAGE_FAILURE, "The selected item is not a training weapon, the training has stopped.")
		leaveTraining(playerId)
		return
	end

	local weaponCharges = weapon:getAttribute(ITEM_ATTRIBUTE_CHARGES)
	if not weaponCharges or weaponCharges <= 0 then
		weapon:remove(1) -- ??
		player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "Your training weapon has disappeared.")
		leaveTraining(playerId)
		return
	end

	local isMagic = false
	local bonusDummy = isInArray(houseDummies, weaponId) or nil
	local skillToTraining = skillsStages or nil
	local skillRateDefault = skillLevelRate
	local skillLevel = player:getSkillLevel(exerciseWeaponsTable[weaponId].skill)

	if exerciseWeaponsTable[weaponId].skill == SKILL_MAGLEVEL then
		skillToTraining = magicLevelStages or nil
		skillRateDefault = magicLevelRate
		skillLevel = player:getBaseMagicLevel()
		isMagic = true
	end

	local expRate = getRateFromTable(skillToTraining, skillLevel, skillRateDefault)
	if bonusDummy then bonusDummy = 1.1 else bonusDummy = 1 end

	if isMagic then
		player:addManaSpent(math.ceil(500 * expRate) * bonusDummy)
	else
		player:addSkillTries(exerciseWeaponsTable[weaponId].skill, (7 * expRate) * bonusDummy)
	end

	weapon:setAttribute(ITEM_ATTRIBUTE_CHARGES, (weaponCharges - 1))
	tilePosition:sendMagicEffect(CONST_ME_HITAREA)

	if exerciseWeaponsTable[weaponId].effect then
		playerPosition:sendDistanceEffect(tilePosition, exerciseWeaponsTable[weaponId].effect)
	end

	if weapon:getAttribute(ITEM_ATTRIBUTE_CHARGES) <= 0 then
		weapon:remove(1)
		player:sendTextMessage(MESSAGE_EVENT_ADVANCE, "Your training weapon has disappeared.")
		leaveTraining(playerId)
		return
	end

	local vocation = player:getVocation()
	onExerciseTraining[playerId].event = addEvent(exerciseEvent, vocation:getBaseAttackSpeed() / configManager.getFloat(configKeys.RATE_EXERCISE_TRAINING_SPEED), playerId, tilePosition, weaponId, dummyId)
	return
end
