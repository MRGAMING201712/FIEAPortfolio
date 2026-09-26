# FIEA Portfolio

What's actually in here

Combat

*	CharacterBase -shared base for player and enemies. Has the attribute set, action state, combo, abilities, equipment, and soft target components so both sides of a fight run the exact same code.
*	ActionStateComponent -one state machine (None, Attacking, Casting, Dodging, Staggered, Dead) that everything else checks against before letting you do something.
*	ComboComponent -light/heavy attacks driven by a weapon's attack data. Buffers your input for a short window so mashing attack during a swing won’t start it instantly.
*	ComboWindowNotifyState / WeaponTraceNotifyState -anim notifies that open the combo window and turn the weapon hitbox on/off.
*	SoftTargetComponent -picks who you're probably trying to hit based on which way you're facing, not a hard lock on. Scores actors on distance and angle, throws out anything dead, on your team, outside the cone, or blocked by a line trace.
*	WeaponActor / WeaponDefinition -the weapon itself, its attack chains, and Souls style stat scaling
*	DamageCalculationLibrary -turns a weapon + attacker stats + upgrade level into a damage number.
Stats
*	AttributeSet / AttributeSetDefinition -every stat is a gameplay tag pointing at a row in a data asset. Rows can be derived from other stats through a curve (Vigor sets max health, that kind of thing), and changing one stat correctly ripples through everything derived from it. Handles modifiers (with duration and removal), instant changes (damage/healing), regen, and tag based immunity.

Enemy AI

*	TokenSubsystem -a world subsystem that hands out a limited number of "attack tokens" so only a couple of enemies commit to an attack at once. Everyone else orbits. Tuning fight density is one number.
*	STT_RequestAndAttack / STT_OrbitTarget / STT_MoveToTarget -custom State Tree tasks (handwritten USTRUCTs, not Blueprint) for requesting a token and attacking, circling the player, or closing in.
*	STC_HasAttackToken / STC_DistanceToTarget -State Tree conditions those tasks check against.
*	EnemyAIController / EnemyCharacter -possesses the enemy pawn and kicks off its State Tree.

Abilities

*	AbilityComponent / AbilityDefinition -cast an ability from a slot, pay its cost, start the cooldown, play its montage.
*	AbilityEffect and its subclasses (ProjectileAbilityEffect, InstantStrikeAbilityEffect, SelfHealAbilityEffect, TickingAuraEffect, AuraAbilityEffect) -what actually happens when an ability lands, spawn a projectile, hit everything in a radius, heal, drop a damage over time zone, or buff/debuff an attribute.
*	AbilityCastNotify -anim notify that fires the effect at the right point in the cast animation.
*	ProjectileActor / AuraActor -the actual actors those effects spawn.

Progression & items

*	LevelingComponent -XP into levels into stat points, plus per attribute leveling for things like Vigor or Strength.
*	InventoryComponent / EquipmentComponent -hold weapons and armor, equip/unequip, weapon upgrading with a cost curve.
*	ArmorDefinition / WeaponDefinition -the data assets behind gear.

Waves & difficulty

*	WaveSpawnerComponent / WaveSpawnerConfig -spawns hand made waves in order, tracks who's still alive to know when a wave's actually done.
*	ProceduralWaveTable -once you run out of hand made waves, generates new ones: enemy count ramps up and caps out, enemy types are picked by weighted roll and gated behind a wave number unlock.
*	DifficultyManager / DifficultyProfile / DifficultyScalable -a difficulty profile builds a map of stat multipliers per wave; anything implementing the IDifficultyScalable interface (usually enemies) gets scaled without the spawner needing to know what it is.
*	WaveGamemode -hooks the spawner up to the player and routes XP/currency rewards on enemy kills.

Quests

*	QuestManagerSubsystem -tracks active quests, objective progress, and fires off events when something in the world matches an objective's gameplay tag.
*	QuestDefinition / QuestBehavior / TimedQuestBehavior -data defined quests with pluggable behavior classes for anything that needs custom logic (a timed quest that fails you if you're too slow, for example).

