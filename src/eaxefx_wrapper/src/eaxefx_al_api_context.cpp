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


#include "eaxefx_al_api_context.h"

#include <cassert>

#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "AL/alc.h"

#include "eaxefx_al_api.h"
#include "eaxefx_al_api_utils.h"
#include "eaxefx_eax_api.h"
#include "eaxefx_exception.h"
#include "eaxefx_string.h"
#include "eaxefx_utils.h"


namespace eaxefx
{


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class AlApiContextException :
	public Exception
{
public:
	explicit AlApiContextException(
		const char* message)
		:
		Exception{"AL_API_CONTEXT", message}
	{
	}
}; // AlApiContextException

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

class AlApiContextImpl final :
	public AlApiContext
{
public:
	AlApiContextImpl();

	AlApiContextImpl(
		AlApiContextImpl&& rhs) noexcept;


	void alc_create_context(
		const AlApiContextInitParam& param) override;

	void alc_make_current() override;

	void alc_destroy() override;

	void* al_get_proc_address(
		std::string_view symbol_name) const noexcept override;

	bool al_is_extension_present(
		const char* extension_name) const noexcept override;

	const char* al_get_string(
		ALenum param) const noexcept override;

	void al_gen_sources(
		ALsizei n,
		ALuint* sources) override;

	void al_delete_sources(
		ALsizei n,
		const ALuint* sources) override;


	ALCcontext* get_al_context() const noexcept override;

	Eaxx& get_eaxx() override;


private:
	static constexpr auto min_attribute_capacity = 128;
	static constexpr auto min_attribute_pair_capacity = 2 * min_attribute_capacity;

	static constexpr auto min_string_buffer_capacity = 1'024;


	using AlEfxSymbolMap = std::unordered_map<std::string_view, void*>;
	using ExtensionSet = std::unordered_set<std::string_view>;
	using Attributes = std::vector<ALCint>;


	ALCdevice* al_device_{};

	Logger* logger_{};
	const AlAlSymbols* al_al_symbols_{};
	const AlAlcSymbols* al_alc_symbols_{};

	ALCint efx_major_version_{};
	ALCint efx_minor_version_{};
	ALCint efx_max_aux_sends_{};
	AlEfxSymbolsUPtr al_efx_symbols_{};
	AlEfxSymbolMap al_efx_symbol_map_{};

	Attributes attributes_{};
	String extensions_string_{};

	String string_buffer_{};

	bool is_made_current_{};
	ALCcontext* al_context_{};

	EaxxUPtr eaxx_{};


	[[noreturn]]
	static void fail(
		const char* message);


	static void validate_init_param(
		const AlApiContextInitParam& param);

	void initialize_string_buffer();

	void initialize_attribute_cache();

	const ALCint* make_attributes(
		const ALCint* al_attributes);

	void get_attributes();

	void get_efx_major_version_attribute();

	void get_efx_minor_version_attribute();

	void get_efx_max_aux_sends_attribute();

	void get_efx_attributes();

	void log_efx_attributes();

	static bool has_extension(
		const char* extension_name,
		const char* extensions_string) noexcept;

	void make_extensions();

	void log_context_info();

	void make_efx_symbols();

	void make_efx_symbol_map();

	void make_eaxx();
}; // AlApiContextImpl

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

AlApiContextImpl::AlApiContextImpl()
{
	//
	logger_ = g_al_api.get_logger();

	if (!logger_)
	{
		fail("Null logger.");
	}

	//
	al_alc_symbols_ = g_al_api.get_al_alc_symbols();

	if (!al_alc_symbols_)
	{
		fail("Null ALC symbols.");
	}

	//
	al_al_symbols_ = g_al_api.get_al_al_symbols();

	if (!al_al_symbols_)
	{
		fail("Null AL symbols.");
	}
}

AlApiContextImpl::AlApiContextImpl(
	AlApiContextImpl&& rhs) noexcept
{
	std::swap(al_device_, rhs.al_device_);

	std::swap(logger_, rhs.logger_);
	std::swap(al_al_symbols_, rhs.al_al_symbols_);
	std::swap(al_alc_symbols_, rhs.al_alc_symbols_);

	std::swap(efx_major_version_, rhs.efx_major_version_);
	std::swap(efx_minor_version_, rhs.efx_minor_version_);
	std::swap(efx_max_aux_sends_, rhs.efx_max_aux_sends_);
	std::swap(al_efx_symbols_, rhs.al_efx_symbols_);

	std::swap(attributes_, rhs.attributes_);
	std::swap(extensions_string_, rhs.extensions_string_);

	std::swap(string_buffer_, rhs.string_buffer_);

	std::swap(is_made_current_, rhs.is_made_current_);
	std::swap(al_context_, rhs.al_context_);

	std::swap(eaxx_, rhs.eaxx_);
}

void AlApiContextImpl::alc_create_context(
	const AlApiContextInitParam& param)
{
	validate_init_param(param);

	al_device_ = param.al_device;

	initialize_string_buffer();
	initialize_attribute_cache();
	make_attributes(param.al_context_attributes);

	al_context_ = al_alc_symbols_->alcCreateContext(
		al_device_,
		attributes_.data()
	);

	if (!al_context_)
	{
		fail("AL failed to create a context.");
	}

	logger_->info("");
	string_buffer_.clear();
	string_buffer_ += "Instance: ";
	string_buffer_ += to_string_hex(al_context_);
	logger_->info(string_buffer_.c_str());
}

void AlApiContextImpl::alc_make_current()
{
	const auto al_is_made_current = (al_alc_symbols_->alcMakeContextCurrent(al_context_) != ALC_FALSE);

	if (!al_is_made_current)
	{
		fail("AL failed to make a context current.");
	}

	if (is_made_current_)
	{
		return;
	}

	is_made_current_ = true;

	try
	{
		get_attributes();
		get_efx_attributes();
		log_context_info();
	}
	catch (...)
	{
		utils::log_exception(logger_);
	}

	try
	{
		make_efx_symbols();
		make_efx_symbol_map();

		make_eaxx();
	}
	catch (...)
	{
		utils::log_exception(logger_);
	}

	try
	{
		make_extensions();
	}
	catch (...)
	{
		utils::log_exception(logger_);
	}
}

void AlApiContextImpl::alc_destroy()
{
	// Set our context.
	//
	static_cast<void>(al_alc_symbols_->alcMakeContextCurrent(al_context_));

	// Release resources.
	//
	eaxx_ = nullptr;
	al_efx_symbols_ = nullptr;

	// Unset current context.
	//
	static_cast<void>(al_alc_symbols_->alcMakeContextCurrent(nullptr));

	// Destroy it.
	//
	static_cast<void>(al_alc_symbols_->alcGetError(al_device_));
	al_alc_symbols_->alcDestroyContext(al_context_);
	const auto alc_error = al_alc_symbols_->alcGetError(al_device_);

	// Clean up.
	//
	al_device_ = nullptr;

	al_al_symbols_ = nullptr;
	al_alc_symbols_ = nullptr;

	efx_major_version_ = 0;
	efx_minor_version_ = 0;
	efx_max_aux_sends_ = 0;

	attributes_.clear();
	string_buffer_.clear();

	is_made_current_ = false;
	al_context_ = nullptr;

	// Check for destroy result.
	//
	if (alc_error != ALC_NO_ERROR)
	{
		fail("AL failed to destroy the context.");
	}
}

bool AlApiContextImpl::al_is_extension_present(
	const char* extension_name) const noexcept
{
	if (!extension_name || extension_name[0] == '\0')
	{
		return false;
	}

	return has_extension(extension_name, extensions_string_.c_str());
}

void* AlApiContextImpl::al_get_proc_address(
	std::string_view symbol_name) const noexcept
{
	if (eaxx_)
	{
		const auto symbol = eaxx_->al_get_proc_address(symbol_name);

		if (symbol)
		{
			return symbol;
		}
	}

	const auto al_efx_symbol_it = al_efx_symbol_map_.find(symbol_name);

	if (al_efx_symbol_it != al_efx_symbol_map_.cend())
	{
		return al_efx_symbol_it->second;
	}

	return nullptr;
}

const char* AlApiContextImpl::al_get_string(
	ALenum param) const noexcept
{
	if (param != AL_EXTENSIONS)
	{
		return nullptr;
	}

	return extensions_string_.c_str();
}

void AlApiContextImpl::al_gen_sources(
	ALsizei n,
	ALuint* sources)
{
	if (!eaxx_)
	{
		return;
	}

	eaxx_->al_gen_sources(n, sources);
}

void AlApiContextImpl::al_delete_sources(
	ALsizei n,
	const ALuint* sources)
{
	if (!eaxx_)
	{
		return;
	}

	eaxx_->al_delete_sources(n, sources);
}

ALCcontext* AlApiContextImpl::get_al_context() const noexcept
{
	return al_context_;
}

Eaxx& AlApiContextImpl::get_eaxx()
{
	if (!eaxx_)
	{
		fail("Null EAXX.");
	}

	return *eaxx_;
}

[[noreturn]]
void AlApiContextImpl::fail(
	const char* message)
{
	throw AlApiContextException{message};
}

void AlApiContextImpl::validate_init_param(
	const AlApiContextInitParam& param)
{
	if (!param.al_device)
	{
		fail("Null AL device.");
	}
}

void AlApiContextImpl::initialize_string_buffer()
{
	string_buffer_.reserve(min_string_buffer_capacity);
}

void AlApiContextImpl::initialize_attribute_cache()
{
	attributes_.reserve(min_attribute_pair_capacity);
}

const ALCint* AlApiContextImpl::make_attributes(
	const ALCint* al_attributes)
{
	attributes_.clear();

	auto max_aux_sends = ALCint{};

	if (al_attributes)
	{
		while (true)
		{
			const auto attr_name = (*al_attributes++);

			if (attr_name == 0)
			{
				break;
			}

			const auto attr_value = (*al_attributes++);

			if (attr_name == ALC_MAX_AUXILIARY_SENDS)
			{
				max_aux_sends = std::max(max_aux_sends, attr_value);
			}
			else
			{
				attributes_.emplace_back(attr_name);
				attributes_.emplace_back(attr_value);
			}
		}
	}

	max_aux_sends = std::max(max_aux_sends, EAX_MAX_FXSLOTS);

	attributes_.emplace_back(ALC_MAX_AUXILIARY_SENDS);
	attributes_.emplace_back(max_aux_sends);
	attributes_.emplace_back(0);
	attributes_.emplace_back(0);

	return attributes_.data();
}

void AlApiContextImpl::get_attributes()
{
	attributes_.clear();

	auto al_size = ALCint{};

	al_alc_symbols_->alcGetIntegerv(al_device_, ALC_ATTRIBUTES_SIZE, 1, &al_size);

	if (al_size > 0)
	{
		attributes_.resize(al_size);
	}

	al_alc_symbols_->alcGetIntegerv(al_device_, ALC_ALL_ATTRIBUTES, al_size, attributes_.data());
}

void AlApiContextImpl::get_efx_major_version_attribute()
{
	efx_major_version_ = 0;
	al_alc_symbols_->alcGetIntegerv(al_device_, ALC_EFX_MAJOR_VERSION, 1, &efx_major_version_);
}

void AlApiContextImpl::get_efx_minor_version_attribute()
{
	efx_minor_version_ = 0;
	al_alc_symbols_->alcGetIntegerv(al_device_, ALC_EFX_MINOR_VERSION, 1, &efx_minor_version_);
}

void AlApiContextImpl::get_efx_max_aux_sends_attribute()
{
	efx_max_aux_sends_ = 0;
	al_alc_symbols_->alcGetIntegerv(al_device_, ALC_MAX_AUXILIARY_SENDS, 1, &efx_max_aux_sends_);
}

void AlApiContextImpl::get_efx_attributes()
{
	get_efx_major_version_attribute();
	get_efx_minor_version_attribute();
	get_efx_max_aux_sends_attribute();
}

void AlApiContextImpl::log_efx_attributes()
{
	logger_->info("");
	logger_->info("EFX attributes:");

	string_buffer_.clear();
	string_buffer_ += al_api::Strings::indent;
	string_buffer_ += al_api::Strings::major_version;
	string_buffer_ += al_api::Strings::space_equals_space;
	string_buffer_ += to_string(efx_major_version_);
	logger_->info(string_buffer_.c_str());

	string_buffer_.clear();
	string_buffer_ += al_api::Strings::indent;
	string_buffer_ += al_api::Strings::minor_version;
	string_buffer_ += al_api::Strings::space_equals_space;
	string_buffer_ += to_string(efx_minor_version_);
	logger_->info(string_buffer_.c_str());

	string_buffer_.clear();
	string_buffer_ += al_api::Strings::indent;
	string_buffer_ += al_api::Strings::max_auxiliary_sends;
	string_buffer_ += al_api::Strings::space_equals_space;
	string_buffer_ += to_string(efx_max_aux_sends_);
	logger_->info(string_buffer_.c_str());
}

bool AlApiContextImpl::has_extension(
	const char* extension_name,
	const char* extensions_string) noexcept
{
	if (!extension_name ||
		extension_name[0] == '\0' ||
		!extensions_string ||
		extensions_string[0] == '\0')
	{
		return false;
	}

	while (extensions_string[0] != '\0')
	{
		auto names_it = extensions_string++;
		auto name_it = extension_name;

		while (true)
		{
			const auto names_ch = (*names_it++);
			const auto name_ch = (*name_it++);

			if (names_ch != name_ch)
			{
				if (name_ch == '\0' &&
					(names_ch == '\0' || names_ch == ' '))
				{
					return true;
				}

				break;
			}

			if (names_ch == '\0' && name_ch == '\0')
			{
				return true;
			}
		}
	}

	return false;
}

void AlApiContextImpl::make_extensions()
{
	extensions_string_.clear();

	constexpr auto eax_2_0_name = std::string_view{"EAX2.0"};
	constexpr auto eax_3_0_name = std::string_view{"EAX3.0"};
	constexpr auto eax_4_0_name = std::string_view{"EAX4.0"};
	constexpr auto eax_5_0_name = std::string_view{"EAX5.0"};

	constexpr std::string_view eax_extensions_names[] =
	{
		eax_2_0_name,
		eax_3_0_name,
		eax_4_0_name,
		eax_5_0_name,
	};

	auto extensions_string_size = std::size_t{};

	if (eaxx_)
	{
		for (const auto& eax_extensions_name : eax_extensions_names)
		{
			extensions_string_size += eax_extensions_name.size() + 1;
		}
	}

	const auto al_extensions_string = al_al_symbols_->alGetString(AL_EXTENSIONS);

	auto al_extensions_view = std::string_view{};

	if (al_extensions_string)
	{
		al_extensions_view = al_extensions_string;
		extensions_string_size += al_extensions_view.size() + 1;
	}

	extensions_string_.reserve(extensions_string_size);

	if (eaxx_)
	{
		for (const auto& eax_extensions_name : eax_extensions_names)
		{
			const auto is_already_supported = has_extension(
				eax_extensions_name.data(),
				al_extensions_string
			);

			if (!is_already_supported)
			{
				extensions_string_ += eax_extensions_name;
				extensions_string_ += ' ';
			}
		}
	}

	extensions_string_ += al_extensions_view;

	if (!extensions_string_.empty() && extensions_string_.back() == ' ')
	{
		extensions_string_.resize(extensions_string_.size() - 1);
	}
}

void AlApiContextImpl::log_context_info()
{
	logger_->info("");
	al_api::log_string(logger_, "Version", al_al_symbols_->alGetString(AL_VERSION));
	al_api::log_string(logger_, "Renderer", al_al_symbols_->alGetString(AL_RENDERER));
	al_api::log_string(logger_, "Vendor", al_al_symbols_->alGetString(AL_VENDOR));

	logger_->info("");
	logger_->info("Extensions:");
	al_api::log_extensions(logger_, al_al_symbols_->alGetString(AL_EXTENSIONS));

	logger_->info("");
	logger_->info("Actual attributes:");
	al_api::log_context_attribute_list(logger_, attributes_.data());

	log_efx_attributes();
}

void AlApiContextImpl::make_efx_symbols()
{
	logger_->info("");
	logger_->info("Load EFX v1.0 symbols.");

	auto al_loader = g_al_api.get_al_loader();

	if (!al_loader)
	{
		fail("Null AL loader.");
	}

	al_efx_symbols_ = al_loader->resolve_efx_symbols();
}

void AlApiContextImpl::make_efx_symbol_map()
{
	constexpr auto capacity = sizeof(AlEfxSymbols) / sizeof(void*);

	al_efx_symbol_map_.clear();
	al_efx_symbol_map_.reserve(capacity);

	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGenEffects, reinterpret_cast<void*>(al_efx_symbols_->alGenEffects));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alDeleteEffects, reinterpret_cast<void*>(al_efx_symbols_->alDeleteEffects));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alIsEffect, reinterpret_cast<void*>(al_efx_symbols_->alIsEffect));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alEffecti, reinterpret_cast<void*>(al_efx_symbols_->alEffecti));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alEffectiv, reinterpret_cast<void*>(al_efx_symbols_->alEffectiv));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alEffectf, reinterpret_cast<void*>(al_efx_symbols_->alEffectf));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alEffectfv, reinterpret_cast<void*>(al_efx_symbols_->alEffectfv));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGetEffecti, reinterpret_cast<void*>(al_efx_symbols_->alGetEffecti));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGetEffectiv, reinterpret_cast<void*>(al_efx_symbols_->alGetEffectiv));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGetEffectf, reinterpret_cast<void*>(al_efx_symbols_->alGetEffectf));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGetEffectfv, reinterpret_cast<void*>(al_efx_symbols_->alGetEffectfv));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGenFilters, reinterpret_cast<void*>(al_efx_symbols_->alGenFilters));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alDeleteFilters, reinterpret_cast<void*>(al_efx_symbols_->alDeleteFilters));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alIsFilter, reinterpret_cast<void*>(al_efx_symbols_->alIsFilter));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alFilteri, reinterpret_cast<void*>(al_efx_symbols_->alFilteri));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alFilteriv, reinterpret_cast<void*>(al_efx_symbols_->alFilteriv));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alFilterf, reinterpret_cast<void*>(al_efx_symbols_->alFilterf));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alFilterfv, reinterpret_cast<void*>(al_efx_symbols_->alFilterfv));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGetFilteri, reinterpret_cast<void*>(al_efx_symbols_->alGetFilteri));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGetFilteriv, reinterpret_cast<void*>(al_efx_symbols_->alGetFilteriv));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGetFilterf, reinterpret_cast<void*>(al_efx_symbols_->alGetFilterf));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGetFilterfv, reinterpret_cast<void*>(al_efx_symbols_->alGetFilterfv));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGenAuxiliaryEffectSlots, reinterpret_cast<void*>(al_efx_symbols_->alGenAuxiliaryEffectSlots));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alDeleteAuxiliaryEffectSlots, reinterpret_cast<void*>(al_efx_symbols_->alDeleteAuxiliaryEffectSlots));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alIsAuxiliaryEffectSlot, reinterpret_cast<void*>(al_efx_symbols_->alIsAuxiliaryEffectSlot));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alAuxiliaryEffectSloti, reinterpret_cast<void*>(al_efx_symbols_->alAuxiliaryEffectSloti));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alAuxiliaryEffectSlotiv, reinterpret_cast<void*>(al_efx_symbols_->alAuxiliaryEffectSlotiv));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alAuxiliaryEffectSlotf, reinterpret_cast<void*>(al_efx_symbols_->alAuxiliaryEffectSlotf));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alAuxiliaryEffectSlotfv, reinterpret_cast<void*>(al_efx_symbols_->alAuxiliaryEffectSlotfv));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGetAuxiliaryEffectSloti, reinterpret_cast<void*>(al_efx_symbols_->alGetAuxiliaryEffectSloti));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGetAuxiliaryEffectSlotiv, reinterpret_cast<void*>(al_efx_symbols_->alGetAuxiliaryEffectSlotiv));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGetAuxiliaryEffectSlotf, reinterpret_cast<void*>(al_efx_symbols_->alGetAuxiliaryEffectSlotf));
	al_efx_symbol_map_.emplace(AlEfxSymbolsNames::alGetAuxiliaryEffectSlotfv, reinterpret_cast<void*>(al_efx_symbols_->alGetAuxiliaryEffectSlotfv));
}

void AlApiContextImpl::make_eaxx()
{
	auto create_param = EaxxCreateParam{};
	create_param.al_efx_symbols = al_efx_symbols_.get();
	eaxx_ = eaxefx::make_eaxx(create_param);
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

AlApiContextUPtr make_al_api_context()
{
	return std::make_unique<AlApiContextImpl>();
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


} // eaxefx
