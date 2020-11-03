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


#ifndef EAXEFX_EAXX_CONTEXT_INCLUDED
#define EAXEFX_EAXX_CONTEXT_INCLUDED


#include <unordered_map>

#include "AL/al.h"
#include "AL/alc.h"

#include "eaxefx_eax_api.h"
#include "eaxefx_eaxx_fx_slots.h"
#include "eaxefx_eaxx_source.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class EaxxContext
{
public:
	EaxxContext(
		ALCdevice* al_device,
		ALCcontext* al_context);


	bool is_tried_to_initialize() const noexcept;

	void initialize();

	bool is_initialized() const noexcept;


	ALCcontext* get_al_context() const noexcept;

	EaxxFxSlot& get_slot(
		int fx_slot_index);


	void initialize_sources(
		ALsizei count,
		ALuint* al_names);

	void uninitialize_sources(
		ALsizei count,
		const ALuint* al_names);


	void set_primary_fx_slot_id(
		const GUID& eax_fx_slot_id);

	void set_context(
		const EAX50CONTEXTPROPERTIES& eax_context);

	void set_session(
		const EAXSESSIONPROPERTIES& eax_session);

	EaxxSource* find_source(
		ALuint al_source);

	void update_filters();


private:
	struct Al
	{
		ALCdevice* device{};
		ALCcontext* context{};
		ALuint filter{};
	}; // Al

	struct Eax
	{
		EAXSESSIONPROPERTIES session{};
		EAX50CONTEXTPROPERTIES context{};
	}; // Eax

	using SourceMap = std::unordered_map<ALuint, EaxxSource>;


	bool is_tried_to_initialize_{};
	bool is_initialized_{};
	EaxxFxSlots fx_slots_{};
	SourceMap source_map_{};
	Al al_{};
	Eax eax_{};


	void ensure_compatibility();


	void set_eax_session_defaults();

	void set_eax_context_defaults();

	void set_eax_defaults();


	void initialize_filter();

	void set_primary_fx_slot_id();

	void set_distance_factor();

	void set_air_absorbtion_hf();

	void set_hf_reference();

	void set_context();

	void initialize_fx_slots();
}; // EaxxContext

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx


#endif // !EAXEFX_EAXX_CONTEXT_INCLUDED
