// clang-format off
extern "C" {
#include <../src/vgmstream.h>
#include <../version.h>

// 普通のtry-catchだとなぜか例外が補足できなかったためsjljを使ったTry-Catchを行う
//// 使ったmingw-w64がdwarfだったせいいか、もしくはコンパイルオプションがおかしかったせいか？
//// もしくはC言語(nothrow?)が間に挟まっていたせいか？
//// ref: https://yupo5656.hatenadiary.org/entry/20051215/p1
//// ref: https://github.com/ThrowTheSwitch/CException
#include "CException/lib/CException.h"
}
#include <stdbool.h>
#include <windows.h>
#include <delayimp.h>

#include <stdexcept>

#include "public_dll.h"
// clang-format on

extern "C" {
void print_fail_info(unsigned dliNotify, PDelayLoadInfo pLoadInfo) {
  char dll_name[32] = {0};
  char func_name[32] = {0};
  if (pLoadInfo->szDll)
    strncpy(dll_name, pLoadInfo->szDll, sizeof(dll_name) - 1);

  if (dliNotify == dliFailLoadLib) {
    printf("module could not be loaded. dll=%s, GLE=%ld\n", dll_name,
           pLoadInfo->dwLastError);

  } else if (dliNotify == dliFailLoadLib) {
    if (!pLoadInfo->dlp.fImportByName) {
      snprintf(func_name, sizeof(func_name) - 1, "/%ld",
               pLoadInfo->dlp.dwOrdinal);
    } else {
      strncpy(func_name, pLoadInfo->dlp.szProcName, sizeof(func_name));
    }
    printf("could not find procedure in module.  dll=%s, func=%s, GLE=%ld\n",
           dll_name, func_name, pLoadInfo->dwLastError);
  }
}

// dll(ffmpeg, etc...) delay load error handler.
FARPROC WINAPI delayHook(unsigned dliNotify, PDelayLoadInfo pLoadInfo) {
  switch (dliNotify) {
    case dliFailLoadLib:
    case dliFailGetProc:
      print_fail_info(dliNotify, pLoadInfo);
      Throw(0);
      break;

    default:
      break;
  };
  return 0;
}

VGMSTREAM_HANDLE __calltype init_vgmstream_std(const char *const filename) {
  // register dll handler
  __pfnDliFailureHook2 = &delayHook;
  VGMSTREAM *vgmstream = NULL;

  STREAMFILE *sf = open_stdio_streamfile(filename);
  if (!sf) {
    return NULL;
  }

  volatile CEXCEPTION_T e;
  Try {
    // try {
    vgmstream = init_vgmstream_from_STREAMFILE(sf);
    // } catch (std::runtime_error &e) {
    //   printf("CATCH %s\n", e.what());
    // } catch (std::exception &e) {
    //   printf("CATCH2 %s\n", e.what());
  }
  Catch(e) {
    printf("May be delay load error. id=%d.\n", e);
    vgmstream = NULL;
  }

  if (!vgmstream) {
    close_streamfile(sf);
  }

  return (VGMSTREAM_HANDLE)vgmstream;
}

void __calltype reset_vgmstream_std(VGMSTREAM_HANDLE handle) {
  auto s = (VGMSTREAM *)handle;
  reset_vgmstream(s);
}

// VGMSTREAM_HANDLE __calltype allocate_vgmstream_std(int channel_count,
//                                                    int looped) {
//   return (VGMSTREAM_HANDLE)allocate_vgmstream(channel_count, looped);
// }

void __calltype close_vgmstream_std(VGMSTREAM_HANDLE handle) {
  auto s = (VGMSTREAM *)handle;
  close_vgmstream(s);
}

int32_t __calltype get_vgmstream_play_samples_std(double looptimes,
                                                  double fadeseconds,
                                                  double fadedelayseconds,
                                                  VGMSTREAM_HANDLE handle) {
  auto s = (VGMSTREAM *)handle;
  return get_vgmstream_play_samples(looptimes, fadeseconds, fadedelayseconds,
                                    s);
}

void __calltype render_vgmstream_std(sample *buffer, int32_t sample_count,
                                     VGMSTREAM_HANDLE handle) {
  auto s = (VGMSTREAM *)handle;
  render_vgmstream(buffer, sample_count, s);
}

void __calltype describe_vgmstream_std(VGMSTREAM_HANDLE handle, char *desc,
                                       int length) {
  auto s = (VGMSTREAM *)handle;
  describe_vgmstream(s, desc, length);
}

const char *__calltype get_vgmstream_version() {
  return "VGMSTREAM DLL: " VGMSTREAM_VERSION;
}

const char **vgmstream_get_formats_std(size_t *size) {
  volatile CEXCEPTION_T e;
  Try { return vgmstream_get_formats(size); }
  Catch(e) { return NULL; }
}

// ============================================================================

bool __calltype vgmstream_get_basic_sound_info_std(VGMSTREAM_HANDLE handle,
                                                   int32_t *num_samples,
                                                   int32_t *sample_rate,
                                                   int *channels) {
  auto s = (VGMSTREAM *)handle;
  if (s == 0) return FALSE;
  if (num_samples != 0) *num_samples = s->num_samples;
  if (sample_rate != 0) *sample_rate = s->sample_rate;
  if (channels != 0) *channels = s->channels;
  return TRUE;
}

int __calltype vgmstream_have_loop_std(VGMSTREAM_HANDLE handle) {
  auto s = (VGMSTREAM *)handle;
  return s->loop_flag;
}

// void __calltype foce_noLoop(VGMSTREAM_HANDLE handle) {
//   auto s = (VGMSTREAM *)handle;
//   s->loop_flag = 0;
// }

// int32_t __calltype get_loopSample(VGMSTREAM_HANDLE handle) {
//   auto s = (VGMSTREAM *)handle;
//   return s->loop_end_sample;
// }

// void foce_loop(VGMSTREAM_HANDLE handle) {
//   auto s = (VGMSTREAM *)handle;
//   /* force only if there aren't already loop points */
//   if (!s->loop_flag) {
//     /* this requires a bit more messing with the VGMSTREAM than I'm
//      * comfortable with... */
//     s->loop_flag = 1;
//     s->loop_start_sample = 0;
//     s->loop_end_sample = s->num_samples;
//     s->loop_ch =
//         (VGMSTREAMCHANNEL *)calloc(s->channels, sizeof(VGMSTREAMCHANNEL));
//   }

//   /* force even if there are loop points */
//   /*    if (really_force_loop) {
//           if (!s->loop_flag)
//      s->loop_ch=calloc(s->channels,sizeof(VGMSTREAMCHANNEL));
//      s->loop_flag=1;
//           s->loop_start_sample=0;
//           s->loop_end_sample=s->num_samples;
//       }*/
// }
}