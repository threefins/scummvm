/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//=============================================================================
//
// ACSOUND - AGS sound system wrapper
//
//=============================================================================

#include "ags/engine/media/audio/audio_defines.h"
#include "ags/engine/media/audio/sound.h"
#include "ags/engine/media/audio/sound_clip.h"
#include "ags/engine/media/audio/clip_my_midi.h"
#include "ags/shared/core/asset_manager.h"
#include "audio/mods/impulsetracker.h"
#include "audio/mods/mod_xm_s3m.h"
#include "audio/mods/protracker.h"
#include "audio/decoders/mp3.h"
#include "audio/decoders/vorbis.h"
#include "audio/decoders/wave.h"
#include "ags/globals.h"

namespace AGS3 {

SOUNDCLIP *my_load_wave(const AssetPath &asset_name, bool loop) {
	Common::SeekableReadStream *data = _GP(AssetMgr)->OpenAssetStream(asset_name.Name, asset_name.Filter);
	if (data) {
		Audio::AudioStream *audioStream = Audio::makeWAVStream(data, DisposeAfterUse::YES);
		return new SoundClipWave<MUS_WAVE>(audioStream, loop);
	} else {
		return nullptr;
	}
}

SOUNDCLIP *my_load_static_mp3(const AssetPath &asset_name, bool loop) {
#ifdef USE_MAD
	Common::SeekableReadStream *data = _GP(AssetMgr)->OpenAssetStream(asset_name.Name, asset_name.Filter);
	if (data) {
		Audio::AudioStream *audioStream = Audio::makeMP3Stream(data, DisposeAfterUse::YES);
		return new SoundClipWave<MUS_MP3>(audioStream, loop);
	} else {
		return nullptr;
	}
#else
	return nullptr;
#endif
}

SOUNDCLIP *my_load_mp3(const AssetPath &asset_name, bool loop) {
	return my_load_static_mp3(asset_name, loop);
}

SOUNDCLIP *my_load_static_ogg(const AssetPath &asset_name, bool loop) {
#ifdef USE_VORBIS
	Common::SeekableReadStream *data = _GP(AssetMgr)->OpenAssetStream(asset_name.Name, asset_name.Filter);
	if (data) {
		Audio::AudioStream *audioStream = Audio::makeVorbisStream(data, DisposeAfterUse::YES);
		return new SoundClipWave<MUS_OGG>(audioStream, loop);
	} else {
		return nullptr;
	}
#else
	return nullptr;
#endif
}

SOUNDCLIP *my_load_ogg(const AssetPath &asset_name, bool loop) {
	return my_load_static_ogg(asset_name, loop);
}

SOUNDCLIP *my_load_midi(const AssetPath &asset_name, bool loop) {
	Common::SeekableReadStream *data = _GP(AssetMgr)->OpenAssetStream(asset_name.Name, asset_name.Filter);
	if (data) {
		return new MYMIDI(data, loop);
	} else {
		return nullptr;
	}
}

SOUNDCLIP *my_load_mod(const AssetPath &asset_name, bool loop) {
	Common::SeekableReadStream *data = _GP(AssetMgr)->OpenAssetStream(asset_name.Name, asset_name.Filter);
	if (data) {
		// determine the file extension
		size_t lastDot = asset_name.Name.FindCharReverse('.');
		if (lastDot == AGS::Shared::String::NoIndex || lastDot == asset_name.Name.GetLength() - 1) {
			delete data;
			return nullptr;
		}
		// get the first char of the extension
		char charAfterDot = toupper(asset_name.Name[lastDot + 1]);

		// use the appropriate loader
		Audio::AudioStream *audioStream = nullptr;
		if (charAfterDot == 'I') {
			// Impulse Tracker
#ifdef USE_MIKMOD
			audioStream = Audio::makeImpulseTrackerStream(data, DisposeAfterUse::YES);
#else
			warning("MIKMOD support was not compiled in! Skipping Impulse Tracker audio");
			audioStream = Audio::makeSilentAudioStream(22050, true);
			delete data;
#endif
		} else if (charAfterDot == 'X') {
			audioStream = Audio::makeModXmS3mStream(data, DisposeAfterUse::YES);
		} else if (charAfterDot == 'S') {
			audioStream = Audio::makeModXmS3mStream(data, DisposeAfterUse::YES);
		} else if (charAfterDot == 'M') {
			audioStream = Audio::makeProtrackerStream(data);
		} else {
			warning("MOD file format not recognized");
			delete data;
			return nullptr;
		}

		return new SoundClipWave<MUS_MOD>(audioStream, loop);
	} else {
		return nullptr;
	}
}

} // namespace AGS3
