/*
 *
 * MIT License:
 * Copyright (c) 2011 Adrian McEwen
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * adrianm@mcqn.com 1/1/2011
 */

#ifndef MACAddress_h
#define MACAddress_h

#include <Printable.h>
#include <stdint.h>
// A class to make it easier to handle and pass around IP addresses

class MACAddress : public Printable {
private:
    uint8_t _address[8];  // IPv4 address
    // Access the raw byte array containing the address.  Because this returns a pointer
    // to the internal structure rather than a copy of the address this function should only
    // be used when you know that the usage of the returned uint8_t* will be transient and not
    // stored.
    uint8_t* raw_address() { return _address; };

public:
    // Constructors
    MACAddress();
	MACAddress(uint8_t first_octet, uint8_t second_octet, uint8_t third_octet, uint8_t fourth_octet,uint8_t fifth_octet, uint8_t sixth_octet, uint8_t seventh_octet, uint8_t eigth_octet);
    MACAddress(uint64_t address);
    MACAddress(const uint8_t *address);

    // Overloaded cast operator to allow MACAddress objects to be used where a pointer
    // to a four-byte uint8_t array is expected
    operator uint64_t() { return *((uint64_t*)_address); };
    bool operator==(const MACAddress& addr) { return (*((uint64_t*)_address)) == (*((uint64_t*)addr._address)); };
    bool operator==(const uint8_t* addr);

    // Overloaded index operator to allow getting and setting individual octets of the address
    uint8_t operator[](int index) const { return _address[index]; };
    uint8_t& operator[](int index) { return _address[index]; };

    // Overloaded copy operators to allow initialisation of MACAddress objects from other types
    MACAddress& operator=(const uint8_t *address);
    MACAddress& operator=(uint64_t address);

    virtual size_t printTo(Print& p) const;

    friend class EthernetClass;
    friend class Client;
    friend class Server;
	friend class ZigBeeClass;
};

const MACAddress MACADDR_NONE(0,0,0,0,0,0,0,0);


#endif
