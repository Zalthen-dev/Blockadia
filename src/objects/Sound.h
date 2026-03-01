#pragma once

#include <raylib.h>

#include "Instance.h"
#include "lua.h"
#include "lualib.h"

// name has to be different since raylib defines its own "Sound" struct

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
		music = {};
		music.looping = false;
	}

	const char* ClassName() const override {
        return "Sound";
    }

	void Update() {
		UpdateMusicStream(music);
	}

	void Play() {
		Update();
		if (IsLoaded && IsMusicValid(music)) {
			PlayMusicStream(music);
		}
	}

	void Stop() {
		Update();
		if (IsLoaded && IsMusicValid(music)) {
			StopMusicStream(music);
		}
	}

	void Pause() {
		Update();
		if (IsLoaded && IsMusicValid(music)) {
			PauseMusicStream(music);
		}
	}

	void Resume() {
		Update();
		if (IsLoaded && IsMusicValid(music)) {
			ResumeMusicStream(music);
		}
	}

	static int l_Play(lua_State* L) {
		ObjectSound* self = *(ObjectSound**)luaL_checkudata(L, 1, "Instance");
		if (!self) {
			return 0;
		}

		self->Play();
		
		return 0;
	}

	static int l_Stop(lua_State* L) {
		ObjectSound* self = *(ObjectSound**)luaL_checkudata(L, 1, "Instance");
		if (!self) {
			return 0;
		}

		self->Stop();

		return 0;
	}

	static int l_Pause(lua_State* L) {
		ObjectSound* self = *(ObjectSound**)luaL_checkudata(L, 1, "Instance");
		if (!self) {
			return 0;
		}

		self->Pause();

		return 0;
	}

	static int l_Resume(lua_State* L) {
		ObjectSound* self = *(ObjectSound**)luaL_checkudata(L, 1, "Instance");
		if (!self) {
			return 0;
		}

		self->Resume();

		return 0;
	}

	bool LuaGet(lua_State* L, const char* key) override {

		// functions
		if (std::strcmp(key, "Play") == 0) {
			lua_pushcfunction(L, l_Play, "Sound:Play");
			return true;
		}

		if (std::strcmp(key, "Stop") == 0) {
			lua_pushcfunction(L, l_Stop, "Sound:Stop");
			return true;
		}

		// mutable

		if (std::strcmp(key, "SoundId") == 0) {
			lua_pushstring(L, SoundId.data());
			return true;
		}

		if (std::strcmp(key, "PlaybackSpeed") == 0) {
			lua_pushnumber(L, static_cast<double>(PlaybackSpeed));
			return true;
		}

		if (std::strcmp(key, "Volume") == 0) {
			lua_pushnumber(L, static_cast<double>(Volume));
			return true;
		}

		if (std::strcmp(key, "Looped") == 0) {
			lua_pushboolean(L, music.looping);
			return true;
		}

		// immutable
		// except for sound.Playing because its mutable

		if (std::strcmp(key, "IsLoaded") == 0) {
			lua_pushboolean(L, IsLoaded);
			return true;
		}

		if (std::strcmp(key, "IsPaused") == 0) {
			if (IsLoaded && IsMusicStreamPlaying(music)) {
				lua_pushboolean(L, !IsMusicStreamPlaying(music));
			} else {
				lua_pushboolean(L, true);
			}

			return true;
		}

		if (std::strcmp(key, "IsPlaying") == 0 || std::strcmp(key, "Playing") == 0) {
			if (IsLoaded && IsMusicStreamPlaying(music)) {
				lua_pushboolean(L, IsMusicStreamPlaying(music));
			} else {
				lua_pushboolean(L, false);
			}

			return true;
		}

		if (std::strcmp(key, "TimeLength") == 0) {
			if (IsLoaded && IsMusicStreamPlaying(music)) {
				lua_pushnumber(L, GetMusicTimeLength(music));
			} else {
				lua_pushnumber(L, 0.0);
			}
			
			return true;
		}

		if (std::strcmp(key, "TimePosition") == 0) {
			if (IsLoaded && IsMusicStreamPlaying(music)) {
				lua_pushnumber(L, GetMusicTimePlayed(music));
			} else {
				lua_pushnumber(L, 0.0);
			}

			return true;
		}

		return Instance::LuaGet(L, key);
	}
	
	bool LuaSet(lua_State* L, const char* key, int valueIndex) override {
		if (std::strcmp(key, "SoundId") == 0) {
			SoundId = luaL_checkstring(L, valueIndex);

			if (IsLoaded && IsMusicValid(music)) {
				UnloadMusicStream(music);
				IsLoaded = false;
			}

			music = LoadMusicStream(SoundId.data());
			music.looping = Looped;

			IsLoaded = IsMusicValid(music);

			if (IsLoaded) {
				SetMusicVolume(music, Volume);
				SetMusicPitch(music, PlaybackSpeed);
			}

			return true;
		}

		if (std::strcmp(key, "PlaybackSpeed") == 0) {
			PlaybackSpeed = static_cast<float>(luaL_checknumber(L, valueIndex));

			if (IsLoaded && IsMusicValid(music)) {
				SetMusicPitch(music, PlaybackSpeed);
			}
			
			return true;
		}

		if (std::strcmp(key, "Volume") == 0) {
			Volume = static_cast<float>(luaL_checknumber(L, valueIndex));

			if (IsLoaded && IsMusicValid(music)) {
				SetMusicVolume(music, Volume);
			}

			return true;
		}

		if (std::strcmp(key, "Looped") == 0) {
			Looped = luaL_checkboolean(L, valueIndex);
			music.looping = Looped;
			return true;
		}

		if (std::strcmp(key, "Playing") == 0) {
			bool startPlayback = luaL_checkboolean(L, valueIndex);

			if (startPlayback) {
				Play();
			} else {
				Pause();
			}

			return true;
		}

		return Instance::LuaSet(L, key, valueIndex);
	}
};