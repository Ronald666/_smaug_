/*
                     R E A L M S    O F    D E S P A I R  !
   ___________________________________________________________________________
  //            /                                                            \\
 [|_____________\   ********   *        *   ********   *        *   *******   |]
 [|   \\._.//   /  **********  **      **  **********  **      **  *********  |]
 [|   (0...0)   \  **********  ***    ***  **********  ***    ***  *********  |]
 [|    ).:.(    /  ***         ****  ****  ***    ***  ***    ***  ***        |]
 [|    {o o}    \  *********   **********  **********  ***    ***  *** ****   |]
 [|   / ' ' \   /   *********  *** ** ***  **********  ***    ***  ***  ****  |]
 [|-'- /   \ -`-\         ***  ***    ***  ***    ***  ***    ***  ***   ***  |]
 [|   .VxvxV.   /   *********  ***    ***  ***    ***  **********  *********  |]
 [|_____________\  **********  **      **  **      **  **********  *********  |]
 [|             /  *********   *        *  *        *   ********    *******   |]
  \\____________\____________________________________________________________//
     |                                                                     |
     |    --{ [S]imulated [M]edieval [A]dventure Multi[U]ser [G]ame }--    |
     |_____________________________________________________________________|
     |                                                                     |
     |               -*- Specific Object Creation Module -*-               |
     |_____________________________________________________________________|
    //                                                                     \\
   [|  SMAUG 2.0 © 2014-2015 Antonio Cao (@burzumishi)                      |]
   [|                                                                       |]
   [|  AFKMud Copyright 1997-2007 by Roger Libiez (Samson),                 |]
   [|  Levi Beckerson (Whir), Michael Ward (Tarl), Erik Wolfe (Dwip),       |]
   [|  Cameron Carroll (Cam), Cyberfox, Karangi, Rathian, Raine,            |]
   [|  Xorith, and Adjani.                                                  |]
   [|  All Rights Reserved. External contributions from Remcon, Quixadhal,  |]
   [|  Zarius and many others.                                              |]
   [|                                                                       |]
   [|  SMAUG 1.4 © 1994-1998 Thoric/Altrag/Blodkai/Narn/Haus/Scryn/Rennard  |]
   [|  Swordbearer/Gorog/Grishnakh/Nivek/Tricops/Fireblade/Edmond/Conran    |]
   [|                                                                       |]
   [|  Merc 2.1 Diku Mud improvments © 1992-1993 Michael Chastain, Michael  |]
   [|  Quan, and Mitchell Tse. Original Diku Mud © 1990-1991 by Sebastian   |]
   [|  Hammer, Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, Katja    |]
   [|  Nyboe. Win32 port Nick Gammon.                                       |]
    \\_____________________________________________________________________//
*/

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"



void make_puddle args ((CHAR_DATA * ch, OBJ_DATA * cont));


/*
 * Make a fire.
 */
void
make_fire (ROOM_INDEX_DATA * in_room, sh_int timer)
{
  OBJ_DATA *fire;

  fire = create_object (get_obj_index (OBJ_VNUM_FIRE), 0);
  fire->timer = number_fuzzy (timer);
#ifdef OVERLANDCODE
  obj_to_room (fire, in_room, NULL);
#else
  obj_to_room (fire, in_room);
#endif
  return;
}

/*
 * Make a trap.
 */
OBJ_DATA *
make_trap (int v0, int v1, int v2, int v3)
{
  OBJ_DATA *trap;

  trap = create_object (get_obj_index (OBJ_VNUM_TRAP), 0);
  trap->timer = 0;
  trap->value[0] = v0;
  trap->value[1] = v1;
  trap->value[2] = v2;
  trap->value[3] = v3;
  return trap;
}


/*
 * Turn an object into scraps.		-Thoric
 */
void
make_scraps (OBJ_DATA * obj)
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *scraps, *tmpobj;
  CHAR_DATA *ch = NULL;

  separate_obj (obj);
  scraps = create_object (get_obj_index (OBJ_VNUM_SCRAPS), 0);
  scraps->timer = number_range (5, 15);

#ifdef OVERLANDCODE
  if( IS_OBJ_STAT( obj, ITEM_ONMAP ) )
  {
	SET_OBJ_STAT( scraps, ITEM_ONMAP );
	scraps->map = obj->map;
	scraps->x = obj->x;
	scraps->y = obj->y;
  }
#endif

  /* don't make scraps of scraps of scraps of ... */
  if (obj->pIndexData->vnum == OBJ_VNUM_SCRAPS)
    {
      STRFREE (scraps->short_descr);
      scraps->short_descr = STRALLOC ("some debris");
      STRFREE (scraps->description);
      scraps->description =
	STRALLOC ("Bits of debris lie on the ground here.");
    }
  else
    {
      sprintf (buf, scraps->short_descr, obj->short_descr);
      STRFREE (scraps->short_descr);
      scraps->short_descr = STRALLOC (buf);
      sprintf (buf, scraps->description, obj->short_descr);
      STRFREE (scraps->description);
      scraps->description = STRALLOC (buf);
    }

  if (obj->carried_by)
    {
      act (AT_OBJECT, "$p falls to the ground in scraps!", obj->carried_by,
	   obj, NULL, TO_CHAR);
      if (obj == get_eq_char (obj->carried_by, WEAR_WIELD)
	  && (tmpobj =
	      get_eq_char (obj->carried_by, WEAR_DUAL_WIELD)) != NULL)
	tmpobj->wear_loc = WEAR_WIELD;

#ifdef OVERLANDCODE
      obj_to_room (scraps, obj->carried_by->in_room, ch);
#else
      obj_to_room (scraps, obj->carried_by->in_room);
#endif
    }
  else if (obj->in_room)
    {
      if ((ch = obj->in_room->first_person) != NULL)
	{
	  act (AT_OBJECT, "$p is reduced to little more than scraps.", ch,
	       obj, NULL, TO_ROOM);
	  act (AT_OBJECT, "$p is reduced to little more than scraps.", ch,
	       obj, NULL, TO_CHAR);
	}
#ifdef OVERLANDCODE
      obj_to_room (scraps, obj->in_room, ch);
#else
      obj_to_room (scraps, obj->in_room);
#endif
    }
  if ((obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_KEYRING
       || obj->item_type == ITEM_QUIVER || obj->item_type == ITEM_CORPSE_PC)
      && obj->first_content)
    {
      if (ch && ch->in_room)
	{
	  act (AT_OBJECT, "The contents of $p fall to the ground.", ch, obj,
	       NULL, TO_ROOM);
	  act (AT_OBJECT, "The contents of $p fall to the ground.", ch, obj,
	       NULL, TO_CHAR);
	}
      if (obj->carried_by)
	empty_obj (obj, NULL, obj->carried_by->in_room);
      else if (obj->in_room)
	empty_obj (obj, NULL, obj->in_room);
      else if (obj->in_obj)
	empty_obj (obj, obj->in_obj, NULL);
    }
  extract_obj (obj);
}


/*
 * Make a corpse out of a character.
 */
void
make_corpse (CHAR_DATA * ch, CHAR_DATA * killer)
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *corpse;
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  char *name;
  
#ifdef ENABLE_MORGUE
  ROOM_INDEX_DATA *location;
  location = get_room_index ( ROOM_VNUM_MORGUE );
#endif

  if (IS_NPC (ch))
    {
      name = ch->short_descr;
      corpse = create_object (get_obj_index (OBJ_VNUM_CORPSE_NPC), 0);
      corpse->timer = 6;

#ifdef ENABLE_GOLD_SILVER_COPPER
			if ( ch->gold > 0 )
			{
				if ( ch->in_room )
				{
				ch->in_room->area->gold_looted += ch->gold;
				sysdata.global_gold_looted += ch->gold/100;
				}
				obj_to_obj( create_money( ch->gold,0 ), corpse );
				ch->gold = 0;
			}
			if ( ch->silver > 0 )
			{
				if ( ch->in_room )
				{
				ch->in_room->area->silver_looted += ch->silver;
				sysdata.global_silver_looted += ch->silver/100;
				}
				obj_to_obj( create_money( ch->silver,1 ), corpse );
				ch->silver = 0;
			}
			if ( ch->copper > 0 )
			{
				if ( ch->in_room )
				{
				ch->in_room->area->copper_looted += ch->copper;
				sysdata.global_copper_looted += ch->copper/100;
				}
				obj_to_obj( create_money( ch->copper,2 ), corpse );
				ch->copper = 0;
			}

			corpse->gold_cost    = (-(int)ch->pIndexData->vnum);
			corpse->silver_cost  = (-(int)ch->pIndexData->vnum);
			corpse->copper_cost  = (-(int)ch->pIndexData->vnum);
#else
      if (ch->gold > 0)
				{
					if (ch->in_room)
						{
							ch->in_room->area->gold_looted += ch->gold;
							sysdata.global_looted += ch->gold / 100;
						}
					obj_to_obj (create_money (ch->gold), corpse);
					ch->gold = 0;
				}

			/*	Using corpse cost to cheat, since corpses not sellable */
      corpse->cost = (-(int) ch->pIndexData->vnum);
#endif

			/* Cannot use these!  They are used.
				corpse->value[0] = (int)ch->pIndexData->vnum;
				corpse->value[1] = (int)ch->max_hit;
			*/
      corpse->value[2] = corpse->timer;
    }
  else
    {
      name = ch->name;
      corpse = create_object (get_obj_index (OBJ_VNUM_CORPSE_PC), 0);
      if (in_arena (ch))
	corpse->timer = 0;
      else
	corpse->timer = 40;
      corpse->value[2] = (int) (corpse->timer / 8);
      corpse->value[4] = ch->level;
      if (CAN_PKILL (ch) && sysdata.pk_loot)
	xSET_BIT (corpse->extra_flags, ITEM_CLANCORPSE);
      /* Pkill corpses get save timers, in ticks (approx 70 seconds)
         This should be enough for the killer to type 'get all corpse'. */
      if (!IS_NPC (ch) && !IS_NPC (killer))
	corpse->value[3] = 1;
      else
	corpse->value[3] = 0;
    }

  if (CAN_PKILL (ch) && CAN_PKILL (killer) && ch != killer && sysdata.pk_loot)
    {
      sprintf (buf, "%s", killer->name);
      STRFREE (corpse->action_desc);
      corpse->action_desc = STRALLOC (buf);
    }

  /* Added corpse name - make locate easier , other skills */
  sprintf (buf, "corpse %s", name);
  STRFREE (corpse->name);
  corpse->name = STRALLOC (buf);

  sprintf (buf, corpse->short_descr, name);
  STRFREE (corpse->short_descr);
  corpse->short_descr = STRALLOC (buf);

  sprintf (buf, corpse->description, name);
  STRFREE (corpse->description);
  corpse->description = STRALLOC (buf);

  for (obj = ch->first_carrying; obj; obj = obj_next)
    {
      obj_next = obj->next_content;
      /* don't put perm player eq in the corpse */
      if (!IS_NPC (ch) && IS_OBJ_STAT (obj, ITEM_PERMANENT))
	continue;
      obj_from_char (obj);
      if (IS_OBJ_STAT (obj, ITEM_INVENTORY)
	  || IS_OBJ_STAT (obj, ITEM_DEATHROT))
	extract_obj (obj);
      else if (IS_OBJ_STAT (obj, ITEM_DEATHDROP))
	{
#ifdef OVERLANDCODE
	  obj_to_room (obj, ch->in_room, ch);
#else
	  obj_to_room (obj, ch->in_room);
#endif
	  oprog_drop_trigger (ch, obj);	/* mudprogs */
	}
      else
	obj_to_obj (obj, corpse);
    }

#ifdef ENABLE_MORGUE
    if ( IS_NPC(ch) || ch->level > MAX_MORGUE_LEVEL )
#ifdef OVERLANDCODE
        obj_to_room( corpse, ch->in_room, ch );
#else
        obj_to_room( corpse, ch->in_room );
#endif
    else
#ifdef OVERLANDCODE
	    obj_to_room( corpse, location, ch );
#else
	    obj_to_room( corpse, location );
#endif
#else	
#ifdef OVERLANDCODE
      obj_to_room (corpse, ch->in_room, ch);
#else
      obj_to_room (corpse, ch->in_room);
#endif
#endif
  
  return;
}


void
make_puddle (CHAR_DATA * ch, OBJ_DATA * cont)
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;
  char buf[20];
  char buf2[70];
  bool found = FALSE;

  for (obj = ch->in_room->first_content; obj; obj = obj_next)
    {
      if (obj->pIndexData->item_type == ITEM_PUDDLE)
	{
	  if (obj->value[2] == cont->value[2])
	    {
	      obj->value[1] += cont->value[1];
	      obj->value[3] += cont->value[3];
	      obj->timer = number_range (2, 4);
	      found = TRUE;
	      break;
	    }
	}
      obj_next = obj->next_content;
    }

  if (!found)
    {
      obj = create_object (get_obj_index (OBJ_VNUM_PUDDLE), 0);
      obj->timer = number_range (2, 4);
      obj->value[1] += cont->value[1];
      obj->value[2] = cont->value[2];
      obj->value[3] = cont->value[3];
#ifdef OVERLANDCODE
      obj_to_room (obj, ch->in_room, ch);
#else
      obj_to_room (obj, ch->in_room);
#endif
    }

  if (obj->value[1] > 15)
    sprintf (buf, "large");
  else if (obj->value[1] > 10)
    sprintf (buf, "rather large");
  else if (obj->value[1] > 5)
    sprintf (buf, "rather small");
  else
    sprintf (buf, "small");
  sprintf (buf2, "There is a %s puddle of %s.", buf,
	   (obj->value[2] >=
	    LIQ_MAX ? "water" : liq_table[obj->value[2]].liq_name));
  obj->description = STRALLOC (buf2);
  return;
}


void
make_blood (CHAR_DATA * ch)
{
  OBJ_DATA *obj;
  OBJ_DATA *obj_next;

  for (obj = ch->in_room->first_content; obj; obj = obj_next)
    {
      if (obj->pIndexData->vnum == OBJ_VNUM_BLOOD)
	{
	  obj->description =
	    STRALLOC ("A large pool of spilled blood lies here.");
	  obj->value[1] += number_range (3, UMIN (5, ch->level));
	  obj->timer = number_range (2, 4);
	  return;
	}
      obj_next = obj->next_content;
    }

  obj = create_object (get_obj_index (OBJ_VNUM_BLOOD), 0);
  obj->timer = number_range (2, 4);
  obj->value[1] = number_range (3, UMIN (5, ch->level));
#ifdef OVERLANDCODE
  obj_to_room (obj, ch->in_room, ch);
#else
  obj_to_room (obj, ch->in_room);
#endif
}


void
make_bloodstain (CHAR_DATA * ch)
{
  OBJ_DATA *obj;

  obj = create_object (get_obj_index (OBJ_VNUM_BLOODSTAIN), 0);
  obj->timer = number_range (1, 2);
#ifdef OVERLANDCODE
  obj_to_room (obj, ch->in_room, ch);
#else
  obj_to_room (obj, ch->in_room);
#endif
}


/*
 * make some coinage
 * G/S/C support -Druid
 * type:
 * 0 = gold
 * 1 = silver
 * 2 = copper
 */
#ifdef ENABLE_GOLD_SILVER_COPPER
OBJ_DATA *
create_money( int amount, int type )
{
    char buf[MAX_STRING_LENGTH];
    OBJ_DATA *obj;

    if ( amount <= 0 )
    {
	bug( "Create_money: zero or negative money %d.", amount );
	amount = 1;
    }
    
    if( type>3 || type<0)
    {
    	bug("Create_money: wrong type: %d!",type);
    	type=2;
    }

	if (type ==0)
	{
    	if ( amount == 1 )
    	{
		obj = create_object( get_obj_index( OBJ_VNUM_GOLD_ONE ), 0 );
		return obj;
    	}
    	else
    	{
		obj = create_object( get_obj_index( OBJ_VNUM_GOLD_SOME ), 0 );	
		sprintf( buf, obj->short_descr, amount );
		STRFREE( obj->short_descr );
		obj->short_descr = STRALLOC( buf );
		obj->value[0]	 = amount;
		return obj;
    	}
	}
	
	if (type ==1)
	{
    	if ( amount == 1 )
    	{
		obj = create_object( get_obj_index( OBJ_VNUM_SILVER_ONE ), 0 );
		return obj;
    	}
    	else
    	{
		obj = create_object( get_obj_index( OBJ_VNUM_SILVER_SOME ), 0 );	
		sprintf( buf, obj->short_descr, amount );
		STRFREE( obj->short_descr );
		obj->short_descr = STRALLOC( buf );
		obj->value[0]	 = amount;
		return obj;
    	}
	}
	
	if (type == 2)
	{
    	if ( amount == 1 )
   	 {
		obj = create_object( get_obj_index( OBJ_VNUM_COPPER_ONE ), 0 );
		return obj;
    	}
    	else
    	{
		obj = create_object( get_obj_index( OBJ_VNUM_COPPER_SOME ), 0 );	
		sprintf( buf, obj->short_descr, amount );
		STRFREE( obj->short_descr );
		obj->short_descr = STRALLOC( buf );
		obj->value[0]	 = amount;
		return obj;
    	}
	}
	obj = create_object( get_obj_index( OBJ_VNUM_COPPER_ONE ), 0 );
	return obj;    
}
#else
OBJ_DATA *
create_money (int amount)
{
  char buf[MAX_STRING_LENGTH];
  OBJ_DATA *obj;

  if (amount <= 0)
    {
      bug ("Create_money: zero or negative money %d.", amount);
      amount = 1;
    }

  if (amount == 1)
    {
      obj = create_object (get_obj_index (OBJ_VNUM_MONEY_ONE), 0);
    }
  else
    {
      obj = create_object (get_obj_index (OBJ_VNUM_MONEY_SOME), 0);
      sprintf (buf, obj->short_descr, amount);
      STRFREE (obj->short_descr);
      obj->short_descr = STRALLOC (buf);
      obj->value[0] = amount;
    }

  return obj;
}
#endif
