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

#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*=========================================================================*/

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#define MAX_PATCH_BLOCK_BYTES (1024)

typedef enum
{
	ps_unsupported = 0,
	ps_patched,
	ps_unpatched
} PatchStatus;

typedef struct
{
	const unsigned char* items;
	size_t count;
} PatchByteSpan;

typedef struct
{
	const char* file_name;
	size_t file_offset;
	PatchByteSpan unpatched_byte_span;
	PatchByteSpan patched_byte_span;
} PatchBlock;

typedef struct
{
	const PatchBlock* items;
	size_t count;
} PatchBlockSpan;

typedef struct
{
	const char* name;
	const char* description;
	PatchBlockSpan block_spans;
} Patch;

typedef struct
{
	const Patch* items;
	size_t count;
} PatchSpan;

/*=========================================================================*/

#define patch_1_file_name "Doom3.exe"

const unsigned char patch_1_unpatched_bytes_1[] =
{
	0x74, 0x18,
	0xA1, 0xA4, 0xFF, 0x7C, 0x00,
	0x8B, 0x08,
	0x68, 0xE4, 0x34, 0x77, 0x00,
	0x50,
	0xFF, 0x51, 0x5C,
	0x83, 0xC4, 0x08,
	0xE9, 0xC5, 0x00, 0x00, 0x00,
};

const unsigned char patch_1_patched_bytes_1[] =
{
	0x0F, 0x84,
	0xCE, 0x00, 0x00, 0x00, 0x00,
	0x8B, 0x08,
	0x68, 0xE4, 0x34, 0x77, 0x00,
	0x50,
	0xFF, 0x51, 0x5C,
	0x83, 0xC4, 0x08,
	0xE9, 0xC5, 0x00, 0x00, 0x00,
};

const unsigned char patch_1_unpatched_bytes_2[] =
{
	0x74, 0x18,
	0xA1, 0xA4, 0xFF, 0x7C, 0x00,
	0x8B, 0x08,
	0x68, 0xE4, 0x34, 0x77, 0x00,
	0x50,
	0xFF, 0x51, 0x5C,
	0x83, 0xC4, 0x08,
	0xE9, 0xEE, 0x00, 0x00, 0x00,
};

const unsigned char patch_1_patched_bytes_2[] =
{
	0x0F, 0x84,
	0xF6, 0x00, 0x00, 0x00, 0x00,
	0x8B, 0x08,
	0x68, 0xE4, 0x34, 0x77, 0x00,
	0x50,
	0xFF, 0x51, 0x5C,
	0x83, 0xC4, 0x08,
	0xE9, 0xEE, 0x00, 0x00, 0x00,
};

const unsigned char patch_1_unpatched_bytes_3[] =
{
	0x8B, 0x44, 0x24, 0x10,
	0x8A, 0x48, 0x4C,
	0x84, 0xC9,
	0x5D,
	0x5B,
	0x74, 0x19,
};

const unsigned char patch_1_patched_bytes_3[] =
{
	0x8B, 0x44, 0x24, 0x10,
	0x8A, 0x48, 0x4C,
	0x84, 0xC9,
	0x5D,
	0x5B,
	0xEB, 0x19,
};

const PatchBlock patch_1_block_array[] =
{
	{
		patch_1_file_name,
		0x000EAF3F,
		{patch_1_unpatched_bytes_1, ARRAY_SIZE(patch_1_unpatched_bytes_1)},
		{patch_1_patched_bytes_1, ARRAY_SIZE(patch_1_patched_bytes_1)}
	},

	{
		patch_1_file_name,
		0x000EB270,
		{patch_1_unpatched_bytes_2, ARRAY_SIZE(patch_1_unpatched_bytes_2)},
		{patch_1_patched_bytes_2, ARRAY_SIZE(patch_1_patched_bytes_2)}
	},

	{
		patch_1_file_name,
		0x000EB38F,
		{patch_1_unpatched_bytes_3, ARRAY_SIZE(patch_1_unpatched_bytes_3)},
		{patch_1_patched_bytes_3, ARRAY_SIZE(patch_1_patched_bytes_3)}
	}
};

/*-------------------------------------------------------------------------*/

#define patch_2_file_name "Doom3.exe"

const unsigned char patch_2_unpatched_bytes_1[] =
{
	0x84, 0xDB,
	0x8B, 0x8C, 0x24, 0x18, 0x81, 0x00, 0x00,
};

const unsigned char patch_2_patched_bytes_1[] =
{
	0xE9, 0x3A, 0x36, 0x25, 0x00,
	0x90,
	0x90,
	0x90,
	0x90,
};

const unsigned char patch_2_unpatched_bytes_2[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00,
};

const unsigned char patch_2_patched_bytes_2[] =
{
	0xC7, 0x44, 0x24, 0x10, 0xAB, 0xAA, 0xAA, 0x3E,
	0xD8, 0x4C, 0x24, 0x10,
	0x84, 0xDB,
	0x8B, 0x8C, 0x24, 0x18, 0x81, 0x00, 0x00,
	0xE9, 0xB0, 0xC9, 0xDA, 0xFF,
};

const PatchBlock patch_2_block_array[] =
{
	{
		patch_2_file_name,
		0x000F7155,
		{patch_2_unpatched_bytes_1, ARRAY_SIZE(patch_2_unpatched_bytes_1)},
		{patch_2_patched_bytes_1, ARRAY_SIZE(patch_2_patched_bytes_1)}
	},

	{
		patch_2_file_name,
		0x0034A794,
		{patch_2_unpatched_bytes_2, ARRAY_SIZE(patch_2_unpatched_bytes_2)},
		{patch_2_patched_bytes_2, ARRAY_SIZE(patch_2_patched_bytes_2)}
	},
};

/*-------------------------------------------------------------------------*/

#define patch_3_file_name "Doom3.exe"

const unsigned char patch_3_unpatched_bytes_1[] =
{
	0x8D, 0x54, 0x24, 0x10,
	0x52,
};

const unsigned char patch_3_patched_bytes_1[] =
{
	0xE9, 0x2A, 0xD8, 0x25, 0x00,
};

const unsigned char patch_3_unpatched_bytes_2[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char patch_3_patched_bytes_2[] =
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
};

const unsigned char patch_3_unpatched_bytes_3[] =
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
};

const unsigned char patch_3_patched_bytes_3[] =
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
};

const PatchBlock patch_3_block_array[] =
{
	{
		patch_3_file_name,
		0x000ECF7F,
		{patch_3_unpatched_bytes_1, ARRAY_SIZE(patch_3_unpatched_bytes_1)},
		{patch_3_patched_bytes_1, ARRAY_SIZE(patch_3_patched_bytes_1)}
	},

	{
		patch_3_file_name,
		0x0034A7AE,
		{patch_3_unpatched_bytes_2, ARRAY_SIZE(patch_3_unpatched_bytes_2)},
		{patch_3_patched_bytes_2, ARRAY_SIZE(patch_3_patched_bytes_2)}
	},

	{
		patch_3_file_name,
		0x003CE558,
		{patch_3_unpatched_bytes_3, ARRAY_SIZE(patch_3_unpatched_bytes_3)},
		{patch_3_patched_bytes_3, ARRAY_SIZE(patch_3_patched_bytes_3)}
	}
};

/*-------------------------------------------------------------------------*/

#define patch_4_file_name "Doom3.exe"

const unsigned char patch_4_unpatched_bytes_1[] =
{
	0xFF, 0x15, 0x00, 0xA0, 0x70, 0x01,
};

const unsigned char patch_4_patched_bytes_1[] =
{
	0xE9, 0x6F, 0x00, 0x26, 0x00,
	0x90,
};

const unsigned char patch_4_unpatched_bytes_2[] =
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
};

const unsigned char patch_4_patched_bytes_2[] =
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
};

const PatchBlock patch_4_block_array[] =
{
	{
		patch_4_file_name,
		0x000EA758,
		{patch_4_unpatched_bytes_1, ARRAY_SIZE(patch_4_unpatched_bytes_1)},
		{patch_4_patched_bytes_1, ARRAY_SIZE(patch_4_patched_bytes_1)}
	},

	{
		patch_4_file_name,
		0x0034A7CC,
		{patch_4_unpatched_bytes_2, ARRAY_SIZE(patch_4_unpatched_bytes_2)},
		{patch_4_patched_bytes_2, ARRAY_SIZE(patch_4_patched_bytes_2)}
	},
};

/*-------------------------------------------------------------------------*/

#define patch_5_file_name "Quake4.exe"

const unsigned char patch_5_unpatched_bytes_1[] =
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
};

const unsigned char patch_5_patched_bytes_1[] =
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
};

const unsigned char patch_5_unpatched_bytes_2[] =
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
};

const unsigned char patch_5_patched_bytes_2[] =
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
};

const unsigned char patch_5_unpatched_bytes_3[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char patch_5_patched_bytes_3[] =
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
};

const PatchBlock patch_5_block_array[] =
{
	{
		patch_5_file_name,
		0x001739DD,
		{patch_5_unpatched_bytes_1, ARRAY_SIZE(patch_5_unpatched_bytes_1)},
		{patch_5_patched_bytes_1, ARRAY_SIZE(patch_5_patched_bytes_1)}
	},

	{
		patch_5_file_name,
		0x001739BC,
		{patch_5_unpatched_bytes_2, ARRAY_SIZE(patch_5_unpatched_bytes_2)},
		{patch_5_patched_bytes_2, ARRAY_SIZE(patch_5_patched_bytes_2)}
	},

	{
		patch_5_file_name,
		0x002AF864,
		{patch_5_unpatched_bytes_3, ARRAY_SIZE(patch_5_unpatched_bytes_3)},
		{patch_5_patched_bytes_3, ARRAY_SIZE(patch_5_patched_bytes_3)}
	}
};

/*-------------------------------------------------------------------------*/

#define patch_6_file_name "prey.exe"

const unsigned char patch_6_unpatched_bytes_1[] =
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
};

const unsigned char patch_6_patched_bytes_1[] =
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
};

const unsigned char patch_6_unpatched_bytes_2[] =
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
};

const unsigned char patch_6_patched_bytes_2[] =
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
};

const unsigned char patch_6_unpatched_bytes_3[] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char patch_6_patched_bytes_3[] =
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
};

const PatchBlock patch_6_block_array[] =
{
	{
		patch_6_file_name,
		0x0011218C,
		{patch_6_unpatched_bytes_1, ARRAY_SIZE(patch_6_unpatched_bytes_1)},
		{patch_6_patched_bytes_1, ARRAY_SIZE(patch_6_patched_bytes_1)}
	},

	{
		patch_6_file_name,
		0x00112173,
		{patch_6_unpatched_bytes_2, ARRAY_SIZE(patch_6_unpatched_bytes_2)},
		{patch_6_patched_bytes_2, ARRAY_SIZE(patch_6_patched_bytes_2)}
	},

	{
		patch_6_file_name,
		0x003A8C48,
		{patch_6_unpatched_bytes_3, ARRAY_SIZE(patch_6_unpatched_bytes_3)},
		{patch_6_patched_bytes_3, ARRAY_SIZE(patch_6_patched_bytes_3)}
	}
};

/*-------------------------------------------------------------------------*/

const Patch patch_array[] =
{
	{
		"Doom 3 (2004) v1.3.1",
		"Fixes visual twitch bug (http://www.pcgamingwiki.com/wiki/Doom_3#Visual_twitch_bug_with_OpenAL.2FEAX).",
		{patch_1_block_array, ARRAY_SIZE(patch_1_block_array)}
	},

	{
		"Doom 3 (2004) v1.3.1",
		"Scales down volume of all sounds (https://github.com/dhewm/dhewm3/commit/3c01757d27a22a3858737f6b51e85895d2fac887).",
		{patch_2_block_array, ARRAY_SIZE(patch_2_block_array)}
	},

	{
		"Doom 3 (2004) v1.3.1",
		"Initializes reverb properties with default values.",
		{patch_3_block_array, ARRAY_SIZE(patch_3_block_array)}
	},

	{
		"Doom 3 (2004) v1.3.1",
		"Fixes error \"idSoundCache: error unloading data from OpenAL hardware buffer\".",
		{patch_4_block_array, ARRAY_SIZE(patch_4_block_array)}
	},

	{
		"Quake 4 (2005) v1.4.2",
		"Fixes source's occlusion out of range.",
		{patch_5_block_array, ARRAY_SIZE(patch_5_block_array)}
	},

	{
		"Prey (2006) v1.4 retail",
		"Fixes source's occlusion out of range.",
		{patch_6_block_array, ARRAY_SIZE(patch_6_block_array)}
	},
};

const PatchSpan patch_span = {patch_array, ARRAY_SIZE(patch_array)};

/*=========================================================================*/

const char* get_patch_status_string(PatchStatus status)
{
	switch (status)
	{
		case ps_patched: return "patched";
		case ps_unpatched: return "unpatched";
		default: return "unsupported";
	}
}

const char* get_patch_status_string_for_action(PatchStatus status)
{
	switch (status)
	{
		case ps_patched: return "patch";
		case ps_unpatched: return "unpatch";
		default: return "unsupported";
	}
}

int has_byte_block(FILE* file, long file_size, size_t file_offset, const PatchByteSpan* byte_span)
{
	unsigned char buffer[MAX_PATCH_BLOCK_BYTES];
	size_t read_count;

	assert(file != NULL);
	assert(file_size > 0);
	assert(file_offset < (size_t)file_size);
	assert(byte_span != NULL);
	assert(byte_span->items != NULL);
	assert(byte_span->count > 0 && byte_span->count <= MAX_PATCH_BLOCK_BYTES);

	if ((file_offset + byte_span->count) <= (size_t)file_size)
	{
		if (fseek(file, (long)file_offset, SEEK_SET) == 0)
		{
			read_count = fread(buffer, 1, byte_span->count, file);

			if (read_count == byte_span->count)
			{
				if (memcmp(buffer, byte_span->items, byte_span->count) == 0)
				{
					return 1;
				}
			}
		}
	}

	return 0;
}

PatchStatus get_patch_block_status(const PatchBlock* block)
{
	FILE* file;
	PatchStatus result = ps_unsupported;

	assert(block->file_offset <= 2147483647);

	file = fopen(block->file_name, "rb");

	if (file != NULL)
	{
		long file_size;

		fseek(file, 0, SEEK_END);
		file_size = ftell(file);

		if (has_byte_block(file, file_size, block->file_offset, &block->patched_byte_span))
		{
			result = ps_patched;
		}
		else if (has_byte_block(file, file_size, block->file_offset, &block->unpatched_byte_span))
		{
			result = ps_unpatched;
		}

		fclose(file);
	}

	return result;
}

PatchStatus get_patch_status(const Patch* patch)
{
	size_t i;
	PatchStatus last_status = ps_unpatched;
	int status_initialized = 0;

	assert(patch != NULL);
	assert(patch->block_spans.count > 0);

	for (i = 0; i < patch->block_spans.count; ++i)
	{
		const PatchBlock* block = &patch->block_spans.items[i];
		const PatchStatus status = get_patch_block_status(block);

		if (status == ps_unsupported || (status_initialized && last_status != status))
		{
			return ps_unsupported;
		}

		status_initialized = 1;
		last_status = status;
	}

	return last_status;
}

int apply_patch_block(const PatchBlock* block, PatchStatus new_status)
{
	FILE* file;
	int result = 0;

	assert(block->file_offset <= 2147483647);

	file = fopen(block->file_name, "r+b");

	if (file != NULL)
	{
		if (fseek(file, 0, SEEK_END) == 0)
		{
			const long file_size = ftell(file);

			if (file_size > 0)
			{
				const PatchByteSpan* byte_span = NULL;

				switch (new_status)
				{
					case ps_patched: byte_span = &block->patched_byte_span; break;
					case ps_unpatched: byte_span = &block->unpatched_byte_span; break;
					default: break;
				}

				if (byte_span != NULL)
				{
					if ((block->file_offset + byte_span->count) <= (size_t)file_size)
					{
						if (fseek(file, (long)block->file_offset, SEEK_SET) == 0)
						{
							if (fwrite(byte_span->items, 1, byte_span->count, file) == byte_span->count)
							{
								result = 1;
							}
						}
					}
				}
			}
		}

		fclose(file);
	}

	return result;
}

int apply_patch(const Patch* patch, PatchStatus new_status)
{
	size_t i;

	assert(patch != NULL);
	assert(patch->block_spans.count > 0);

	for (i = 0; i < patch->block_spans.count; ++i)
	{
		const PatchBlock* block = &patch->block_spans.items[i];

		assert(block != NULL);

		if (!apply_patch_block(block, new_status))
		{
			return 0;
		}
	}

	return 1;
}

/*=========================================================================*/

const char* read_answer(void)
{
#define MAX_ANSWER_SIZE 16
	static char answer[MAX_ANSWER_SIZE];
	*answer = '\0';
	fgets(answer, MAX_ANSWER_SIZE, stdin);
	return answer;
#undef MAX_ANSWER_SIZE
}

int main(void)
{
	size_t i;
	const Patch* patches[ARRAY_SIZE(patch_array)];
	PatchStatus patch_statuses[ARRAY_SIZE(patch_array)];
	size_t patch_count = 0;
	const char* answer;
	const char* status_string;
	int is_cancelled = 0;

	printf("========================================================\n");
	printf(
		"EAXEFX Application Patcher v%d.%d.%d\n",
		EAXEFX_APP_PATCHER_VERSION_MAJOR,
		EAXEFX_APP_PATCHER_VERSION_MINOR,
		EAXEFX_APP_PATCHER_VERSION_PATCH);
	printf("========================================================\n");

	assert(patch_span.count > 0);

	for (i = 0; i < patch_span.count; ++i)
	{
		const Patch* patch = &patch_span.items[i];
		PatchStatus status = get_patch_status(patch);

		if (status == ps_unsupported)
		{
			continue;
		}

		patches[patch_count] = patch;
		patch_statuses[patch_count] = status;
		patch_count += 1;
	}

	if (patch_count > 0)
	{
		size_t patch_number = 0;
		const Patch* patch;
		PatchStatus new_status = ps_unsupported;

		for (i = 0; i < patch_count; ++i)
		{
			const PatchStatus status = patch_statuses[i];
			patch = patches[i];
			assert(patch->name != NULL);
			assert(patch->description != NULL);
			status_string = get_patch_status_string(status);

			printf("\n");

			if (patch_count > 1)
			{
				printf("%d)\n", (int)(i + 1));
			}

			printf("%s\n", patch->name);
			printf("%s\n", patch->description);
			printf("Status: %s\n", status_string);
		}

		printf("\n--------------------------------------------------------\n");

		if (patch_count > 1)
		{
			while (1)
			{
				printf("\n%s", "Type in patch number or just press \"ENTER\" to cancel: ");

				answer = read_answer();

				if (*answer == '\n')
				{
					is_cancelled = 1;
					break;
				}

				patch_number = strtoul(answer, NULL, 10);

				if (patch_number >= 1 && patch_number <= patch_count)
				{
					break;
				}
			}
		}
		else
		{
			printf("\n");
			patch_number = 1;
		}

		if (!is_cancelled)
		{
			size_t patch_index = patch_number - 1;
			patch = patches[patch_index];

			switch (patch_statuses[patch_index])
			{
				case ps_patched: new_status = ps_unpatched; break;
				case ps_unpatched: new_status = ps_patched; break;
				default: assert(0 && "Invalid patch status."); break;
			}

			status_string = get_patch_status_string_for_action(new_status);

			while (1)
			{
				printf("Type in \"y\" to [%s] or just press \"ENTER\" to cancel: ", status_string);
				answer = read_answer();

				if (answer[0] == '\n')
				{
					is_cancelled = 1;
					break;
				}

				if (answer[0] == 'y' && answer[1] == '\n')
				{
					break;
				}
			}

			if (!is_cancelled)
			{
				if (apply_patch(patch, new_status))
				{
					printf("Succeeded.\n");
				}
				else
				{
					printf("Failed.\n");
				}
			}
		}
	}

	printf("\n");

	if (patch_count == 0)
	{
		printf("Not found any supported applications.\n");
	}

	if (is_cancelled)
	{
		printf("Cancelled.\n");
	}

	printf("Press \"ENTER\" to exit.\n");
	read_answer();
	return 0;
}
