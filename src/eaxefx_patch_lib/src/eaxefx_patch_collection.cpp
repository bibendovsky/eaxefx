/*

EAX OpenAL Extension

Copyright (c) 2020-2021 Boris I. Bendovsky (bibendovsky@hotmail.com) and Contributors.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.

*/


#include "eaxefx_patch_collection.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

PatchCollection make_patch_collection()
{
	return PatchCollection
	{
		// ==================================================================
		// Doom 3 (2004)

		Patch
		{
			// Name.
			"Doom 3 (2004)",

			// File name.
			"Doom3.exe",

			// Description.
			"Fixes visual twitch bug (http://www.pcgamingwiki.com/wiki/Doom_3#Visual_twitch_bug_with_OpenAL.2FEAX).",

			PatchBlocks
			{

				PatchBlock
				{
					0x000EAF3F,

					// Unpatched.
					PatchBytes
					{
						0x74, 0x18,
						0xA1, 0xA4, 0xFF, 0x7C, 0x00,
						0x8B, 0x08,
						0x68, 0xE4, 0x34, 0x77, 0x00,
						0x50,
						0xFF, 0x51, 0x5C,
						0x83, 0xC4, 0x08,
						0xE9, 0xC5, 0x00, 0x00, 0x00,
					},

					// Patched.
					PatchBytes
					{
						0x0F, 0x84,
						0xCE, 0x00, 0x00, 0x00, 0x00,
						0x8B, 0x08,
						0x68, 0xE4, 0x34, 0x77, 0x00,
						0x50,
						0xFF, 0x51, 0x5C,
						0x83, 0xC4, 0x08,
						0xE9, 0xC5, 0x00, 0x00, 0x00,
					},
				},

				PatchBlock
				{
					0x000EB270,

					// Unpatched.
					PatchBytes
					{
						0x74, 0x18,
						0xA1, 0xA4, 0xFF, 0x7C, 0x00,
						0x8B, 0x08,
						0x68, 0xE4, 0x34, 0x77, 0x00,
						0x50,
						0xFF, 0x51, 0x5C,
						0x83, 0xC4, 0x08,
						0xE9, 0xEE, 0x00, 0x00, 0x00,
					},

					// Patched.
					PatchBytes
					{
						0x0F, 0x84,
						0xF6, 0x00, 0x00, 0x00, 0x00,
						0x8B, 0x08,
						0x68, 0xE4, 0x34, 0x77, 0x00,
						0x50,
						0xFF, 0x51, 0x5C,
						0x83, 0xC4, 0x08,
						0xE9, 0xEE, 0x00, 0x00, 0x00,
					},
				},

				PatchBlock
				{
					0x000EB38F,

					// Unpatched.
					PatchBytes
					{
						0x8B, 0x44, 0x24, 0x10,
						0x8A, 0x48, 0x4C,
						0x84, 0xC9,
						0x5D,
						0x5B,
						0x74, 0x19,
					},

					// Patched.
					PatchBytes
					{
						0x8B, 0x44, 0x24, 0x10,
						0x8A, 0x48, 0x4C,
						0x84, 0xC9,
						0x5D,
						0x5B,
						0xEB, 0x19,
					},
				},
			},
		},

		Patch
		{
			// Name.
			"Doom 3 (2004)",

			// File name.
			"Doom3.exe",

			// Description.
			"Scales down volume of all sounds (https://github.com/dhewm/dhewm3/commit/3c01757d27a22a3858737f6b51e85895d2fac887).",

			PatchBlocks
			{

				PatchBlock
				{
					0x000F7155,

					// Unpatched.
					PatchBytes
					{
						0x84, 0xDB,
						0x8B, 0x8C, 0x24, 0x18, 0x81, 0x00, 0x00,
					},

					// Patched.
					PatchBytes
					{
						0xE9, 0x3A, 0x36, 0x25, 0x00,
						0x90,
						0x90,
						0x90,
						0x90,
					},
				},

				PatchBlock
				{
					0x0034A794,

					// Unpatched.
					PatchBytes
					{
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00,
					},

					// Patched.
					PatchBytes
					{
						0xC7, 0x44, 0x24, 0x10, 0xAB, 0xAA, 0xAA, 0x3E,
						0xD8, 0x4C, 0x24, 0x10,
						0x84, 0xDB,
						0x8B, 0x8C, 0x24, 0x18, 0x81, 0x00, 0x00,
						0xE9, 0xB0, 0xC9, 0xDA, 0xFF,
					},
				},
			},
		},

		Patch
		{
			// Name.
			"Doom 3 (2004)",

			// File name.
			"Doom3.exe",

			// Description.
			"Initializes reverb properties with default values.",

			PatchBlocks
			{

				PatchBlock
				{
					0x000ECF7F,

					// Unpatched.
					PatchBytes
					{
						0x8D, 0x54, 0x24, 0x10,
						0x52,
					},

					// Patched.
					PatchBytes
					{
						0xE9, 0x2A, 0xD8, 0x25, 0x00,
					},
				},

				PatchBlock
				{
					0x0034A7AE,

					// Unpatched.
					PatchBytes
					{
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					},

					// Patched.
					PatchBytes
					{
						0x56,
						0x57,
						0x51,
						0xBE, 0x58, 0xE5, 0x7C, 0x00,
						0x89, 0xEF,
						0xB9, 0x1C, 0x00, 0x00, 0x00,
						0xF3, 0xA5,
						0x59,
						0x5F,
						0x5E,
						0x8D, 0x54, 0x24, 0x10,
						0x52,
						0xE9, 0xB8, 0x27, 0xDA, 0xFF,
					},
				},

				PatchBlock
				{
					0x003CE558,

					// Unpatched.
					PatchBytes
					{
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					},

					// Patched.
					PatchBytes
					{
						0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0xF0, 0x40,
						0x00, 0x00, 0x80, 0x3F,
						0x18, 0xFC, 0xFF, 0xFF,
						0x9C, 0xFF, 0xFF, 0xFF,
						0x00, 0x00, 0x00, 0x00,
						0x52, 0xB8, 0xBE, 0x3F,
						0xE1, 0x7A, 0x54, 0x3F,
						0x00, 0x00, 0x80, 0x3F,
						0xD6, 0xF5, 0xFF, 0xFF,
						0x42, 0x60, 0xE5, 0x3B,
						0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00,
						0xC8, 0x00, 0x00, 0x00,
						0x58, 0x39, 0x34, 0x3C,
						0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x80, 0x3E,
						0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x80, 0x3E,
						0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0xA0, 0xC0,
						0x00, 0x40, 0x9C, 0x45,
						0x00, 0x00, 0x7A, 0x43,
						0x00, 0x00, 0x00, 0x00,
						0x3F, 0x00, 0x00, 0x00,
					},
				},
			},
		},

		Patch
		{
			// Name.
			"Doom 3 (2004)",

			// File name.
			"Doom3.exe",

			// Description.
			"Fixes error \"idSoundCache: error unloading data from OpenAL hardware buffer\".",

			PatchBlocks
			{

				PatchBlock
				{
					0x000EA758,

					// Unpatched.
					PatchBytes
					{
						0xFF, 0x15, 0x00, 0xA0, 0x70, 0x01,
					},

					// Patched.
					PatchBytes
					{
						0xE9, 0x6F, 0x00, 0x26, 0x00,
						0x90,
					},
				},

				PatchBlock
				{
					0x0034A7CC,

					// Unpatched.
					PatchBytes
					{
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00,
					},

					// Patched.
					PatchBytes
					{
						0x8B, 0x4E, 0x48,
						0x85, 0xC9,
						0x74, 0x52,
						0x55,
						0xBD, 0x54, 0xCA, 0x6B, 0x01,
						0x8B, 0x7D, 0x00,
						0x83, 0xC5, 0x04,
						0xC1, 0xE7, 0x04,
						0x01, 0xEF,
						0x50,
						0x39, 0xEF,
						0x74, 0x3A,
						0x80, 0x7D, 0x0C, 0x00,
						0x74, 0x2F,
						0x8B, 0x45, 0x08,
						0x85, 0xC0,
						0x74, 0x28,
						0x31, 0xC0,
						0x89, 0x04, 0x24,
						0x8B, 0x45, 0x00,
						0x54,
						0x68, 0x09, 0x10, 0x00, 0x00,
						0x50,
						0xFF, 0x15, 0x10, 0xA0, 0x70, 0x01,
						0x83, 0xC4, 0x0C,
						0x8B, 0x46, 0x48,
						0x2B, 0x04, 0x24,
						0x75, 0x08,
						0x8B, 0x4D, 0x08,
						0xE8, 0x32, 0x31, 0xDA, 0xFF,
						0x83, 0xC5, 0x10,
						0xEB, 0xC2,
						0x58,
						0x5D,
						0xFF, 0x15, 0x00, 0xA0, 0x70, 0x01,
						0xE9, 0x2E, 0xFF, 0xD9, 0xFF,
					},
				},
			},
		},

		// Doom 3 (2004)
		// ==================================================================


		// ==================================================================
		// Quake 4 (2005)

		Patch
		{
			// Name.
			"Quake 4 (2005)",

			// File name.
			"Quake4.exe",

			// Description.
			"Fixes source's occlusion out of range.",

			PatchBlocks
			{

				PatchBlock
				{
					0x001739DD,

					// Unpatched.
					PatchBytes
					{
						0x75, 0x17,
						0x8B, 0x4D, 0x78,
						0x6A, 0x10,
						0x8D, 0x54, 0x24, 0x58,
						0x52,
						0x51,
						0x6A, 0x03,
						0x68, 0x80, 0xDE, 0x3F, 0x10,
						0xFF, 0xD0,
					},

					// Patched.
					PatchBytes
					{
						0x75, 0x17,
						0x8B, 0x4D, 0x78,
						0x6A, 0x10,
						0x8D, 0x54, 0x24, 0x58,
						0xE9, 0x77, 0xBE,
						0x13, 0x00,
						0x80,
						0xDE,
						0x3F,
						0x10,
						0xFF, 0xD0,
					},
				},

				PatchBlock
				{
					0x001739BC,

					// Unpatched.
					PatchBytes
					{
						0x75, 0x14,
						0x8B, 0x4D, 0x78,
						0x6A, 0x10,
						0x8D, 0x54, 0x24, 0x58,
						0x52,
						0x51,
						0x6A, 0x03,
						0x68, 0xE0, 0xDE, 0x3F, 0x10,
						0xEB, 0x1F,
					},

					// Patched.
					PatchBytes
					{
						0x75, 0x14,
						0x8B, 0x4D, 0x78,
						0x6A, 0x10,
						0x8D, 0x54, 0x24, 0x58,
						0xE9, 0xB4, 0xBE, 0x13, 0x00,
						0xE0,
						0xDE,
						0x3F,
						0x10,
						0xEB, 0x1F,
					},
				},

				PatchBlock
				{
					0x002AF864,

					// Unpatched.
					PatchBytes
					{
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					},

					// Patched.
					PatchBytes
					{
						0x81, 0x3A, 0xF0, 0xD8, 0xFF, 0xFF,
						0x7D, 0x06,
						0xC7, 0x02, 0xF0, 0xD8, 0xFF, 0xFF,
						0x52,
						0x51,
						0x6A, 0x03,
						0x68, 0x80, 0xDE, 0x3F, 0x10,
						0xE9, 0x71, 0x41, 0xEC, 0xFF,

						0x81, 0x3A, 0xF0, 0xD8, 0xFF, 0xFF,
						0x7D, 0x06,
						0xC7, 0x02, 0xF0, 0xD8, 0xFF, 0xFF,
						0x52,
						0x51,
						0x6A, 0x03,
						0x68, 0xE0, 0xDE, 0x3F, 0x10,
						0xE9, 0x55, 0x41, 0xEC, 0xFF,
					},
				},
			},
		},

		// Quake 4 (2004)
		// ==================================================================


		// ==================================================================
		// Prey (2006)

		Patch
		{
			// Name.
			"Prey (2006)",

			// File name.
			"prey.exe",

			// Description.
			"Fixes source's occlusion out of range.",

			PatchBlocks
			{

				PatchBlock
				{
					0x0011218C,

					// Unpatched.
					PatchBytes
					{
						0x75, 0x1C,
						0x8B, 0x4D, 0x7C,
						0x6A, 0x10,
						0x8D, 0x54, 0x24, 0x54,
						0x52,
						0x51,
						0x6A, 0x03,
						0x68, 0xB0, 0x26, 0x93, 0x00,
						0xFF, 0xD0,
					},

					// Patched.
					PatchBytes
					{
						0x75, 0x1C,
						0x8B, 0x4D, 0x7C,
						0x6A, 0x10,
						0x8D, 0x54, 0x24, 0x54,
						0xE9, 0xAC, 0x6A, 0x29, 0x00,
						0xB0,
						0x26,
						0x93,
						0x00,
						0xFF, 0xD0
					},
				},

				PatchBlock
				{
					0x00112173,

					// Unpatched.
					PatchBytes
					{
						0x75, 0x14,
						0x8B, 0x4D, 0x7C,
						0x6A, 0x10,
						0x8D, 0x54, 0x24, 0x54,
						0x52,
						0x51,
						0x6A, 0x03,
						0x68, 0x10, 0x27, 0x93, 0x00,
						0xEB, 0x17,
					},

					// Patched.
					PatchBytes
					{
						0x75, 0x14,
						0x8B, 0x4D, 0x7C,
						0x6A, 0x10,
						0x8D, 0x54, 0x24, 0x54,
						0xE9, 0xE1, 0x6A, 0x29, 0x00,
						0x10,
						0x27,
						0x93,
						0x00,
						0xEB, 0x17,
					},
				},

				PatchBlock
				{
					0x003A8C48,

					// Unpatched.
					PatchBytes
					{
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					},

					// Patched.
					PatchBytes
					{
						0x81, 0x3A, 0xF0, 0xD8, 0xFF, 0xFF,
						0x7D, 0x06,
						0xC7, 0x02, 0xF0, 0xD8, 0xFF, 0xFF,
						0x52,
						0x51,
						0x6A, 0x03,
						0x68, 0xB0, 0x26, 0x93, 0x00,
						0xE9, 0x3C, 0x95, 0xD6, 0xFF,

						0x81, 0x3A, 0xF0, 0xD8, 0xFF, 0xFF,
						0x7D, 0x06,
						0xC7, 0x02, 0xF0, 0xD8, 0xFF, 0xFF,
						0x52,
						0x51,
						0x6A, 0x03,
						0x68, 0x10, 0x27, 0x93, 0x00,
						0xE9, 0x20, 0x95, 0xD6, 0xFF,
					},
				},
			},
		},

		// Prey (2006)
		// ==================================================================
	};
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
