#include <QList>
#include <aoba/font/Font.hpp>
#include <aoba/font/FontRegistry.hpp>
#include <aoba/font/FontLetterRegistry.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
void FontLetterRegistry::buildIndex(FontRegistry *reg) {
  if (mIndexIsBuilt) {
    return;
  }

  // Generates a map with all letters being used
  FontLetterIndex letterUsage;
  for (FontSetInterface *set : reg->allSets()) {
    QMapIterator<uint32_t, int> it(set->letterUsage());
    while (it.hasNext()) {
      it.next();
      letterUsage[it.key()] += it.value();
    }
  }

  // Generates a list of all letters being used
  QList<QPair<int, uint32_t>> letterUsageList;
  for (auto letter : letterUsage.keys()) {
    letterUsageList.push_back(QPair<int, uint32_t>(letterUsage[letter], letter));
  }

  std::sort(std::begin(letterUsageList), std::end(letterUsageList));

  int index = 0;
  for (auto &pair : letterUsageList) {
    mLetterIndex.insert(pair.second, index++);
  }

  mIndexIsBuilt = true;
}

// -----------------------------------------------------------------------------
