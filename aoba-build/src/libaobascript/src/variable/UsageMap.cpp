#include <aoba/variable/UsageMap.hpp>
#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace Aoba;

// ----------------------------------------------------------------------------
UsageMap::UsageMap()
  : offset(0), size(0)
{
}

// ----------------------------------------------------------------------------
UsageMap::UsageMap(const uint64_t &offset, const uint64_t &size, UsageMapSection::Usage usage)
  : offset(offset), size(size)
{
  if (size > 0) {
    map.push_back(UsageMapSection(usage, offset, size));
  }
}

// ----------------------------------------------------------------------------
UsageMap *UsageMap::clone(UsageMap *other) {
  UsageMap *result = new UsageMap(other->offset, other->size, UsageMapSection::BLOCKED);
  result->map = other->map;
  return result;
}

// ----------------------------------------------------------------------------
bool UsageMap::block(const uint64_t &start, const uint64_t &size) {
  return set(start, size, UsageMapSection::BLOCKED);
}

// ----------------------------------------------------------------------------
bool UsageMap::free(const uint64_t &start, const uint64_t &size) {
  return set(start, size, UsageMapSection::FREE);
}

// ----------------------------------------------------------------------------
UsageMapAddressRanges UsageMap::findFreeAddresses(const uint64_t &from, const uint64_t &to, const uint64_t &size) {
  UsageMapAddressRanges result;

  if (size == 0) {
    return result;
  }

  auto it = find(offset);
  if (it == map.end()) {
    return result;
  }

  bool currentIsFree = false;
  int64_t currentStart, currentEnd;
  for (; it != map.end() && it->start <= to; it++) {
    if (it->isFree()) {
      if (!currentIsFree) {
        currentIsFree = true;
        currentStart = from > it->start ? from : it->start;
      }
      currentEnd = to < it->end() ? to : it->end(); 
    } else {
      if (currentIsFree) {
        currentIsFree = false;
        currentEnd -= size - 1;
        if (currentStart <= currentEnd) {
          result.push_back(UsageMapAddressRange(currentStart, currentEnd));
        }
      }
    }
  }

  if (currentIsFree) {
    currentEnd -= size - 1;
    if (currentStart <= currentEnd) {
      result.push_back(UsageMapAddressRange(currentStart, currentEnd));
    }
  }

  return result;
}

// ----------------------------------------------------------------------------
bool UsageMap::set(const uint64_t &offset, const uint64_t &size, UsageMapSection::Usage usage) {
  if (size == 0) {
    return true;
  }

  auto it = find(offset);
  if (it == map.end()) {
    return false;
  }

  uint64_t end = offset + size;
  for (; it != map.end() && it->start < end; it++) {
    // Join with next if equal
    auto next = it;
    ++next;
    if (next != map.end()) {
      if (next->size == 0 || it->usage == next->usage) {
        next->size += it->size;
        next->start = it->start;
        it = map.erase(it);
      }
    }

    // Skip if usage already matches
    if (it->usage == usage) {
      continue;
    }

    // Split if start is lower than requested
    if (it->start < offset) {
      uint64_t oldStart = it->start;
      uint64_t oldSize = offset - it->start;
      UsageMapSection::Usage oldUsage = it->usage;

      it->start = oldStart + oldSize;
      it->size -= oldSize;

      it = map.insert(it, UsageMapSection(oldUsage, oldStart, oldSize));
      continue;
    }

    // Split if requested end is inside current
    if (it->start + it->size > end) {
      uint64_t oldStart = it->start;

      it->start = end;
      it->size -= end - oldStart;

      it = map.insert(it, UsageMapSection(usage, oldStart, end - oldStart));
      it++;

      continue;
    }

    // Skip if section is inside requested
    if (it->start + it->size <= end) {
      it->usage = usage;
      continue;
    }
  }

  return true;
}

// ----------------------------------------------------------------------------
void UsageMap::block(const UsageMap &other) {
  set(other, UsageMapSection::BLOCKED, UsageMapSection::BLOCKED);
}

// ----------------------------------------------------------------------------
void UsageMap::free(const UsageMap &other) {
  set(other, UsageMapSection::FREE, UsageMapSection::FREE);
}

// ----------------------------------------------------------------------------
void UsageMap::set(const UsageMap &other, UsageMapSection::Usage where, UsageMapSection::Usage to) {
  for (auto &item : other.all()) {
    if (item.usage != where) {
      continue;
    }

    set(item.start, item.size, to);
  }
}

// ----------------------------------------------------------------------------
UsageMapSectionList::iterator UsageMap::find(const uint64_t &offset) {
  // TODO: This does not work as expected
  return std::lower_bound(
    map.begin(),
    map.end(),
    offset,
    [](const UsageMapSection &section, const uint64_t &value) { return (section.start + section.size - 1) < value; }
  );
}

// ----------------------------------------------------------------------------
uint64_t UsageMap::getSizeOf(UsageMapSection::Usage usage) const {
  uint64_t size = 0;

  for (const auto &item : all()) {
    if (item.usage != usage) {
      continue;
    }

    size += item.size;
  }

  return size;
}

// ----------------------------------------------------------------------------
