/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_H
#define _FLUIDSYNTH_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BUILD_SHARED_LIBS 1

#if (BUILD_SHARED_LIBS == 0)
    #define FLUIDSYNTH_API // building static lib? no visibility control then
#elif defined(WIN32)
    #if defined(FLUIDSYNTH_NOT_A_DLL)
        #define FLUIDSYNTH_API
    #elif defined(FLUIDSYNTH_DLL_EXPORTS)
        #define FLUIDSYNTH_API __declspec(dllexport)
    #else
        #define FLUIDSYNTH_API __declspec(dllimport)
    #endif

#elif defined(MACOS9)
#define FLUIDSYNTH_API __declspec(export)

#elif defined(__GNUC__)
#define FLUIDSYNTH_API __attribute__ ((visibility ("default")))

#else
#define FLUIDSYNTH_API

#endif

#if defined(__GNUC__) || defined(__clang__)
#    define FLUID_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER) && _MSC_VER > 1200
#    define FLUID_DEPRECATED __declspec(deprecated)
#else
#    define FLUID_DEPRECATED
#endif


/**
 * @file fluidsynth.h
 * @brief FluidSynth is a real-time synthesizer designed for SoundFont(R) files.
 *
 * This is the header of the fluidsynth library and contains the
 * synthesizer's public API.
 *
 * Depending on how you want to use or extend the synthesizer you
 * will need different API functions. You probably do not need all
 * of them. Here is what you might want to do:
 *
 * - Embedded synthesizer: create a new synthesizer and send MIDI
 *   events to it. The sound goes directly to the audio output of
 *   your system.
 *
 * - Plugin synthesizer: create a synthesizer and send MIDI events
 *   but pull the audio back into your application.
 *
 * - SoundFont plugin: create a new type of "SoundFont" and allow
 *   the synthesizer to load your type of SoundFonts.
 *
 * - MIDI input: Create a MIDI handler to read the MIDI input on your
 *   machine and send the MIDI events directly to the synthesizer.
 *
 * - MIDI files: Open MIDI files and send the MIDI events to the
 *   synthesizer.
 *
 * - Command lines: You can send textual commands to the synthesizer.
 *
 * SoundFont(R) is a registered trademark of E-mu Systems, Inc.
 */



#ifdef __cplusplus
}
#endif

#endif /* _FLUIDSYNTH_H */
/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_TYPES_H
#define _FLUIDSYNTH_TYPES_H



#ifdef __cplusplus
extern "C" {
#endif


/**
 * @file types.h
 * @brief Type declarations
 */

typedef struct _fluid_hashtable_t fluid_settings_t;             /**< Configuration settings instance */
typedef struct _fluid_synth_t fluid_synth_t;                    /**< Synthesizer instance */
typedef struct _fluid_voice_t fluid_voice_t;                    /**< Synthesis voice instance */
typedef struct _fluid_sfloader_t fluid_sfloader_t;              /**< SoundFont loader plugin */
typedef struct _fluid_sfont_t fluid_sfont_t;                    /**< SoundFont */
typedef struct _fluid_preset_t fluid_preset_t;                  /**< SoundFont preset */
typedef struct _fluid_sample_t fluid_sample_t;                  /**< SoundFont sample */
typedef struct _fluid_mod_t fluid_mod_t;                        /**< SoundFont modulator */
typedef struct _fluid_audio_driver_t fluid_audio_driver_t;      /**< Audio driver instance */
typedef struct _fluid_file_renderer_t fluid_file_renderer_t;    /**< Audio file renderer instance */
typedef struct _fluid_player_t fluid_player_t;                  /**< MIDI player instance */
typedef struct _fluid_midi_event_t fluid_midi_event_t;          /**< MIDI event */
typedef struct _fluid_midi_driver_t fluid_midi_driver_t;        /**< MIDI driver instance */
typedef struct _fluid_midi_router_t fluid_midi_router_t;        /**< MIDI router instance */
typedef struct _fluid_midi_router_rule_t fluid_midi_router_rule_t;      /**< MIDI router rule */
typedef struct _fluid_hashtable_t fluid_cmd_hash_t;             /**< Command handler hash table */
typedef struct _fluid_shell_t fluid_shell_t;                    /**< Command shell */
typedef struct _fluid_server_t fluid_server_t;                  /**< TCP/IP shell server instance */
typedef struct _fluid_event_t fluid_event_t;                    /**< Sequencer event */
typedef struct _fluid_sequencer_t fluid_sequencer_t;            /**< Sequencer instance */
typedef struct _fluid_ramsfont_t fluid_ramsfont_t;              /**< RAM SoundFont */
typedef struct _fluid_rampreset_t fluid_rampreset_t;            /**< RAM SoundFont preset */
typedef struct _fluid_cmd_handler_t fluid_cmd_handler_t;        /**< Shell Command Handler */
typedef struct _fluid_ladspa_fx_t fluid_ladspa_fx_t;            /**< LADSPA effects instance */
typedef struct _fluid_file_callbacks_t fluid_file_callbacks_t;  /**< Callback struct to perform custom file loading of soundfonts */

typedef int fluid_istream_t;    /**< Input stream descriptor */
typedef int fluid_ostream_t;    /**< Output stream descriptor */

typedef short fluid_seq_id_t; /**< Unique client IDs used by the sequencer and #fluid_event_t, obtained by fluid_sequencer_register_client() and fluid_sequencer_register_fluidsynth() */

#ifdef __cplusplus
}
#endif

#endif /* _FLUIDSYNTH_TYPES_H */
/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_SETTINGS_H
#define _FLUIDSYNTH_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file settings.h
 * @brief Synthesizer settings
 * @defgroup SettingsFunctions Functions for settings management
 *
 * To create a synthesizer object you will have to specify its
 * settings. These settings are stored in a fluid_settings_t object.
 * @code
 *     void
 *     my_synthesizer ()
 *     {
 *       fluid_settings_t *settings;
 *       fluid_synth_t *synth;
 *       fluid_audio_driver_t *adriver;
 *
 *       settings = new_fluid_settings ();
 *       fluid_settings_setstr(settings, "audio.driver", "alsa");
 *       // ... change settings ...
 *       synth = new_fluid_synth (settings);
 *       adriver = new_fluid_audio_driver (settings, synth);
 *       // ...
 *     }
 * @endcode
 * @sa @ref CreatingSettings
 */

/**
 * Hint FLUID_HINT_BOUNDED_BELOW indicates that the LowerBound field
 * of the FLUID_PortRangeHint should be considered meaningful. The
 * value in this field should be considered the (inclusive) lower
 * bound of the valid range. If FLUID_HINT_SAMPLE_RATE is also
 * specified then the value of LowerBound should be multiplied by the
 * sample rate.
 */
#define FLUID_HINT_BOUNDED_BELOW   0x1

/** Hint FLUID_HINT_BOUNDED_ABOVE indicates that the UpperBound field
   of the FLUID_PortRangeHint should be considered meaningful. The
   value in this field should be considered the (inclusive) upper
   bound of the valid range. If FLUID_HINT_SAMPLE_RATE is also
   specified then the value of UpperBound should be multiplied by the
   sample rate. */
#define FLUID_HINT_BOUNDED_ABOVE   0x2

/**
 * Hint FLUID_HINT_TOGGLED indicates that the data item should be
 * considered a Boolean toggle. Data less than or equal to zero should
 * be considered `off' or `false,' and data above zero should be
 * considered `on' or `true.' FLUID_HINT_TOGGLED may not be used in
 * conjunction with any other hint.
 */
#define FLUID_HINT_TOGGLED         0x4

#define FLUID_HINT_OPTIONLIST      0x02         /**< Setting is a list of string options */


/**
 * Settings type
 *
 * Each setting has a defined type: numeric (double), integer, string or a
 * set of values. The type of each setting can be retrieved using the
 * function fluid_settings_get_type()
 */
enum fluid_types_enum
{
    FLUID_NO_TYPE = -1, /**< Undefined type */
    FLUID_NUM_TYPE,     /**< Numeric (double) */
    FLUID_INT_TYPE,     /**< Integer */
    FLUID_STR_TYPE,     /**< String */
    FLUID_SET_TYPE      /**< Set of values */
};


FLUIDSYNTH_API fluid_settings_t *new_fluid_settings(void);
FLUIDSYNTH_API void delete_fluid_settings(fluid_settings_t *settings);

FLUIDSYNTH_API
int fluid_settings_get_type(fluid_settings_t *settings, const char *name);

FLUIDSYNTH_API
int fluid_settings_get_hints(fluid_settings_t *settings, const char *name, int *val);

FLUIDSYNTH_API
int fluid_settings_is_realtime(fluid_settings_t *settings, const char *name);

FLUIDSYNTH_API
int fluid_settings_setstr(fluid_settings_t *settings, const char *name, const char *str);

FLUIDSYNTH_API
int fluid_settings_copystr(fluid_settings_t *settings, const char *name, char *str, int len);

FLUIDSYNTH_API
int fluid_settings_dupstr(fluid_settings_t *settings, const char *name, char **str);

FLUIDSYNTH_API
int fluid_settings_getstr_default(fluid_settings_t *settings, const char *name, char **def);

FLUIDSYNTH_API
int fluid_settings_str_equal(fluid_settings_t *settings, const char *name, const char *value);

FLUIDSYNTH_API
int fluid_settings_setnum(fluid_settings_t *settings, const char *name, double val);

FLUIDSYNTH_API
int fluid_settings_getnum(fluid_settings_t *settings, const char *name, double *val);

FLUIDSYNTH_API
int fluid_settings_getnum_default(fluid_settings_t *settings, const char *name, double *val);

FLUIDSYNTH_API
int fluid_settings_getnum_range(fluid_settings_t *settings, const char *name,
                                double *min, double *max);

FLUIDSYNTH_API
int fluid_settings_setint(fluid_settings_t *settings, const char *name, int val);

FLUIDSYNTH_API
int fluid_settings_getint(fluid_settings_t *settings, const char *name, int *val);

FLUIDSYNTH_API
int fluid_settings_getint_default(fluid_settings_t *settings, const char *name, int *val);

FLUIDSYNTH_API
int fluid_settings_getint_range(fluid_settings_t *settings, const char *name,
                                int *min, int *max);

/**
 * Callback function type used with fluid_settings_foreach_option()
 * @param data User defined data pointer
 * @param name Setting name
 * @param option A string option for this setting (iterates through the list)
 */
typedef void (*fluid_settings_foreach_option_t)(void *data, const char *name, const char *option);

FLUIDSYNTH_API
void fluid_settings_foreach_option(fluid_settings_t *settings,
                                   const char *name, void *data,
                                   fluid_settings_foreach_option_t func);
FLUIDSYNTH_API
int fluid_settings_option_count(fluid_settings_t *settings, const char *name);
FLUIDSYNTH_API char *fluid_settings_option_concat(fluid_settings_t *settings,
        const char *name,
        const char *separator);

/**
 * Callback function type used with fluid_settings_foreach()
 * @param data User defined data pointer
 * @param name Setting name
 * @param type Setting type (#fluid_types_enum)
 */
typedef void (*fluid_settings_foreach_t)(void *data, const char *name, int type);

FLUIDSYNTH_API
void fluid_settings_foreach(fluid_settings_t *settings, void *data,
                            fluid_settings_foreach_t func);

#ifdef __cplusplus
}
#endif

#endif /* _FLUIDSYNTH_SETTINGS_H */
/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_SYNTH_H
#define _FLUIDSYNTH_SYNTH_H


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @file synth.h
 * @brief Embeddable SoundFont synthesizer
 *
 * You create a new synthesizer with new_fluid_synth() and you destroy
 * if with delete_fluid_synth(). Use the settings structure to specify
 * the synthesizer characteristics.
 *
 * You have to load a SoundFont in order to hear any sound. For that
 * you use the fluid_synth_sfload() function.
 *
 * You can use the audio driver functions described below to open
 * the audio device and create a background audio thread.
 *
 * The API for sending MIDI events is probably what you expect:
 * fluid_synth_noteon(), fluid_synth_noteoff(), ...
 */


FLUIDSYNTH_API fluid_synth_t *new_fluid_synth(fluid_settings_t *settings);
FLUIDSYNTH_API void delete_fluid_synth(fluid_synth_t *synth);
FLUIDSYNTH_API fluid_settings_t *fluid_synth_get_settings(fluid_synth_t *synth);

/* MIDI channel messages */

FLUIDSYNTH_API int fluid_synth_noteon(fluid_synth_t *synth, int chan, int key, int vel);
FLUIDSYNTH_API int fluid_synth_noteoff(fluid_synth_t *synth, int chan, int key);
FLUIDSYNTH_API int fluid_synth_cc(fluid_synth_t *synth, int chan, int ctrl, int val);
FLUIDSYNTH_API int fluid_synth_get_cc(fluid_synth_t *synth, int chan, int ctrl, int *pval);
FLUIDSYNTH_API int fluid_synth_sysex(fluid_synth_t *synth, const char *data, int len,
                                     char *response, int *response_len, int *handled, int dryrun);
FLUIDSYNTH_API int fluid_synth_pitch_bend(fluid_synth_t *synth, int chan, int val);
FLUIDSYNTH_API int fluid_synth_get_pitch_bend(fluid_synth_t *synth, int chan, int *ppitch_bend);
FLUIDSYNTH_API int fluid_synth_pitch_wheel_sens(fluid_synth_t *synth, int chan, int val);
FLUIDSYNTH_API int fluid_synth_get_pitch_wheel_sens(fluid_synth_t *synth, int chan, int *pval);
FLUIDSYNTH_API int fluid_synth_program_change(fluid_synth_t *synth, int chan, int program);
FLUIDSYNTH_API int fluid_synth_channel_pressure(fluid_synth_t *synth, int chan, int val);
FLUIDSYNTH_API int fluid_synth_key_pressure(fluid_synth_t *synth, int chan, int key, int val);
FLUIDSYNTH_API int fluid_synth_bank_select(fluid_synth_t *synth, int chan, int bank);
FLUIDSYNTH_API int fluid_synth_sfont_select(fluid_synth_t *synth, int chan, int sfont_id);
FLUIDSYNTH_API
int fluid_synth_program_select(fluid_synth_t *synth, int chan, int sfont_id,
                               int bank_num, int preset_num);
FLUIDSYNTH_API int
fluid_synth_program_select_by_sfont_name(fluid_synth_t *synth, int chan,
        const char *sfont_name, int bank_num,
        int preset_num);
FLUIDSYNTH_API
int fluid_synth_get_program(fluid_synth_t *synth, int chan, int *sfont_id,
                            int *bank_num, int *preset_num);
FLUIDSYNTH_API int fluid_synth_unset_program(fluid_synth_t *synth, int chan);
FLUIDSYNTH_API int fluid_synth_program_reset(fluid_synth_t *synth);
FLUIDSYNTH_API int fluid_synth_system_reset(fluid_synth_t *synth);

FLUIDSYNTH_API int fluid_synth_all_notes_off(fluid_synth_t *synth, int chan);
FLUIDSYNTH_API int fluid_synth_all_sounds_off(fluid_synth_t *synth, int chan);

/**
 * The midi channel type used by fluid_synth_set_channel_type()
 */
enum fluid_midi_channel_type
{
    CHANNEL_TYPE_MELODIC = 0, /**< Melodic midi channel */
    CHANNEL_TYPE_DRUM = 1 /**< Drum midi channel */
};

FLUIDSYNTH_API int fluid_synth_set_channel_type(fluid_synth_t *synth, int chan, int type);


/* Low level access */
FLUIDSYNTH_API fluid_preset_t *fluid_synth_get_channel_preset(fluid_synth_t *synth, int chan);
FLUIDSYNTH_API int fluid_synth_start(fluid_synth_t *synth, unsigned int id,
                                     fluid_preset_t *preset, int audio_chan,
                                     int midi_chan, int key, int vel);
FLUIDSYNTH_API int fluid_synth_stop(fluid_synth_t *synth, unsigned int id);


/* SoundFont management */

FLUIDSYNTH_API
int fluid_synth_sfload(fluid_synth_t *synth, const char *filename, int reset_presets);
FLUIDSYNTH_API int fluid_synth_sfreload(fluid_synth_t *synth, int id);
FLUIDSYNTH_API int fluid_synth_sfunload(fluid_synth_t *synth, int id, int reset_presets);
FLUIDSYNTH_API int fluid_synth_add_sfont(fluid_synth_t *synth, fluid_sfont_t *sfont);
FLUIDSYNTH_API int fluid_synth_remove_sfont(fluid_synth_t *synth, fluid_sfont_t *sfont);
FLUIDSYNTH_API int fluid_synth_sfcount(fluid_synth_t *synth);
FLUIDSYNTH_API fluid_sfont_t *fluid_synth_get_sfont(fluid_synth_t *synth, unsigned int num);
FLUIDSYNTH_API fluid_sfont_t *fluid_synth_get_sfont_by_id(fluid_synth_t *synth, int id);
FLUIDSYNTH_API fluid_sfont_t *fluid_synth_get_sfont_by_name(fluid_synth_t *synth,
        const char *name);
FLUIDSYNTH_API int fluid_synth_set_bank_offset(fluid_synth_t *synth, int sfont_id, int offset);
FLUIDSYNTH_API int fluid_synth_get_bank_offset(fluid_synth_t *synth, int sfont_id);


/* Reverb  */


FLUIDSYNTH_API int fluid_synth_set_reverb(fluid_synth_t *synth, double roomsize,
        double damping, double width, double level);
FLUIDSYNTH_API int fluid_synth_set_reverb_roomsize(fluid_synth_t *synth, double roomsize);
FLUIDSYNTH_API int fluid_synth_set_reverb_damp(fluid_synth_t *synth, double damping);
FLUIDSYNTH_API int fluid_synth_set_reverb_width(fluid_synth_t *synth, double width);
FLUIDSYNTH_API int fluid_synth_set_reverb_level(fluid_synth_t *synth, double level);

FLUIDSYNTH_API void fluid_synth_set_reverb_on(fluid_synth_t *synth, int on);
FLUIDSYNTH_API double fluid_synth_get_reverb_roomsize(fluid_synth_t *synth);
FLUIDSYNTH_API double fluid_synth_get_reverb_damp(fluid_synth_t *synth);
FLUIDSYNTH_API double fluid_synth_get_reverb_level(fluid_synth_t *synth);
FLUIDSYNTH_API double fluid_synth_get_reverb_width(fluid_synth_t *synth);


/* Chorus */

/**
 * Chorus modulation waveform type.
 */
enum fluid_chorus_mod
{
    FLUID_CHORUS_MOD_SINE = 0,            /**< Sine wave chorus modulation */
    FLUID_CHORUS_MOD_TRIANGLE = 1         /**< Triangle wave chorus modulation */
};

FLUIDSYNTH_API int fluid_synth_set_chorus(fluid_synth_t *synth, int nr, double level,
        double speed, double depth_ms, int type);
FLUIDSYNTH_API int fluid_synth_set_chorus_nr(fluid_synth_t *synth, int nr);
FLUIDSYNTH_API int fluid_synth_set_chorus_level(fluid_synth_t *synth, double level);
FLUIDSYNTH_API int fluid_synth_set_chorus_speed(fluid_synth_t *synth, double speed);
FLUIDSYNTH_API int fluid_synth_set_chorus_depth(fluid_synth_t *synth, double depth_ms);
FLUIDSYNTH_API int fluid_synth_set_chorus_type(fluid_synth_t *synth, int type);

FLUIDSYNTH_API void fluid_synth_set_chorus_on(fluid_synth_t *synth, int on);
FLUIDSYNTH_API int fluid_synth_get_chorus_nr(fluid_synth_t *synth);
FLUIDSYNTH_API double fluid_synth_get_chorus_level(fluid_synth_t *synth);
FLUIDSYNTH_API double fluid_synth_get_chorus_speed(fluid_synth_t *synth);
FLUIDSYNTH_API double fluid_synth_get_chorus_depth(fluid_synth_t *synth);
FLUIDSYNTH_API int fluid_synth_get_chorus_type(fluid_synth_t *synth); /* see fluid_chorus_mod */


/* Audio and MIDI channels */

FLUIDSYNTH_API int fluid_synth_count_midi_channels(fluid_synth_t *synth);
FLUIDSYNTH_API int fluid_synth_count_audio_channels(fluid_synth_t *synth);
FLUIDSYNTH_API int fluid_synth_count_audio_groups(fluid_synth_t *synth);
FLUIDSYNTH_API int fluid_synth_count_effects_channels(fluid_synth_t *synth);
FLUIDSYNTH_API int fluid_synth_count_effects_groups(fluid_synth_t *synth);


/* Synthesis parameters */

FLUIDSYNTH_API void fluid_synth_set_sample_rate(fluid_synth_t *synth, float sample_rate);
FLUIDSYNTH_API void fluid_synth_set_gain(fluid_synth_t *synth, float gain);
FLUIDSYNTH_API float fluid_synth_get_gain(fluid_synth_t *synth);
FLUIDSYNTH_API int fluid_synth_set_polyphony(fluid_synth_t *synth, int polyphony);
FLUIDSYNTH_API int fluid_synth_get_polyphony(fluid_synth_t *synth);
FLUIDSYNTH_API int fluid_synth_get_active_voice_count(fluid_synth_t *synth);
FLUIDSYNTH_API int fluid_synth_get_internal_bufsize(fluid_synth_t *synth);

FLUIDSYNTH_API
int fluid_synth_set_interp_method(fluid_synth_t *synth, int chan, int interp_method);

/**
 * Synthesis interpolation method.
 */
enum fluid_interp
{
    FLUID_INTERP_NONE = 0,        /**< No interpolation: Fastest, but questionable audio quality */
    FLUID_INTERP_LINEAR = 1,      /**< Straight-line interpolation: A bit slower, reasonable audio quality */
    FLUID_INTERP_4THORDER = 4,    /**< Fourth-order interpolation, good quality, the default */
    FLUID_INTERP_7THORDER = 7,    /**< Seventh-order interpolation */

    FLUID_INTERP_DEFAULT = FLUID_INTERP_4THORDER, /**< Default interpolation method */
    FLUID_INTERP_HIGHEST = FLUID_INTERP_7THORDER, /**< Highest interpolation method */
};

/* Generator interface */

FLUIDSYNTH_API int fluid_synth_set_gen(fluid_synth_t *synth, int chan,
                                       int param, float value);
FLUIDSYNTH_API float fluid_synth_get_gen(fluid_synth_t *synth, int chan, int param);


/* Tuning */

FLUIDSYNTH_API
int fluid_synth_activate_key_tuning(fluid_synth_t *synth, int bank, int prog,
                                    const char *name, const double *pitch, int apply);
FLUIDSYNTH_API
int fluid_synth_activate_octave_tuning(fluid_synth_t *synth, int bank, int prog,
                                       const char *name, const double *pitch, int apply);
FLUIDSYNTH_API
int fluid_synth_tune_notes(fluid_synth_t *synth, int bank, int prog,
                           int len, const int *keys, const double *pitch, int apply);
FLUIDSYNTH_API
int fluid_synth_activate_tuning(fluid_synth_t *synth, int chan, int bank, int prog,
                                int apply);
FLUIDSYNTH_API
int fluid_synth_deactivate_tuning(fluid_synth_t *synth, int chan, int apply);
FLUIDSYNTH_API void fluid_synth_tuning_iteration_start(fluid_synth_t *synth);
FLUIDSYNTH_API
int fluid_synth_tuning_iteration_next(fluid_synth_t *synth, int *bank, int *prog);
FLUIDSYNTH_API int fluid_synth_tuning_dump(fluid_synth_t *synth, int bank, int prog,
        char *name, int len, double *pitch);

/* Misc */

FLUIDSYNTH_API double fluid_synth_get_cpu_load(fluid_synth_t *synth);
FLUIDSYNTH_API const char *fluid_synth_error(fluid_synth_t *synth);


/* Default modulators */

/**
 * Enum used with fluid_synth_add_default_mod() to specify how to handle duplicate modulators.
 */
enum fluid_synth_add_mod
{
    FLUID_SYNTH_OVERWRITE,        /**< Overwrite any existing matching modulator */
    FLUID_SYNTH_ADD,              /**< Add (sum) modulator amounts */
};

FLUIDSYNTH_API int fluid_synth_add_default_mod(fluid_synth_t *synth, const fluid_mod_t *mod, int mode);
FLUIDSYNTH_API int fluid_synth_remove_default_mod(fluid_synth_t *synth, const fluid_mod_t *mod);


/*
 * Synthesizer plugin
 *
 * To create a synthesizer plugin, create the synthesizer as
 * explained above. Once the synthesizer is created you can call
 * any of the functions below to get the audio.
 */

FLUIDSYNTH_API int fluid_synth_write_s16(fluid_synth_t *synth, int len,
        void *lout, int loff, int lincr,
        void *rout, int roff, int rincr);
FLUIDSYNTH_API int fluid_synth_write_float(fluid_synth_t *synth, int len,
        void *lout, int loff, int lincr,
        void *rout, int roff, int rincr);
FLUID_DEPRECATED FLUIDSYNTH_API int fluid_synth_nwrite_float(fluid_synth_t *synth, int len,
        float **left, float **right,
        float **fx_left, float **fx_right);
FLUIDSYNTH_API int fluid_synth_process(fluid_synth_t *synth, int len,
                                       int nfx, float *fx[],
                                       int nout, float *out[]);


/* Synthesizer's interface to handle SoundFont loaders */

FLUIDSYNTH_API void fluid_synth_add_sfloader(fluid_synth_t *synth, fluid_sfloader_t *loader);
FLUIDSYNTH_API fluid_voice_t *fluid_synth_alloc_voice(fluid_synth_t *synth,
        fluid_sample_t *sample,
        int channum, int key, int vel);
FLUIDSYNTH_API void fluid_synth_start_voice(fluid_synth_t *synth, fluid_voice_t *voice);
FLUIDSYNTH_API void fluid_synth_get_voicelist(fluid_synth_t *synth,
        fluid_voice_t *buf[], int bufsize, int ID);
FLUIDSYNTH_API int fluid_synth_handle_midi_event(void *data, fluid_midi_event_t *event);

/**
 * Specifies the type of filter to use for the custom IIR filter
 */
enum fluid_iir_filter_type
{
    FLUID_IIR_DISABLED = 0, /**< Custom IIR filter is not operating */
    FLUID_IIR_LOWPASS, /**< Custom IIR filter is operating as low-pass filter */
    FLUID_IIR_HIGHPASS, /**< Custom IIR filter is operating as high-pass filter */
    FLUID_IIR_LAST /**< @internal Value defines the count of filter types (#fluid_iir_filter_type) @warning This symbol is not part of the public API and ABI stability guarantee and may change at any time! */
};

/**
 * Specifies optional settings to use for the custom IIR filter
 */
enum fluid_iir_filter_flags
{
    FLUID_IIR_Q_LINEAR = 1 << 0, /**< The Soundfont spec requires the filter Q to be interpreted in dB. If this flag is set the filter Q is instead assumed to be in a linear range */
    FLUID_IIR_Q_ZERO_OFF = 1 << 1, /**< If this flag the filter is switched off if Q == 0 (prior to any transformation) */
    FLUID_IIR_NO_GAIN_AMP = 1 << 2 /**< The Soundfont spec requires to correct the gain of the filter depending on the filter's Q. If this flag is set the filter gain will not be corrected. */
};

FLUIDSYNTH_API int fluid_synth_set_custom_filter(fluid_synth_t *, int type, int flags);


/* LADSPA */

FLUIDSYNTH_API fluid_ladspa_fx_t *fluid_synth_get_ladspa_fx(fluid_synth_t *synth);


/* API: Poly mono mode */

/** Interface to poly/mono mode variables
 *
 * Channel mode bits OR-ed together so that it matches with the midi spec: poly omnion (0), mono omnion (1), poly omnioff (2), mono omnioff (3)
 */
enum fluid_channel_mode_flags
{
    FLUID_CHANNEL_POLY_OFF = 0x01, /**< if flag is set, the basic channel is in mono on state, if not set poly is on */
    FLUID_CHANNEL_OMNI_OFF = 0x02, /**< if flag is set, the basic channel is in omni off state, if not set omni is on */
};

/** Indicates the breath mode a channel is set to */
enum fluid_channel_breath_flags
{
    FLUID_CHANNEL_BREATH_POLY = 0x10,  /**< when channel is poly, this flag indicates that the default velocity to initial attenuation modulator is replaced by a breath to initial attenuation modulator */
    FLUID_CHANNEL_BREATH_MONO = 0x20,  /**< when channel is mono, this flag indicates that the default velocity to initial attenuation modulator is replaced by a breath modulator */
    FLUID_CHANNEL_BREATH_SYNC = 0x40,  /**< when channel is mono, this flag indicates that the breath controler(MSB)triggers noteon/noteoff on the running note */
};

/** Indicates the mode a basic channel is set to */
enum fluid_basic_channel_modes
{
    FLUID_CHANNEL_MODE_MASK = (FLUID_CHANNEL_OMNI_OFF | FLUID_CHANNEL_POLY_OFF), /**< Mask Poly and Omni bits of #fluid_channel_mode_flags, usually only used internally */
    FLUID_CHANNEL_MODE_OMNION_POLY = FLUID_CHANNEL_MODE_MASK & (~FLUID_CHANNEL_OMNI_OFF & ~FLUID_CHANNEL_POLY_OFF), /**< corresponds to MIDI mode 0 */
    FLUID_CHANNEL_MODE_OMNION_MONO = FLUID_CHANNEL_MODE_MASK & (~FLUID_CHANNEL_OMNI_OFF & FLUID_CHANNEL_POLY_OFF), /**< corresponds to MIDI mode 1 */
    FLUID_CHANNEL_MODE_OMNIOFF_POLY = FLUID_CHANNEL_MODE_MASK & (FLUID_CHANNEL_OMNI_OFF & ~FLUID_CHANNEL_POLY_OFF), /**< corresponds to MIDI mode 2 */
    FLUID_CHANNEL_MODE_OMNIOFF_MONO = FLUID_CHANNEL_MODE_MASK & (FLUID_CHANNEL_OMNI_OFF | FLUID_CHANNEL_POLY_OFF), /**< corresponds to MIDI mode 3 */
    FLUID_CHANNEL_MODE_LAST /**< @internal Value defines the count of basic channel modes (#fluid_basic_channel_modes) @warning This symbol is not part of the public API and ABI stability guarantee and may change at any time! */
};

FLUIDSYNTH_API int fluid_synth_reset_basic_channel(fluid_synth_t *synth, int chan);

FLUIDSYNTH_API int  fluid_synth_get_basic_channel(fluid_synth_t *synth, int chan,
        int *basic_chan_out,
        int *mode_chan_out,
        int *basic_val_out);
FLUIDSYNTH_API int fluid_synth_set_basic_channel(fluid_synth_t *synth, int chan, int mode, int val);

/** Interface to mono legato mode
 *
 * Indicates the legato mode a channel is set to
 * n1,n2,n3,.. is a legato passage. n1 is the first note, and n2,n3,n4 are played legato with previous note. */
enum fluid_channel_legato_mode
{
    FLUID_CHANNEL_LEGATO_MODE_RETRIGGER, /**< Mode 0 - Release previous note, start a new note */
    FLUID_CHANNEL_LEGATO_MODE_MULTI_RETRIGGER, /**< Mode 1 - On contiguous notes retrigger in attack section using current value, shape attack using current dynamic and make use of previous voices if any */
    FLUID_CHANNEL_LEGATO_MODE_LAST /**< @internal Value defines the count of legato modes (#fluid_channel_legato_mode) @warning This symbol is not part of the public API and ABI stability guarantee and may change at any time! */
};

FLUIDSYNTH_API int fluid_synth_set_legato_mode(fluid_synth_t *synth, int chan, int legatomode);
FLUIDSYNTH_API int fluid_synth_get_legato_mode(fluid_synth_t *synth, int chan, int  *legatomode);

/** Interface to portamento mode
 *
 * Indicates the portamento mode a channel is set to
 */
enum fluid_channel_portamento_mode
{
    FLUID_CHANNEL_PORTAMENTO_MODE_EACH_NOTE, /**< Mode 0 - Portamento on each note (staccato or legato) */
    FLUID_CHANNEL_PORTAMENTO_MODE_LEGATO_ONLY, /**< Mode 1 - Portamento only on legato note */
    FLUID_CHANNEL_PORTAMENTO_MODE_STACCATO_ONLY, /**< Mode 2 - Portamento only on staccato note */
    FLUID_CHANNEL_PORTAMENTO_MODE_LAST /**< @internal Value defines the count of portamento modes (#fluid_channel_portamento_mode) @warning This symbol is not part of the public API and ABI stability guarantee and may change at any time! */
};

FLUIDSYNTH_API int fluid_synth_set_portamento_mode(fluid_synth_t *synth,
        int chan, int portamentomode);
FLUIDSYNTH_API int fluid_synth_get_portamento_mode(fluid_synth_t *synth,
        int chan, int   *portamentomode);

/* Interface to breath mode   */
FLUIDSYNTH_API int fluid_synth_set_breath_mode(fluid_synth_t *synth,
        int chan, int breathmode);
FLUIDSYNTH_API int fluid_synth_get_breath_mode(fluid_synth_t *synth,
        int chan, int  *breathmode);


#ifdef __cplusplus
}
#endif

#endif /* _FLUIDSYNTH_SYNTH_H */
/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_SFONT_H
#define _FLUIDSYNTH_SFONT_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @file sfont.h
 * @brief SoundFont plugins
 *
 * It is possible to add new SoundFont loaders to the
 * synthesizer. This API allows for virtual SoundFont files to be loaded
 * and synthesized, which may not actually be SoundFont files, as long as they
 * can be represented by the SoundFont synthesis model.
 *
 * To add a new SoundFont loader to the synthesizer, call
 * fluid_synth_add_sfloader() and pass a pointer to an
 * #fluid_sfloader_t instance created by new_fluid_sfloader().
 * On creation, you must specify a callback function \p load
 * that will be called for every file attempting to load it and
 * if successful returns a #fluid_sfont_t instance, or NULL if it fails.
 *
 * The #fluid_sfont_t structure contains a callback to obtain the
 * name of the SoundFont. It contains two functions to iterate
 * though the contained presets, and one function to obtain a
 * preset corresponding to a bank and preset number. This
 * function should return a #fluid_preset_t instance.
 *
 * The #fluid_preset_t instance contains some functions to obtain
 * information from the preset (name, bank, number). The most
 * important callback is the noteon function. The noteon function
 * is called by fluidsynth internally and
 * should call fluid_synth_alloc_voice() for every sample that has
 * to be played. fluid_synth_alloc_voice() expects a pointer to a
 * #fluid_sample_t instance and returns a pointer to the opaque
 * #fluid_voice_t structure. To set or increment the values of a
 * generator, use fluid_voice_gen_set() or fluid_voice_gen_incr(). When you are
 * finished initializing the voice call fluid_voice_start() to
 * start playing the synthesis voice.
 */

/**
 * Some notification enums for presets and samples.
 */
enum
{
    FLUID_PRESET_SELECTED,                /**< Preset selected notify */
    FLUID_PRESET_UNSELECTED,              /**< Preset unselected notify */
    FLUID_SAMPLE_DONE                     /**< Sample no longer needed notify */
};

/**
 * Indicates the type of a sample used by the _fluid_sample_t::sampletype field.
 */
enum fluid_sample_type
{
    FLUID_SAMPLETYPE_MONO = 0x1, /**< Used for mono samples */
    FLUID_SAMPLETYPE_RIGHT = 0x2, /**< Used for right samples of a stereo pair */
    FLUID_SAMPLETYPE_LEFT = 0x4, /**< Used for left samples of a stereo pair */
    FLUID_SAMPLETYPE_LINKED = 0x8, /**< Currently not used */
    FLUID_SAMPLETYPE_OGG_VORBIS = 0x10, /**< Used for Ogg Vorbis compressed samples @since 1.1.7 */
    FLUID_SAMPLETYPE_ROM = 0x8000 /**< Indicates ROM samples, causes sample to be ignored */
};


/**
 * Method to load an instrument file (does not actually need to be a real file name,
 * could be another type of string identifier that the \a loader understands).
 * @param loader SoundFont loader
 * @param filename File name or other string identifier
 * @return The loaded instrument file (SoundFont) or NULL if an error occured.
 */
typedef fluid_sfont_t *(*fluid_sfloader_load_t)(fluid_sfloader_t *loader, const char *filename);

/**
 * The free method should free the memory allocated for a fluid_sfloader_t instance in
 * addition to any private data. Any custom user provided cleanup function must ultimately call
 * delete_fluid_sfloader() to ensure proper cleanup of the #fluid_sfloader_t struct. If no private data
 * needs to be freed, setting this to delete_fluid_sfloader() is sufficient.
 * @param loader SoundFont loader
 */
typedef void (*fluid_sfloader_free_t)(fluid_sfloader_t *loader);


FLUIDSYNTH_API fluid_sfloader_t *new_fluid_sfloader(fluid_sfloader_load_t load, fluid_sfloader_free_t free);
FLUIDSYNTH_API void delete_fluid_sfloader(fluid_sfloader_t *loader);

FLUIDSYNTH_API fluid_sfloader_t *new_fluid_defsfloader(fluid_settings_t *settings);

/**
 * Opens the file or memory indicated by \c filename in binary read mode.
 * \c filename matches the string provided during the fluid_synth_sfload() call.
 *
 * @return returns a file handle on success, NULL otherwise
 */
typedef void *(* fluid_sfloader_callback_open_t)(const char *filename);

/**
 * Reads \c count bytes to the specified buffer \c buf.
 *
 * @return returns #FLUID_OK if exactly \c count bytes were successfully read, else returns #FLUID_FAILED and leaves \a buf unmodified.
 */
typedef int (* fluid_sfloader_callback_read_t)(void *buf, int count, void *handle);

/**
 * Same purpose and behaviour as fseek.
 *
 * @param origin either \c SEEK_SET, \c SEEK_CUR or \c SEEK_END
 *
 * @return returns #FLUID_OK if the seek was successfully performed while not seeking beyond a buffer or file, #FLUID_FAILED otherwise
 */
typedef int (* fluid_sfloader_callback_seek_t)(void *handle, long offset, int origin);

/**
 * Closes the handle returned by #fluid_sfloader_callback_open_t and frees used ressources.
 *
 * @return returns #FLUID_OK on success, #FLUID_FAILED on error
 */
typedef int (* fluid_sfloader_callback_close_t)(void *handle);

/** @return returns current file offset or #FLUID_FAILED on error */
typedef long (* fluid_sfloader_callback_tell_t)(void *handle);


FLUIDSYNTH_API int fluid_sfloader_set_callbacks(fluid_sfloader_t *loader,
        fluid_sfloader_callback_open_t open,
        fluid_sfloader_callback_read_t read,
        fluid_sfloader_callback_seek_t seek,
        fluid_sfloader_callback_tell_t tell,
        fluid_sfloader_callback_close_t close);

FLUIDSYNTH_API int fluid_sfloader_set_data(fluid_sfloader_t *loader, void *data);
FLUIDSYNTH_API void *fluid_sfloader_get_data(fluid_sfloader_t *loader);



/**
 * Method to return the name of a virtual SoundFont.
 * @param sfont Virtual SoundFont
 * @return The name of the virtual SoundFont.
 */
typedef const char *(*fluid_sfont_get_name_t)(fluid_sfont_t *sfont);

/**
 * Get a virtual SoundFont preset by bank and program numbers.
 * @param sfont Virtual SoundFont
 * @param bank MIDI bank number (0-16383)
 * @param prenum MIDI preset number (0-127)
 * @return Should return an allocated virtual preset or NULL if it could not
 *   be found.
 */
typedef fluid_preset_t *(*fluid_sfont_get_preset_t)(fluid_sfont_t *sfont, int bank, int prenum);

/**
 * Start virtual SoundFont preset iteration method.
 * @param sfont Virtual SoundFont
 *
 * Starts/re-starts virtual preset iteration in a SoundFont.
 */
typedef void (*fluid_sfont_iteration_start_t)(fluid_sfont_t *sfont);

/**
 * Virtual SoundFont preset iteration function.
 * @param sfont Virtual SoundFont
 * @param preset Caller supplied uninitialized buffer to fill in with current preset information
 * @return NULL when no more presets are available, otherwise the a pointer to the current preset
 *
 * Should store preset information to the caller supplied \a preset structure
 * and advance the internal iteration state to the next preset for subsequent
 * calls.
 */
typedef fluid_preset_t *(*fluid_sfont_iteration_next_t)(fluid_sfont_t *sfont);

/**
 * Method to free a virtual SoundFont bank. Any custom user provided cleanup function must ultimately call
 * delete_fluid_sfont() to ensure proper cleanup of the #fluid_sfont_t struct. If no private data
 * needs to be freed, setting this to delete_fluid_sfont() is sufficient.
 * @param sfont Virtual SoundFont to free.
 * @return Should return 0 when it was able to free all resources or non-zero
 *   if some of the samples could not be freed because they are still in use,
 *   in which case the free will be tried again later, until success.
 */
typedef int (*fluid_sfont_free_t)(fluid_sfont_t *sfont);


FLUIDSYNTH_API fluid_sfont_t *new_fluid_sfont(fluid_sfont_get_name_t get_name,
        fluid_sfont_get_preset_t get_preset,
        fluid_sfont_iteration_start_t iter_start,
        fluid_sfont_iteration_next_t iter_next,
        fluid_sfont_free_t free);

FLUIDSYNTH_API int delete_fluid_sfont(fluid_sfont_t *sfont);

FLUIDSYNTH_API int fluid_sfont_set_data(fluid_sfont_t *sfont, void *data);
FLUIDSYNTH_API void *fluid_sfont_get_data(fluid_sfont_t *sfont);

FLUIDSYNTH_API int fluid_sfont_get_id(fluid_sfont_t *sfont);
FLUIDSYNTH_API const char *fluid_sfont_get_name(fluid_sfont_t *sfont);
FLUIDSYNTH_API fluid_preset_t *fluid_sfont_get_preset(fluid_sfont_t *sfont, int bank, int prenum);
FLUIDSYNTH_API void fluid_sfont_iteration_start(fluid_sfont_t *sfont);
FLUIDSYNTH_API fluid_preset_t *fluid_sfont_iteration_next(fluid_sfont_t *sfont);

/**
 * Method to get a virtual SoundFont preset name.
 * @param preset Virtual SoundFont preset
 * @return Should return the name of the preset.  The returned string must be
 *   valid for the duration of the virtual preset (or the duration of the
 *   SoundFont, in the case of preset iteration).
 */
typedef const char *(*fluid_preset_get_name_t)(fluid_preset_t *preset);

/**
 * Method to get a virtual SoundFont preset MIDI bank number.
 * @param preset Virtual SoundFont preset
 * @param return The bank number of the preset
 */
typedef int (*fluid_preset_get_banknum_t)(fluid_preset_t *preset);

/**
 * Method to get a virtual SoundFont preset MIDI program number.
 * @param preset Virtual SoundFont preset
 * @param return The program number of the preset
 */
typedef int (*fluid_preset_get_num_t)(fluid_preset_t *preset);

/**
 * Method to handle a noteon event (synthesize the instrument).
 * @param preset Virtual SoundFont preset
 * @param synth Synthesizer instance
 * @param chan MIDI channel number of the note on event
 * @param key MIDI note number (0-127)
 * @param vel MIDI velocity (0-127)
 * @return #FLUID_OK on success (0) or #FLUID_FAILED (-1) otherwise
 *
 * This method may be called from within synthesis context and therefore
 * should be as efficient as possible and not perform any operations considered
 * bad for realtime audio output (memory allocations and other OS calls).
 *
 * Call fluid_synth_alloc_voice() for every sample that has
 * to be played. fluid_synth_alloc_voice() expects a pointer to a
 * #fluid_sample_t structure and returns a pointer to the opaque
 * #fluid_voice_t structure. To set or increment the values of a
 * generator, use fluid_voice_gen_set() or fluid_voice_gen_incr(). When you are
 * finished initializing the voice call fluid_voice_start() to
 * start playing the synthesis voice.  Starting with FluidSynth 1.1.0 all voices
 * created will be started at the same time.
 */
typedef int (*fluid_preset_noteon_t)(fluid_preset_t *preset, fluid_synth_t *synth, int chan, int key, int vel);

/**
 * Method to free a virtual SoundFont preset. Any custom user provided cleanup function must ultimately call
 * delete_fluid_preset() to ensure proper cleanup of the #fluid_preset_t struct. If no private data
 * needs to be freed, setting this to delete_fluid_preset() is sufficient.
 * @param preset Virtual SoundFont preset
 * @return Should return 0
 */
typedef void (*fluid_preset_free_t)(fluid_preset_t *preset);

FLUIDSYNTH_API fluid_preset_t *new_fluid_preset(fluid_sfont_t *parent_sfont,
        fluid_preset_get_name_t get_name,
        fluid_preset_get_banknum_t get_bank,
        fluid_preset_get_num_t get_num,
        fluid_preset_noteon_t noteon,
        fluid_preset_free_t free);
FLUIDSYNTH_API void delete_fluid_preset(fluid_preset_t *preset);

FLUIDSYNTH_API int fluid_preset_set_data(fluid_preset_t *preset, void *data);
FLUIDSYNTH_API void *fluid_preset_get_data(fluid_preset_t *preset);

FLUIDSYNTH_API const char *fluid_preset_get_name(fluid_preset_t *preset);
FLUIDSYNTH_API int fluid_preset_get_banknum(fluid_preset_t *preset);
FLUIDSYNTH_API int fluid_preset_get_num(fluid_preset_t *preset);
FLUIDSYNTH_API fluid_sfont_t *fluid_preset_get_sfont(fluid_preset_t *preset);

FLUIDSYNTH_API fluid_sample_t *new_fluid_sample(void);
FLUIDSYNTH_API void delete_fluid_sample(fluid_sample_t *sample);
FLUIDSYNTH_API size_t fluid_sample_sizeof(void);

FLUIDSYNTH_API int fluid_sample_set_name(fluid_sample_t *sample, const char *name);
FLUIDSYNTH_API int fluid_sample_set_sound_data(fluid_sample_t *sample,
        short *data,
        char *data24,
        unsigned int nbframes,
        unsigned int sample_rate,
        short copy_data);

FLUIDSYNTH_API int fluid_sample_set_loop(fluid_sample_t *sample, unsigned int loop_start, unsigned int loop_end);
FLUIDSYNTH_API int fluid_sample_set_pitch(fluid_sample_t *sample, int root_key, int fine_tune);

#ifdef __cplusplus
}
#endif

#endif /* _FLUIDSYNTH_SFONT_H */
/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_EVENT_H
#define _FLUIDSYNTH_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file event.h
 * @brief Sequencer event functions and defines.
 *
 * Functions and constants for creating/processing sequencer events.
 */

/**
 * Sequencer event type enumeration.
 */
enum fluid_seq_event_type
{
    FLUID_SEQ_NOTE = 0,		/**< Note event with duration */
    FLUID_SEQ_NOTEON,		/**< Note on event */
    FLUID_SEQ_NOTEOFF,		/**< Note off event */
    FLUID_SEQ_ALLSOUNDSOFF,	/**< All sounds off event */
    FLUID_SEQ_ALLNOTESOFF,	/**< All notes off event */
    FLUID_SEQ_BANKSELECT,		/**< Bank select message */
    FLUID_SEQ_PROGRAMCHANGE,	/**< Program change message */
    FLUID_SEQ_PROGRAMSELECT,	/**< Program select message */
    FLUID_SEQ_PITCHBEND,		/**< Pitch bend message */
    FLUID_SEQ_PITCHWHEELSENS,	/**< Pitch wheel sensitivity set message @since 1.1.0 was mispelled previously */
    FLUID_SEQ_MODULATION,		/**< Modulation controller event */
    FLUID_SEQ_SUSTAIN,		/**< Sustain controller event */
    FLUID_SEQ_CONTROLCHANGE,	/**< MIDI control change event */
    FLUID_SEQ_PAN,		/**< Stereo pan set event */
    FLUID_SEQ_VOLUME,		/**< Volume set event */
    FLUID_SEQ_REVERBSEND,		/**< Reverb send set event */
    FLUID_SEQ_CHORUSSEND,		/**< Chorus send set event */
    FLUID_SEQ_TIMER,		/**< Timer event (useful for giving a callback at a certain time) */
    FLUID_SEQ_ANYCONTROLCHANGE,	/**< Any control change message (only internally used for remove_events) */
    FLUID_SEQ_CHANNELPRESSURE,    /**< Channel aftertouch event @since 1.1.0 */
    FLUID_SEQ_KEYPRESSURE,        /**< Polyphonic aftertouch event @since 2.0.0 */
    FLUID_SEQ_SYSTEMRESET,        /**< System reset event @since 1.1.0 */
    FLUID_SEQ_UNREGISTERING,      /**< Called when a sequencer client is being unregistered. @since 1.1.0 */
#ifndef __DOXYGEN__
    FLUID_SEQ_LASTEVENT		/**< @internal Defines the count of events enums @warning This symbol is not part of the public API and ABI stability guarantee and may change at any time! */
#endif
};

/* Event alloc/free */
FLUIDSYNTH_API fluid_event_t *new_fluid_event(void);
FLUIDSYNTH_API void delete_fluid_event(fluid_event_t *evt);

/* Initializing events */
FLUIDSYNTH_API void fluid_event_set_source(fluid_event_t *evt, fluid_seq_id_t src);
FLUIDSYNTH_API void fluid_event_set_dest(fluid_event_t *evt, fluid_seq_id_t dest);

/* Timer events */
FLUIDSYNTH_API void fluid_event_timer(fluid_event_t *evt, void *data);

/* Note events */
FLUIDSYNTH_API void fluid_event_note(fluid_event_t *evt, int channel,
                                     short key, short vel,
                                     unsigned int duration);

FLUIDSYNTH_API void fluid_event_noteon(fluid_event_t *evt, int channel, short key, short vel);
FLUIDSYNTH_API void fluid_event_noteoff(fluid_event_t *evt, int channel, short key);
FLUIDSYNTH_API void fluid_event_all_sounds_off(fluid_event_t *evt, int channel);
FLUIDSYNTH_API void fluid_event_all_notes_off(fluid_event_t *evt, int channel);

/* Instrument selection */
FLUIDSYNTH_API void fluid_event_bank_select(fluid_event_t *evt, int channel, short bank_num);
FLUIDSYNTH_API void fluid_event_program_change(fluid_event_t *evt, int channel, short preset_num);
FLUIDSYNTH_API void fluid_event_program_select(fluid_event_t *evt, int channel, unsigned int sfont_id, short bank_num, short preset_num);

/* Real-time generic instrument controllers */
FLUIDSYNTH_API
void fluid_event_control_change(fluid_event_t *evt, int channel, short control, short val);

/* Real-time instrument controllers shortcuts */
FLUIDSYNTH_API void fluid_event_pitch_bend(fluid_event_t *evt, int channel, int val);
FLUIDSYNTH_API void fluid_event_pitch_wheelsens(fluid_event_t *evt, int channel, short val);
FLUIDSYNTH_API void fluid_event_modulation(fluid_event_t *evt, int channel, short val);
FLUIDSYNTH_API void fluid_event_sustain(fluid_event_t *evt, int channel, short val);
FLUIDSYNTH_API void fluid_event_pan(fluid_event_t *evt, int channel, short val);
FLUIDSYNTH_API void fluid_event_volume(fluid_event_t *evt, int channel, short val);
FLUIDSYNTH_API void fluid_event_reverb_send(fluid_event_t *evt, int channel, short val);
FLUIDSYNTH_API void fluid_event_chorus_send(fluid_event_t *evt, int channel, short val);

FLUIDSYNTH_API void fluid_event_key_pressure(fluid_event_t *evt, int channel, short key, short val);
FLUIDSYNTH_API void fluid_event_channel_pressure(fluid_event_t *evt, int channel, short val);
FLUIDSYNTH_API void fluid_event_system_reset(fluid_event_t *evt);


/* Only for removing events */
FLUIDSYNTH_API void fluid_event_any_control_change(fluid_event_t *evt, int channel);

/* Only when unregistering clients */
FLUIDSYNTH_API void fluid_event_unregistering(fluid_event_t *evt);

/* Accessing event data */
FLUIDSYNTH_API int fluid_event_get_type(fluid_event_t *evt);
FLUIDSYNTH_API fluid_seq_id_t fluid_event_get_source(fluid_event_t *evt);
FLUIDSYNTH_API fluid_seq_id_t fluid_event_get_dest(fluid_event_t *evt);
FLUIDSYNTH_API int fluid_event_get_channel(fluid_event_t *evt);
FLUIDSYNTH_API short fluid_event_get_key(fluid_event_t *evt);
FLUIDSYNTH_API short fluid_event_get_velocity(fluid_event_t *evt);
FLUIDSYNTH_API short fluid_event_get_control(fluid_event_t *evt);
FLUIDSYNTH_API short fluid_event_get_value(fluid_event_t *evt);
FLUIDSYNTH_API short fluid_event_get_program(fluid_event_t *evt);
FLUIDSYNTH_API void *fluid_event_get_data(fluid_event_t *evt);
FLUIDSYNTH_API unsigned int fluid_event_get_duration(fluid_event_t *evt);
FLUIDSYNTH_API short fluid_event_get_bank(fluid_event_t *evt);
FLUIDSYNTH_API int fluid_event_get_pitch(fluid_event_t *evt);
FLUIDSYNTH_API unsigned int fluid_event_get_sfont_id(fluid_event_t *evt);

#ifdef __cplusplus
}
#endif
#endif /* _FLUIDSYNTH_EVENT_H */
/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_MIDI_H
#define _FLUIDSYNTH_MIDI_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file midi.h
 * @brief Functions for MIDI events, drivers and MIDI file playback.
 */

FLUIDSYNTH_API fluid_midi_event_t *new_fluid_midi_event(void);
FLUIDSYNTH_API void delete_fluid_midi_event(fluid_midi_event_t *event);

FLUIDSYNTH_API int fluid_midi_event_set_type(fluid_midi_event_t *evt, int type);
FLUIDSYNTH_API int fluid_midi_event_get_type(fluid_midi_event_t *evt);
FLUIDSYNTH_API int fluid_midi_event_set_channel(fluid_midi_event_t *evt, int chan);
FLUIDSYNTH_API int fluid_midi_event_get_channel(fluid_midi_event_t *evt);
FLUIDSYNTH_API int fluid_midi_event_get_key(fluid_midi_event_t *evt);
FLUIDSYNTH_API int fluid_midi_event_set_key(fluid_midi_event_t *evt, int key);
FLUIDSYNTH_API int fluid_midi_event_get_velocity(fluid_midi_event_t *evt);
FLUIDSYNTH_API int fluid_midi_event_set_velocity(fluid_midi_event_t *evt, int vel);
FLUIDSYNTH_API int fluid_midi_event_get_control(fluid_midi_event_t *evt);
FLUIDSYNTH_API int fluid_midi_event_set_control(fluid_midi_event_t *evt, int ctrl);
FLUIDSYNTH_API int fluid_midi_event_get_value(fluid_midi_event_t *evt);
FLUIDSYNTH_API int fluid_midi_event_set_value(fluid_midi_event_t *evt, int val);
FLUIDSYNTH_API int fluid_midi_event_get_program(fluid_midi_event_t *evt);
FLUIDSYNTH_API int fluid_midi_event_set_program(fluid_midi_event_t *evt, int val);
FLUIDSYNTH_API int fluid_midi_event_get_pitch(fluid_midi_event_t *evt);
FLUIDSYNTH_API int fluid_midi_event_set_pitch(fluid_midi_event_t *evt, int val);
FLUIDSYNTH_API int fluid_midi_event_set_sysex(fluid_midi_event_t *evt, void *data,
        int size, int dynamic);
FLUIDSYNTH_API int fluid_midi_event_set_text(fluid_midi_event_t *evt,
        void *data, int size, int dynamic);
FLUIDSYNTH_API int fluid_midi_event_set_lyrics(fluid_midi_event_t *evt,
        void *data, int size, int dynamic);

/**
 * MIDI router rule type.
 * @since 1.1.0
 */
typedef enum
{
    FLUID_MIDI_ROUTER_RULE_NOTE,                  /**< MIDI note rule */
    FLUID_MIDI_ROUTER_RULE_CC,                    /**< MIDI controller rule */
    FLUID_MIDI_ROUTER_RULE_PROG_CHANGE,           /**< MIDI program change rule */
    FLUID_MIDI_ROUTER_RULE_PITCH_BEND,            /**< MIDI pitch bend rule */
    FLUID_MIDI_ROUTER_RULE_CHANNEL_PRESSURE,      /**< MIDI channel pressure rule */
    FLUID_MIDI_ROUTER_RULE_KEY_PRESSURE,          /**< MIDI key pressure rule */
#ifndef __DOXYGEN__
    FLUID_MIDI_ROUTER_RULE_COUNT                  /**< @internal Total count of rule types @warning This symbol is not part of the public API and ABI stability guarantee and may change at any time!*/
#endif
} fluid_midi_router_rule_type;

/**
 * Generic callback function for MIDI events.
 * @param data User defined data pointer
 * @param event The MIDI event
 * @return Should return #FLUID_OK on success, #FLUID_FAILED otherwise
 *
 * Will be used between
 * - MIDI driver and MIDI router
 * - MIDI router and synth
 * to communicate events.
 * In the not-so-far future...
 */
typedef int (*handle_midi_event_func_t)(void *data, fluid_midi_event_t *event);

FLUIDSYNTH_API fluid_midi_router_t *new_fluid_midi_router(fluid_settings_t *settings,
        handle_midi_event_func_t handler,
        void *event_handler_data);
FLUIDSYNTH_API void delete_fluid_midi_router(fluid_midi_router_t *handler);
FLUIDSYNTH_API int fluid_midi_router_set_default_rules(fluid_midi_router_t *router);
FLUIDSYNTH_API int fluid_midi_router_clear_rules(fluid_midi_router_t *router);
FLUIDSYNTH_API int fluid_midi_router_add_rule(fluid_midi_router_t *router,
        fluid_midi_router_rule_t *rule, int type);
FLUIDSYNTH_API fluid_midi_router_rule_t *new_fluid_midi_router_rule(void);
FLUIDSYNTH_API void delete_fluid_midi_router_rule(fluid_midi_router_rule_t *rule);
FLUIDSYNTH_API void fluid_midi_router_rule_set_chan(fluid_midi_router_rule_t *rule,
        int min, int max, float mul, int add);
FLUIDSYNTH_API void fluid_midi_router_rule_set_param1(fluid_midi_router_rule_t *rule,
        int min, int max, float mul, int add);
FLUIDSYNTH_API void fluid_midi_router_rule_set_param2(fluid_midi_router_rule_t *rule,
        int min, int max, float mul, int add);
FLUIDSYNTH_API int fluid_midi_router_handle_midi_event(void *data, fluid_midi_event_t *event);
FLUIDSYNTH_API int fluid_midi_dump_prerouter(void *data, fluid_midi_event_t *event);
FLUIDSYNTH_API int fluid_midi_dump_postrouter(void *data, fluid_midi_event_t *event);


FLUIDSYNTH_API
fluid_midi_driver_t *new_fluid_midi_driver(fluid_settings_t *settings,
        handle_midi_event_func_t handler,
        void *event_handler_data);

FLUIDSYNTH_API void delete_fluid_midi_driver(fluid_midi_driver_t *driver);


/**
 * MIDI player status enum.
 * @since 1.1.0
 */
enum fluid_player_status
{
    FLUID_PLAYER_READY,           /**< Player is ready */
    FLUID_PLAYER_PLAYING,         /**< Player is currently playing */
    FLUID_PLAYER_DONE             /**< Player is finished playing */
};

FLUIDSYNTH_API fluid_player_t *new_fluid_player(fluid_synth_t *synth);
FLUIDSYNTH_API void delete_fluid_player(fluid_player_t *player);
FLUIDSYNTH_API int fluid_player_add(fluid_player_t *player, const char *midifile);
FLUIDSYNTH_API int fluid_player_add_mem(fluid_player_t *player, const void *buffer, size_t len);
FLUIDSYNTH_API int fluid_player_play(fluid_player_t *player);
FLUIDSYNTH_API int fluid_player_stop(fluid_player_t *player);
FLUIDSYNTH_API int fluid_player_join(fluid_player_t *player);
FLUIDSYNTH_API int fluid_player_set_loop(fluid_player_t *player, int loop);
FLUIDSYNTH_API int fluid_player_set_midi_tempo(fluid_player_t *player, int tempo);
FLUIDSYNTH_API int fluid_player_set_bpm(fluid_player_t *player, int bpm);
FLUIDSYNTH_API int fluid_player_set_playback_callback(fluid_player_t *player, handle_midi_event_func_t handler, void *handler_data);

FLUIDSYNTH_API int fluid_player_get_status(fluid_player_t *player);
FLUIDSYNTH_API int fluid_player_get_current_tick(fluid_player_t *player);
FLUIDSYNTH_API int fluid_player_get_total_ticks(fluid_player_t *player);
FLUIDSYNTH_API int fluid_player_get_bpm(fluid_player_t *player);
FLUIDSYNTH_API int fluid_player_get_midi_tempo(fluid_player_t *player);
FLUIDSYNTH_API int fluid_player_seek(fluid_player_t *player, int ticks);

///

#ifdef __cplusplus
}
#endif

#endif /* _FLUIDSYNTH_MIDI_H */
/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_SEQ_H
#define _FLUIDSYNTH_SEQ_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file seq.h
 * @brief MIDI event sequencer.
 */

/**
 * Event callback prototype for destination clients.
 * @param time Current sequencer tick value (see fluid_sequencer_get_tick()).
 * @param event The event being received
 * @param seq The sequencer instance
 * @param data User defined data registered with the client
 */
typedef void (*fluid_event_callback_t)(unsigned int time, fluid_event_t *event,
                                       fluid_sequencer_t *seq, void *data);


FLUIDSYNTH_API fluid_sequencer_t *new_fluid_sequencer(void);
FLUIDSYNTH_API fluid_sequencer_t *new_fluid_sequencer2(int use_system_timer);
FLUIDSYNTH_API void delete_fluid_sequencer(fluid_sequencer_t *seq);
FLUIDSYNTH_API int fluid_sequencer_get_use_system_timer(fluid_sequencer_t *seq);
FLUIDSYNTH_API
fluid_seq_id_t fluid_sequencer_register_client(fluid_sequencer_t *seq, const char *name,
        fluid_event_callback_t callback, void *data);
FLUIDSYNTH_API void fluid_sequencer_unregister_client(fluid_sequencer_t *seq, fluid_seq_id_t id);
FLUIDSYNTH_API int fluid_sequencer_count_clients(fluid_sequencer_t *seq);
FLUIDSYNTH_API fluid_seq_id_t fluid_sequencer_get_client_id(fluid_sequencer_t *seq, int index);
FLUIDSYNTH_API char *fluid_sequencer_get_client_name(fluid_sequencer_t *seq, fluid_seq_id_t id);
FLUIDSYNTH_API int fluid_sequencer_client_is_dest(fluid_sequencer_t *seq, fluid_seq_id_t id);
FLUIDSYNTH_API void fluid_sequencer_process(fluid_sequencer_t *seq, unsigned int msec);
FLUIDSYNTH_API void fluid_sequencer_send_now(fluid_sequencer_t *seq, fluid_event_t *evt);
FLUIDSYNTH_API
int fluid_sequencer_send_at(fluid_sequencer_t *seq, fluid_event_t *evt,
                            unsigned int time, int absolute);
FLUIDSYNTH_API
void fluid_sequencer_remove_events(fluid_sequencer_t *seq, fluid_seq_id_t source, fluid_seq_id_t dest, int type);
FLUIDSYNTH_API unsigned int fluid_sequencer_get_tick(fluid_sequencer_t *seq);
FLUIDSYNTH_API void fluid_sequencer_set_time_scale(fluid_sequencer_t *seq, double scale);
FLUIDSYNTH_API double fluid_sequencer_get_time_scale(fluid_sequencer_t *seq);

// Compile in internal traceing functions
#define FLUID_SEQ_WITH_TRACE 0

#if FLUID_SEQ_WITH_TRACE
FLUIDSYNTH_API char *fluid_seq_gettrace(fluid_sequencer_t *seq);
FLUIDSYNTH_API void fluid_seq_cleartrace(fluid_sequencer_t *seq);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _FLUIDSYNTH_SEQ_H */
/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_SEQBIND_H
#define _FLUIDSYNTH_SEQBIND_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file seqbind.h
 * @brief Functions for binding sequencer objects to other subsystems.
 */

FLUIDSYNTH_API
fluid_seq_id_t fluid_sequencer_register_fluidsynth(fluid_sequencer_t *seq, fluid_synth_t *synth);
FLUIDSYNTH_API int
fluid_sequencer_add_midi_event_to_buffer(void *data, fluid_midi_event_t *event);


#ifdef __cplusplus
}
#endif
#endif /* _FLUIDSYNTH_SEQBIND_H */

/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_LOG_H
#define _FLUIDSYNTH_LOG_H


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @file log.h
 * @brief Logging interface
 *
 * The default logging function of the fluidsynth prints its messages
 * to the stderr. The synthesizer uses five level of messages: #FLUID_PANIC,
 * #FLUID_ERR, #FLUID_WARN, #FLUID_INFO, and #FLUID_DBG.
 *
 * A client application can install a new log function to handle the
 * messages differently. In the following example, the application
 * sets a callback function to display #FLUID_PANIC messages in a dialog,
 * and ignores all other messages by setting the log function to
 * NULL:
 *
 * @code
 *   fluid_set_log_function(FLUID_PANIC, show_dialog, (void*) root_window);
 *   fluid_set_log_function(FLUID_ERR, NULL, NULL);
 *   fluid_set_log_function(FLUID_WARN, NULL, NULL);
 *   fluid_set_log_function(FLUID_DBG, NULL, NULL);
 * @endcode
 */

/**
 * FluidSynth log levels.
 */
enum fluid_log_level
{
    FLUID_PANIC,   /**< The synth can't function correctly any more */
    FLUID_ERR,     /**< Serious error occurred */
    FLUID_WARN,    /**< Warning */
    FLUID_INFO,    /**< Verbose informational messages */
    FLUID_DBG,     /**< Debugging messages */
#ifndef __DOXYGEN__
    LAST_LOG_LEVEL /**< @warning This symbol is not part of the public API and ABI stability guarantee and may change at any time! */
#endif
};

/**
 * Log function handler callback type used by fluid_set_log_function().
 * @param level Log level (#fluid_log_level)
 * @param message Log message text
 * @param data User data pointer supplied to fluid_set_log_function().
 */
typedef void (*fluid_log_function_t)(int level, char *message, void *data);

FLUIDSYNTH_API
fluid_log_function_t fluid_set_log_function(int level, fluid_log_function_t fun, void *data);

FLUIDSYNTH_API void fluid_default_log_function(int level, char *message, void *data);

FLUIDSYNTH_API int fluid_log(int level, const char *fmt, ...);


#ifdef __cplusplus
}
#endif

#endif /* _FLUIDSYNTH_LOG_H */
/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_MISC_H
#define _FLUIDSYNTH_MISC_H


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @file misc.h
 * @brief Miscellaneous utility functions and defines
 */

/**
 * Value that indicates success, used by most libfluidsynth functions.
 * @since 1.1.0
 *
 * @note This was not publicly defined prior to libfluidsynth 1.1.0.  When
 * writing code which should also be compatible with older versions, something
 * like the following can be used:
 *
 * @code
 *   #include <fluidsynth.h>
 *
 *   #ifndef FLUID_OK
 *   #define FLUID_OK      (0)
 *   #define FLUID_FAILED  (-1)
 *   #endif
 * @endcode
 */
#define FLUID_OK        (0)

/**
 * Value that indicates failure, used by most libfluidsynth functions.
 * @since 1.1.0
 *
 * @note See #FLUID_OK for more details.
 */
#define FLUID_FAILED    (-1)


FLUIDSYNTH_API int fluid_is_soundfont(const char *filename);
FLUIDSYNTH_API int fluid_is_midifile(const char *filename);


#ifdef __cplusplus
}
#endif

#endif /* _FLUIDSYNTH_MISC_H */
/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_MOD_H
#define _FLUIDSYNTH_MOD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file mod.h
 * @brief SoundFont modulator functions and constants.
 */


/**
 * Flags defining the polarity, mapping function and type of a modulator source.
 * Compare with SoundFont 2.04 PDF section 8.2.
 *
 * Note: Bit values do not correspond to the SoundFont spec!  Also note that
 * #FLUID_MOD_GC and #FLUID_MOD_CC are in the flags field instead of the source field.
 */
enum fluid_mod_flags
{
    FLUID_MOD_POSITIVE = 0,       /**< Mapping function is positive */
    FLUID_MOD_NEGATIVE = 1,       /**< Mapping function is negative */
    FLUID_MOD_UNIPOLAR = 0,       /**< Mapping function is unipolar */
    FLUID_MOD_BIPOLAR = 2,        /**< Mapping function is bipolar */
    FLUID_MOD_LINEAR = 0,         /**< Linear mapping function */
    FLUID_MOD_CONCAVE = 4,        /**< Concave mapping function */
    FLUID_MOD_CONVEX = 8,         /**< Convex mapping function */
    FLUID_MOD_SWITCH = 12,        /**< Switch (on/off) mapping function */
    FLUID_MOD_GC = 0,             /**< General controller source type (#fluid_mod_src) */
    FLUID_MOD_CC = 16,             /**< MIDI CC controller (source will be a MIDI CC number) */

    FLUID_MOD_SIN = 0x80,            /**< Custom non-standard sinus mapping function */
};

/**
 * General controller (if #FLUID_MOD_GC in flags).  This
 * corresponds to SoundFont 2.04 PDF section 8.2.1
 */
enum fluid_mod_src
{
    FLUID_MOD_NONE = 0,                   /**< No source controller */
    FLUID_MOD_VELOCITY = 2,               /**< MIDI note-on velocity */
    FLUID_MOD_KEY = 3,                    /**< MIDI note-on note number */
    FLUID_MOD_KEYPRESSURE = 10,           /**< MIDI key pressure */
    FLUID_MOD_CHANNELPRESSURE = 13,       /**< MIDI channel pressure */
    FLUID_MOD_PITCHWHEEL = 14,            /**< Pitch wheel */
    FLUID_MOD_PITCHWHEELSENS = 16         /**< Pitch wheel sensitivity */
};

FLUIDSYNTH_API fluid_mod_t *new_fluid_mod(void);
FLUIDSYNTH_API void delete_fluid_mod(fluid_mod_t *mod);
FLUIDSYNTH_API size_t fluid_mod_sizeof(void);

FLUIDSYNTH_API void fluid_mod_set_source1(fluid_mod_t *mod, int src, int flags);
FLUIDSYNTH_API void fluid_mod_set_source2(fluid_mod_t *mod, int src, int flags);
FLUIDSYNTH_API void fluid_mod_set_dest(fluid_mod_t *mod, int dst);
FLUIDSYNTH_API void fluid_mod_set_amount(fluid_mod_t *mod, double amount);

FLUIDSYNTH_API int fluid_mod_get_source1(const fluid_mod_t *mod);
FLUIDSYNTH_API int fluid_mod_get_flags1(const fluid_mod_t *mod);
FLUIDSYNTH_API int fluid_mod_get_source2(const fluid_mod_t *mod);
FLUIDSYNTH_API int fluid_mod_get_flags2(const fluid_mod_t *mod);
FLUIDSYNTH_API int fluid_mod_get_dest(const fluid_mod_t *mod);
FLUIDSYNTH_API double fluid_mod_get_amount(const fluid_mod_t *mod);

FLUIDSYNTH_API int fluid_mod_test_identity(const fluid_mod_t *mod1, const fluid_mod_t *mod2);
FLUIDSYNTH_API int fluid_mod_has_source(const fluid_mod_t *mod, int cc, int ctrl);
FLUIDSYNTH_API int fluid_mod_has_dest(const fluid_mod_t *mod, int gen);

FLUIDSYNTH_API void fluid_mod_clone(fluid_mod_t *mod, const fluid_mod_t *src);

#ifdef __cplusplus
}
#endif
#endif /* _FLUIDSYNTH_MOD_H */

/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_GEN_H
#define _FLUIDSYNTH_GEN_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file gen.h
 * @brief Functions and defines for SoundFont generator effects.
 */

/**
 * Generator (effect) numbers (Soundfont 2.01 specifications section 8.1.3)
 */
enum fluid_gen_type
{
    GEN_STARTADDROFS,		/**< Sample start address offset (0-32767) */
    GEN_ENDADDROFS,		/**< Sample end address offset (-32767-0) */
    GEN_STARTLOOPADDROFS,		/**< Sample loop start address offset (-32767-32767) */
    GEN_ENDLOOPADDROFS,		/**< Sample loop end address offset (-32767-32767) */
    GEN_STARTADDRCOARSEOFS,	/**< Sample start address coarse offset (X 32768) */
    GEN_MODLFOTOPITCH,		/**< Modulation LFO to pitch */
    GEN_VIBLFOTOPITCH,		/**< Vibrato LFO to pitch */
    GEN_MODENVTOPITCH,		/**< Modulation envelope to pitch */
    GEN_FILTERFC,			/**< Filter cutoff */
    GEN_FILTERQ,			/**< Filter Q */
    GEN_MODLFOTOFILTERFC,		/**< Modulation LFO to filter cutoff */
    GEN_MODENVTOFILTERFC,		/**< Modulation envelope to filter cutoff */
    GEN_ENDADDRCOARSEOFS,		/**< Sample end address coarse offset (X 32768) */
    GEN_MODLFOTOVOL,		/**< Modulation LFO to volume */
    GEN_UNUSED1,			/**< Unused */
    GEN_CHORUSSEND,		/**< Chorus send amount */
    GEN_REVERBSEND,		/**< Reverb send amount */
    GEN_PAN,			/**< Stereo panning */
    GEN_UNUSED2,			/**< Unused */
    GEN_UNUSED3,			/**< Unused */
    GEN_UNUSED4,			/**< Unused */
    GEN_MODLFODELAY,		/**< Modulation LFO delay */
    GEN_MODLFOFREQ,		/**< Modulation LFO frequency */
    GEN_VIBLFODELAY,		/**< Vibrato LFO delay */
    GEN_VIBLFOFREQ,		/**< Vibrato LFO frequency */
    GEN_MODENVDELAY,		/**< Modulation envelope delay */
    GEN_MODENVATTACK,		/**< Modulation envelope attack */
    GEN_MODENVHOLD,		/**< Modulation envelope hold */
    GEN_MODENVDECAY,		/**< Modulation envelope decay */
    GEN_MODENVSUSTAIN,		/**< Modulation envelope sustain */
    GEN_MODENVRELEASE,		/**< Modulation envelope release */
    GEN_KEYTOMODENVHOLD,		/**< Key to modulation envelope hold */
    GEN_KEYTOMODENVDECAY,		/**< Key to modulation envelope decay */
    GEN_VOLENVDELAY,		/**< Volume envelope delay */
    GEN_VOLENVATTACK,		/**< Volume envelope attack */
    GEN_VOLENVHOLD,		/**< Volume envelope hold */
    GEN_VOLENVDECAY,		/**< Volume envelope decay */
    GEN_VOLENVSUSTAIN,		/**< Volume envelope sustain */
    GEN_VOLENVRELEASE,		/**< Volume envelope release */
    GEN_KEYTOVOLENVHOLD,		/**< Key to volume envelope hold */
    GEN_KEYTOVOLENVDECAY,		/**< Key to volume envelope decay */
    GEN_INSTRUMENT,		/**< Instrument ID (shouldn't be set by user) */
    GEN_RESERVED1,		/**< Reserved */
    GEN_KEYRANGE,			/**< MIDI note range */
    GEN_VELRANGE,			/**< MIDI velocity range */
    GEN_STARTLOOPADDRCOARSEOFS,	/**< Sample start loop address coarse offset (X 32768) */
    GEN_KEYNUM,			/**< Fixed MIDI note number */
    GEN_VELOCITY,			/**< Fixed MIDI velocity value */
    GEN_ATTENUATION,		/**< Initial volume attenuation */
    GEN_RESERVED2,		/**< Reserved */
    GEN_ENDLOOPADDRCOARSEOFS,	/**< Sample end loop address coarse offset (X 32768) */
    GEN_COARSETUNE,		/**< Coarse tuning */
    GEN_FINETUNE,			/**< Fine tuning */
    GEN_SAMPLEID,			/**< Sample ID (shouldn't be set by user) */
    GEN_SAMPLEMODE,		/**< Sample mode flags */
    GEN_RESERVED3,		/**< Reserved */
    GEN_SCALETUNE,		/**< Scale tuning */
    GEN_EXCLUSIVECLASS,		/**< Exclusive class number */
    GEN_OVERRIDEROOTKEY,		/**< Sample root note override */

    /* the initial pitch is not a "standard" generator. It is not
     * mentioned in the list of generator in the SF2 specifications. It
     * is used, however, as the destination for the default pitch wheel
     * modulator. */
    GEN_PITCH,			/**< Pitch @note Not a real SoundFont generator */

    GEN_CUSTOM_BALANCE,          /**< Balance @note Not a real SoundFont generator */
    /* non-standard generator for an additional custom high- or low-pass filter */
    GEN_CUSTOM_FILTERFC,		/**< Custom filter cutoff frequency */
    GEN_CUSTOM_FILTERQ,		/**< Custom filter Q */

#ifndef __DOXYGEN__
    GEN_LAST			/**< @internal Value defines the count of generators (#fluid_gen_type) @warning This symbol is not part of the public API and ABI stability guarantee and may change at any time! */
#endif
};


#ifdef __cplusplus
}
#endif
#endif /* _FLUIDSYNTH_GEN_H */

/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_VOICE_H
#define _FLUIDSYNTH_VOICE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file voice.h
 * @brief Synthesis voice manipulation functions.
 *
 * The interface to the synthesizer's voices.
 * Examples on using them can be found in fluid_defsfont.c.
 * Most of these functions should only be called from within synthesis context,
 * such as the SoundFont loader's noteon method.
 */


/**
 * Enum used with fluid_voice_add_mod() to specify how to handle duplicate modulators.
 */
enum fluid_voice_add_mod
{
    FLUID_VOICE_OVERWRITE,        /**< Overwrite any existing matching modulator */
    FLUID_VOICE_ADD,              /**< Add (sum) modulator amounts */
    FLUID_VOICE_DEFAULT           /**< For default modulators only, no need to check for duplicates */
};

FLUIDSYNTH_API void fluid_voice_add_mod(fluid_voice_t *voice, fluid_mod_t *mod, int mode);
FLUIDSYNTH_API float fluid_voice_gen_get(fluid_voice_t *voice, int gen);
FLUIDSYNTH_API void fluid_voice_gen_set(fluid_voice_t *voice, int gen, float val);
FLUIDSYNTH_API void fluid_voice_gen_incr(fluid_voice_t *voice, int gen, float val);

FLUIDSYNTH_API unsigned int fluid_voice_get_id(const fluid_voice_t *voice);
FLUIDSYNTH_API int fluid_voice_get_channel(const fluid_voice_t *voice);
FLUIDSYNTH_API int fluid_voice_get_key(const fluid_voice_t *voice);
FLUIDSYNTH_API int fluid_voice_get_actual_key(const fluid_voice_t *voice);
FLUIDSYNTH_API int fluid_voice_get_velocity(const fluid_voice_t *voice);
FLUIDSYNTH_API int fluid_voice_get_actual_velocity(const fluid_voice_t *voice);
FLUIDSYNTH_API int fluid_voice_is_playing(const fluid_voice_t *voice);
FLUIDSYNTH_API int fluid_voice_is_on(const fluid_voice_t *voice);
FLUIDSYNTH_API int fluid_voice_is_sustained(const fluid_voice_t *voice);
FLUIDSYNTH_API int fluid_voice_is_sostenuto(const fluid_voice_t *voice);
FLUIDSYNTH_API int fluid_voice_optimize_sample(fluid_sample_t *s);
FLUIDSYNTH_API void fluid_voice_update_param(fluid_voice_t *voice, int gen);


#ifdef __cplusplus
}
#endif
#endif /* _FLUIDSYNTH_VOICE_H */

/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *  
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUIDSYNTH_VERSION_H
#define _FLUIDSYNTH_VERSION_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file version.h
 * @brief Library version functions and defines
 */

#define FLUIDSYNTH_VERSION       "2.0.0"           /**< String constant of libfluidsynth version. */
#define FLUIDSYNTH_VERSION_MAJOR 2     /**< libfluidsynth major version integer constant. */
#define FLUIDSYNTH_VERSION_MINOR 0     /**< libfluidsynth minor version integer constant. */
#define FLUIDSYNTH_VERSION_MICRO 0     /**< libfluidsynth micro version integer constant. */

FLUIDSYNTH_API void fluid_version(int *major, int *minor, int *micro);
FLUIDSYNTH_API char* fluid_version_str(void);


#ifdef __cplusplus
}
#endif

#endif /* _FLUIDSYNTH_VERSION_H */
