#ifndef PHASHMODEL_H
#define PHASHMODEL_H

#include <vector>

typedef struct phashblock_t
{
public:
  phashblock_t():
    phashcnt(0),
    prevcnt(-1),
    isseek(FALSE) {}

  WAVEFORMATEX format;
  std::vector<unsigned char> phashdata;
  int phashcnt;
  int prevcnt;
  int type;
  BOOL isseek;
} PHASHBLOCK;

enum{
  PCM8 = 0,
  PCM16,
  PCM24,
  PCM32,
  FPCM32,
  FPCM64
};

enum {
  CFG_PHASHTIMES = 0,
  CFG_PHASHDATASECS,
  CFG_PHASHSTARTTIME
};

const int g_phash_collectcfg[] = {
  // times;
  2,
  // datasecs;
  15,
  // start collect time;
  0,  // this is a placeholder
  0,
  25//,
//  20,
//  30
};


#endif //PHASHMODEL_H