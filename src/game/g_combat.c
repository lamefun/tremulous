/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2000-2013 Darklegion Development

This file is part of Tremulous.

Tremulous is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Tremulous is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Tremulous; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "g_local.h"

#define MIX(x, y, factor) ((x) * (1 - (factor)) + (y) * (factor))

static vec3_t zeroVector = { 0.0f, 0.0f, 0.0f };

// Names of damage methods.
char *modNames[ ] =
{
  "MOD_UNKNOWN",
  "MOD_SHOTGUN",
  "MOD_BLASTER",
  "MOD_PAINSAW",
  "MOD_MACHINEGUN",
  "MOD_CHAINGUN",
  "MOD_PRIFLE",
  "MOD_MDRIVER",
  "MOD_LASGUN",
  "MOD_LCANNON",
  "MOD_LCANNON_SPLASH",
  "MOD_FLAMER",
  "MOD_FLAMER_SPLASH",
  "MOD_GRENADE",
  "MOD_WATER",
  "MOD_SLIME",
  "MOD_LAVA",
  "MOD_CRUSH",
  "MOD_TELEFRAG",
  "MOD_FALLING",
  "MOD_SUICIDE",
  "MOD_TARGET_LASER",
  "MOD_TRIGGER_HURT",

  "MOD_ABUILDER_CLAW",
  "MOD_LEVEL0_BITE",
  "MOD_LEVEL1_CLAW",
  "MOD_LEVEL1_PCLOUD",
  "MOD_LEVEL3_CLAW",
  "MOD_LEVEL3_POUNCE",
  "MOD_LEVEL3_BOUNCEBALL",
  "MOD_LEVEL2_CLAW",
  "MOD_LEVEL2_ZAP",
  "MOD_LEVEL4_CLAW",
  "MOD_LEVEL4_TRAMPLE",
  "MOD_LEVEL4_CRUSH",

  "MOD_SLOWBLOB",
  "MOD_POISON",
  "MOD_SWARM",

  "MOD_HSPAWN",
  "MOD_TESLAGEN",
  "MOD_MGTURRET",
  "MOD_REACTOR",

  "MOD_ASPAWN",
  "MOD_ATUBE",
  "MOD_OVERMIND",
  "MOD_DECONSTRUCT",
  "MOD_REPLACE",
  "MOD_NOCREEP"
};


// Same as damageInfo_t, but with some extra convenience fields which are
// computed from the original damageInfo_t.
typedef struct {

  // Fields which are shared with damageInfo_t.
  // ==========================================

  // Target of the attack.
  gentity_t *target;
  // Entity which caused the damage (eg. the player in case of instant attacks
  // or the projectile in case of projectile attacks). Note that it is never
  // NULL: if G_Damage receives no inflictor entity, it is substituted with the
  // world entity.
  gentity_t *inflictor;
  // Entity which initiated the attack (eg. the player). Note that it is never
  // NULL: if G_Damage receives no attacker entity, it is substituted with the
  // world entity.
  gentity_t *attacker;
  // Source of the attack to use in locational damage calculations. This is
  // equal to the current position of the player in case of instant attacks and
  // the position where the projectile was fired from in case of projectile
  // attacks. Set to zero vector if haveSourceOrigin is false.
  qboolean haveSourceOrigin;
  vec3_t sourceOrigin;
  // Direction into which to push the target. Set to zero vector if
  // haveKnockbackDir is false.
  qboolean haveKnockbackDir;
  vec3_t knockbackDir;
  // Where the damage was inflicted. Set to zero vector if havePoint is false.
  qboolean havePoint;
  vec3_t point;
  // How much damage to deal.
  int damage;
  // DAMAGE_RADIUS - Damage was indirect (from a nearby explosion).
  // DAMAGE_NO_ARMOR - Armor does not protect from this damage.
  // DAMAGE_NO_KNOCKBACK - Do not affect velocity, just view angles.
  // DAMAGE_NO_PROTECTION - Kills everything except godmode.
  int dflags;
  // Damage method.
  int mod;

  // Fields which are computed for convenience from damageInfo_t.
  // ============================================================

  // Whether inflictor was originally NULL.
  qboolean haveInflictor;
  // Whether attacker was originally NULL.
  qboolean haveAttacker;
  // Whether attacker is the same as the target.
  qboolean damageToSelf;
  // Whether the target is a buildable.
  qboolean targetIsBuildable;
  // Whether the attack if friendly fire.
  qboolean friendlyFire;
  // Position of the target to use in locational damage calculations.
  vec3_t targetOrigin;
  // Target and attacker clients (may be NULL).
  gclient_t *targetClient;
  gclient_t *attackerClient;
  // Target and attacker teams (may be -1).
  int targetTeam;
  int attackerTeam;
  // Target and attacker player classes (may be -1).
  int targetClass;
  int attackerClass;
  // Target and attacker player states (NULL if not applicable).
  playerState_t *targetPS;
  playerState_t *attackerPS;

} damageInfoPlus_t;


// Attack parameters, used for locational damage calculation.
typedef struct {
  // How close did the line of attack come to the central axis of the target,
  // from 1.0f (dead on) to 0.0f (grazed the edge of the cylinder).
  float precision;
  // Where did the attack come from vertically, from 1.0f (directly from the
  // above) to 0.0f (directly from below).
  float elevation;
  // Where did the attack come from horizontally, from 1.0f (directly from the
  // front) to 0.0f (directly from the back).
  float flanking;
} attackParms_t;


/*
===========
G_GetObjectOrigin
===========
*/
static void G_GetObjectOrigin( gentity_t *ent, vec3_t out )
{
  if( g_unlagged.integer && ent->client && ent->client->unlaggedCalc.used )
    VectorCopy( ent->client->unlaggedCalc.origin, out );
  else
    VectorCopy( ent->r.currentOrigin, out );
}


/*
==========
G_ComputeDamageInfoPlus
==========
*/
static void G_ComputeDamageInfoPlus( damageInfo_t *dmg, damageInfoPlus_t *out )
{
  out->target = dmg->target;
  out->inflictor = dmg->inflictor;
  out->attacker = dmg->attacker;
  out->haveSourceOrigin = qfalse;
  VectorCopy( zeroVector, out->sourceOrigin );
  out->haveKnockbackDir = qfalse;
  VectorCopy( zeroVector, out->knockbackDir );
  out->havePoint = qfalse;
  VectorCopy( zeroVector, out->point );
  out->damage = dmg->damage;
  out->dflags = dmg->dflags;
  out->mod = dmg->mod;
  out->haveAttacker = qtrue;
  out->haveInflictor = qtrue;
  out->damageToSelf = dmg->target == dmg->attacker;
  out->targetIsBuildable = qfalse;
  out->friendlyFire = qfalse;
  out->targetClient = NULL;
  out->attackerClient = NULL;
  out->targetTeam = -1;
  out->attackerTeam = -1;
  out->targetClass = -1;
  out->attackerClass = -1;
  out->targetPS = NULL;
  out->attackerPS = NULL;

  if( !dmg->inflictor )
  {
    out->inflictor = &g_entities[ ENTITYNUM_WORLD ];
    out->haveInflictor = qfalse;
  }

  if( !dmg->attacker )
  {
    out->attacker = &g_entities[ ENTITYNUM_WORLD ];
    out->haveAttacker = qfalse;
  }

  if( dmg->haveSourceOrigin )
  {
    VectorCopy( dmg->sourceOrigin, out->sourceOrigin );
    out->haveSourceOrigin = qtrue;
  }
  else if( out->haveAttacker )
  {
    G_GetObjectOrigin( dmg->attacker, out->sourceOrigin );
    out->haveSourceOrigin = qtrue;
  }
  else if( out->haveInflictor )
  {
    G_GetObjectOrigin( dmg->inflictor, out->sourceOrigin );
    out->haveSourceOrigin = qtrue;
  }

  if( dmg->haveKnockbackDir )
  {
    VectorNormalize2( dmg->knockbackDir, out->knockbackDir );
    out->haveKnockbackDir = qtrue;
  }
  else
  {
    dmg->dflags |= DAMAGE_NO_KNOCKBACK;
  }

  if( dmg->havePoint )
  {
    VectorCopy( dmg->point, out->point );
    out->havePoint = qtrue;
  }

  G_GetObjectOrigin( dmg->target, out->targetOrigin );

  if( dmg->target->client )
  {
    out->targetClient = dmg->target->client;
    out->targetPS = &dmg->target->client->ps;
    out->targetTeam = out->targetPS->stats[ STAT_TEAM ];
    out->targetClass = out->targetPS->stats[ STAT_CLASS ];
  }
  else if( dmg->target->s.eType == ET_BUILDABLE )
  {
    out->targetIsBuildable = qtrue;
    out->targetTeam = dmg->target->buildableTeam;
  }

  if( dmg->attacker && dmg->attacker->client )
  {
    out->attackerClient = dmg->attacker->client;
    out->attackerPS = &dmg->attacker->client->ps;
    out->attackerTeam = out->attackerPS->stats[ STAT_TEAM ];
    out->attackerClass = out->attackerPS->stats[ STAT_CLASS ];
  }

  if( out->attackerClient && out->targetClient && !out->damageToSelf )
    out->friendlyFire = OnSameTeam( dmg->target, dmg->attacker );
  else if( out->attackerTeam != -1 )
    out->friendlyFire = out->targetTeam == out->attackerTeam;
}


/*
============
AddScore

Adds score to the client
============
*/
void AddScore( gentity_t *ent, int score )
{
  if( !ent->client )
    return;

  // make alien and human scores equivalent 
  if ( ent->client->pers.teamSelection == TEAM_ALIENS )
  {
    score = rint( ((float)score) / 2.0f );
  }

  // scale values down to fit the scoreboard better
  score = rint( ((float)score) / 50.0f );

  ent->client->ps.persistant[ PERS_SCORE ] += score;
  CalculateRanks( );
}


/*
==================
LookAtKiller
==================
*/
void LookAtKiller( gentity_t *self, gentity_t *inflictor, gentity_t *attacker )
{

  if ( attacker && attacker != self )
    self->client->ps.stats[ STAT_VIEWLOCK ] = attacker - g_entities;
  else if( inflictor && inflictor != self )
    self->client->ps.stats[ STAT_VIEWLOCK ] = inflictor - g_entities;
  else
    self->client->ps.stats[ STAT_VIEWLOCK ] = self - g_entities;
}


/*
==================
G_RewardAttackers

Function to distribute rewards to entities that killed this one.
Returns the total damage dealt.
==================
*/
float G_RewardAttackers( gentity_t *self )
{
  float     value, totalDamage = 0;
  int       team, i, maxHealth = 0;
  int       alienCredits = 0, humanCredits = 0;
  gentity_t *player;

  // Total up all the damage done by non-teammates
  for( i = 0; i < level.maxclients; i++ )
  {
    player = g_entities + i;

    if( !OnSameTeam( self, player ) || 
        self->buildableTeam != player->client->ps.stats[ STAT_TEAM ] )
      totalDamage += (float)self->credits[ i ];
  }

  if( totalDamage <= 0.0f )
    return 0.0f;

  // Only give credits for killing players and buildables
  if( self->client )
  {
    value = BG_GetValueOfPlayer( &self->client->ps );
    team = self->client->pers.teamSelection;
    maxHealth = self->client->ps.stats[ STAT_MAX_HEALTH ];
  }
  else if( self->s.eType == ET_BUILDABLE )
  {
    value = BG_Buildable( self->s.modelindex )->value;

    // only give partial credits for a buildable not yet completed
    if( !self->spawned )
    {
      value *= (float)( level.time - self->buildTime ) /
        BG_Buildable( self->s.modelindex )->buildTime;
    }

    team = self->buildableTeam;
    maxHealth = BG_Buildable( self->s.modelindex )->health;
  }
  else
    return totalDamage;

  // Give credits and empty the array
  for( i = 0; i < level.maxclients; i++ )
  {
    int stageValue = value * self->credits[ i ] / totalDamage;
    player = g_entities + i;

    if( player->client->pers.teamSelection != team )
    {
      if( totalDamage < maxHealth )
        stageValue *= totalDamage / maxHealth;

      if( !self->credits[ i ] || player->client->ps.stats[ STAT_TEAM ] == team )
        continue;

      AddScore( player, stageValue );

      // killing buildables earns score, but not credits
      if( self->s.eType != ET_BUILDABLE )
      {
        G_AddCreditToClient( player->client, stageValue, qtrue );

        // add to stage counters
        if( player->client->ps.stats[ STAT_TEAM ] == TEAM_ALIENS )
          alienCredits += stageValue;
        else if( player->client->ps.stats[ STAT_TEAM ] == TEAM_HUMANS )
          humanCredits += stageValue;
      }
    }
    self->credits[ i ] = 0;
  }

  if( alienCredits )
  {
    trap_Cvar_Set( "g_alienCredits",
      va( "%d", g_alienCredits.integer + alienCredits ) );
    trap_Cvar_Update( &g_alienCredits );
  }
  if( humanCredits )
  {
    trap_Cvar_Set( "g_humanCredits",
      va( "%d", g_humanCredits.integer + humanCredits ) );
    trap_Cvar_Update( &g_humanCredits );
  }
  
  return totalDamage;
}

/*
==================
player_die
==================
*/
void player_die( gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int meansOfDeath )
{
  gentity_t *ent;
  int       anim;
  int       killer;
  int       i;
  const char      *killerName, *obit;

  if( self->client->ps.pm_type == PM_DEAD )
    return;

  if( level.intermissiontime )
    return;

  self->client->ps.pm_type = PM_DEAD;
  self->suicideTime = 0;

  if( attacker )
  {
    killer = attacker->s.number;

    if( attacker->client )
      killerName = attacker->client->pers.netname;
    else
      killerName = "<world>";
  }
  else
  {
    killer = ENTITYNUM_WORLD;
    killerName = "<world>";
  }

  if( meansOfDeath < 0 || meansOfDeath >= ARRAY_LEN( modNames ) )
    // fall back on the number
    obit = va( "%d", meansOfDeath );
  else
    obit = modNames[ meansOfDeath ];

  G_LogPrintf( "Die: %d %d %s: %s" S_COLOR_WHITE " killed %s\n",
    killer,
    (int)( self - g_entities ),
    obit,
    killerName,
    self->client->pers.netname );

  // deactivate all upgrades
  for( i = UP_NONE + 1; i < UP_NUM_UPGRADES; i++ )
    BG_DeactivateUpgrade( i, self->client->ps.stats );

  // broadcast the death event to everyone
  ent = G_TempEntity( self->r.currentOrigin, EV_OBITUARY );
  ent->s.eventParm = meansOfDeath;
  ent->s.otherEntityNum = self->s.number;
  ent->s.otherEntityNum2 = killer;
  ent->r.svFlags = SVF_BROADCAST; // send to everyone

  self->enemy = attacker;
  self->client->ps.persistant[ PERS_KILLED ]++;

  if( attacker && attacker->client )
  {
    attacker->client->lastkilled_client = self->s.number;

    if( ( attacker == self || OnSameTeam( self, attacker ) ) && meansOfDeath != MOD_HSPAWN )
    {
      //punish team kills and suicides
      if( attacker->client->ps.stats[ STAT_TEAM ] == TEAM_ALIENS )
      {
        G_AddCreditToClient( attacker->client, -ALIEN_TK_SUICIDE_PENALTY, qtrue );
        AddScore( attacker, -ALIEN_TK_SUICIDE_PENALTY );
      }
      else if( attacker->client->ps.stats[ STAT_TEAM ] == TEAM_HUMANS )
      {
        G_AddCreditToClient( attacker->client, -HUMAN_TK_SUICIDE_PENALTY, qtrue );
        AddScore( attacker, -HUMAN_TK_SUICIDE_PENALTY );
      }
    }
  }
  else if( attacker->s.eType != ET_BUILDABLE )
  {
    if( self->client->ps.stats[ STAT_TEAM ] == TEAM_ALIENS )
      AddScore( self, -ALIEN_TK_SUICIDE_PENALTY );
    else if( self->client->ps.stats[ STAT_TEAM ] == TEAM_HUMANS )
      AddScore( self, -HUMAN_TK_SUICIDE_PENALTY );
  }

  // give credits for killing this player
  G_RewardAttackers( self );

  ScoreboardMessage( self );    // show scores

  // send updated scores to any clients that are following this one,
  // or they would get stale scoreboards
  for( i = 0 ; i < level.maxclients ; i++ )
  {
    gclient_t *client;

    client = &level.clients[ i ];
    if( client->pers.connected != CON_CONNECTED )
      continue;

    if( client->sess.spectatorState == SPECTATOR_NOT )
      continue;

    if( client->sess.spectatorClient == self->s.number )
      ScoreboardMessage( g_entities + i );
  }

  VectorCopy( self->r.currentOrigin, self->client->pers.lastDeathLocation );

  self->takedamage = qfalse; // can still be gibbed

  self->s.weapon = WP_NONE;
  if( self->client->noclip )
    self->client->cliprcontents = CONTENTS_CORPSE;
  else
    self->r.contents = CONTENTS_CORPSE;

  self->client->ps.viewangles[ PITCH ] = 0; // zomg
  self->client->ps.viewangles[ YAW ] = self->s.apos.trBase[ YAW ];
  self->client->ps.viewangles[ ROLL ] = 0;
  LookAtKiller( self, inflictor, attacker );

  self->s.loopSound = 0;

  self->r.maxs[ 2 ] = -8;

  // don't allow respawn until the death anim is done
  // g_forcerespawn may force spawning at some later time
  self->client->respawnTime = level.time + 1700;

  // clear misc
  memset( self->client->ps.misc, 0, sizeof( self->client->ps.misc ) );

  {
    // normal death
    static int i;

    if( !( self->client->ps.persistant[ PERS_STATE ] & PS_NONSEGMODEL ) )
    {
      switch( i )
      {
        case 0:
          anim = BOTH_DEATH1;
          break;
        case 1:
          anim = BOTH_DEATH2;
          break;
        case 2:
        default:
          anim = BOTH_DEATH3;
          break;
      }
    }
    else
    {
      switch( i )
      {
        case 0:
          anim = NSPA_DEATH1;
          break;
        case 1:
          anim = NSPA_DEATH2;
          break;
        case 2:
        default:
          anim = NSPA_DEATH3;
          break;
      }
    }

    self->client->ps.legsAnim =
      ( ( self->client->ps.legsAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;

    if( !( self->client->ps.persistant[ PERS_STATE ] & PS_NONSEGMODEL ) )
    {
      self->client->ps.torsoAnim =
        ( ( self->client->ps.torsoAnim & ANIM_TOGGLEBIT ) ^ ANIM_TOGGLEBIT ) | anim;
    }

    // use own entityid if killed by non-client to prevent uint8_t overflow
    G_AddEvent( self, EV_DEATH1 + i,
      ( killer < MAX_CLIENTS ) ? killer : self - g_entities );

    // globally cycle through the different death animations
    i = ( i + 1 ) % 3;
  }

  trap_LinkEntity( self );

  self->client->pers.infoChangeTime = level.time;
}


/*
============
G_GetAttackParms
============
*/
static qboolean G_GetAttackParms( damageInfoPlus_t *dmg, attackParms_t *parms )
{
  // Alias for dmg->target.
  gentity_t *targ = dmg->target;
  // The target's UP direction.
  vec3_t targetUpDir;
  // Target forward direction.
  vec3_t targetForwardDir;
  // Direction from the attacker to the target.
  vec3_t sourceToTargetDir;
  // Direction of the attack.
  vec3_t attackDir;
  // Distance between the attack line and the target axis.
  float attackMiss;
  // How large can attackMiss get before precision becomes zero.
  float radius;
  // Used to compute attackMiss.
  vec3_t axisA, axisB, attackB;
  float attackLen, s, t;
  // Precision value before any modifications.
  float baseFlanking;
  // How much baseFlanking ifluences the final flanking value.
  float flankingModifier;

  if( !dmg->targetClient )
    return qfalse;

  if( !dmg->haveSourceOrigin || !dmg->havePoint )
    return qfalse;

  radius = 1.2f * MAX( 1.0f, MIN(
    -MIN( dmg->target->r.mins[ 0 ], dmg->target->r.mins[ 1 ] ),
     MAX( dmg->target->r.maxs[ 0 ], dmg->target->r.maxs[ 1 ] )
  ) );

  BG_GetClientNormal( dmg->targetPS, targetUpDir );

  AngleVectors( dmg->targetPS->viewangles, targetForwardDir, NULL, NULL );

  VectorSubtract( dmg->point, dmg->sourceOrigin, attackDir );
  VectorNormalize( attackDir );

  VectorSubtract( dmg->targetOrigin, dmg->sourceOrigin, sourceToTargetDir );
  VectorNormalize( sourceToTargetDir );

  attackLen = Distance( dmg->sourceOrigin, dmg->point );
  VectorMA( dmg->targetOrigin, dmg->target->r.mins[ 2 ], targetUpDir, axisA );
  VectorMA( dmg->targetOrigin, dmg->target->r.maxs[ 2 ], targetUpDir, axisB );
  VectorMA( dmg->sourceOrigin, attackLen + 10000.0f, attackDir, attackB );

  attackMiss = DistanceBetweenLineSegments(
    dmg->sourceOrigin, attackB, axisA, axisB, &s, &t );

  // Calculate the precision.
  parms->precision = 1.0f - MIN( attackMiss / radius, 1.0f );

  // Calculate the elevation factor.
  parms->elevation = -DotProduct( targetUpDir, sourceToTargetDir );

  // Calcualate the flanking factor. If the attack comes from above or from
  // below, reduce the influence of flanking to avoid situations where a few
  // pixels can mean the difference between a large bonus and a large penalty.
  baseFlanking = DotProduct( targetForwardDir, sourceToTargetDir );
  flankingModifier = 1.0f - fabs( parms->elevation );
  flankingModifier *= flankingModifier;
  parms->flanking = baseFlanking * flankingModifier;

  return qtrue;
}


/*
============
G_GetHumanArmorModifier
============
*/
static float G_GetHumanArmorModifier( gentity_t *ent )
{
  float modifier = 1.0f;

  if( BG_InventoryContainsUpgrade( UP_HELMET, ent->client->ps.stats ) )
    modifier *= HUMAN_HELMET_MODIFIER;

  if( BG_InventoryContainsUpgrade( UP_LIGHTARMOUR, ent->client->ps.stats ) )
    modifier *= HUMAN_LIGHTARMOUR_MODIFIER;

  if( BG_InventoryContainsUpgrade( UP_BATTLESUIT, ent->client->ps.stats ) )
    modifier *= HUMAN_BATTLESUIT_MODIFIER;

  return modifier;
}


/*
============
G_CalcHumanDamageModifier
============
*/
static float G_CalcHumanDamageModifier( damageInfoPlus_t *dmg,
                                        attackParms_t *parms )
{
  float precisionModifier;
  float flankingModifier;
  float elevationBonus = 0.0f;
  float armorModifier;
  float baseModifier;

  // Get the armor modifier.
  armorModifier = G_GetHumanArmorModifier( dmg->target );

  // Modify the damage based on how precise the attack is.
  precisionModifier = MIX(
    HUMAN_PRECISION_MIN_MODIFIER, HUMAN_PRECISION_MAX_MODIFIER,
    HUMAN_PRECISION_FUNCTION( parms->precision )
  );

  // Add bonus damage from flanking.
  flankingModifier = 1.0f + MAX( 0.0f, parms->flanking ) *
                            HUMAN_FLANKING_MODIFIER;

  // Dretches and Marauders get bonus damage when they attack from above.
  if( dmg->mod == MOD_LEVEL0_BITE || dmg->mod == MOD_LEVEL2_CLAW )
    elevationBonus += sqrt( MAX( 0.0f, parms->elevation ) ) * 
                      DRETCH_MARAUDER_ELEVATION_BONUS;

  // Helmet reduces the elevation bonus.
  if( BG_InventoryContainsUpgrade( UP_HELMET, dmg->targetPS->stats ) )
    elevationBonus *= HUMAN_HELMET_ELEVATION_MODIFIER;

  // Calculate the base damage modifier.
  baseModifier = armorModifier * precisionModifier * flankingModifier;

  // Calculate the final damage modifier.
  return baseModifier + elevationBonus * armorModifier;
}


/*
============
G_CalcAlienDamageModifier
============
*/
static float G_CalcAlienDamageModifier( damageInfoPlus_t *dmg,
                                        attackParms_t *parms )
{
  float precisionModifier;
  float flankingModifier;
  float elevationModifier;

  // Modify the damage based on how precise the attack is.
  precisionModifier = MIX(
    ALIEN_PRECISION_MIN_MODIFIER, ALIEN_PRECISION_MAX_MODIFIER,
    ALIEN_PRECISION_FUNCTION( parms->precision ) 
  );

  // Modify the damage based on flanking.
  flankingModifier = 1.0f + MIN( 0.0f, parms->flanking ) *
                            ALIEN_FLANKING_MODIFIER;

  // Modify the damage based on the elevation.
  elevationModifier = 1.0f + MAX( 0.0f, parms->elevation ) *
                             ALIEN_ELEVATION_MODIFIER;

  return precisionModifier * flankingModifier * elevationModifier;
}


/*
============
GetNonLocDamageModifier
============
*/
static float GetNonLocDamageModifier( damageInfoPlus_t *dmg )
{
  if( !dmg->targetClient )
    return 1.0f;

  if( dmg->targetTeam == TEAM_HUMANS )
    return G_GetHumanArmorModifier( dmg->target );

  return 1.0f;
}


/*
============
G_IsMovementAttack
============
*/
static qboolean G_IsMovementAttack( damageInfoPlus_t *dmg )
{
  return dmg->mod == MOD_LEVEL4_TRAMPLE || 
         dmg->mod == MOD_LEVEL3_POUNCE ||
         dmg->mod == MOD_LEVEL4_CRUSH;
}


/*
============
G_AttackCanAddPoison
============
*/
static qboolean G_AttackCanAddPoison( damageInfoPlus_t *dmg )
{
  return dmg->mod != MOD_LEVEL2_ZAP &&
         dmg->mod != MOD_POISON &&
         dmg->mod != MOD_LEVEL1_PCLOUD &&
         dmg->mod != MOD_HSPAWN &&
         dmg->mod != MOD_ASPAWN;
}


/*
============
G_MoverAtStartingPos
============
*/
static qboolean G_MoverAtStartingPos( gentity_t *targ )
{
  return targ->moverState == MOVER_POS1 ||
         targ->moverState == ROTATOR_POS1;
}


/*
============
G_IsAttackAgainstBuildable
============
*/
static qboolean G_IsAttackAgainstBuildable( damageInfoPlus_t *dmg )
{
  return dmg->targetIsBuildable && 
         dmg->attackerClient &&
         dmg->mod != MOD_DECONSTRUCT &&
         dmg->mod != MOD_SUICIDE &&
         dmg->mod != MOD_REPLACE &&
         dmg->mod != MOD_NOCREEP;
}


/*
============
G_BuildableProtectedFromFirendlyFire
============
*/
static qboolean G_BuildableProtectedFromFirendlyFire( damageInfoPlus_t *dmg )
{
  if( g_friendlyBuildableFire.integer )
    return qfalse;

  return dmg->targetIsBuildable && dmg->friendlyFire;
}


/*
============
G_TryActivatingShootableObject
============
*/
static qboolean G_TryActivatingShootableObject( damageInfoPlus_t *dmg )
{
  if( dmg->target->s.eType != ET_MOVER )
    return qfalse;

  if( dmg->target->use && G_MoverAtStartingPos( dmg->target ) )
    dmg->target->use( dmg->target, dmg->inflictor, dmg->attacker );

  return qtrue;
}


/*
============
G_ApplyKnockback
============
*/
static int G_ApplyKnockback( damageInfoPlus_t *dmg )
{
  int knockback = 0;

  if( !dmg->haveKnockbackDir )
    return 0;

  knockback = dmg->damage;

  if( dmg->inflictor->s.weapon != WP_NONE )
  {
    float scale = BG_Weapon( dmg->inflictor->s.weapon )->knockbackScale;
    knockback = ( float ) knockback * scale;
  }

  if( dmg->targetClient )
  {
    float scale = BG_Class( dmg->targetClass )->knockbackScale;
    knockback = ( float ) knockback * scale;
  }

  // Too much knockback from falling really far makes you "bounce" and 
  // looks silly. However, none at all also looks bad. Cap it.
  if( dmg->mod == MOD_FALLING && knockback > 50 ) 
    knockback = 50;

  if( knockback > 200 )
    knockback = 200;

  if( dmg->target->flags & FL_NO_KNOCKBACK )
    knockback = 0;

  if( dmg->dflags & DAMAGE_NO_KNOCKBACK )
    knockback = 0;

  // figure momentum add, even if the damage won't be taken
  if( knockback && dmg->targetClient )
  {
    float mass = 200;
    float pushLen = g_knockback.value * ( float ) knockback / mass;
    vec3_t push;

    VectorScale( dmg->knockbackDir, pushLen, push );
    VectorAdd( dmg->targetPS->velocity, push, dmg->targetPS->velocity );

    // Set the timer so that the other client can't cancel
    // out the movement immediately.
    if( !dmg->targetPS->pm_time )
    {
      int t = knockback * 2;
      if( t < 50  ) t = 50;
      if( t > 200 ) t = 200;
      dmg->targetPS->pm_time = t;
      dmg->targetPS->pm_flags |= PMF_TIME_KNOCKBACK;
    }
  }

  return knockback;
}


/*
============
G_DretchPunt
============
*/
static qboolean G_DretchPunt( damageInfoPlus_t *dmg )
{
  vec3_t dir, push;
  
  if( !g_dretchPunt.integer || dmg->targetClass != PCL_ALIEN_LEVEL0 )
    return qfalse;

  VectorSubtract( dmg->targetOrigin, dmg->sourceOrigin, dir );
  VectorNormalizeFast( dir );
  VectorScale( dir, ( dmg->damage * 10.0f ), push );
  push[2] = 64.0f;
  VectorAdd( dmg->targetPS->velocity, push, dmg->targetPS->velocity );

  return qtrue;
}


/*
============
G_HumanBaseAttackNotify
============
*/
static void G_HumanBaseAttackNotify( damageInfoPlus_t *dmg )
{
  if( dmg->targetTeam != TEAM_HUMANS )
    return;
  
  // Check for a Defense Computer.
  if( !G_FindDCC( dmg->target ) )
    return;

  // Do not notify too frequently.
  if( level.time <= level.humanBaseAttackTimer )
    return;
  
  G_BroadcastEvent( EV_DCC_ATTACK, 0 );
  level.humanBaseAttackTimer = level.time + DC_ATTACK_PERIOD;
}


/*
============
G_IsDamageBlockedEssential
============
*/
static qboolean G_IsDamageBlockedEssential( damageInfoPlus_t *dmg )
{
  // Target cannot take damage.
  if( !dmg->target->takedamage )
    return qtrue;

  // Target is already dead.
  if( dmg->target->health <= 0 )
    return qtrue;

  // Do not deal damage during level intermission.
  if( level.intermissionQueued )
    return qtrue;

  // Do no damage to players who are using the noclip debug feature.
  if( dmg->targetClient && dmg->targetClient->noclip )
    return qtrue;

  // Check for godmode.
  if( dmg->target->flags & FL_GODMODE )
    return qtrue;

  // Movement attacks don't hurt friendly buildables.
  if( G_IsMovementAttack( dmg ) && dmg->targetIsBuildable &&
      dmg->friendlyFire )
    return qtrue;

  return qfalse;
}


/*
============
G_IsDamageBlocked
============
*/
static qboolean G_IsDamageBlocked( damageInfoPlus_t *dmg )
{
  // Check for damage bypassing all non-essential protections.
  if( dmg->dflags & DAMAGE_NO_PROTECTION )
    return qfalse;
  
  // if TF_NO_FRIENDLY_FIRE is set, don't do damage to the target
  // if the attacker was on the same team
  if( dmg->friendlyFire )
  {
    // Don't do friendly fire on movement attacks.
    if( G_IsMovementAttack( dmg ) )
      return qtrue;

    // Push dretches instead of damaging them if g_dretchPunt is enabled.
    if( G_DretchPunt( dmg ) )
      return qtrue;

    // Check if friendly fire has been disabled.
    if( !g_friendlyFire.integer )
      return qtrue;
  }

  if( G_IsAttackAgainstBuildable( dmg ) )
  {
    if( G_BuildableProtectedFromFirendlyFire( dmg ) )
      return qtrue;
  }

  return qfalse;
}


/*
============
G_SetAttackerClientAttackInfo
============
*/
static void G_SetAttackerClientAttackInfo( damageInfoPlus_t *dmg )
{
  if( !dmg->attackerClient )
    return;

  if( dmg->damageToSelf || dmg->target->health < 0 )
    return;
  
  if( dmg->target->s.eType == ET_MISSILE )
    return;

  if( dmg->target->s.eType == ET_GENERAL )
    return;

  if( dmg->friendlyFire )
    dmg->attackerClient->ps.persistant[ PERS_HITS ]--;
  else
    dmg->attackerClient->ps.persistant[ PERS_HITS ]++;
}


/*
============
G_SetTargetClientAttackInfo
============
*/
static void G_SetTargetClientAttackInfo( damageInfoPlus_t *dmg,
                                         int knockback, int finalDamage )
{
  if( !dmg->targetClient )
    return;

  if( dmg->attacker )
    dmg->targetPS->persistant[ PERS_ATTACKER ] = dmg->attacker->s.number;
  else
    dmg->targetPS->persistant[ PERS_ATTACKER ] = ENTITYNUM_WORLD;

  dmg->targetClient->damage_armor += dmg->damage - finalDamage;
  dmg->targetClient->damage_blood += finalDamage;
  dmg->targetClient->damage_knockback += knockback;

  if( dmg->knockbackDir )
  {
    VectorCopy ( dmg->knockbackDir, dmg->targetClient->damage_from );
    dmg->targetClient->damage_fromWorld = qfalse;
  }
  else
  {
    VectorCopy ( dmg->target->r.currentOrigin, dmg->targetClient->damage_from );
    dmg->targetClient->damage_fromWorld = qtrue;
  }

  // Set the last client who damaged the target.
  dmg->targetClient->lasthurt_client = dmg->attacker->s.number;
  dmg->targetClient->lasthurt_mod = dmg->mod;  
}


/*
============
G_ApplyPoison
============
*/
static void G_ApplyPoison( damageInfoPlus_t *dmg )
{
  if( !dmg->targetClient || !dmg->attackerClient )
    return;

  if( dmg->targetTeam != TEAM_HUMANS )
    return;

  if( dmg->targetClient->poisonImmunityTime >= level.time )
    return;
  
  if( !G_AttackCanAddPoison( dmg ) )
    return;
  
  if( !( dmg->attackerPS->stats[ STAT_STATE ] & SS_BOOSTED ) )
    return;

  dmg->targetPS->stats[ STAT_STATE ] |= SS_POISONED;
  dmg->targetClient->lastPoisonTime = level.time;
  dmg->targetClient->lastPoisonClient = dmg->attacker;
}


/*
============
G_CalculateFinalDamage
============
*/
static int G_CalculateFinalDamage( damageInfoPlus_t *dmg )
{
  attackParms_t parms;
  float modifier = 1.0f;
  int damage;

  if( dmg->dflags & DAMAGE_NO_LOCDAMAGE )
    modifier = GetNonLocDamageModifier( dmg );
  else if ( !G_GetAttackParms( dmg, &parms ) )
    modifier = GetNonLocDamageModifier( dmg );
  else if( dmg->targetTeam == TEAM_HUMANS )
    modifier = G_CalcHumanDamageModifier( dmg, &parms );
  else if( dmg->targetTeam == TEAM_ALIENS )
    modifier = G_CalcAlienDamageModifier( dmg, &parms );

  damage = MAX( 1.0f, dmg->damage * modifier + 0.5f );

  if( g_debugDamage.integer )
    trap_SendServerCommand( -1, va(
      "print \"%i (%s) | mod: %1.1f prec: %1.1f elev: %1.1f flank: %1.1f\n\"", 
      damage, modNames[ dmg->mod ], modifier, parms.precision,
      parms.elevation, parms.flanking ) );

  return damage;
}


/*
============
G_ApplyDamage

Applies the final damage value to the target.
============
*/
static void G_ApplyDamage( damageInfoPlus_t *dmg, int finalDamage )
{
  dmg->target->health -= finalDamage;

  if( dmg->targetClient )
  {
    dmg->targetPS->stats[ STAT_HEALTH ] = dmg->target->health;
    dmg->targetClient->pers.infoChangeTime = level.time;
  }

  dmg->target->lastDamageTime = level.time;
  dmg->target->nextRegenTime = level.time + ALIEN_REGEN_DAMAGE_TIME;

  // add to the attackers "account" on the target
  if( dmg->attackerClient && dmg->damageToSelf )
    dmg->target->credits[ dmg->attackerPS->clientNum ] += finalDamage;

  if( dmg->target->health <= 0 )
  {
    if( dmg->targetClient )
      dmg->target->flags |= FL_NO_KNOCKBACK;

    if( dmg->target->health < -999 )
    {
      dmg->target->health = -999;
      if( dmg->targetClient )
        dmg->targetPS->stats[ STAT_HEALTH ] = -999;
    }

    dmg->target->enemy = dmg->attacker;
    dmg->target->die( dmg->target, dmg->inflictor, dmg->attacker,
                      finalDamage, dmg->mod );
  }
  else if( dmg->target->pain )
    dmg->target->pain( dmg->target, dmg->attacker, finalDamage );  
}


/*
============
G_SelectiveDamage2
============
*/
void G_SelectiveDamage2( damageInfo_t *dmg, int immuneTeam )
{
  if( !dmg->target->client ) return;
  if( dmg->target->client->ps.stats[ STAT_TEAM ] == immuneTeam ) return;
  G_Damage2( dmg );
}


/*
============
G_Damage2
============
*/
void G_Damage2( damageInfo_t *baseDmg )
{
  damageInfoPlus_t dmg;
  int knockback;
  int finalDamage;

  G_ComputeDamageInfoPlus( baseDmg, &dmg );

  // Activate shootable buttons and doors.
  if( G_TryActivatingShootableObject( &dmg ) )
    return;

  // Check if the damage is blocked.
  if( G_IsDamageBlockedEssential( &dmg ) || G_IsDamageBlocked( &dmg ) )
    return;

  // Apply knockback.
  knockback = G_ApplyKnockback( &dmg );
  
  // Calculate how much damage to deal.
  finalDamage = G_CalculateFinalDamage( &dmg );

  // Set attack info on clients.
  G_SetAttackerClientAttackInfo( &dmg );
  G_SetTargetClientAttackInfo( &dmg, knockback, finalDamage );

  // Notify humans about a base attack.
  if( G_IsAttackAgainstBuildable( &dmg ) )
    G_HumanBaseAttackNotify( &dmg );

  // Do the actual damage.
  G_ApplyPoison( &dmg );
  G_ApplyDamage( &dmg, finalDamage );
}


/*
============
CanDamage

Returns qtrue if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/
qboolean CanDamage( gentity_t *targ, vec3_t origin )
{
  vec3_t  dest;
  trace_t tr;
  vec3_t  midpoint;

  // use the midpoint of the bounds instead of the origin, because
  // bmodels may have their origin is 0,0,0
  VectorAdd( targ->r.absmin, targ->r.absmax, midpoint );
  VectorScale( midpoint, 0.5, midpoint );

  VectorCopy( midpoint, dest );
  trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
  if( tr.fraction == 1.0  || tr.entityNum == targ->s.number )
    return qtrue;

  // this should probably check in the plane of projection,
  // rather than in world coordinate, and also include Z
  VectorCopy( midpoint, dest );
  dest[ 0 ] += 15.0;
  dest[ 1 ] += 15.0;
  trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
  if( tr.fraction == 1.0 )
    return qtrue;

  VectorCopy( midpoint, dest );
  dest[ 0 ] += 15.0;
  dest[ 1 ] -= 15.0;
  trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
  if( tr.fraction == 1.0 )
    return qtrue;

  VectorCopy( midpoint, dest );
  dest[ 0 ] -= 15.0;
  dest[ 1 ] += 15.0;
  trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
  if( tr.fraction == 1.0 )
    return qtrue;

  VectorCopy( midpoint, dest );
  dest[ 0 ] -= 15.0;
  dest[ 1 ] -= 15.0;
  trap_Trace( &tr, origin, vec3_origin, vec3_origin, dest, ENTITYNUM_NONE, MASK_SOLID );
  if( tr.fraction == 1.0 )
    return qtrue;

  return qfalse;
}

/*
============
G_SelectiveRadiusDamage
============
*/
qboolean G_SelectiveRadiusDamage( vec3_t origin, gentity_t *attacker, float damage,
                                  float radius, gentity_t *ignore, int mod, int team )
{
  float     points, dist;
  gentity_t *ent;
  int       entityList[ MAX_GENTITIES ];
  int       numListedEntities;
  vec3_t    mins, maxs;
  vec3_t    v;
  vec3_t    dir;
  int       i, e;
  qboolean  hitClient = qfalse;

  if( radius < 1 )
    radius = 1;

  for( i = 0; i < 3; i++ )
  {
    mins[ i ] = origin[ i ] - radius;
    maxs[ i ] = origin[ i ] + radius;
  }

  numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

  for( e = 0; e < numListedEntities; e++ )
  {
    ent = &g_entities[ entityList[ e ] ];

    if( ent == ignore )
      continue;

    if( !ent->takedamage )
      continue;

    if( ent->flags & FL_NOTARGET )
      continue;

    // find the distance from the edge of the bounding box
    for( i = 0 ; i < 3 ; i++ )
    {
      if( origin[ i ] < ent->r.absmin[ i ] )
        v[ i ] = ent->r.absmin[ i ] - origin[ i ];
      else if( origin[ i ] > ent->r.absmax[ i ] )
        v[ i ] = origin[ i ] - ent->r.absmax[ i ];
      else
        v[ i ] = 0;
    }

    dist = VectorLength( v );
    if( dist >= radius )
      continue;

    points = damage * ( 1.0 - dist / radius );

    if( CanDamage( ent, origin ) && ent->client &&
        ent->client->ps.stats[ STAT_TEAM ] != team )
    {
      VectorSubtract( ent->r.currentOrigin, origin, dir );
      // push the center of mass higher than the origin so players
      // get knocked into the air more
      dir[ 2 ] += 24;
      hitClient = qtrue;
      G_Damage( ent, NULL, attacker, dir, origin,
          (int)points, DAMAGE_RADIUS|DAMAGE_NO_LOCDAMAGE, mod );
    }
  }

  return hitClient;
}


/*
============
G_RadiusDamage
============
*/
qboolean G_RadiusDamage( vec3_t origin, gentity_t *attacker, float damage,
                         float radius, gentity_t *ignore, int mod )
{
  float     points, dist;
  gentity_t *ent;
  int       entityList[ MAX_GENTITIES ];
  int       numListedEntities;
  vec3_t    mins, maxs;
  vec3_t    v;
  vec3_t    dir;
  int       i, e;
  qboolean  hitClient = qfalse;

  if( radius < 1 )
    radius = 1;

  for( i = 0; i < 3; i++ )
  {
    mins[ i ] = origin[ i ] - radius;
    maxs[ i ] = origin[ i ] + radius;
  }

  numListedEntities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );

  for( e = 0; e < numListedEntities; e++ )
  {
    ent = &g_entities[ entityList[ e ] ];

    if( ent == ignore )
      continue;

    if( !ent->takedamage )
      continue;

    // find the distance from the edge of the bounding box
    for( i = 0; i < 3; i++ )
    {
      if( origin[ i ] < ent->r.absmin[ i ] )
        v[ i ] = ent->r.absmin[ i ] - origin[ i ];
      else if( origin[ i ] > ent->r.absmax[ i ] )
        v[ i ] = origin[ i ] - ent->r.absmax[ i ];
      else
        v[ i ] = 0;
    }

    dist = VectorLength( v );
    if( dist >= radius )
      continue;

    points = damage * ( 1.0 - dist / radius );

    if( CanDamage( ent, origin ) )
    {
      VectorSubtract( ent->r.currentOrigin, origin, dir );
      // push the center of mass higher than the origin so players
      // get knocked into the air more
      dir[ 2 ] += 24;
      hitClient = qtrue;
      G_Damage( ent, NULL, attacker, dir, origin,
          (int)points, DAMAGE_RADIUS|DAMAGE_NO_LOCDAMAGE, mod );
    }
  }

  return hitClient;
}

/*
================
G_LogDestruction

Log deconstruct/destroy events
================
*/
void G_LogDestruction( gentity_t *self, gentity_t *actor, int mod )
{
  buildFate_t fate;

  switch( mod )
  {
    case MOD_DECONSTRUCT:
      fate = BF_DECONSTRUCT;
      break;
    case MOD_REPLACE:
      fate = BF_REPLACE;
      break;
    case MOD_NOCREEP:
      fate = ( actor->client ) ? BF_UNPOWER : BF_AUTO;
      break;
    default:
      if( actor->client )
      {
        if( actor->client->pers.teamSelection == 
            BG_Buildable( self->s.modelindex )->team )
        {
          fate = BF_TEAMKILL;
        }
        else
          fate = BF_DESTROY;
      }
      else
        fate = BF_AUTO;
      break;
  }
  G_BuildLogAuto( actor, self, fate );

  // don't log when marked structures are removed
  if( mod == MOD_REPLACE )
    return;

  G_LogPrintf( S_COLOR_YELLOW "Deconstruct: %d %d %s %s: %s %s by %s\n",
    (int)( actor - g_entities ),
    (int)( self - g_entities ),
    BG_Buildable( self->s.modelindex )->name,
    modNames[ mod ],
    BG_Buildable( self->s.modelindex )->humanName,
    mod == MOD_DECONSTRUCT ? "deconstructed" : "destroyed",
    actor->client ? actor->client->pers.netname : "<world>" );

  // No-power deaths for humans come after some minutes and it's confusing
  //  when the messages appear attributed to the deconner. Just don't print them.
  if( mod == MOD_NOCREEP && actor->client && 
      actor->client->pers.teamSelection == TEAM_HUMANS )
    return;

  if( actor->client && actor->client->pers.teamSelection ==
    BG_Buildable( self->s.modelindex )->team )
  {
    G_TeamCommand( actor->client->ps.stats[ STAT_TEAM ],
      va( "print \"%s ^3%s^7 by %s\n\"",
        BG_Buildable( self->s.modelindex )->humanName,
        mod == MOD_DECONSTRUCT ? "DECONSTRUCTED" : "DESTROYED",
        actor->client->pers.netname ) );
  }

}


/* Compatibility shims. */


#define VECTOR_COPY_HAVE( a, b, have ) \
  if( a ) { VectorCopy( a, b ); have = qtrue; } \
  else have = qfalse;


void G_Damage( gentity_t *targ, gentity_t *inflictor, gentity_t *attacker,
               vec3_t dir, vec3_t point, int damage, int dflags, int mod )
{
  damageInfo_t dmg = DAMAGE_INFO_INIT;
  dmg.target = targ;
  dmg.inflictor = inflictor;
  dmg.attacker = attacker;
  dmg.haveSourceOrigin = qfalse;
  VectorCopy( zeroVector, dmg.sourceOrigin );
  VECTOR_COPY_HAVE( dir, dmg.knockbackDir, dmg.haveKnockbackDir );
  VECTOR_COPY_HAVE( point, dmg.point, dmg.havePoint );
  dmg.damage = damage;
  dmg.dflags = dflags;
  dmg.mod = mod;
  G_Damage2( &dmg );
}


void G_SelectiveDamage( gentity_t *targ, gentity_t *inflictor,
                        gentity_t *attacker, vec3_t dir, vec3_t point,
                        int damage, int dflags, int mod, int immuneTeam )
{
  damageInfo_t dmg = DAMAGE_INFO_INIT;
  dmg.target = targ;
  dmg.inflictor = inflictor;
  dmg.attacker = attacker;
  dmg.haveSourceOrigin = qfalse;
  VectorCopy( zeroVector, dmg.sourceOrigin );
  VECTOR_COPY_HAVE( dir, dmg.knockbackDir, dmg.haveKnockbackDir );
  VECTOR_COPY_HAVE( point, dmg.point, dmg.havePoint );
  dmg.damage = damage;
  dmg.dflags = dflags;
  dmg.mod = mod;
  G_SelectiveDamage2( &dmg, immuneTeam );
}
