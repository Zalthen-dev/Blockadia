#pragma once

#include <raylib.h>

#include "Instance.h"
#include "lua.h"
#include "lualib.h"

// name has to be different since raylib defines its own "Sound" struct


#define AreStringsEqual(a, b) (std::strcmp(a, b) == 0)

struct ObjectSound : Instance {
	std::string Name = "Sound";
	std::string SoundId = "";

	Music music;

	float PlaybackSpeed = 1.0f;
	float Volume = 0.5f;

	bool IsLoaded = false;
	//bool IsPaused = false;
	//bool IsPlaying = false;
	bool Looped = false;

	ObjectSound() {
		music.looping = false;
	}

	const char* ClassName() const override {
        return "Sound";
    }

	static int l_Play(lua_State* L) {
		ObjectSound* self = *(ObjectSound**)luaL_checkudata(L, 1, "Instance");
		if (!self) {
			return 0;
		}

		if (!IsMusicStreamPlaying(self->music)) {
			PlayMusicStream(self->music);
		}
		
		return 0;
	}

	static int l_Stop(lua_State* L) {
		ObjectSound* self = *(ObjectSound**)luaL_checkudata(L, 1, "Instance");
		if (!self) {
			return 0;
		}

		if (IsMusicStreamPlaying(self->music)) {
			StopMusicStream(self->music);
		}

		return 0;
	}

	static int l_Pause(lua_State* L) {
		ObjectSound* self = *(ObjectSound**)luaL_checkudata(L, 1, "Instance");
		if (!self) {
			return 0;
		}

		if (IsMusicStreamPlaying(self->music)) {
			PauseMusicStream(self->music);
		}

		return 0;
	}

	static int l_Resume(lua_State* L) {
		ObjectSound* self = *(ObjectSound**)luaL_checkudata(L, 1, "Instance");
		if (!self) {
			return 0;
		}

		if (IsMusicStreamPlaying(self->music)) {
			ResumeMusicStream(self->music);
		}

		return 0;
	}

	bool LuaGet(lua_State* L, const char* key) override {

		// functions
		if (AreStringsEqual(key, "Play")) {
			lua_pushcfunction(L, l_Play, "Sound:Play");
			return true;
		}

		if (AreStringsEqual(key, "Stop")) {
			lua_pushcfunction(L, l_Stop, "Sound:Stop");
			return true;
		}

		// mutable

		if (AreStringsEqual(key, "SoundId")) {
			lua_pushstring(L, SoundId.data());
			return true;
		}

		if (AreStringsEqual(key, "PlaybackSpeed")) {
			lua_pushnumber(L, static_cast<double>(PlaybackSpeed));
			return true;
		}

		if (AreStringsEqual(key, "Volume")) {
			lua_pushnumber(L, static_cast<double>(Volume));
			return true;
		}

		if (AreStringsEqual(key, "Looped")) {
			lua_pushboolean(L, music.looping);
			return true;
		}

		// immutable
		// except for sound.Playing because its mutable

		if (AreStringsEqual(key, "IsLoaded")) {
			lua_pushboolean(L, IsLoaded);
			return true;
		}

		if (AreStringsEqual(key, "IsPaused")) {
			lua_pushboolean(L, !IsMusicStreamPlaying(music));
			return true;
		}

		if (AreStringsEqual(key, "IsPlaying") || AreStringsEqual(key, "Playing")) {
			lua_pushboolean(L, IsMusicStreamPlaying(music));
			return true;
		}

		if (AreStringsEqual(key, "TimeLength")) {
			lua_pushboolean(L, GetMusicTimeLength(music));
			return true;
		}

		if (AreStringsEqual(key, "TimePosition")) {
			lua_pushboolean(L, GetMusicTimePlayed(music));
			return true;
		}

		return Instance::LuaGet(L, key);
	}
	
	bool LuaSet(lua_State* L, const char* key, int valueIndex) override {
		if (AreStringsEqual(key, "SoundId")) {
			SoundId = luaL_checkstring(L, valueIndex);

			IsLoaded = false;
			UnloadMusicStream(music);

			music = LoadMusicStream(SoundId.data());
			music.looping = Looped;

			IsLoaded = IsMusicValid(music);

			return true;
		}

		if (AreStringsEqual(key, "PlaybackSpeed")) {
			PlaybackSpeed = static_cast<float>(luaL_checknumber(L, valueIndex));
			SetMusicPitch(music, PlaybackSpeed);
			return true;
		}

		if (AreStringsEqual(key, "Volume")) {
			Volume = static_cast<float>(luaL_checknumber(L, valueIndex));
			SetMusicVolume(music, Volume);
			return true;
		}

		if (AreStringsEqual(key, "Volume")) {
			music.looping = luaL_checkboolean(L, valueIndex);
			Looped = music.looping;
			return true;
		}

		if (AreStringsEqual(key, "Playing")) {
			bool stopPlayback = !luaL_checkboolean(L, valueIndex);

			if (stopPlayback) {
				StopMusicStream(music);
			} else {
				PlayMusicStream(music);
			}
		}

		return Instance::LuaSet(L, key, valueIndex);
	}
};