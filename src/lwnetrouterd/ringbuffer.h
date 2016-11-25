// ringbuffer.h
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

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>

class Ringbuffer
{
 public:
  Ringbuffer(size_t size);
  ~Ringbuffer();
  size_t size() const;
  size_t peek(void *data,size_t len);
  size_t read(void *data,size_t len);
  size_t readSpace() const;
  size_t write(void *data,size_t len);
  size_t writeSpace() const;
  size_t dump(size_t len);

 private:
  uint8_t *ring_buffer;
  size_t ring_size;
  size_t ring_length;
};


#endif  // RINGBUFFER_H
