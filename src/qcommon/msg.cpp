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

#include "msg.h"

#include "alternatePlayerstate.h"
#include "cvar.h"
#include "huffman.h"
#include "q_shared.h"
#include "qcommon.h"

static huffman_t msgHuff;

static bool msgInit = false;

int pcount[256];

/*
==============================================================================

                        MESSAGE IO FUNCTIONS

Handles uint8_t ordering and avoids alignment errors
==============================================================================
*/

int oldsize = 0;

void MSG_initHuffman(void);

void MSG_Init(msg_t *buf, uint8_t *data, int length)
{
    if (!msgInit)
    {
        MSG_initHuffman();
    }
    ::memset(buf, 0, sizeof(*buf));
    buf->data = data;
    buf->maxsize = length;
}

void MSG_InitOOB(msg_t *buf, uint8_t *data, int length)
{
    if (!msgInit)
    {
        MSG_initHuffman();
    }
    ::memset(buf, 0, sizeof(*buf));
    buf->data = data;
    buf->maxsize = length;
    buf->oob = true;
}

void MSG_Clear(msg_t *buf)
{
    buf->cursize = 0;
    buf->overflowed = false;
    buf->bit = 0;  //<- in bits
}

void MSG_Bitstream(msg_t *buf) { buf->oob = false; }
void MSG_BeginReading(msg_t *msg)
{
    msg->readcount = 0;
    msg->bit = 0;
    msg->oob = false;
}

void MSG_BeginReadingOOB(msg_t *msg)
{
    msg->readcount = 0;
    msg->bit = 0;
    msg->oob = true;
}

void MSG_Copy(msg_t *buf, uint8_t *data, int length, msg_t *src)
{
    if (length < src->cursize)
    {
        Com_Error(ERR_DROP, "MSG_Copy: can't copy into a smaller msg_t buffer");
    }
    ::memcpy(buf, src, sizeof(msg_t));
    buf->data = data;
    ::memcpy(buf->data, src->data, src->cursize);
}

/*
=============================================================================

bit functions

=============================================================================
*/

int overflows;

// negative bit values include signs
void MSG_WriteBits(msg_t *msg, int value, int bits)
{
    int i;
    //FILE* fp;

    oldsize += bits;

    if ( msg->overflowed )
    {
        return;
    }

    if (bits == 0 || bits < -31 || bits > 32)
    {
        Com_Error(ERR_DROP, "MSG_WriteBits: bad bits %i", bits);
    }

    // check for overflows
    if (bits != 32)
    {
        if (bits > 0)
        {
            if (value > ((1 << bits) - 1) || value < 0)
            {
                overflows++;
            }
        }
        else
        {
            int r;

            r = 1 << (bits - 1);

            if (value > r - 1 || value < -r)
            {
                overflows++;
            }
        }
    }
    if (bits < 0)
    {
        bits = -bits;
    }
    if (msg->oob)
    {
        if (msg->cursize + (bits >> 3) > msg->maxsize)
        {
            msg->overflowed = true;
            return;
        }
        if (bits == 8)
        {
            msg->data[msg->cursize] = value;
            msg->cursize += 1;
            msg->bit += 8;
        }
        else if (bits == 16)
        {
            short temp = value;

            CopyLittleShort(&msg->data[msg->cursize], &temp);
            msg->cursize += 2;
            msg->bit += 16;
        }
        else if (bits == 32)
        {
            CopyLittleLong(&msg->data[msg->cursize], &value);
            msg->cursize += 4;
            msg->bit += 32;
        }
        else
            Com_Error(ERR_DROP, "can't write %d bits", bits);
    }
    else
    {
        value &= (0xffffffff >> (32 - bits));
        if (bits & 7)
        {
            int nbits;
            nbits = bits & 7;

            if ( msg->bit + nbits > msg->maxsize << 3 )
            {
                msg->overflowed = true;
                return;
            }

            for (i = 0; i < nbits; i++)
            {
                Huff_putBit((value & 1), msg->data, &msg->bit);
                value = (value >> 1);
            }
            bits = bits - nbits;
        }
        if (bits)
        {
            for (i = 0; i < bits; i += 8)
            {
                Huff_offsetTransmit(&msgHuff.compressor, (value & 0xff), msg->data, &msg->bit, msg->maxsize << 3);
                value = (value >> 8);

                if (msg->bit > msg->maxsize << 3)
                {
                    msg->overflowed = true;
                    return;
                }
            }
        }
        msg->cursize = (msg->bit >> 3) + 1;
    }
}

int MSG_ReadBits(msg_t *msg, int bits)
{
    int value;
    int get;
    bool sgn;


    if (msg->readcount > msg->cursize)
    {
        return 0;
    }

    value = 0;

    if (bits < 0)
    {
        bits = -bits;
        sgn = true;
    }
    else
    {
        sgn = false;
    }

    if (msg->oob)
    {
        if (msg->readcount + (bits >> 3) > msg->cursize)
        {
            msg->readcount = msg->cursize + 1;
            return 0;
        }

        if (bits == 8)
        {
            value = msg->data[msg->readcount];
            msg->readcount += 1;
            msg->bit += 8;
        }
        else if (bits == 16)
        {
            short temp;

            CopyLittleShort(&temp, &msg->data[msg->readcount]);
            value = temp;
            msg->readcount += 2;
            msg->bit += 16;
        }
        else if (bits == 32)
        {
            CopyLittleLong(&value, &msg->data[msg->readcount]);
            msg->readcount += 4;
            msg->bit += 32;
        }
        else
            Com_Error(ERR_DROP, "can't read %d bits", bits);
    }
    else
    {
        int nbits = 0;
        if (bits & 7)
        {
            nbits = bits & 7;
            if (msg->bit + nbits > msg->cursize << 3)
            {
                msg->readcount = msg->cursize + 1;
                return 0;
            }
            for (int i = 0; i < nbits; i++)
            {
                value |= (Huff_getBit(msg->data, &msg->bit) << i);
            }
            bits = bits - nbits;
        }
        if (bits)
        {
            for (int i = 0; i < bits; i += 8)
            {
                Huff_offsetReceive(msgHuff.decompressor.tree, &get, msg->data, &msg->bit, msg->cursize << 3);
                value |= (get << (i + nbits));

                if (msg->bit > msg->cursize << 3)
                {
                    msg->readcount = msg->cursize + 1;
                    return 0;
                }
            }
        }
        msg->readcount = (msg->bit >> 3) + 1;
    }
    if (sgn && bits > 0 && bits < 32)
    {
        if (value & (1 << (bits - 1)))
        {
            value |= -1 ^ ((1 << bits) - 1);
        }
    }

    return value;
}

//================================================================================

//
// writing functions
//

void MSG_WriteChar(msg_t *sb, int c)
{
#ifdef PARANOID
    if (c < -128 || c > 127) Com_Error(ERR_FATAL, "MSG_WriteChar: range error");
#endif

    MSG_WriteBits(sb, c, 8);
}

void MSG_WriteByte(msg_t *sb, int c)
{
#ifdef PARANOID
    if (c < 0 || c > 255) Com_Error(ERR_FATAL, "MSG_WriteByte: range error");
#endif

    MSG_WriteBits(sb, c, 8);
}

void MSG_WriteData(msg_t *buf, const void *data, int length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        MSG_WriteByte(buf, ((uint8_t *)data)[i]);
    }
}

void MSG_WriteShort(msg_t *sb, int c)
{
#ifdef PARANOID
    if (c < ((short)0x8000) || c > (short)0x7fff) Com_Error(ERR_FATAL, "MSG_WriteShort: range error");
#endif

    MSG_WriteBits(sb, c, 16);
}

void MSG_WriteLong(msg_t *sb, int c) { MSG_WriteBits(sb, c, 32); }
void MSG_WriteFloat(msg_t *sb, float f)
{
    floatint_t dat;
    dat.f = f;
    MSG_WriteBits(sb, dat.i, 32);
}

static void MSG_WriteString2(msg_t *sb, const char *s, int maxsize)
{
    int size = strlen(s) + 1;

    if (size > maxsize)
    {
        Com_Printf("MSG_WriteString2: %i > %i\n", size, maxsize);
        MSG_WriteData(sb, "", 1);
        return;
    }

    MSG_WriteData(sb, s, size);
}

void MSG_WriteString(msg_t *sb, const char *s) { MSG_WriteString2(sb, s, MAX_STRING_CHARS); }
void MSG_WriteBigString(msg_t *sb, const char *s) { MSG_WriteString2(sb, s, BIG_INFO_STRING); }
void MSG_WriteAngle(msg_t *sb, float f) { MSG_WriteByte(sb, (int)(f * 256 / 360) & 255); }
void MSG_WriteAngle16(msg_t *sb, float f) { MSG_WriteShort(sb, ANGLE2SHORT(f)); }
//============================================================

//
// reading functions
//

// returns -1 if no more characters are available
int MSG_ReadChar(msg_t *msg)
{
    int c;

    c = (signed char)MSG_ReadBits(msg, 8);
    if (msg->readcount > msg->cursize)
    {
        c = -1;
    }

    return c;
}

int MSG_ReadByte(msg_t *msg)
{
    int c;

    c = (unsigned char)MSG_ReadBits(msg, 8);
    if (msg->readcount > msg->cursize)
    {
        c = -1;
    }
    return c;
}

int MSG_LookaheadByte(msg_t *msg)
{
    const int bloc = Huff_getBloc();
    const int readcount = msg->readcount;
    const int bit = msg->bit;
    int c = MSG_ReadByte(msg);
    Huff_setBloc(bloc);
    msg->readcount = readcount;
    msg->bit = bit;
    return c;
}

int MSG_ReadShort(msg_t *msg)
{
    int c;

    c = (short)MSG_ReadBits(msg, 16);
    if (msg->readcount > msg->cursize)
    {
        c = -1;
    }

    return c;
}

int MSG_ReadLong(msg_t *msg)
{
    int c;

    c = MSG_ReadBits(msg, 32);
    if (msg->readcount > msg->cursize)
    {
        c = -1;
    }

    return c;
}

float MSG_ReadFloat(msg_t *msg)
{
    floatint_t dat;

    dat.i = MSG_ReadBits(msg, 32);
    if (msg->readcount > msg->cursize)
    {
        dat.f = -1;
    }

    return dat.f;
}

char *MSG_ReadString(msg_t *msg)
{
    static char string[MAX_STRING_CHARS];

    size_t l = 0;
    do
    {
        int c = MSG_ReadByte(msg);  // use ReadByte so -1 is out of bounds
        if (c == -1 || c == 0)
        {
            break;
        }

        string[l] = c;
        l++;
    } while (l < sizeof(string) - 1);

    string[l] = 0;

    return string;
}

char *MSG_ReadBigString(msg_t *msg)
{
    static char string[BIG_INFO_STRING];

    size_t l = 0;
    do
    {
        int c = MSG_ReadByte(msg);  // use ReadByte so -1 is out of bounds
        if (c == -1 || c == 0)
        {
            break;
        }

        string[l] = c;
        l++;
    } while (l < sizeof(string) - 1);

    string[l] = 0;

    return string;
}

char *MSG_ReadStringLine(msg_t *msg)
{
    static char string[MAX_STRING_CHARS];

    size_t l = 0;
    do
    {
        int c = MSG_ReadByte(msg);  // use ReadByte so -1 is out of bounds
        if (c == -1 || c == 0 || c == '\n')
        {
            break;
        }

        string[l] = c;
        l++;
    } while (l < sizeof(string) - 1);

    string[l] = 0;

    return string;
}

float MSG_ReadAngle16(msg_t *msg) { return SHORT2ANGLE(MSG_ReadShort(msg)); }
void MSG_ReadData(msg_t *msg, void *data, int len)
{
    int i;

    for (i = 0; i < len; i++)
    {
        ((uint8_t *)data)[i] = MSG_ReadByte(msg);
    }
}

// a string hasher which gives the same hash value even if the
// string is later modified via the legacy MSG read/write code
int MSG_HashKey(int alternateProtocol, const char *string, int maxlen)
{
    int hash, i;

    hash = 0;
    for (i = 0; i < maxlen && string[i] != '\0'; i++)
    {
        if (string[i] & 0x80 || (alternateProtocol == 2 && string[i] == '%'))
            hash += '.' * (119 + i);
        else
            hash += string[i] * (119 + i);
    }
    hash = (hash ^ (hash >> 10) ^ (hash >> 20));
    return hash;
}

/*
=============================================================================

delta functions

=============================================================================
*/

extern cvar_t *cl_shownet;

#define LOG(x)                                  \
    if (cl_shownet && cl_shownet->integer == 4) \
    {                                           \
        Com_Printf("%s ", x);                   \
    };

void MSG_WriteDelta(msg_t *msg, int oldV, int newV, int bits)
{
    if (oldV == newV)
    {
        MSG_WriteBits(msg, 0, 1);
        return;
    }
    MSG_WriteBits(msg, 1, 1);
    MSG_WriteBits(msg, newV, bits);
}

int MSG_ReadDelta(msg_t *msg, int oldV, int bits)
{
    if (MSG_ReadBits(msg, 1))
    {
        return MSG_ReadBits(msg, bits);
    }
    return oldV;
}

void MSG_WriteDeltaFloat(msg_t *msg, float oldV, float newV)
{
    floatint_t fi;
    if (oldV == newV)
    {
        MSG_WriteBits(msg, 0, 1);
        return;
    }
    fi.f = newV;
    MSG_WriteBits(msg, 1, 1);
    MSG_WriteBits(msg, fi.i, 32);
}

float MSG_ReadDeltaFloat(msg_t *msg, float oldV)
{
    if (MSG_ReadBits(msg, 1))
    {
        floatint_t fi;

        fi.i = MSG_ReadBits(msg, 32);
        return fi.f;
    }
    return oldV;
}

/*
=============================================================================

delta functions with keys

=============================================================================
*/

unsigned int kbitmask[32] = {
    0x00000001, 0x00000003, 0x00000007, 0x0000000F, 0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF, 0x000001FF,
    0x000003FF, 0x000007FF, 0x00000FFF, 0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF, 0x0001FFFF, 0x0003FFFF,
    0x0007FFFF, 0x000FFFFF, 0x001FFFFf, 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF,
    0x0FFFFFFF, 0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF,
};

void MSG_WriteDeltaKey(msg_t *msg, int key, int oldV, int newV, int bits)
{
    if (oldV == newV)
    {
        MSG_WriteBits(msg, 0, 1);
        return;
    }
    MSG_WriteBits(msg, 1, 1);
    MSG_WriteBits(msg, newV ^ key, bits);
}

int MSG_ReadDeltaKey(msg_t *msg, int key, int oldV, int bits)
{
    if (MSG_ReadBits(msg, 1))
    {
        return MSG_ReadBits(msg, bits) ^ (key & kbitmask[bits - 1]);
    }
    return oldV;
}

void MSG_WriteDeltaKeyFloat(msg_t *msg, int key, float oldV, float newV)
{
    floatint_t fi;
    if (oldV == newV)
    {
        MSG_WriteBits(msg, 0, 1);
        return;
    }
    fi.f = newV;
    MSG_WriteBits(msg, 1, 1);
    MSG_WriteBits(msg, fi.i ^ key, 32);
}

float MSG_ReadDeltaKeyFloat(msg_t *msg, int key, float oldV)
{
    if (MSG_ReadBits(msg, 1))
    {
        floatint_t fi;

        fi.i = MSG_ReadBits(msg, 32) ^ key;
        return fi.f;
    }
    return oldV;
}

/*
============================================================================

usercmd_t communication

============================================================================
*/

/*
=====================
MSG_WriteDeltaUsercmdKey
=====================
*/
void MSG_WriteDeltaUsercmdKey(msg_t *msg, int key, usercmd_t *from, usercmd_t *to)
{
    if (to->serverTime - from->serverTime < 256)
    {
        MSG_WriteBits(msg, 1, 1);
        MSG_WriteBits(msg, to->serverTime - from->serverTime, 8);
    }
    else
    {
        MSG_WriteBits(msg, 0, 1);
        MSG_WriteBits(msg, to->serverTime, 32);
    }
    if ( from->angles[0] == to->angles[0]
      && from->angles[1] == to->angles[1]
      && from->angles[2] == to->angles[2]
      && from->forwardmove == to->forwardmove
      && from->rightmove == to->rightmove
      && from->upmove == to->upmove
      && from->buttons == to->buttons
      && from->weapon == to->weapon )
    {
        MSG_WriteBits(msg, 0, 1);  // no change
        oldsize += 7;
        return;
    }
    key ^= to->serverTime;
    MSG_WriteBits(msg, 1, 1);
    MSG_WriteDeltaKey(msg, key, from->angles[0], to->angles[0], 16);
    MSG_WriteDeltaKey(msg, key, from->angles[1], to->angles[1], 16);
    MSG_WriteDeltaKey(msg, key, from->angles[2], to->angles[2], 16);
    MSG_WriteDeltaKey(msg, key, from->forwardmove, to->forwardmove, 8);
    MSG_WriteDeltaKey(msg, key, from->rightmove, to->rightmove, 8);
    MSG_WriteDeltaKey(msg, key, from->upmove, to->upmove, 8);
    MSG_WriteDeltaKey(msg, key, from->buttons, to->buttons, 16);
    MSG_WriteDeltaKey(msg, key, from->weapon, to->weapon, 8);
}

/*
=====================
MSG_ReadDeltaUsercmdKey
=====================
*/
void MSG_ReadDeltaUsercmdKey(msg_t *msg, int key, usercmd_t *from, usercmd_t *to)
{
    if (MSG_ReadBits(msg, 1))
    {
        to->serverTime = from->serverTime + MSG_ReadBits(msg, 8);
    }
    else
    {
        to->serverTime = MSG_ReadBits(msg, 32);
    }
    if (MSG_ReadBits(msg, 1))
    {
        key ^= to->serverTime;
        to->angles[0] = MSG_ReadDeltaKey(msg, key, from->angles[0], 16);
        to->angles[1] = MSG_ReadDeltaKey(msg, key, from->angles[1], 16);
        to->angles[2] = MSG_ReadDeltaKey(msg, key, from->angles[2], 16);
        to->forwardmove = MSG_ReadDeltaKey(msg, key, from->forwardmove, 8);
        if (to->forwardmove == -128) to->forwardmove = -127;
        to->rightmove = MSG_ReadDeltaKey(msg, key, from->rightmove, 8);
        if (to->rightmove == -128) to->rightmove = -127;
        to->upmove = MSG_ReadDeltaKey(msg, key, from->upmove, 8);
        if (to->upmove == -128) to->upmove = -127;
        to->buttons = MSG_ReadDeltaKey(msg, key, from->buttons, 16);
        to->weapon = MSG_ReadDeltaKey(msg, key, from->weapon, 8);
    }
    else
    {
        to->angles[0] = from->angles[0];
        to->angles[1] = from->angles[1];
        to->angles[2] = from->angles[2];
        to->forwardmove = from->forwardmove;
        to->rightmove = from->rightmove;
        to->upmove = from->upmove;
        to->buttons = from->buttons;
        to->weapon = from->weapon;
    }
}

/*
=============================================================================

entityState_t communication

=============================================================================
*/

/*
=================
MSG_ReportChangeVectors_f

Prints out a table from the current statistics for copying to code
=================
*/
void MSG_ReportChangeVectors_f(void)
{
    int i;
    for (i = 0; i < 256; i++)
    {
        if (pcount[i])
        {
            Com_Printf("%d used %d\n", i, pcount[i]);
        }
    }
}

typedef struct {
    const char *name;
    size_t offset;
    int bits;  // 0 = float
} netField_t;

// using the stringizing operator to save typing...
#define NETF(x) #x, (size_t) & ((entityState_t *) 0)->x

netField_t entityStateFields[] = {
    {NETF(pos.trTime), 32},
    {NETF(pos.trBase[0]), 0},
    {NETF(pos.trBase[1]), 0},
    {NETF(pos.trDelta[0]), 0},
    {NETF(pos.trDelta[1]), 0},
    {NETF(pos.trBase[2]), 0},
    {NETF(apos.trBase[1]), 0},
    {NETF(pos.trDelta[2]), 0},
    {NETF(apos.trBase[0]), 0},
    {NETF(event), 10},
    {NETF(angles2[1]), 0},
    {NETF(eType), 8},
    {NETF(torsoAnim), 8},
    {NETF(weaponAnim), 8},
    {NETF(eventParm), 8},
    {NETF(legsAnim), 8},
    {NETF(groundEntityNum), GENTITYNUM_BITS},
    {NETF(pos.trType), 8},
    {NETF(eFlags), 19},
    {NETF(otherEntityNum), GENTITYNUM_BITS},
    {NETF(weapon), 8},
    {NETF(clientNum), 8},
    {NETF(angles[1]), 0},
    {NETF(pos.trDuration), 32},
    {NETF(apos.trType), 8},
    {NETF(origin[0]), 0},
    {NETF(origin[1]), 0},
    {NETF(origin[2]), 0},
    {NETF(solid), 24},
    {NETF(misc), MAX_MISC},
    {NETF(modelindex), 8},
    {NETF(otherEntityNum2), GENTITYNUM_BITS},
    {NETF(loopSound), 8},
    {NETF(generic1), 10},
    {NETF(origin2[2]), 0},
    {NETF(origin2[0]), 0},
    {NETF(origin2[1]), 0},
    {NETF(modelindex2), 8},
    {NETF(angles[0]), 0},
    {NETF(time), 32},
    {NETF(apos.trTime), 32},
    {NETF(apos.trDuration), 32},
    {NETF(apos.trBase[2]), 0},
    {NETF(apos.trDelta[0]), 0},
    {NETF(apos.trDelta[1]), 0},
    {NETF(apos.trDelta[2]), 0},
    {NETF(time2), 32},
    {NETF(angles[2]), 0},
    {NETF(angles2[0]), 0},
    {NETF(angles2[2]), 0},
    {NETF(constantLight), 32},
    {NETF(frame), 16}
};

// if (int)f == f and (int)f + ( 1<<(FLOAT_INT_BITS-1) ) < ( 1 << FLOAT_INT_BITS )
// the float will be sent with FLOAT_INT_BITS, otherwise all 32 bits will be sent
#define FLOAT_INT_BITS 13
#define FLOAT_INT_BIAS (1 << (FLOAT_INT_BITS - 1))

/*
==================
MSG_WriteDeltaEntity

Writes part of a packetentities message, including the entity number.
Can delta from either a baseline or a previous packet_entity
If to is NULL, a remove entity update will be sent
If force is not set, then nothing at all will be generated if the entity is
identical, under the assumption that the in-order delta code will catch it.
==================
*/
void MSG_WriteDeltaEntity(int alternateProtocol, msg_t *msg, struct entityState_s *from, struct entityState_s *to, bool force)
{
    int i, lc;
    int numFields;
    netField_t *field;
    int trunc;
    float fullFloat;
    int *fromF, *toF;

    numFields = ARRAY_LEN(entityStateFields);

    // all fields should be 32 bits to avoid any compiler packing issues
    // the "number" field is not part of the field list
    // if this assert fails, someone added a field to the entityState_t
    // struct without updating the message fields
    assert(numFields + 1 == sizeof(*from) / 4);

    // a NULL to is a delta remove message
    if (to == NULL)
    {
        if (from == NULL)
        {
            return;
        }
        MSG_WriteBits(msg, from->number, GENTITYNUM_BITS);
        MSG_WriteBits(msg, 1, 1);
        return;
    }

    if (to->number < 0 || to->number >= MAX_GENTITIES)
    {
        Com_Error(ERR_FATAL, "MSG_WriteDeltaEntity: Bad entity number: %i", to->number);
    }

    lc = 0;
    // build the change vector as bytes so it is endien independent
    for (i = 0, field = entityStateFields; i < numFields; i++, field++)
    {
        if (alternateProtocol == 2 && i == 13)
        {
            continue;
        }
        fromF = (int *)((uint8_t *)from + field->offset);
        toF = (int *)((uint8_t *)to + field->offset);
        if (*fromF != *toF)
        {
            lc = i + 1;
        }
    }

    if (lc == 0)
    {
        // nothing at all changed
        if (!force)
        {
            return;  // nothing at all
        }
        // write two bits for no change
        MSG_WriteBits(msg, to->number, GENTITYNUM_BITS);
        MSG_WriteBits(msg, 0, 1);  // not removed
        MSG_WriteBits(msg, 0, 1);  // no delta
        return;
    }

    MSG_WriteBits(msg, to->number, GENTITYNUM_BITS);
    MSG_WriteBits(msg, 0, 1);  // not removed
    MSG_WriteBits(msg, 1, 1);  // we have a delta

    if (alternateProtocol == 2 && lc - 1 > 13)
    {
        MSG_WriteByte(msg, lc - 1);  // # of changes
    }
    else
    {
        MSG_WriteByte(msg, lc);  // # of changes
    }

    oldsize += numFields;

    for (i = 0, field = entityStateFields; i < lc; i++, field++)
    {
        if (alternateProtocol == 2 && i == 13)
        {
            continue;
        }

        fromF = (int *)((uint8_t *)from + field->offset);
        toF = (int *)((uint8_t *)to + field->offset);

        if (*fromF == *toF)
        {
            MSG_WriteBits(msg, 0, 1);  // no change
            continue;
        }

        MSG_WriteBits(msg, 1, 1);  // changed

        if (field->bits == 0)
        {
            // float
            fullFloat = *(float *)toF;
            trunc = (int)fullFloat;

            if (fullFloat == 0.0f)
            {
                MSG_WriteBits(msg, 0, 1);
                oldsize += FLOAT_INT_BITS;
            }
            else
            {
                MSG_WriteBits(msg, 1, 1);
                if (trunc == fullFloat && trunc + FLOAT_INT_BIAS >= 0 && trunc + FLOAT_INT_BIAS < (1 << FLOAT_INT_BITS))
                {
                    // send as small integer
                    MSG_WriteBits(msg, 0, 1);
                    MSG_WriteBits(msg, trunc + FLOAT_INT_BIAS, FLOAT_INT_BITS);
                }
                else
                {
                    // send as full floating point value
                    MSG_WriteBits(msg, 1, 1);
                    MSG_WriteBits(msg, *toF, 32);
                }
            }
        }
        else
        {
            if (*toF == 0)
            {
                MSG_WriteBits(msg, 0, 1);
            }
            else
            {
                MSG_WriteBits(msg, 1, 1);
                // integer
                if (alternateProtocol == 2 && i == 33)
                {
                    MSG_WriteBits(msg, *toF, 8);
                }
                else
                {
                    MSG_WriteBits(msg, *toF, field->bits);
                }
            }
        }
    }
}

/*
==================
MSG_ReadDeltaEntity

The entity number has already been read from the message, which
is how the from state is identified.

If the delta removes the entity, entityState_t->number will be set to MAX_GENTITIES-1

Can go from either a baseline or a previous packet_entity
==================
*/
void MSG_ReadDeltaEntity(int alternateProtocol, msg_t *msg, entityState_t *from, entityState_t *to, int number)
{
    int i, lc;
    int numFields;
    netField_t *field;
    int *fromF, *toF;
    int print;
    int trunc;
    int startBit, endBit;

    if (number < 0 || number >= MAX_GENTITIES)
    {
        Com_Error(ERR_DROP, "Bad delta entity number: %i", number);
    }

    if (msg->bit == 0)
    {
        startBit = msg->readcount * 8 - GENTITYNUM_BITS;
    }
    else
    {
        startBit = (msg->readcount - 1) * 8 + msg->bit - GENTITYNUM_BITS;
    }

    // check for a remove
    if (MSG_ReadBits(msg, 1) == 1)
    {
        ::memset(to, 0, sizeof(*to));
        to->number = MAX_GENTITIES - 1;
        if (cl_shownet && (cl_shownet->integer >= 2 || cl_shownet->integer == -1))
        {
            Com_Printf("%3i: #%-3i remove\n", msg->readcount, number);
        }
        return;
    }

    // check for no delta
    if (MSG_ReadBits(msg, 1) == 0)
    {
        *to = *from;
        to->number = number;
        return;
    }

    numFields = ARRAY_LEN(entityStateFields);
    lc = MSG_ReadByte(msg);
    if (alternateProtocol == 2 && lc - 1 >= 13)
    {
        ++lc;
    }

    if (lc > numFields || lc < 0)
    {
        Com_Error(ERR_DROP, "invalid entityState field count");
    }

    // shownet 2/3 will interleave with other printed info, -1 will
    // just print the delta records`
    if (cl_shownet && (cl_shownet->integer >= 2 || cl_shownet->integer == -1))
    {
        print = 1;
        Com_Printf("%3i: #%-3i ", msg->readcount, to->number);
    }
    else
    {
        print = 0;
    }

    to->number = number;

    for (i = 0, field = entityStateFields; i < lc; i++, field++)
    {
        fromF = (int *)((uint8_t *)from + field->offset);
        toF = (int *)((uint8_t *)to + field->offset);
        if (alternateProtocol == 2 && i == 13)
        {
            *toF = 0;
            continue;
        }

        if (!MSG_ReadBits(msg, 1))
        {
            // no change
            *toF = *fromF;
        }
        else
        {
            if (field->bits == 0)
            {
                // float
                if (MSG_ReadBits(msg, 1) == 0)
                {
                    *(float *)toF = 0.0f;
                }
                else
                {
                    if (MSG_ReadBits(msg, 1) == 0)
                    {
                        // integral float
                        trunc = MSG_ReadBits(msg, FLOAT_INT_BITS);
                        // bias to allow equal parts positive and negative
                        trunc -= FLOAT_INT_BIAS;
                        *(float *)toF = trunc;
                        if (print)
                        {
                            Com_Printf("%s:%i ", field->name, trunc);
                        }
                    }
                    else
                    {
                        // full floating point value
                        *toF = MSG_ReadBits(msg, 32);
                        if (print)
                        {
                            Com_Printf("%s:%f ", field->name, *(float *)toF);
                        }
                    }
                }
            }
            else
            {
                if (MSG_ReadBits(msg, 1) == 0)
                {
                    *toF = 0;
                }
                else
                {
                    // integer
                    if (alternateProtocol == 2 && i == 33)
                    {
                        *toF = MSG_ReadBits(msg, 8);
                    }
                    else
                    {
                        *toF = MSG_ReadBits(msg, field->bits);
                    }
                    if (print)
                    {
                        Com_Printf("%s:%i ", field->name, *toF);
                    }
                }
            }
            //			pcount[i]++;
        }
    }
    for (i = lc, field = &entityStateFields[lc]; i < numFields; i++, field++)
    {
        fromF = (int *)((uint8_t *)from + field->offset);
        toF = (int *)((uint8_t *)to + field->offset);
        // no change
        *toF = *fromF;
    }

    if (print)
    {
        if (msg->bit == 0)
        {
            endBit = msg->readcount * 8 - GENTITYNUM_BITS;
        }
        else
        {
            endBit = (msg->readcount - 1) * 8 + msg->bit - GENTITYNUM_BITS;
        }
        Com_Printf(" (%i bits)\n", endBit - startBit);
    }
}

/*
============================================================================

plyer_state_t communication

============================================================================
*/

// using the stringizing operator to save typing...
#define PSF(x) #x, (size_t) & ((playerState_t *) 0)->x

netField_t playerStateFields[] = {
    {PSF(commandTime), 32},
    {PSF(origin[0]), 0},
    {PSF(origin[1]), 0},
    {PSF(bobCycle), 8},
    {PSF(velocity[0]), 0},
    {PSF(velocity[1]), 0},
    {PSF(viewangles[1]), 0},
    {PSF(viewangles[0]), 0},
    {PSF(weaponTime), -16},
    {PSF(origin[2]), 0},
    {PSF(velocity[2]), 0},
    {PSF(legsTimer), 8},
    {PSF(pm_time), -16},
    {PSF(eventSequence), 16},
    {PSF(torsoAnim), 8},
    {PSF(weaponAnim), 8},
    {PSF(movementDir), 4},
    {PSF(events[0]), 8},
    {PSF(legsAnim), 8},
    {PSF(events[1]), 8},
    {PSF(pm_flags), 24},
    {PSF(groundEntityNum), GENTITYNUM_BITS},
    {PSF(weaponstate), 4},
    {PSF(eFlags), 16},
    {PSF(externalEvent), 10},
    {PSF(gravity), -16},
    {PSF(speed), -16},
    {PSF(delta_angles[1]), 16},
    {PSF(externalEventParm), 8},
    {PSF(viewheight), -8},
    {PSF(damageEvent), 8},
    {PSF(damageYaw), 8},
    {PSF(damagePitch), 8},
    {PSF(damageCount), 8},
    {PSF(ammo), 12},
    {PSF(clips), 4},
    {PSF(generic1), 10},
    {PSF(pm_type), 8},
    {PSF(delta_angles[0]), 16},
    {PSF(delta_angles[2]), 16},
    {PSF(torsoTimer), 12},
    {PSF(tauntTimer), 12},
    {PSF(eventParms[0]), 8},
    {PSF(eventParms[1]), 8},
    {PSF(clientNum), 8},
    {PSF(weapon), 5},
    {PSF(viewangles[2]), 0},
    {PSF(grapplePoint[0]), 0},
    {PSF(grapplePoint[1]), 0},
    {PSF(grapplePoint[2]), 0},
    {PSF(otherEntityNum), GENTITYNUM_BITS},
    {PSF(loopSound), 16}
};

#define APSF(x) #x, (size_t) & ((alternatePlayerState_t *) 0)->x

netField_t alternatePlayerStateFields[] = {
    {APSF(commandTime), 32},
    {APSF(origin[0]), 0},
    {APSF(origin[1]), 0},
    {APSF(bobCycle), 8},
    {APSF(velocity[0]), 0},
    {APSF(velocity[1]), 0},
    {APSF(viewangles[1]), 0},
    {APSF(viewangles[0]), 0},
    {APSF(weaponTime), -16},
    {APSF(origin[2]), 0},
    {APSF(velocity[2]), 0},
    {APSF(legsTimer), 8},
    {APSF(pm_time), -16},
    {APSF(eventSequence), 16},
    {APSF(torsoAnim), 8},
    {APSF(movementDir), 4},
    {APSF(events[0]), 8},
    {APSF(legsAnim), 8},
    {APSF(events[1]), 8},
    {APSF(pm_flags), 16},
    {APSF(groundEntityNum), GENTITYNUM_BITS},
    {APSF(weaponstate), 4},
    {APSF(eFlags), 16},
    {APSF(externalEvent), 10},
    {APSF(gravity), -16},
    {APSF(speed), -16},
    {APSF(delta_angles[1]), 16},
    {APSF(externalEventParm), 8},
    {APSF(viewheight), -8},
    {APSF(damageEvent), 8},
    {APSF(damageYaw), 8},
    {APSF(damagePitch), 8},
    {APSF(damageCount), 8},
    {APSF(generic1), 8},
    {APSF(pm_type), 8},
    {APSF(delta_angles[0]), 16},
    {APSF(delta_angles[2]), 16},
    {APSF(torsoTimer), 12},
    {APSF(eventParms[0]), 8},
    {APSF(eventParms[1]), 8},
    {APSF(clientNum), 8},
    {APSF(weapon), 5},
    {APSF(viewangles[2]), 0},
    {APSF(grapplePoint[0]), 0},
    {APSF(grapplePoint[1]), 0},
    {APSF(grapplePoint[2]), 0},
    {APSF(otherEntityNum), GENTITYNUM_BITS},
    {APSF(loopSound), 16}
};

/*
=============
MSG_WriteDeltaPlayerstate

=============
*/
void MSG_WriteDeltaPlayerstate(int alternateProtocol, msg_t *msg, struct playerState_s *from, struct playerState_s *to)
{
    int i;
    playerState_t dummy;
    int statsbits;
    int persistantbits;
    int altFromAmmo[3];
    int altToAmmo[3];
    int ammobits;
    int miscbits;
    int numFields;
    netField_t *field;
    int *fromF, *toF;
    float fullFloat;
    int trunc, lc;

    if (!from)
    {
        from = &dummy;
        ::memset(&dummy, 0, sizeof(dummy));
    }

    numFields = ARRAY_LEN(playerStateFields);

    lc = 0;
    for (i = 0, field = playerStateFields; i < numFields; i++, field++)
    {
        if (alternateProtocol == 2 && (i == 15 || i == 34 || i == 35 || i == 41))
        {
            continue;
        }
        fromF = (int *)((uint8_t *)from + field->offset);
        toF = (int *)((uint8_t *)to + field->offset);
        if (*fromF != *toF)
        {
            lc = i + 1;
        }
    }

    if (alternateProtocol == 2)
    {
        if (lc - 1 > 41)
        {
            MSG_WriteByte(msg, lc - 4);  // # of changes
        }
        else if (lc - 1 > 35)
        {
            MSG_WriteByte(msg, lc - 3);  // # of changes
        }
        else if (lc - 1 > 34)
        {
            MSG_WriteByte(msg, lc - 2);  // # of changes
        }
        else if (lc - 1 > 15)
        {
            MSG_WriteByte(msg, lc - 1);  // # of changes
        }
        else
        {
            MSG_WriteByte(msg, lc);  // # of changes
        }
    }
    else
    {
        MSG_WriteByte(msg, lc);  // # of changes
    }

    oldsize += numFields - lc;

    for (i = 0, field = playerStateFields; i < lc; i++, field++)
    {
        if (alternateProtocol == 2 && (i == 15 || i == 34 || i == 35 || i == 41))
        {
            continue;
        }

        fromF = (int *)((uint8_t *)from + field->offset);
        toF = (int *)((uint8_t *)to + field->offset);

        if (*fromF == *toF)
        {
            MSG_WriteBits(msg, 0, 1);  // no change
            continue;
        }

        MSG_WriteBits(msg, 1, 1);  // changed
        //		pcount[i]++;

        if (field->bits == 0)
        {
            // float
            fullFloat = *(float *)toF;
            trunc = (int)fullFloat;

            if (trunc == fullFloat && trunc + FLOAT_INT_BIAS >= 0 && trunc + FLOAT_INT_BIAS < (1 << FLOAT_INT_BITS))
            {
                // send as small integer
                MSG_WriteBits(msg, 0, 1);
                MSG_WriteBits(msg, trunc + FLOAT_INT_BIAS, FLOAT_INT_BITS);
            }
            else
            {
                // send as full floating point value
                MSG_WriteBits(msg, 1, 1);
                MSG_WriteBits(msg, *toF, 32);
            }
        }
        else
        {
            // integer
            if (alternateProtocol == 2)
            {
                if (i == 20)
                {
                    MSG_WriteBits(msg, *toF, 16);
                }
                else if (i == 36)
                {
                    MSG_WriteBits(msg, *toF, 8);
                }
                else
                {
                    MSG_WriteBits(msg, *toF, field->bits);
                }
            }
            else
            {
                MSG_WriteBits(msg, *toF, field->bits);
            }
        }
    }

    //
    // send the arrays
    //
    statsbits = 0;
    for (i = 0; i < MAX_STATS; i++)
    {
        if (to->stats[i] != from->stats[i])
        {
            statsbits |= 1 << i;
        }
    }
    persistantbits = 0;
    for (i = 0; i < MAX_PERSISTANT; i++)
    {
        if (to->persistant[i] != from->persistant[i])
        {
            persistantbits |= 1 << i;
        }
    }
    if (alternateProtocol == 2)
    {
        altFromAmmo[0] = (from->weaponAnim & 0xFF) | ((from->pm_flags >> 8) & 0xFF00);
        altFromAmmo[1] = (from->ammo & 0xFFF) | ((from->clips << 12) & 0xF000);
        altFromAmmo[2] = (from->tauntTimer & 0xFFF) | ((from->generic1 << 4) & 0x3000);
        altToAmmo[0] = (to->weaponAnim & 0xFF) | ((to->pm_flags >> 8) & 0xFF00);
        altToAmmo[1] = (to->ammo & 0xFFF) | ((to->clips << 12) & 0xF000);
        altToAmmo[2] = (to->tauntTimer & 0xFFF) | ((to->generic1 << 4) & 0x3000);
        ammobits = 0;
        for (i = 0; i < 3; i++)
        {
            if (altToAmmo[i] != altFromAmmo[i])
            {
                ammobits |= 1 << i;
            }
        }
    }
    else
    {
        ammobits = 0;
    }
    miscbits = 0;
    for (i = 0; i < MAX_MISC; i++)
    {
        if (to->misc[i] != from->misc[i])
        {
            miscbits |= 1 << i;
        }
    }

    if (!statsbits && !persistantbits && !ammobits && !miscbits)
    {
        MSG_WriteBits(msg, 0, 1);  // no change
        oldsize += 4;
        return;
    }
    MSG_WriteBits(msg, 1, 1);  // changed

    if (statsbits)
    {
        MSG_WriteBits(msg, 1, 1);  // changed
        MSG_WriteBits(msg, statsbits, MAX_STATS);
        for (i = 0; i < MAX_STATS; i++)
            if (statsbits & (1 << i)) MSG_WriteShort(msg, to->stats[i]);
    }
    else
    {
        MSG_WriteBits(msg, 0, 1);  // no change
    }

    if (persistantbits)
    {
        MSG_WriteBits(msg, 1, 1);  // changed
        MSG_WriteBits(msg, persistantbits, MAX_PERSISTANT);
        for (i = 0; i < MAX_PERSISTANT; i++)
            if (persistantbits & (1 << i)) MSG_WriteShort(msg, to->persistant[i]);
    }
    else
    {
        MSG_WriteBits(msg, 0, 1);  // no change
    }

    if (alternateProtocol == 2)
    {
        if (ammobits)
        {
            MSG_WriteBits(msg, 1, 1);  // changed
            MSG_WriteBits(msg, ammobits, 16);
            for (i = 0; i < 3; i++)
                if (ammobits & (1 << i)) MSG_WriteShort(msg, altToAmmo[i]);
        }
        else
        {
            MSG_WriteBits(msg, 0, 1);  // no change
        }
    }

    if (miscbits)
    {
        MSG_WriteBits(msg, 1, 1);  // changed
        MSG_WriteBits(msg, miscbits, MAX_MISC);
        for (i = 0; i < MAX_MISC; i++)
            if (miscbits & (1 << i)) MSG_WriteLong(msg, to->misc[i]);
    }
    else
    {
        MSG_WriteBits(msg, 0, 1);  // no change
    }
}

/*
===================
MSG_ReadDeltaPlayerstate
===================
*/
void MSG_ReadDeltaPlayerstate(msg_t *msg, playerState_t *from, playerState_t *to)
{
    int i, lc;
    int bits;
    netField_t *field;
    int numFields;
    int startBit, endBit;
    int print;
    int *fromF, *toF;
    int trunc;
    playerState_t dummy;

    if (!from)
    {
        from = &dummy;
        ::memset(&dummy, 0, sizeof(dummy));
    }
    *to = *from;

    if (msg->bit == 0)
    {
        startBit = msg->readcount * 8 - GENTITYNUM_BITS;
    }
    else
    {
        startBit = (msg->readcount - 1) * 8 + msg->bit - GENTITYNUM_BITS;
    }

    // shownet 2/3 will interleave with other printed info, -2 will
    // just print the delta records
    if (cl_shownet && (cl_shownet->integer >= 2 || cl_shownet->integer == -2))
    {
        print = 1;
        Com_Printf("%3i: playerstate ", msg->readcount);
    }
    else
    {
        print = 0;
    }

    numFields = ARRAY_LEN(playerStateFields);
    lc = MSG_ReadByte(msg);

    if (lc > numFields || lc < 0)
    {
        Com_Error(ERR_DROP, "invalid playerState field count");
    }

    for (i = 0, field = playerStateFields; i < lc; i++, field++)
    {
        fromF = (int *)((uint8_t *)from + field->offset);
        toF = (int *)((uint8_t *)to + field->offset);

        if (!MSG_ReadBits(msg, 1))
        {
            // no change
            *toF = *fromF;
        }
        else
        {
            if (field->bits == 0)
            {
                // float
                if (MSG_ReadBits(msg, 1) == 0)
                {
                    // integral float
                    trunc = MSG_ReadBits(msg, FLOAT_INT_BITS);
                    // bias to allow equal parts positive and negative
                    trunc -= FLOAT_INT_BIAS;
                    *(float *)toF = trunc;
                    if (print)
                    {
                        Com_Printf("%s:%i ", field->name, trunc);
                    }
                }
                else
                {
                    // full floating point value
                    *toF = MSG_ReadBits(msg, 32);
                    if (print)
                    {
                        Com_Printf("%s:%f ", field->name, *(float *)toF);
                    }
                }
            }
            else
            {
                // integer
                *toF = MSG_ReadBits(msg, field->bits);
                if (print)
                {
                    Com_Printf("%s:%i ", field->name, *toF);
                }
            }
        }
    }
    for (i = lc, field = &playerStateFields[lc]; i < numFields; i++, field++)
    {
        fromF = (int *)((uint8_t *)from + field->offset);
        toF = (int *)((uint8_t *)to + field->offset);
        // no change
        *toF = *fromF;
    }

    // read the arrays
    if (MSG_ReadBits(msg, 1))
    {
        // parse stats
        if (MSG_ReadBits(msg, 1))
        {
            LOG("PS_STATS");
            bits = MSG_ReadBits(msg, MAX_STATS);
            for (i = 0; i < MAX_STATS; i++)
            {
                if (bits & (1 << i))
                {
                    to->stats[i] = MSG_ReadShort(msg);
                }
            }
        }

        // parse persistant stats
        if (MSG_ReadBits(msg, 1))
        {
            LOG("PS_PERSISTANT");
            bits = MSG_ReadBits(msg, MAX_PERSISTANT);
            for (i = 0; i < MAX_PERSISTANT; i++)
            {
                if (bits & (1 << i))
                {
                    to->persistant[i] = MSG_ReadShort(msg);
                }
            }
        }

        // parse misc data
        if (MSG_ReadBits(msg, 1))
        {
            LOG("PS_MISC");
            bits = MSG_ReadBits(msg, MAX_MISC);
            for (i = 0; i < MAX_MISC; i++)
            {
                if (bits & (1 << i))
                {
                    to->misc[i] = MSG_ReadLong(msg);
                }
            }
        }
    }

    if (print)
    {
        if (msg->bit == 0)
        {
            endBit = msg->readcount * 8 - GENTITYNUM_BITS;
        }
        else
        {
            endBit = (msg->readcount - 1) * 8 + msg->bit - GENTITYNUM_BITS;
        }
        Com_Printf(" (%i bits)\n", endBit - startBit);
    }
}

void MSG_ReadDeltaAlternatePlayerstate(msg_t *msg, alternatePlayerState_t *from, alternatePlayerState_t *to)
{
    int i, lc;
    int bits;
    netField_t *field;
    int numFields;
    int startBit, endBit;
    int print;
    int *fromF, *toF;
    int trunc;
    alternatePlayerState_t dummy;

    if (!from)
    {
        from = &dummy;
        ::memset(&dummy, 0, sizeof(dummy));
    }
    *to = *from;

    if (msg->bit == 0)
    {
        startBit = msg->readcount * 8 - GENTITYNUM_BITS;
    }
    else
    {
        startBit = (msg->readcount - 1) * 8 + msg->bit - GENTITYNUM_BITS;
    }

    // shownet 2/3 will interleave with other printed info, -2 will
    // just print the delta records
    if (cl_shownet && (cl_shownet->integer >= 2 || cl_shownet->integer == -2))
    {
        print = 1;
        Com_Printf("%3i: playerstate ", msg->readcount);
    }
    else
    {
        print = 0;
    }

    numFields = ARRAY_LEN(alternatePlayerStateFields);
    lc = MSG_ReadByte(msg);

    if (lc > numFields || lc < 0)
    {
        Com_Error(ERR_DROP, "invalid playerState field count");
    }

    for (i = 0, field = alternatePlayerStateFields; i < lc; i++, field++)
    {
        fromF = (int *)((uint8_t *)from + field->offset);
        toF = (int *)((uint8_t *)to + field->offset);

        if (!MSG_ReadBits(msg, 1))
        {
            // no change
            *toF = *fromF;
        }
        else
        {
            if (field->bits == 0)
            {
                // float
                if (MSG_ReadBits(msg, 1) == 0)
                {
                    // integral float
                    trunc = MSG_ReadBits(msg, FLOAT_INT_BITS);
                    // bias to allow equal parts positive and negative
                    trunc -= FLOAT_INT_BIAS;
                    *(float *)toF = trunc;
                    if (print)
                    {
                        Com_Printf("%s:%i ", field->name, trunc);
                    }
                }
                else
                {
                    // full floating point value
                    *toF = MSG_ReadBits(msg, 32);
                    if (print)
                    {
                        Com_Printf("%s:%f ", field->name, *(float *)toF);
                    }
                }
            }
            else
            {
                // integer
                *toF = MSG_ReadBits(msg, field->bits);
                if (print)
                {
                    Com_Printf("%s:%i ", field->name, *toF);
                }
            }
        }
    }
    for (i = lc, field = &alternatePlayerStateFields[lc]; i < numFields; i++, field++)
    {
        fromF = (int *)((uint8_t *)from + field->offset);
        toF = (int *)((uint8_t *)to + field->offset);
        // no change
        *toF = *fromF;
    }

    // read the arrays
    if (MSG_ReadBits(msg, 1))
    {
        // parse stats
        if (MSG_ReadBits(msg, 1))
        {
            LOG("PS_STATS");
            bits = MSG_ReadBits(msg, MAX_STATS);
            for (i = 0; i < MAX_STATS; i++)
            {
                if (bits & (1 << i))
                {
                    to->stats[i] = MSG_ReadShort(msg);
                }
            }
        }

        // parse persistant stats
        if (MSG_ReadBits(msg, 1))
        {
            LOG("PS_PERSISTANT");
            bits = MSG_ReadBits(msg, MAX_PERSISTANT);
            for (i = 0; i < MAX_PERSISTANT; i++)
            {
                if (bits & (1 << i))
                {
                    to->persistant[i] = MSG_ReadShort(msg);
                }
            }
        }

        // parse ammo
        if (MSG_ReadBits(msg, 1))
        {
            LOG("PS_AMMO");
            bits = MSG_ReadBits(msg, MAX_WEAPONS);
            for (i = 0; i < MAX_WEAPONS; i++)
            {
                if (bits & (1 << i))
                {
                    to->ammo[i] = MSG_ReadShort(msg);
                }
            }
        }

        // parse misc data
        if (MSG_ReadBits(msg, 1))
        {
            LOG("PS_MISC");
            bits = MSG_ReadBits(msg, MAX_MISC);
            for (i = 0; i < MAX_MISC; i++)
            {
                if (bits & (1 << i))
                {
                    to->misc[i] = MSG_ReadLong(msg);
                }
            }
        }
    }

    if (print)
    {
        if (msg->bit == 0)
        {
            endBit = msg->readcount * 8 - GENTITYNUM_BITS;
        }
        else
        {
            endBit = (msg->readcount - 1) * 8 + msg->bit - GENTITYNUM_BITS;
        }
        Com_Printf(" (%i bits)\n", endBit - startBit);
    }
}

int msg_hData[256] = {
    250315,  // 0
    41193,  // 1
    6292,  // 2
    7106,  // 3
    3730,  // 4
    3750,  // 5
    6110,  // 6
    23283,  // 7
    33317,  // 8
    6950,  // 9
    7838,  // 10
    9714,  // 11
    9257,  // 12
    17259,  // 13
    3949,  // 14
    1778,  // 15
    8288,  // 16
    1604,  // 17
    1590,  // 18
    1663,  // 19
    1100,  // 20
    1213,  // 21
    1238,  // 22
    1134,  // 23
    1749,  // 24
    1059,  // 25
    1246,  // 26
    1149,  // 27
    1273,  // 28
    4486,  // 29
    2805,  // 30
    3472,  // 31
    21819,  // 32
    1159,  // 33
    1670,  // 34
    1066,  // 35
    1043,  // 36
    1012,  // 37
    1053,  // 38
    1070,  // 39
    1726,  // 40
    888,  // 41
    1180,  // 42
    850,  // 43
    960,  // 44
    780,  // 45
    1752,  // 46
    3296,  // 47
    10630,  // 48
    4514,  // 49
    5881,  // 50
    2685,  // 51
    4650,  // 52
    3837,  // 53
    2093,  // 54
    1867,  // 55
    2584,  // 56
    1949,  // 57
    1972,  // 58
    940,  // 59
    1134,  // 60
    1788,  // 61
    1670,  // 62
    1206,  // 63
    5719,  // 64
    6128,  // 65
    7222,  // 66
    6654,  // 67
    3710,  // 68
    3795,  // 69
    1492,  // 70
    1524,  // 71
    2215,  // 72
    1140,  // 73
    1355,  // 74
    971,  // 75
    2180,  // 76
    1248,  // 77
    1328,  // 78
    1195,  // 79
    1770,  // 80
    1078,  // 81
    1264,  // 82
    1266,  // 83
    1168,  // 84
    965,  // 85
    1155,  // 86
    1186,  // 87
    1347,  // 88
    1228,  // 89
    1529,  // 90
    1600,  // 91
    2617,  // 92
    2048,  // 93
    2546,  // 94
    3275,  // 95
    2410,  // 96
    3585,  // 97
    2504,  // 98
    2800,  // 99
    2675,  // 100
    6146,  // 101
    3663,  // 102
    2840,  // 103
    14253,  // 104
    3164,  // 105
    2221,  // 106
    1687,  // 107
    3208,  // 108
    2739,  // 109
    3512,  // 110
    4796,  // 111
    4091,  // 112
    3515,  // 113
    5288,  // 114
    4016,  // 115
    7937,  // 116
    6031,  // 117
    5360,  // 118
    3924,  // 119
    4892,  // 120
    3743,  // 121
    4566,  // 122
    4807,  // 123
    5852,  // 124
    6400,  // 125
    6225,  // 126
    8291,  // 127
    23243,  // 128
    7838,  // 129
    7073,  // 130
    8935,  // 131
    5437,  // 132
    4483,  // 133
    3641,  // 134
    5256,  // 135
    5312,  // 136
    5328,  // 137
    5370,  // 138
    3492,  // 139
    2458,  // 140
    1694,  // 141
    1821,  // 142
    2121,  // 143
    1916,  // 144
    1149,  // 145
    1516,  // 146
    1367,  // 147
    1236,  // 148
    1029,  // 149
    1258,  // 150
    1104,  // 151
    1245,  // 152
    1006,  // 153
    1149,  // 154
    1025,  // 155
    1241,  // 156
    952,  // 157
    1287,  // 158
    997,  // 159
    1713,  // 160
    1009,  // 161
    1187,  // 162
    879,  // 163
    1099,  // 164
    929,  // 165
    1078,  // 166
    951,  // 167
    1656,  // 168
    930,  // 169
    1153,  // 170
    1030,  // 171
    1262,  // 172
    1062,  // 173
    1214,  // 174
    1060,  // 175
    1621,  // 176
    930,  // 177
    1106,  // 178
    912,  // 179
    1034,  // 180
    892,  // 181
    1158,  // 182
    990,  // 183
    1175,  // 184
    850,  // 185
    1121,  // 186
    903,  // 187
    1087,  // 188
    920,  // 189
    1144,  // 190
    1056,  // 191
    3462,  // 192
    2240,  // 193
    4397,  // 194
    12136,  // 195
    7758,  // 196
    1345,  // 197
    1307,  // 198
    3278,  // 199
    1950,  // 200
    886,  // 201
    1023,  // 202
    1112,  // 203
    1077,  // 204
    1042,  // 205
    1061,  // 206
    1071,  // 207
    1484,  // 208
    1001,  // 209
    1096,  // 210
    915,  // 211
    1052,  // 212
    995,  // 213
    1070,  // 214
    876,  // 215
    1111,  // 216
    851,  // 217
    1059,  // 218
    805,  // 219
    1112,  // 220
    923,  // 221
    1103,  // 222
    817,  // 223
    1899,  // 224
    1872,  // 225
    976,  // 226
    841,  // 227
    1127,  // 228
    956,  // 229
    1159,  // 230
    950,  // 231
    7791,  // 232
    954,  // 233
    1289,  // 234
    933,  // 235
    1127,  // 236
    3207,  // 237
    1020,  // 238
    927,  // 239
    1355,  // 240
    768,  // 241
    1040,  // 242
    745,  // 243
    952,  // 244
    805,  // 245
    1073,  // 246
    740,  // 247
    1013,  // 248
    805,  // 249
    1008,  // 250
    796,  // 251
    996,  // 252
    1057,  // 253
    11457,  // 254
    13504,  // 255
};

void MSG_initHuffman(void)
{
    int i, j;

    msgInit = true;
    Huff_Init(&msgHuff);
    for (i = 0; i < 256; i++)
    {
        for (j = 0; j < msg_hData[i]; j++)
        {
            Huff_addRef(&msgHuff.compressor, (uint8_t)i);  // Do update
            Huff_addRef(&msgHuff.decompressor, (uint8_t)i);  // Do update
        }
    }
}

/*
void MSG_NUinitHuffman() {
        uint8_t	*data;
        int		size, i, ch;
        int		array[256];

        msgInit = true;

        Huff_Init(&msgHuff);
        // load it in
        size = FS_ReadFile( "netchan/netchan.bin", (void **)&data );

        for(i=0;i<256;i++) {
                array[i] = 0;
        }
        for(i=0;i<size;i++) {
                ch = data[i];
                Huff_addRef(&msgHuff.compressor,	ch);			// Do update
                Huff_addRef(&msgHuff.decompressor,	ch);			// Do update
                array[ch]++;
        }
        Com_Printf("msg_hData {\n");
        for(i=0;i<256;i++) {
                if (array[i] == 0) {
                        Huff_addRef(&msgHuff.compressor,	i);			// Do update
                        Huff_addRef(&msgHuff.decompressor,	i);			// Do update
                }
                Com_Printf("%d,			// %d\n", array[i], i);
        }
        Com_Printf("};\n");
        FS_FreeFile( data );
        Cbuf_AddText( "condump dump.txt\n" );
}
*/

//===========================================================================
