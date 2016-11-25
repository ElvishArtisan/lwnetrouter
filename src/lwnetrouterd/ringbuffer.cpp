// ringbuffer.cpp
//
// A single-threaded ringbuffer class.
//
// (C) Copyright 2016 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU Library General Public License 
//   version 2 as published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <stdio.h>
#include <string.h>

#include "ringbuffer.h"

Ringbuffer::Ringbuffer(size_t size)
{
  ring_buffer=new uint8_t[size];
  ring_size=size;
  ring_length=0;
}


Ringbuffer::~Ringbuffer()
{
  delete ring_buffer;
}


size_t Ringbuffer::size() const
{
  return ring_size;
}


size_t Ringbuffer::peek(void *data,size_t len)
{
  if(len>readSpace()) {
    len=readSpace();
  }
  memcpy(data,ring_buffer,len);
  return len;
}


size_t Ringbuffer::read(void *data,size_t len)
{
  if(len>readSpace()) {
    len=readSpace();
  }
  memcpy(data,ring_buffer,len);
  memmove(ring_buffer,ring_buffer+len,ring_length-len);
  ring_length-=len;

  return len;
}


size_t Ringbuffer::readSpace() const
{
  return ring_length;
}


size_t Ringbuffer::write(void *data,size_t len)
{
  if(len>writeSpace()) {
    len=writeSpace();
  }
  memcpy(ring_buffer+ring_length,data,len);
  ring_length+=len;

  return len;
}


size_t Ringbuffer::writeSpace() const
{
  return ring_size-ring_length;
}


size_t Ringbuffer::dump(size_t len)
{
  if(len>readSpace()) {
    len=readSpace();
  }
  ring_length-=len;

  return len;
}
