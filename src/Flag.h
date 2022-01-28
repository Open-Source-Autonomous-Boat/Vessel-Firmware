/**
 * @file Flag.h
 * @author Micahel MacDonald (@Michael2MacDonald)
 * @brief This file provides the Flag class which allows the easy creation and manipulation of bit flags
 */


#ifndef FLAG_H_
#define FLAG_H_

#include <Arduino.h>
#include <vector>

// struct BitFlag {
// 	uint32_t* ptr;
// 	uint8_t bit;
// };

// uint32_t* FlagBytes[2];
// Flag BitFlags[16] = {
// 	// Byte 0
// 	{.bytePtr = FlagBytes[0], .bitOffset = 0},
// 	{.bytePtr = FlagBytes[0], .bitOffset = 1},
// 	{.bytePtr = FlagBytes[0], .bitOffset = 2},
// 	{.bytePtr = FlagBytes[0], .bitOffset = 3},
// 	{.bytePtr = FlagBytes[0], .bitOffset = 4},
// 	{.bytePtr = FlagBytes[0], .bitOffset = 5},
// 	{.bytePtr = FlagBytes[0], .bitOffset = 6},
// 	{.bytePtr = FlagBytes[0], .bitOffset = 7},
// 	// Byte 1
// 	{.bytePtr = FlagBytes[1], .bitOffset = 0},
// 	{.bytePtr = FlagBytes[1], .bitOffset = 1},
// 	{.bytePtr = FlagBytes[1], .bitOffset = 2},
// 	{.bytePtr = FlagBytes[1], .bitOffset = 3},
// 	{.bytePtr = FlagBytes[1], .bitOffset = 4},
// 	{.bytePtr = FlagBytes[1], .bitOffset = 5},
// 	{.bytePtr = FlagBytes[1], .bitOffset = 6},
// 	{.bytePtr = FlagBytes[1], .bitOffset = 7},
// };

class Flag {
private:
	// For managed flags we have to use an index because when we remove bytes from the
	// bitfield vector all the memory addresses change and it is easier to deal with an index
	bool managed; // Should '*bytePtr'/'bitOffset' or 'flagId' be used
	uint8_t flagId;
	uint8_t *bytePtr;
	uint8_t bitOffset;

	static std::vector<uint8_t> BitField;
	static std::vector<unsigned int[8]> LUT; // Holds an array of flag byteId's. The vector and array index maps to the byte and bit index. 0 = not used by a flag

	int getFlagByteIndex(unsigned int id) {
		for (unsigned int i=0; i<LUT.size(); i++) { // for each byte
			for (int p=0; p<8; p++) { // for each bit
				if (LUT[i][p] == id) { // if bit is empty
					return i;
				}
			}
		}
		return -1;
	}
	int getFlagBitIndex(unsigned int id) {
		for (unsigned int i=0; i<LUT.size(); i++) { // for each byte
			for (int p=0; p<8; p++) { // for each bit
				if (LUT[i][p] == id) { // if bit is empty
					return p;
				}
			}
		}
		return -1;
	}

	void createManagedFlag() {
		managed = true; // This is a flag managed by the built in management system
		// Find an unused bit in the bitfield to place this flag in
		int byteIndex;
		uint8_t bitIndex;
		bool found = false;
		for (unsigned int i=0; i<LUT.size(); i++) { // for each byte
			for (int p=0; p<8; p++) { // for each bit
				if (LUT[i][p] == 0) { // if bit is empty
					found = true;
					byteIndex = i;
					bitIndex = p;
				}
			}
		}
		// If no unused bit is found then add a new byte to the bitfield
		if (!found) {
			byteIndex = BitField.size() + 1;
			bitIndex = 0; // First bit of new byte
			BitField.resize(byteIndex);
		}
		// Find a unused ID number for this flag
		for (unsigned int i=1; i<((LUT.size()*8)); i++) { // Go through possible IDs starting at 1 and going to 'NumberOfBytes * 8_Bits'
			bool found = false;
			for (unsigned int p=0; p<LUT.size(); p++) { // for each byte
				for (int j=0; j<8; j++) { // for each bit
					if(LUT[p][j]==i){ found=true; } // if that ID is used
				}
			}
			if (found==false) { // If an unused ID is found, use it
				flagId = i; // Set flag ID
				LUT[byteIndex][bitIndex] = i; // Set lut with ID (using the empty bit we found earlier)
				break;
			}
		}
	}

public:
	Flag() { createManagedFlag(); } // Use built in auto memory management
	Flag(bool val) { createManagedFlag(); write(val); } // Use built in auto memory management (set initial value)
	Flag(uint32_t _addr, uint8_t _bitOffset) : bytePtr((uint8_t *)_addr), bitOffset(_bitOffset) { if(0>bitOffset||bitOffset>7){bitOffset=0;} managed = false; } // For inputing an address
	Flag(uint32_t _addr, uint8_t _bitOffset, bool val) : bytePtr((uint8_t *)_addr), bitOffset(_bitOffset) { if(0>bitOffset||bitOffset>7){bitOffset=0;} managed = false; write(val); } // For inputing an address (set initial value)
	Flag(uint8_t* _bytePtr, uint8_t _bitOffset) : bytePtr(_bytePtr), bitOffset(_bitOffset) { if(0>bitOffset||bitOffset>7){bitOffset=0;} managed = false; } // For inputing a pointer
	Flag(uint8_t* _bytePtr, uint8_t _bitOffset, bool val) : bytePtr(_bytePtr), bitOffset(_bitOffset) { if(0>bitOffset||bitOffset>7){bitOffset=0;} managed = false; write(val); } // For inputing a pointer (set initial value)

	bool read() {
		bool val;
		if (managed) {
			int index = getFlagByteIndex(flagId); int offset = getFlagBitIndex(flagId);
			if (index==-1||offset==-1) { return false; } // Error: returns false
			val = (*LUT[index] & (1<<offset)) >> offset;
		} else {
			val = (*bytePtr & (1<<bitOffset)) >> bitOffset;
		}
		return val;
	}
	void write(bool val) {
		if (managed) {
			int index = Flag::getFlagByteIndex(flagId);
			int offset = getFlagBitIndex(flagId);
			if (index==-1||offset==-1) { return; } // Error
			*LUT[index] = (*LUT[index] & (0xFFFFFFFF-(1<<offset)) ) | (val<<offset);
		} else {
			*bytePtr = (*bytePtr & (0xFFFFFFFF-(1<<bitOffset)) ) | (val<<bitOffset);
		}
	}

	static void defragment() { // checks for empty bits, rearranges the full (Bits that are being used for flags) bits to fill in the empty bits (This can reduce the number of used bytes in some cases)
		// Check for empty bytes, remove them from vector
		for (unsigned int i=0; i<LUT.size(); i++) { // For each byte in the bitfield
			int count = 0;
			for (int p=0; p<8; p++) { // For each flag (bit)
				if(LUT[i][p] == 0) { // If it is empty (Not used by a flag)
					count++;
				} 
			}
			if (count>=8) { // If all 4 bytes are empty
				LUT.erase(LUT.begin()+i); // Remove that Byte
				i--; // Prevent i from increasing because the next byte shifted back into this index when we removed the byte that was here
			}
		}
		// Find the last full bit
		// Check first empty bit; 
		int lastByte, lastBit;
		for (unsigned int i=0; i<LUT.size(); i++) { // For each bitfield (byte)
			for (int p=0; p<8; p++) { // For each flag (bit)
				if(!LUT[i][p]) { // If it is empty (Not used by a flag)
					// Move last full bit to that space
					LUT[i][p] = true; LUT[lastByte][lastBit] = false;
					// Find the next last full bit
				} 
			}
		}
		// Move the last full bit to the first empty bit, repeat until there are no more empty bits
		// Check if the last byte is empty, remove it from vector; repeat until false in case of multable empty bytes
	}

	// Flag Value Operators
	void operator=(bool val) { this->write(val); }
	bool operator==(bool) { return this->read(); } // Return the boolean value
	// Other Operators
	void operator=(Flag flag) {
		this->bytePtr = flag.bytePtr;
		this->bitOffset = flag.bitOffset;
	}
	// void operator=(uint8_t *ptr) { bytePtr = ptr; }

};

// uint32_t* FlagByte; /** TODO: Make EXTMEM */
// Flag BitFlags[32] = {
// 	// Byte 0; Bits 0:7
// 	Flag(FlagBytes[0], 0),
// 	Flag(FlagBytes[0], 1),
// 	Flag(FlagBytes[0], 2),
// 	Flag(FlagBytes[0], 3),
// 	Flag(FlagBytes[0], 4),
// 	Flag(FlagBytes[0], 5),
// 	Flag(FlagBytes[0], 6),
// 	Flag(FlagBytes[0], 7),
// 	// Byte 0; Bits 8:15
// 	Flag(FlagBytes[0], 8),
// 	Flag(FlagBytes[0], 9),
// 	Flag(FlagBytes[0], 10),
// 	Flag(FlagBytes[0], 11),
// 	Flag(FlagBytes[0], 12),
// 	Flag(FlagBytes[0], 13),
// 	Flag(FlagBytes[0], 14),
// 	Flag(FlagBytes[0], 15),
// 	// Byte 0; Bits 16:23
// 	Flag(FlagBytes[0], 16),
// 	Flag(FlagBytes[0], 17),
// 	Flag(FlagBytes[0], 18),
// 	Flag(FlagBytes[0], 19),
// 	Flag(FlagBytes[0], 20),
// 	Flag(FlagBytes[0], 21),
// 	Flag(FlagBytes[0], 22),
// 	Flag(FlagBytes[0], 23),
// 	// Byte 0; Bits 24:31
// 	Flag(FlagBytes[0], 24),
// 	Flag(FlagBytes[0], 25),
// 	Flag(FlagBytes[0], 26),
// 	Flag(FlagBytes[0], 27),
// 	Flag(FlagBytes[0], 28),
// 	Flag(FlagBytes[0], 29),
// 	Flag(FlagBytes[0], 30),
// 	Flag(FlagBytes[0], 31),

// };

#endif // FLAG_H_