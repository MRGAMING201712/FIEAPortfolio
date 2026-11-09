# FIEA Portfolio

What's actually in here

Abilities

*	AbilityComponent / AbilityDefinition -cast an ability from a slot, pay its cost, start the cooldown, play its montage.
*	AbilityEffect and its subclasses (ProjectileAbilityEffect, InstantStrikeAbilityEffect, SelfHealAbilityEffect, TickingAuraEffect, AuraAbilityEffect) -what happens when an ability lands, spawn a projectile, hit everything in a radius, heal, drop a damage over time zone, or buff/debuff an attribute.
*	AbilityCastNotify -anim notify that fires the effect at the right point in the cast animation.
*	ProjectileActor / AuraActor -the actual actors those effects spawn.

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

Painting
* This is the main game mechanic for my godot game expo game where you are in a 2d level using tile mapping and have to paint the city back to life while defeating the enemies along the way. In order to have this work I built it in two parts: first the shader which does the coloring of the tilemap, from gray to color or color to gray, and second the painting script which reads the tilemap and has all the logic for it.
* World Reveal Shader - Using my knowledge from Computer Graphics I made a shader that allows me to change the tint/intensity of the paint to allow it to not overwrite the pixels and allow you to still see the detail of the tilemap.
* PaintManager Script - This was originally made to work with images, but was slightly modified to work with tilemaps to allow dynamic changes to the levels. This has the logic for defining the zone, setting it gray, what painting does to the tilemap, and some gameplay helpers built for other scripts to utilize.

Progression & items

*	LevelingComponent -XP into levels into stat points, plus per attribute leveling for things like Vigor or Strength.
*	InventoryComponent / EquipmentComponent -hold weapons and armor, equip/unequip, weapon upgrading with a cost curve.
*	ArmorDefinition / WeaponDefinition -the data assets behind gear.

Quests

*	QuestManagerSubsystem -tracks active quests, objective progress, and fires off events when something in the world matches an objective's gameplay tag.
*	QuestDefinition / QuestBehavior / TimedQuestBehavior -data defined quests with behavior classes for anything that needs custom logic (a timed quest that fails you if you're too slow, for example).

Waves & difficulty

*	WaveSpawnerComponent / WaveSpawnerConfig -spawns hand made waves in order, tracks who's still alive to know when a wave's actually done.
*	ProceduralWaveTable -once you run out of hand made waves, generates new ones: enemy count ramps up and caps out, enemy types are picked by weighted roll and gated behind a wave number unlock.
*	DifficultyManager / DifficultyProfile / DifficultyScalable -a difficulty profile builds a map of stat multipliers per wave; anything implementing the IDifficultyScalable interface (usually enemies) gets scaled without the spawner needing to know what it is.
*	WaveGamemode -hooks the spawner up to the player and routes XP/currency rewards on enemy kills.
