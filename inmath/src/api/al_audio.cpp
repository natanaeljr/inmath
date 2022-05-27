#include "./al_audio.hpp"

#include <memory>
#include <string>
#include <optional>

#include <glbinding/gl33core/gl.h>
using namespace gl;
#include <AL/al.h>
#include <AL/alc.h>
#include <drlibs/dr_wav.h>

#include "./log.hpp"
#include "./file.hpp"
#include "./unique_num.hpp"

using namespace std::string_literals;

/// Read WAV audio file and load it into an OpenAL buffer
auto load_wav_audio(const std::string& audiopath) -> std::optional<ALBuffer>
{
  DEBUG("Loading audio {}", audiopath);
  const std::string filepath = INMATH_ASSETS_PATH + "/audio/"s + audiopath;
  auto audio = read_file_to_string(filepath);
  if (!audio) { ERROR("Failed to read audio '{}'", audiopath); return std::nullopt; }
  unsigned int channels, samples;
  drwav_uint64 pcm_frame_count;
  drwav_int16* data = drwav_open_memory_and_read_pcm_frames_s16(audio->data(), audio->size(), &channels, &samples, &pcm_frame_count, nullptr);
  if (!data) { ERROR("Failed to load audio path ({})", filepath); return std::nullopt; }
  size_t size = (pcm_frame_count * channels * sizeof(float));
  int err;
  ALuint abo;
  alGenBuffers(1, &abo);
  alBufferData(abo, AL_FORMAT_MONO16, data, size, samples);
  if ((err = alGetError()) != AL_NO_ERROR) {
    ERROR("Failed to buffer audio {}, error {}", audiopath, err);
    alDeleteBuffers(1, &abo);
    drwav_free(data, NULL);
    return std::nullopt;
  } 
  drwav_free(data, NULL);
  return ALBuffer{ abo };
}

/// Create a new Audio Source object
ALSource create_audio_source(float gain)
{
  ALuint aso;
  alGenSources(1, &aso);
  alSourcef(aso, AL_PITCH, 1.0f);
  alSourcef(aso, AL_GAIN, gain);
  alSource3f(aso, AL_POSITION, 0.0f, 0.0f, 0.0f);
  alSource3f(aso, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
  alSourcei(aso, AL_LOOPING, AL_FALSE);
  return ALSource{ aso };
}

