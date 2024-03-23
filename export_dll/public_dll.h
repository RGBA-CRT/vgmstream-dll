extern "C" {
#include <stdbool.h>
#include <stdint.h>

#define __calltype WINAPI
typedef void *VGMSTREAM_HANDLE;

VGMSTREAM_HANDLE __calltype init_vgmstream_std(const char *const filename);
void __calltype reset_vgmstream_std(VGMSTREAM_HANDLE handle);
void __calltype close_vgmstream_std(VGMSTREAM_HANDLE handle);
int32_t __calltype get_vgmstream_play_samples_std(double looptimes,
                                                  double fadeseconds,
                                                  double fadedelayseconds,
                                                  VGMSTREAM_HANDLE handle);
void __calltype render_vgmstream_std(sample *buffer, int32_t sample_count,
                                     VGMSTREAM_HANDLE handle);
void __calltype describe_vgmstream_std(VGMSTREAM_HANDLE handle, char *desc,
                                       int length);
const char *__calltype get_vgmstream_version();
const char **vgmstream_get_formats_std(size_t *size);

// ============================================================================

bool __calltype vgmstream_get_basic_sound_info_std(VGMSTREAM_HANDLE handle,
                                                   int32_t *num_samples,
                                                   int32_t *sample_rate,
                                                   int *channels);
int __calltype vgmstream_have_loop_std(VGMSTREAM_HANDLE handle);
}