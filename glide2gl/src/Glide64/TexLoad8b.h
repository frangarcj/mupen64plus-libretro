/*
* Glide64 - Glide video plugin for Nintendo 64 emulators.
* Copyright (c) 2002  Dave2001
* Copyright (c) 2003-2009  Sergey 'Gonetz' Lipski
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

//****************************************************************
//
// Glide64 - Glide Plugin for Nintendo 64 emulators
// Project started on December 29th, 2001
//
// Authors:
// Dave2001, original author, founded the project in 2001, left it in 2002
// Gugaman, joined the project in 2002, left it in 2002
// Sergey 'Gonetz' Lipski, joined the project in 2002, main author since fall of 2002
// Hiroshi 'KoolSmoky' Morii, joined the project in 2007
//
//****************************************************************
//
// To modify Glide64:
// * Write your name and (optional)email, commented by your work, so I know who did it, and so that you can find which parts you modified when it comes time to send it to me.
// * Do NOT send me the whole project or file that you modified.  Take out your modified code sections, and tell me where to put them.  If people sent the whole thing, I would have many different versions, but no idea how to combine them all.
//
//****************************************************************
#include <stdint.h>

static INLINE void load8bCI(uint8_t *src, uint8_t *dst, int wid_64, int height, int line, int ext, uint16_t *pal)
{
   uint32_t *src32 = (uint32_t*)src;
   uint32_t *dst32 = (uint32_t *)dst;
   unsigned odd = 0;

   while (height--)
   {
      int width = wid_64;

      while (width--)
      {
         uint32_t abcd = m64p_swap32(src32[odd]);
         uint32_t efgh = m64p_swap32(src32[!odd]);
         uint32_t pix = width + 1;

         ALOWORD(pix) = ror16(*(uint16_t*)((uint8_t*)pal + ((abcd >> 15) & 0x1FE)), 1);
         pix = pix << 16;
         ALOWORD(pix) = ror16(*(uint16_t*)((uint8_t*)pal + ((abcd >> 23) & 0x1FE)), 1);
         *dst32++ = pix;

         ALOWORD(pix) = ror16(*(uint16_t*)((uint8_t*)pal + (2 * (uint16_t)abcd & 0x1FE)), 1);
         pix <<= 16;
         ALOWORD(pix) = ror16(*(uint16_t*)((uint8_t*)pal + ((abcd >> 7) & 0x1FE)), 1);
         *dst32++ = pix;

         ALOWORD(pix) = ror16(*(uint16_t*)((uint8_t*)pal + ((efgh >> 15) & 0x1FE)), 1);
         pix <<= 16;
         ALOWORD(pix) = ror16(*(uint16_t*)((uint8_t*)pal + ((efgh >> 23) & 0x1FE)), 1);
         *dst32++ = pix;

         ALOWORD(pix) = ror16(*(uint16_t*)((uint8_t*)pal + (2 * (uint16_t)efgh & 0x1FE)), 1);
         pix <<= 16;
         ALOWORD(pix) = ror16(*(uint16_t*)((uint8_t*)pal + ((efgh >> 7) & 0x1FE)), 1);
         *dst32++ = pix;

         src32 += 2;
      }

      src32 = (uint32_t*)&src[(line + (uintptr_t)src32 - (uintptr_t)src) & 0x7FF];
      dst32 = (uint32_t*)((char *)dst32 + ext);

      odd ^= 1;
   }
}

static INLINE void load8bIA8(uint8_t *src, uint8_t *dst, int wid_64, int height, int line, int ext, uint16_t *pal)
{
   uint32_t *src32 = (uint32_t *)src;
   uint32_t *dst32 = (uint32_t *)dst;
   unsigned odd = 0;

   while (height--)
   {
      int width = wid_64;

      while (width--)
      {
         uint32_t abcd = m64p_swap32(src32[odd]);
         uint32_t efgh = m64p_swap32(src32[!odd]);
         uint32_t pix  = width + 1;

         ALOWORD(pix) = ror16(*(uint16_t *)((uint8_t*)pal + ((abcd >> 15) & 0x1FE)), 8);
         pix = pix << 16;
         ALOWORD(pix) = ror16(*(uint16_t *)((uint8_t*)pal + ((abcd >> 23) & 0x1FE)), 8);
         *dst32++ = pix;

         ALOWORD(pix) = ror16(*(uint16_t *)((uint8_t*)pal + (2 * (uint16_t)abcd & 0x1FE)), 8);
         pix <<= 16;
         ALOWORD(pix) = ror16(*(uint16_t *)((uint8_t*)pal + ((abcd >> 7) & 0x1FE)), 8);
         *dst32++ = pix;

         ALOWORD(pix) = ror16(*(uint16_t *)((uint8_t*)pal + ((efgh >> 15) & 0x1FE)), 8);
         pix <<= 16;
         ALOWORD(pix) = ror16(*(uint16_t *)((uint8_t*)pal + ((efgh >> 23) & 0x1FE)), 8);
         *dst32++ = pix;

         ALOWORD(pix) = ror16(*(uint16_t *)((uint8_t*)pal + (2 * (uint16_t)efgh & 0x1FE)), 8);
         pix <<= 16;
         ALOWORD(pix) = ror16(*(uint16_t *)((uint8_t*)pal + ((efgh >> 7) & 0x1FE)), 8);
         *dst32++ = pix;

         src32 += 2;
      }

      src32 = (uint32_t *)((uint8_t*)src32 + line);
      dst32 = (uint32_t *)((uint8_t*)dst32 + ext);
      odd ^= 1;
   }
}

static INLINE void load8bIA4(uint8_t *src, uint8_t *dst, int wid_64, int height, int line, int ext)
{
   uint32_t *src32 = (uint32_t *)src;
   uint32_t *dst32 = (uint32_t *)dst;
   unsigned odd = 0;

   while (height--)
   {
      int width = wid_64;

      while(width--)
      {
         uint32_t ab = src32[odd];
         uint32_t cd = src32[!odd];

         *dst32++ = (16 * ab & 0xF0F0F0F0) | ((ab >> 4) & 0xF0F0F0F);
         *dst32++ = (16 * cd & 0xF0F0F0F0) | ((cd >> 4) & 0xF0F0F0F);

         src32 += 2;
      }

      src32 = (uint32_t *)((uint8_t*)src32 + line);
      dst32 = (uint32_t *)((uint8_t*)dst32 + ext);
      odd ^= 1;
   }
}

static INLINE void load8bI(uint8_t *src, uint8_t *dst, int wid_64, int height, int line, int ext)
{
   uint32_t *src32 = (uint32_t *)src;
   uint32_t *dst32 = (uint32_t *)dst;
   unsigned odd = 0;

   while (height--)
   {
      int width = wid_64;

      while (width--)
      {
         *dst32++ = src32[odd];
         *dst32++ = src32[!odd];
         src32 += 2;
      }

      src32 = (uint32_t *)((uint8_t*)src32 + line);
      dst32 = (uint32_t *)((uint8_t*)dst32 + ext);
      odd ^= 1;
   }
}

//****************************************************************
// Size: 1, Format: 2
//

uint32_t Load8bCI (uintptr_t dst, uintptr_t src, int wid_64, int height, int line, int real_width, int tile)
{
  int ext;
  unsigned short *pal;

  if (wid_64 < 1)
     wid_64 = 1;
  if (height < 1)
     height = 1;

  ext = (real_width - (wid_64 << 3));
  pal = (unsigned short*)rdp.pal_8;

  switch (rdp.tlut_mode)
  {
    case 0:
       //palette is not used
      //in tlut DISABLE mode load CI texture as plain intensity texture instead of palette dereference.
      //Thanks to angrylion for the advice
      load8bI ((uint8_t *)src, (uint8_t *)dst, wid_64, height, line, ext);
      return /*(0 << 16) | */GR_TEXFMT_ALPHA_8;
    case 2: //color palette
      ext <<= 1;
      load8bCI ((uint8_t *)src, (uint8_t *)dst, wid_64, height, line, ext, pal);
      return (1 << 16) | GR_TEXFMT_ARGB_1555;
    default: //IA palette
      ext <<= 1;
      load8bIA8 ((uint8_t *)src, (uint8_t *)dst, wid_64, height, line, ext, pal);
      return (1 << 16) | GR_TEXFMT_ALPHA_INTENSITY_88;
  }
}

//****************************************************************
// Size: 1, Format: 3
//
// ** by Gugaman **

uint32_t Load8bIA (uintptr_t dst, uintptr_t src, int wid_64, int height, int line, int real_width, int tile)
{
  int ext;
  if (rdp.tlut_mode != 0)
    return Load8bCI (dst, src, wid_64, height, line, real_width, tile);

  if (wid_64 < 1) wid_64 = 1;
  if (height < 1) height = 1;
  ext = (real_width - (wid_64 << 3));
  load8bIA4 ((uint8_t *)src, (uint8_t *)dst, wid_64, height, line, ext);
  return /*(0 << 16) | */GR_TEXFMT_ALPHA_INTENSITY_44;
} 

//****************************************************************
// Size: 1, Format: 4
//
// ** by Gugaman **

uint32_t Load8bI (uintptr_t dst, uintptr_t src, int wid_64, int height, int line, int real_width, int tile)
{
  int ext;
  if (rdp.tlut_mode != 0)
    return Load8bCI (dst, src, wid_64, height, line, real_width, tile);

  if (wid_64 < 1) wid_64 = 1;
  if (height < 1) height = 1;
  ext = (real_width - (wid_64 << 3));
  load8bI ((uint8_t *)src, (uint8_t *)dst, wid_64, height, line, ext);
  return /*(0 << 16) | */GR_TEXFMT_ALPHA_8;
}
