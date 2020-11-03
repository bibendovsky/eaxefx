/*

EAX OpenAL Extension

Copyright (c) 2020 Boris I. Bendovsky (bibendovsky@hotmail.com) and Contributors.

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


#ifndef EAXEFX_EAX_CONVERTERS_INCLUDED
#define EAXEFX_EAX_CONVERTERS_INCLUDED


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct EaxReverbToEfx
{
	static float environment_diffusion(
		float environment_diffusion);

	static float room(
		long room);

	static float room_hf(
		long room_hf);

	static float room_lf(
		long room_lf);

	static float decay_time(
		float decay_time);

	static float decay_hf_ratio(
		float decay_hf_ratio);

	static float decay_lf_ratio(
		float decay_lf_ratio);

	static float reflections(
		long reflections);

	static float reflections_delay(
		float reflections_delay);

	static float reverb(
		long reverb);

	static float reverb_delay(
		float reverb_delay);

	static float echo_time(
		float echo_time);

	static float echo_depth(
		float echo_depth);

	static float modulation_time(
		float modulation_time);

	static float modulation_depth(
		float modulation_depth);

	static float air_absorption_hf(
		float air_absorption_hf);

	static float hf_reference(
		float hf_reference);

	static float lf_reference(
		float lf_reference);

	static float room_rolloff_factor(
		float room_rolloff_factor);
}; // EaxReverbToEfx

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

struct EaxFxSlotToEfx
{
	static float volume(
		long volume);
}; // EaxFxSlotToEfx

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_EAX_CONVERTERS_INCLUDED
